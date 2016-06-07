#ifndef TABLEWINDOW_H
#define TABLEWINDOW_H

#include <QMainWindow>
#include <QTableWidgetItem>

namespace Ui {
  class TableWindow;
}

class TableWindow : public QMainWindow
{
  Q_OBJECT

public:
  explicit TableWindow(QWidget *parent = 0);
  ~TableWindow();

  void setFileNames(QStringList fileNames);
  void setFolder(QString folder);

private slots:
  void on_pushButton_clicked();

private:
  Ui::TableWindow *ui;
  QStringList m_fileNames;
  QString m_folder;
};

#endif // TABLEWINDOW_H
