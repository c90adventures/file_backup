#include <QFileInfo>
#include <QDir>
#include "tablewindow.h"
#include "ui_tablewindow.h"

#define tw ui->tableWidget

TableWindow::TableWindow(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::TableWindow)
{
  ui->setupUi(this);
}

TableWindow::~TableWindow()
{
  delete ui;
}

void TableWindow::on_pushButton_clicked()
{
  for (int i = 0; i < ui->tableWidget->rowCount(); ++i) {
    QFileInfo qfi(ui->tableWidget->item(i, 0)->text());

    QDir directory(m_folder);
    QStringList foundFiles = directory.entryList(QStringList(qfi.fileName()));

    QTableWidgetItem* ptr = tw->takeItem(i, 2);  // just in case
    if (ptr) {
        delete ptr;  // should we? i don't even know
      }

    QString str = tr("(%1) %2").arg(QString::number(foundFiles.size())).arg(foundFiles.join(","));
    tw->setItem(i, 2, new QTableWidgetItem(str));
    //tw->item(i, 2)->setText(foundFiles.join(","));
  }
}

void TableWindow::setFileNames(QStringList fileNames)
{
  ui->tableWidget->setRowCount(fileNames.size());
  for (int i = 0; i < fileNames.size(); ++i) {
    QTableWidgetItem *newItem = new QTableWidgetItem(fileNames[i]);
    ui->tableWidget->setItem(i, 0, newItem);
  }
}



void TableWindow::setFolder(QString folder)
{
  m_folder = folder;
  ui->tableWidget->horizontalHeaderItem(2)->setText(tr("Found in %1").arg(folder));
}
