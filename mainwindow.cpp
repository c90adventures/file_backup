#include <QFileDialog>
#include <QTableWidget>
#include <QDebug>
#include <QCryptographicHash>
#include <QDirIterator>
#include <QtConcurrent/QtConcurrent>
#include <iostream>
#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::MainWindow)
{
  ui->setupUi(this);
  qRegisterMetaType<QTreeWidgetItem*>("QTreeWidgetItem*");
  setWindowTitle(tr("Ed's duplicate files finder, build from %1, %2").arg(QString::fromLocal8Bit(__DATE__)).arg(QString::fromLocal8Bit(__TIME__)));
  connect(this, SIGNAL(comparingComplete()), this, SLOT(colorizeResults()));
}

MainWindow::~MainWindow()
{
  delete ui;
}

void MainWindow::on_pb_addFolderContents_clicked()
{
  QString sDir = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
                                                           "",
                                                           QFileDialog::ShowDirsOnly
                                                           | QFileDialog::DontResolveSymlinks);
  QDir dir(sDir);
  m_itemsCount = addItems(dir, NULL);
}

int MainWindow::addItems(QDir a_dir, QTreeWidgetItem* a_parent)
{
  a_dir.setFilter(QDir::Files | QDir::NoSymLinks | QDir::Dirs | QDir::NoDotAndDotDot);
  a_dir.setSorting(QDir::DirsLast);
  QFileInfoList list = a_dir.entryInfoList();

  int itemsAdded = 0;

  for (int i = 0; i < list.size(); ++i)
  {
    QFileInfo file = list.at(i);

    QTreeWidgetItem* child = NULL;
    if (a_parent == NULL) {
      child = new QTreeWidgetItem(ui->treeWidget);
      child->setText(0, file.absoluteFilePath());
      ui->treeWidget->addTopLevelItem(child);
    } else {
      child = new QTreeWidgetItem();
      child->setText(0, file.absoluteFilePath());
      a_parent->addChild(child);
    }

    if (true == file.isDir()) {
      if (false == a_dir.cd(file.fileName())) {
        qCritical() << "Failed to open " << file.fileName();
      } else {
        itemsAdded += addItems(a_dir, child) ;
        a_dir.cdUp();
      }
    } else {
      itemsAdded++;
    }
  }
  return itemsAdded;
}

void MainWindow::on_pb_setFolder_clicked()
{
  QString dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
                                               "",
                                               QFileDialog::ShowDirsOnly
                                               | QFileDialog::DontResolveSymlinks);


  ui->le_folder->setText(dir);
  m_folder = dir;
}

QByteArray MainWindow::getFileHash(QFile &file)
{
  QMetaObject::invokeMethod(statusBar(), "showMessage", Qt::QueuedConnection, Q_ARG(QString, tr("Getting hash for %1...").arg(file.fileName())));
  QCryptographicHash hash(QCryptographicHash::Sha1);

  int chunkSize = 8192;

  if (file.open(QIODevice::ReadOnly)) {
    while (!file.atEnd()) {
      hash.addData(file.read(chunkSize));
    }
  } else {
    qWarning() << "File couldn't be opened. " << file.fileName();
  }
  file.close();
  return hash.result();
}

// @args if hash1 is not empty, it will be assumed that it is a hash for file1
//       if it is empty, it will be filled, so that it can be reZused in the future
bool MainWindow::compareFiles(QString f1, QString f2, QByteArray &hash1)
{
  QFile file1(f1), file2(f2);
  if (file1.size() == file2.size())
  {
    hash1 = hash1.size() ? hash1 : getFileHash(file1);
    QByteArray hash2 = getFileHash(file2);
    return hash1 == hash2;
  } else {
    // qDebug() << "size mismatch" << file1.fileName() << " " << file2.fileName();
  }
  return false;
}

void MainWindow::on_pbGo_clicked()
{
  QFuture<void> future = QtConcurrent::run(this, &MainWindow::startWorking);
}

void MainWindow::startWorking()
{
  QMetaObject::invokeMethod(ui->progressBar, "setRange", Qt::QueuedConnection, Q_ARG(int, 0), Q_ARG(int, m_itemsCount));

  //foreach entry in table, search for it in the folder
  QTreeWidgetItemIterator treeIt(ui->treeWidget);
  int itemsProcessed = 0;
  while (*treeIt) {
    QFileInfo qfi((*treeIt)->text(0));
    if (!qfi.isDir()) {
      QStringList foundFiles;
      qDebug() << "Searching for " << (*treeIt)->text(0) << "...";

      QDirIterator dirIt(m_folder, QStringList(qfi.fileName()), QDir::Files, QDirIterator::Subdirectories);
      QMetaObject::invokeMethod(statusBar(), "showMessage", Qt::QueuedConnection, Q_ARG(QString, tr("Searching for %1...").arg(qfi.fileName())));
      while (dirIt.hasNext()) {
        dirIt.next();
        foundFiles << dirIt.filePath();
      }

      qDebug() << "  comparing to " << foundFiles.size() << "files...";
      QStringList sameFiles;
      QByteArray hashOfSourceFile;
      for (int i = 0; i < foundFiles.size(); i++) {
        if (compareFiles((*treeIt)->text(0), foundFiles[i], hashOfSourceFile)) {
          sameFiles << foundFiles[i];
        }
      }

      qDebug() << "Found " << sameFiles.size() << " same files for file " << (*treeIt)->text(0);

      // what to do if multiple files have been found?
      QString str;
      if (sameFiles.size()) {
        str = tr("(%1) %2").arg(QString::number(sameFiles.size())).arg(sameFiles.join(", "));
      } else {
        str = "Not found.";
      }


      QMetaObject::invokeMethod(this, "setItemTextInTable", Qt::QueuedConnection, Q_ARG(QTreeWidgetItem*, (*treeIt)), Q_ARG(int, 1), Q_ARG(QString, str));
      QMetaObject::invokeMethod(ui->progressBar, "setValue", Qt::QueuedConnection, Q_ARG(int, itemsProcessed));
      itemsProcessed++;
    } // if (!qfi.isDir())
  ++treeIt;
  }
  QMetaObject::invokeMethod(ui->progressBar, "setValue", Qt::QueuedConnection, Q_ARG(int, m_itemsCount));  // set progressbar to 100%

  // main work is done, go back to main thread
  emit comparingComplete();
}

void MainWindow::colorizeResults()
{
  // I can either sent some model data from startWorking() (and be done with 'invokeMethod' calls)
  // or I could go through treeWidgetItems again here

  // Whole folder contents found - green. Else red.
  // Files too.
}


void MainWindow::setItemTextInTable(QTreeWidgetItem* item, int col, QString str)
{
  item->setText(col, str);
}
