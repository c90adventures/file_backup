#include <QFileDialog>
#include <QTableWidget>
#include <QDebug>
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

void MainWindow::on_pushButton_clicked()
{
  QStringList fileNames = QFileDialog::getOpenFileNames(this,
      tr("Open files"), "/home/mapn");


  m_fileNames = fileNames;
  ui->le_sourceFiles->setText(fileNames.join(", "));
}

void MainWindow::on_pushButton_2_clicked()
{
  QString dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
                                               "/home/mapn",
                                               QFileDialog::ShowDirsOnly
                                               | QFileDialog::DontResolveSymlinks);


  ui->le_folder->setText(dir);
  m_folder = dir;
}

void MainWindow::on_pushButton_3_clicked()
{
  TableWindow* window = new TableWindow();
  window->show();

  qDebug() << m_fileNames.size() << "<= size";

  window->setFileNames(m_fileNames);
  window->setFolder(m_folder);


}
