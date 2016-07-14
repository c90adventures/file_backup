#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDir>

class QTreeWidgetItem;
class DuplicatesFinder;

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
  void showContextMenu(const QPoint & pos);
  void showAbout();

private:
  int addItems(QDir a_dir, QTreeWidgetItem* a_parent);
  bool determineTreeItemColor(QTreeWidgetItem *item);

  Ui::MainWindow *ui;
  DuplicatesFinder *m_duplicatesFinder;
  QStringList m_fileNames;
  int m_itemsCount, m_notFoundCount, m_totalFilesCount;
  QColor m_foundColor, m_notFoundColor;

  const QString STR_PROGRAM_NAME;

signals:
  void comparingComplete();
};

#endif // MAINWINDOW_H