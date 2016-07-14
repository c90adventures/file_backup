#include <QFileDialog>
#include <QTableWidget>
#include <QDebug>
#include <QCryptographicHash>
#include <QDirIterator>
#include <QtConcurrent/QtConcurrent>
#include <QMessageBox>
#include <QMenu>
#include <QFuture>
#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::MainWindow),
  STR_NOT_FOUND("Not found."),
  STR_PROGRAM_NAME("Ed's duplicate files finder")
{
  QSettings settings("./marched.ini", QSettings::IniFormat);
  m_foundColor = QColor(settings.value("foundColor", "#449333").toString());
  m_notFoundColor = QColor(settings.value("notFoundColor", "#FD0600").toString());

  ui->setupUi(this);
  qRegisterMetaType<QTreeWidgetItem*>("QTreeWidgetItem*");
  setWindowTitle(tr("%1, build from %2, %3").arg(STR_PROGRAM_NAME).arg(QString::fromLocal8Bit(__DATE__)).arg(QString::fromLocal8Bit(__TIME__)));
  connect(this, SIGNAL(comparingComplete()), this, SLOT(colorizeResults()));

  this->setContextMenuPolicy(Qt::CustomContextMenu);

  connect(this, SIGNAL(customContextMenuRequested(const QPoint &)),
          this, SLOT(showContextMenu(const QPoint &)));
}

MainWindow::~MainWindow()
{
  delete ui;
}

void MainWindow::showContextMenu(const QPoint & pos)
{
  QMenu contextMenu(tr("Context menu"), this);

  QAction actionAbout("About..", this);
  connect(&actionAbout, SIGNAL(triggered()), this, SLOT(showAbout()));
  contextMenu.addAction(&actionAbout);
  contextMenu.exec(mapToGlobal(pos));
}

void MainWindow::showAbout()
{
  QMessageBox msgBox(this);
  msgBox.setWindowTitle("About");
  msgBox.setTextFormat(Qt::RichText);   //this is what makes the links clickable
  msgBox.setText(tr("%1 was designed by Ed March and created by Marek Panek in 2016.<br>"
                    "Project page and source code can be found <a href='https://gitlab.com/MarPan/marched'>here</a>.<br>"
                    "Go visit <a href='http://www.c90adventures.co.uk/'>Ed's page</a>, watch his <a href='https://www.youtube.com/user/c90adventures'>films</a> or find him on <a href='https://www.facebook.com/edwin.march'>Facebook</a>!").arg(STR_PROGRAM_NAME));
  msgBox.exec();
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

  for (int i = 0; i < list.size(); ++i) {
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
  QtConcurrent::run(this, &MainWindow::startWorking);
}

void MainWindow::findDuplicates(QList<QTreeWidgetItem*> listOfItems)
{
  for (int m = 0; m < listOfItems.length(); m++) {
    QFileInfo qfi(listOfItems[m]->text(0));
    if (!qfi.isDir()) {

      QStringList foundFiles;
      qDebug() << "Searching for " << listOfItems[m]->text(0) << "...";

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
        if (compareFiles(listOfItems[m]->text(0), foundFiles[i], hashOfSourceFile)) {
          sameFiles << foundFiles[i];
        }
      }

      qDebug() << "Found " << sameFiles.size() << " same files for file " << listOfItems[m]->text(0);

      // what to do if multiple files have been found:
      QString str;
      if (sameFiles.size()) {
        str = tr("(%1) %2").arg(QString::number(sameFiles.size())).arg(sameFiles.join(", "));
      } else {
        str = STR_NOT_FOUND;
      }

      QMetaObject::invokeMethod(this, "setItemTextInTable", Qt::QueuedConnection, Q_ARG(QTreeWidgetItem*, listOfItems[m]), Q_ARG(int, 1), Q_ARG(QString, str));
//      QMetaObject::invokeMethod(ui->progressBar, "setValue", Qt::QueuedConnection, Q_ARG(int, itemsProcessed));
//      itemsProcessed++;
    } // if (!qfi.isDir())
  }
}

void MainWindow::startWorking()
{
  QMetaObject::invokeMethod(ui->progressBar, "setRange", Qt::QueuedConnection, Q_ARG(int, 0), Q_ARG(int, m_itemsCount));

  QList<QTreeWidgetItem*> filesList;
  QTreeWidgetItemIterator treeIt(ui->treeWidget);
  int coresCount = QThread::idealThreadCount();
  m_totalFilesCount = 0;

  while (*treeIt) {
    if ((*treeIt)->childCount() == 0) {
      m_totalFilesCount++;
      filesList.push_back(*treeIt);
    }
    ++treeIt;
  }

  int chunkSize = ceil(double(filesList.length()) / double(coresCount));
  QList< QList<QTreeWidgetItem*> > splitFilesList;
  QList< QFuture<void> > futures;
  for (int i = 0; i < coresCount; i++) {
    splitFilesList.push_back(filesList.mid(i * chunkSize, chunkSize));
  }

  for (int i = 0; i < coresCount; i++) {
    futures.push_back(QtConcurrent::run(this, &MainWindow::findDuplicates, splitFilesList[i]));
  }

  for (int i = 0; i < coresCount; i++) {
    futures[i].waitForFinished();
  }

  QMetaObject::invokeMethod(ui->progressBar, "setValue", Qt::QueuedConnection, Q_ARG(int, m_itemsCount));  // set progressbar to 100%

  // main work is done, go back to main thread
  emit comparingComplete();
}

void MainWindow::colorizeResults()
{
  for (int i = 0; i < ui->treeWidget->topLevelItemCount(); i++) {
    determineTreeItemColor(ui->treeWidget->topLevelItem(i));
  }

  QMessageBox msgBox;
  msgBox.setText(tr("Searched for:\t%1 file(s).\nSuccesfully found:\t%2 file(s).\nCould not find:\t%3 file(s).")
      .arg(m_totalFilesCount).arg(m_totalFilesCount - m_notFoundCount).arg(m_notFoundCount));
  msgBox.setStandardButtons(QMessageBox::Ok);
  msgBox.exec();
}

// if item is a file, decision is based on right column
// else we call same func recursively for all our children.
bool MainWindow::determineTreeItemColor(QTreeWidgetItem* item)
{
  bool amIGreen = false;
  if (item->childCount()) {
    bool allContentsGreen = true;
    for (int i = 0; i < item->childCount(); i++) {
      allContentsGreen = determineTreeItemColor(item->child(i)) && allContentsGreen;
    }
    amIGreen = allContentsGreen;
  } else {
    if (item->text(1).compare(STR_NOT_FOUND)) {
      amIGreen = true;
    }
  }

  if (amIGreen) {
    item->setBackgroundColor(0, m_foundColor);
  } else {
    item->setBackgroundColor(0, m_notFoundColor);
  }
  return amIGreen;
}

void MainWindow::setItemTextInTable(QTreeWidgetItem* item, int col, QString str)
{
  item->setText(col, str);
}
