#ifndef DUPLICATESFINDER_H
#define DUPLICATESFINDER_H

#include <QObject>
#include <QStandardItemModel>
#include <QDir>

class QModelItem;


class DuplicatesFinder : public QObject
{
  friend class DuplicatesFinderTest;
  Q_OBJECT
public:
  explicit DuplicatesFinder(QObject *parent = 0);

  void addSourceFiles(QString sDirectory);
  void startWorking();

  QStandardItemModel m_model;
  QString m_folderToBeSearched;
  int m_filesCount;
  bool m_cancelled;

private:
  void addFilesToTheModel(QDir directory, QStandardItem* parent);
  int findDuplicates(QList<QPersistentModelIndex> listOfItems, int index);
  QByteArray getFileHash(QFile &file);
  bool compareFiles(QString f1, QString f2, QByteArray &hash1);

  QList<QPersistentModelIndex> convertModelToAList();
  void convertModelToAListRecursive(const QModelIndex &top, QList<QPersistentModelIndex> &list);
  void colorizeDirectories();
  int colorizeDirectoriesRecursive(const QModelIndex &top);

  const QString STR_NOT_FOUND;
  QColor m_foundColor, m_notFoundColor;

signals:
  void reportProgress(int id, int progress, int max);
  void comparingComplete(int files, int notFoundFiles);

public slots:
};

#endif // DUPLICATESFINDER_H
