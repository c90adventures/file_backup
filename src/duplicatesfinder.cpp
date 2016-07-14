#include <QDir>
#include <QFuture>
#include <QtConcurrent/QtConcurrent>
#include <QDebug>
#include "duplicatesfinder.h"

DuplicatesFinder::DuplicatesFinder(QObject *parent)
  : QObject(parent)
  , m_filesCount(0)
  , STR_NOT_FOUND("Not found.")
{
  m_model.setColumnCount(2);
  m_model.setHorizontalHeaderItem(0, new QStandardItem(tr("Source file")));
  m_model.setHorizontalHeaderItem(1, new QStandardItem(tr("Corresponding file")));

  QSettings settings("./marched.ini", QSettings::IniFormat);
  m_foundColor = QColor(settings.value("foundColor", "#449333").toString());
  m_notFoundColor = QColor(settings.value("notFoundColor", "#FD0600").toString());

}

void DuplicatesFinder::addSourceFiles(QString sDirectory)
{
  QDir directory(sDirectory);
  directory.setFilter(QDir::Files | QDir::NoSymLinks | QDir::Dirs | QDir::NoDotAndDotDot);
  directory.setSorting(QDir::DirsLast);
  addFilesToTheModel(directory, m_model.invisibleRootItem());
}


void DuplicatesFinder::addFilesToTheModel(QDir directory, QStandardItem* parent)
{
  QFileInfoList list = directory.entryInfoList();

  for (int i = 0; i < list.size(); ++i) {
    QFileInfo file = list.at(i);
    QStandardItem *newItem = new QStandardItem(file.absoluteFilePath());
    parent->appendRow(newItem);

    if (true == file.isDir()) {
      if (false == directory.cd(file.fileName())) {
        qCritical() << "Failed to open " << file.fileName();
      } else {
        addFilesToTheModel(directory, newItem) ;
        directory.cdUp();
      }
    } else {
      m_filesCount++;
    }
  }
}


// generates a list of all files (not folders) in the model
QList<QModelIndex> DuplicatesFinder::convertModelToAList()
{
  qDebug() << Q_FUNC_INFO << m_model.rowCount(m_model.item(0, 0)->index());

  QList<QModelIndex> retList;
  for (int i = 0; i < m_model.rowCount(); i++) {
    QList<QModelIndex> filesList;
    convertModelToAListRecursive(m_model.item(i, 0)->index(), filesList);
    retList.append(filesList);
  }
  return retList;
}

// recursively scans tree and populates list with files only
void DuplicatesFinder::convertModelToAListRecursive(const QModelIndex &top, QList<QModelIndex> &list)
{
  if (!top.isValid() || top.data().isNull()) {
    return;
  }

  // m_model.rowCount(top)  -> this gives number of children of "top";
  if (m_model.rowCount(top)) {
    for (int r = 0; r < m_model.rowCount(top); r++) {
      convertModelToAListRecursive(m_model.index(r, 0, top), list);
    }
  } else {
    list.push_back(top);
  }
}


void DuplicatesFinder::colorizeDirectories()
{
  for (int i = 0; i < m_model.rowCount(); i++) {
    int notFoundCounter = colorizeDirectoriesRecursive(m_model.item(i, 0)->index());

    if (notFoundCounter > 0 && m_model.rowCount(m_model.item(i, 0)->index())) {
      m_model.setData(m_model.item(i, 0)->index(), m_notFoundColor, Qt::BackgroundRole);
      m_model.setData(m_model.item(i, 1)->index(), tr("%1 file(s) missing").arg(notFoundCounter));
    } else if (m_model.rowCount(m_model.item(i, 0)->index())) {
      m_model.setData(m_model.item(i, 0)->index(), m_foundColor, Qt::BackgroundRole);
    }
  }
}

// recursive
int DuplicatesFinder::colorizeDirectoriesRecursive(const QModelIndex &top)
{
  if (!top.isValid() || top.data().isNull()) {
    return 0;
  }

  int howManyChildrenNotFound = 0;

  if (m_model.rowCount(top)) {
    for (int r = 0; r < m_model.rowCount(top); r++) {
      howManyChildrenNotFound += colorizeDirectoriesRecursive(m_model.index(r, 0, top));  // this handles insides of current dir
    }
    // decide on color here
    if (howManyChildrenNotFound) {
      m_model.setData(top, m_notFoundColor, Qt::BackgroundRole);
      m_model.setData(top.sibling(top.row(), top.column()+1), tr("%1 file(s) missing").arg(howManyChildrenNotFound));
    } else {
      m_model.setData(top, m_foundColor, Qt::BackgroundRole);
    }
  } else {
    if (top.sibling(top.row(), top.column() + 1).data().toString().compare(STR_NOT_FOUND) == 0) {
      howManyChildrenNotFound++;
    }
  }
  return howManyChildrenNotFound;
}

void DuplicatesFinder::startWorking()
{
  QList<QModelIndex> filesList = convertModelToAList();
  int coresCount = QThread::idealThreadCount();

  int chunkSize = ceil(double(filesList.length()) / double(coresCount));
  QList< QList<QModelIndex> > splitFilesList;
  QList< QFuture<int> > futures;
  for (int i = 0; i < coresCount; i++) {
    splitFilesList.push_back(filesList.mid(i * chunkSize, chunkSize));
  }

  for (int i = 0; i < coresCount; i++) {
    futures.push_back(QtConcurrent::run(this, &DuplicatesFinder::findDuplicates, splitFilesList[i], i));
  }

  int notFoundCount = 0;
  for (int i = 0; i < coresCount; i++) {
    futures[i].waitForFinished();
    notFoundCount += futures[i].result();
  }

  // now all the files are properly colord - but not directories!
  colorizeDirectories();


  emit comparingComplete(m_filesCount, notFoundCount);
}

// returns number of files that were not found
int DuplicatesFinder::findDuplicates(QList<QModelIndex> listOfItems, int index)
{
  int filesProcessed = 0;
  int notFoundCounter = 0;

  for (int m = 0; m < listOfItems.length(); m++) {
    QFileInfo qfi(listOfItems[m].data().toString());
    if (!qfi.isDir()) {
      QStringList foundFiles;
      QDirIterator dirIt(m_folderToBeSearched, QStringList(qfi.fileName()), QDir::Files, QDirIterator::Subdirectories);

      //QMetaObject::invokeMethod(statusBar(), "showMessage", Qt::QueuedConnection, Q_ARG(QString, tr("Searching for %1...").arg(qfi.fileName())));

      while (dirIt.hasNext()) {
        dirIt.next();
        foundFiles << dirIt.filePath();
      }

      QStringList sameFiles;
      QByteArray hashOfSourceFile;
      for (int i = 0; i < foundFiles.size(); i++) {
        if (compareFiles(qfi.absoluteFilePath(), foundFiles[i], hashOfSourceFile)) {
          sameFiles << foundFiles[i];
        }
      }

      // what to do if multiple files have been found:
      QString str;
      if (sameFiles.size()) {
        str = tr("(%1) %2").arg(QString::number(sameFiles.size())).arg(sameFiles.join(", "));
        m_model.setData(listOfItems[m], m_foundColor, Qt::BackgroundRole);
      } else {
        str = STR_NOT_FOUND;
        notFoundCounter++;
        m_model.setData(listOfItems[m], m_notFoundColor, Qt::BackgroundRole);
      }

      QModelIndex secondColumn = listOfItems[m].sibling(listOfItems[m].row(), listOfItems[m].column() + 1);
      m_model.setData(secondColumn, str);

      emit reportProgress(index, ++filesProcessed, listOfItems.length());
    } // if (!qfi.isDir())
  }
  return notFoundCounter;
}

QByteArray DuplicatesFinder::getFileHash(QFile &file)
{
  //QMetaObject::invokeMethod(statusBar(), "showMessage", Qt::QueuedConnection, Q_ARG(QString, tr("Getting hash for %1...").arg(file.fileName())));
  QCryptographicHash hash(QCryptographicHash::Sha1);

  int chunkSize = 8192;

  if (file.open(QIODevice::ReadOnly)) {
    while (!file.atEnd()) {
      hash.addData(file.read(chunkSize));
    }
  } else {
    qWarning() << "File couldn't be opened. " << file.fileName();
  }
  file.close();
  return hash.result();
}

// @args if hash1 is not empty, it will be assumed that it is a hash for file1
//       if it is empty, it will be filled, so that it can be reZused in the future
bool DuplicatesFinder::compareFiles(QString f1, QString f2, QByteArray &hash1)
{
  QFile file1(f1), file2(f2);
  if (file1.size() == file2.size())
  {
    hash1 = hash1.size() ? hash1 : getFileHash(file1);
    QByteArray hash2 = getFileHash(file2);
    return hash1 == hash2;
  } else {
    // qDebug() << "size mismatch" << file1.fileName() << " " << file2.fileName();
  }
  return false;
}
