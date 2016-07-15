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

  qRegisterMetaType<QVector<int> >("QVector<int>");
  qRegisterMetaType<QList<QPersistentModelIndex> >("QList<QPersistentModelIndex>");

  setWindowTitle(tr("%1, build from %2, %3").arg(STR_PROGRAM_NAME).arg(QString::fromLocal8Bit(__DATE__)).arg(QString::fromLocal8Bit(__TIME__)));

  ui->pbCancel->setEnabled(false);

  connect(m_duplicatesFinder, SIGNAL(comparingComplete(int, int)), this, SLOT(showResults(int, int)));

  this->setContextMenuPolicy(Qt::CustomContextMenu);

  connect(this, SIGNAL(customContextMenuRequested(const QPoint &)),
          this, SLOT(showContextMenu(const QPoint &)));
}

MainWindow::~MainWindow()
{
  m_duplicatesFinder->m_cancelled = true;
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
  if (m_duplicatesFinder->m_filesCount) {
    ui->progressBar->setMaximum(m_duplicatesFinder->m_filesCount);
    connect(m_duplicatesFinder, SIGNAL(reportProgress(int,int,int)), this, SLOT(updateProgress(int,int,int)));
    m_processedFilesPerThread.resize(QThread::idealThreadCount());
    QtConcurrent::run(m_duplicatesFinder, &DuplicatesFinder::startWorking);

    ui->pbGo->setEnabled(false);
    ui->pbCancel->setEnabled(true);
    statusBar()->showMessage(tr("Working in %1 thread(s)...").arg(QThread::idealThreadCount()));
  } else {
    statusBar()->showMessage(tr("Add some source files first!"));
  }
}

void MainWindow::updateProgress(int id, int progress, int max)
{
  Q_UNUSED(max);
  m_processedFilesPerThread[id] = progress;
  int sum = 0;
  for (int i = 0; i < m_processedFilesPerThread.size(); ++i) {
    sum += m_processedFilesPerThread[i];
  }
  ui->progressBar->setValue(sum);
}

void MainWindow::showResults(int totalFiles, int notFoundFiles)
{
  if (totalFiles != -1) {
    QMessageBox msgBox;
    msgBox.setText(tr("Searched for:\t%1 file(s).\nSuccesfully found:\t%2 file(s).\nCould not find:\t%3 file(s).")
        .arg(totalFiles).arg(totalFiles - notFoundFiles).arg(notFoundFiles));
    msgBox.setStandardButtons(QMessageBox::Ok);
    msgBox.exec();
    ui->statusBar->showMessage(tr("Done."));
  } else {
    ui->statusBar->showMessage(tr("Cancelled."));
  }
  ui->pbCancel->setText(tr("Cancel"));
  ui->pbGo->setEnabled(true);
  ui->pbCancel->setEnabled(false);
}

void MainWindow::on_pbCancel_clicked()
{
  ui->pbCancel->setText(tr("Cancelling..."));
  ui->statusBar->showMessage(ui->pbCancel->text());
  ui->pbCancel->setEnabled(false);
  m_duplicatesFinder->m_cancelled = true;
}
