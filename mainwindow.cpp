#include <QFileDialog>
#include <QTableWidget>
#include <QDebug>
#include <QCryptographicHash>
#include <QDirIterator>
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "tablewindow.h"

MainWindow::MainWindow(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::MainWindow)
{
  ui->setupUi(this);
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
  addItems(dir, NULL) ;
}

void MainWindow::addItems(QDir a_dir, QTreeWidgetItem* a_parent)
{
  a_dir.setFilter(QDir::Files | QDir::NoSymLinks | QDir::Dirs | QDir::NoDotAndDotDot);
  a_dir.setSorting(QDir::Type);
  QFileInfoList list = a_dir.entryInfoList();

  for (int i = 0; i < list.size(); ++i)
  {
    QFileInfo file = list.at(i);
//    qDebug() << qPrintable( QString("%1 %2").arg(file.size(), 10).arg(file.fileName()) );

    QTreeWidgetItem* child = NULL;
    if (a_parent == NULL)
    {
      child = new QTreeWidgetItem(ui->treeWidget);
      child->setText(0, file.absoluteFilePath());
      ui->treeWidget->addTopLevelItem(child);
    }
    else
    {
      child = new QTreeWidgetItem();
      child->setText(0, file.absoluteFilePath());
      a_parent->addChild(child);
    }

    if (true == file.isDir())
    {
      a_dir.cd(file.baseName());
      addItems(a_dir, child) ;
      a_dir.cdUp();
    }
  }
}

void MainWindow::on_pb_setFolder_clicked()
{
  QString dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
                                               "/home/mapn",
                                               QFileDialog::ShowDirsOnly
                                               | QFileDialog::DontResolveSymlinks);


  ui->le_folder->setText(dir);
  m_folder = dir;
}

QByteArray MainWindow::getFileHash(QString fileName)
{
  QCryptographicHash hash(QCryptographicHash::Sha1);

  QFile file(fileName);
  if (file.open(QIODevice::ReadOnly)) {
    hash.addData(file.readAll());
  } else {
    qWarning() << "File couldn't be opened. " << fileName;
  }
  return hash.result();
}

bool MainWindow::compareFiles(QString f1, QString f2)
{
  QByteArray hash1 = getFileHash(f1), hash2 = getFileHash(f2);
  return hash1 == hash2;
}

void MainWindow::on_pbGo_clicked()
{
  //foreach entry in table, search for it in the folder
  QTreeWidgetItemIterator treeIt(ui->treeWidget);
  while (*treeIt) {
    //if ((*treeIt)->text(1) == "") {
    QFileInfo qfi((*treeIt)->text(0));
    if (qfi.isDir()) {
      // (*treeIt)->setText(1, "It's a folder.");
    } else {
      QStringList foundFiles;

      QDirIterator dirIt(m_folder, QStringList(qfi.fileName()), QDir::Files, QDirIterator::Subdirectories);
      while (dirIt.hasNext()) {
        if (dirIt.filePath() == "")
          break;
        foundFiles << dirIt.filePath();
      }

      QStringList sameFiles;
      for (int i = 0; i < foundFiles.size(); i++) {
        if (compareFiles((*treeIt)->text(0), foundFiles[i])) {
          sameFiles << foundFiles[i];
        }
      }

      // what to do if multiple files have been found?
      QString str;
      if (sameFiles.size()) {
        str = tr("(%1) %2").arg(QString::number(sameFiles.size())).arg(sameFiles.join(", "));
      } else {
        str = "Not found.";
      }

      (*treeIt)->setText(1, str);
    } // else
  ++treeIt;
  }
}
