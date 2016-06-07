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

private:
  void addItems(QDir a_dir, QTreeWidgetItem* a_parent);
  QByteArray getFileHash(QString fileName);
  bool compareFiles(QString f1, QString f2);

  Ui::MainWindow *ui;
  QStringList m_fileNames;
  QString m_folder;
};

#endif // MAINWINDOW_H
