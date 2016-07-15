#include <QDebug>
#include "duplicatesfinder.h"
#include "duplicatesfindertest.h"

DuplicatesFinderTest::DuplicatesFinderTest(QObject *parent) :
  QObject(parent)
  {
  qRegisterMetaType<QVector<int> >("QVector<int>");
  }

//void DuplicatesFinderTest::benchmarkFileSearch_data()
//{
//  QTest::addColumn<bool>("testNewApproach");
//  QTest::newRow("newWay") << true;
//  QTest::newRow("oldWay") << false;
//}

//void DuplicatesFinderTest::benchmarkFileSearch()
//{
//  QString folderToBeSearched = "E:/dev/marched";
//  QString filePath = "E:/dev/marched/src/release/mainwindow.o";
//  QStringList filesList;
//  for (int i = 0; i < 100; i++) {
//    filesList.append(filePath);
//  }

//  QFETCH(bool, testNewApproach);

//  QBENCHMARK {
//    if (testNewApproach) {
//      QMultiHash<QString,QString> filesHashtable;  // filename, filepath
//      QDirIterator dirIt(folderToBeSearched, QDir::Files, QDirIterator::Subdirectories);

//      while (dirIt.hasNext()) {
//        dirIt.next();
//        filesHashtable.insert(dirIt.fileName(), dirIt.filePath());
//      }

//      for (int i = 0; i < filesList.length(); ++i) {
//        QFileInfo qfi(filesList[i]);
//        QStringList foundFiles = filesHashtable.values(qfi.fileName());
//        // now hash-comparing takes place
//      }

//    } else {
//      for (int i = 0; i < filesList.length(); ++i) {
//        QStringList foundFiles;
//        QFileInfo qfi(filesList[i]);
//        QDirIterator dirIt(folderToBeSearched, QStringList(qfi.fileName()),
//                           QDir::Files, QDirIterator::Subdirectories);

//        while (dirIt.hasNext()) {
//          dirIt.next();
//          foundFiles << dirIt.filePath();
//        }
//        // now hash-comparing takes place
//      }
//    }
//  }
//}

void DuplicatesFinderTest::benchmarkAll()
{
  DuplicatesFinder df;
  QBENCHMARK {
    df.addSourceFiles(QDir::currentPath());
    df.m_folderToBeSearched = QDir::homePath();

    df.startWorking();
  }
}

//void DuplicatesFinderTest::benchmarkRecursive()
//{
//  DuplicatesFinder *df = new DuplicatesFinder();
////  df->addSourceFiles(QDir::homePath());
//  df->addSourceFiles(QDir::currentPath());
//  QList<QPersistentModelIndex> filesList;

//  QBENCHMARK {
//      filesList = df->convertModelToAList();
//  }
//}


//void DuplicatesFinderTest::benchmarkRegexp()
//{
//  DuplicatesFinder *df = new DuplicatesFinder();
//  //df->addSourceFiles(QDir::homePath());
//  df->addSourceFiles(QDir::currentPath());

//  QBENCHMARK {
//      df->m_model.findItems(".*", Qt::MatchRecursive | Qt::MatchRegExp);
//  }
//}
