#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDir>

class QTreeWidgetItem;

namespace Ui {
  class MainWindow;
}

class MainWindow : public QMainWindow
{
  Q_OBJECT

public:
  explicit MainWindow(QWidget *parent = 0);
  ~MainWindow();

private slots:
  void on_pb_setFolder_clicked();
  void on_pb_addFolderContents_clicked();
  void on_pbGo_clicked();
  void setItemTextInTable(QTreeWidgetItem *item, int col, QString str);
  void colorizeResults();

private:
  int addItems(QDir a_dir, QTreeWidgetItem* a_parent);
  QByteArray getFileHash(QFile &file);
  bool compareFiles(QString f1, QString f2, QByteArray &hash1);
  void startWorking();
  bool determineTreeItemColor(QTreeWidgetItem *item);

  Ui::MainWindow *ui;
  QStringList m_fileNames;
  QString m_folder;
  int m_itemsCount;

  const QString STR_NOT_FOUND;

signals:
  void comparingComplete();
};

#endif // MAINWINDOW_H
