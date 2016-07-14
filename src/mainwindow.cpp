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
#include "duplicatesfinder.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::MainWindow),
  STR_PROGRAM_NAME("Ed's duplicate files finder")
{
  m_duplicatesFinder = new DuplicatesFinder(this);

  ui->setupUi(this);
  ui->treeView->setModel(&m_duplicatesFinder->m_model);
//  qRegisterMetaType<QTreeWidgetItem*>("QTreeWidgetItem*");
  qRegisterMetaType<QVector<int> >("QVector<int>");
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
  m_duplicatesFinder->addSourceFiles(sDir);
}

void MainWindow::on_pb_setFolder_clicked()
{
  QString dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
                                               "",
                                               QFileDialog::ShowDirsOnly
                                               | QFileDialog::DontResolveSymlinks);


  ui->le_folder->setText(dir);
  m_duplicatesFinder->m_folderToBeSearched = dir;
}

void MainWindow::on_pbGo_clicked()
{
  ui->progressBar->setMaximum(m_duplicatesFinder->m_filesCount);
  connect(m_duplicatesFinder, SIGNAL(reportProgress(int,int,int)), this, SLOT(updateProgress(int,int,int)));
  m_processedFilesPerThread.resize(QThread::idealThreadCount());
  QtConcurrent::run(m_duplicatesFinder, &DuplicatesFinder::startWorking);
}

void MainWindow::updateProgress(int id, int progress, int max)
{
  m_processedFilesPerThread[id] = progress;
  int sum = 0;
  for (int i = 0; i < m_processedFilesPerThread.size(); ++i) {
    sum += m_processedFilesPerThread[i];
  }
  ui->progressBar->setValue(sum);
}

void MainWindow::colorizeResults()
{
//  for (int i = 0; i < ui->treeWidget->topLevelItemCount(); i++) {
//    determineTreeItemColor(ui->treeWidget->topLevelItem(i));
//  }

//  QMessageBox msgBox;
//  msgBox.setText(tr("Searched for:\t%1 file(s).\nSuccesfully found:\t%2 file(s).\nCould not find:\t%3 file(s).")
//      .arg(m_totalFilesCount).arg(m_totalFilesCount - m_notFoundCount).arg(m_notFoundCount));
//  msgBox.setStandardButtons(QMessageBox::Ok);
//  msgBox.exec();
}

// if item is a file, decision is based on right column
// else we call same func recursively for all our children.
bool MainWindow::determineTreeItemColor(QTreeWidgetItem* item)
{
//  bool amIGreen = false;
//  if (item->childCount()) {
//    bool allContentsGreen = true;
//    for (int i = 0; i < item->childCount(); i++) {
//      allContentsGreen = determineTreeItemColor(item->child(i)) && allContentsGreen;
//    }
//    amIGreen = allContentsGreen;
//  } else {
//    if (item->text(1).compare(STR_NOT_FOUND)) {
//      amIGreen = true;
//    }
//  }

//  if (amIGreen) {
//    item->setBackgroundColor(0, m_foundColor);
//  } else {
//    item->setBackgroundColor(0, m_notFoundColor);
//  }
//  return amIGreen;
}

void MainWindow::setItemTextInTable(QTreeWidgetItem* item, int col, QString str)
{
//  item->setText(col, str);
}
