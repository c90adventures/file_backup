#include <QDebug>
#include "duplicatesfinder.h"
#include "duplicatesfindertest.h"

DuplicatesFinderTest::DuplicatesFinderTest(QObject *parent) :
  QObject(parent)
  {
  qRegisterMetaType<QVector<int> >("QVector<int>");
  }

void DuplicatesFinderTest::initTestCase()
  {
  }

void DuplicatesFinderTest::cleanupTestCase()
  {  }

void DuplicatesFinderTest::benchmarkAll()
{
  DuplicatesFinder df;
  QBENCHMARK {
    df.addSourceFiles(QDir::currentPath());
    df.m_folderToBeSearched = QDir::homePath();

    df.startWorking();
  }
}

void DuplicatesFinderTest::benchmarkRecursive()
{
  DuplicatesFinder *df = new DuplicatesFinder();
//  df->addSourceFiles(QDir::homePath());
  df->addSourceFiles(QDir::currentPath());
  QList<QModelIndex> filesList;

  QBENCHMARK {
      df->convertModelToAList(df->m_model.item(0, 0)->index(), filesList);
  }
}


void DuplicatesFinderTest::benchmarkRegexp()
{
  DuplicatesFinder *df = new DuplicatesFinder();
  //df->addSourceFiles(QDir::homePath());
  df->addSourceFiles(QDir::currentPath());

  QBENCHMARK {
      df->m_model.findItems(".*", Qt::MatchRecursive | Qt::MatchRegExp);
  }
}
