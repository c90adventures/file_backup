#include "mainwindowtest.h"
#include "mainwindow.h"

MainWindowTest::MainWindowTest(QObject *parent, MainWindow *am) :
  QObject(parent), m(am)
  {
  }

void MainWindowTest::initTestCase()
  {
  }

void MainWindowTest::cleanupTestCase()
  {  }

void MainWindowTest::theBenchmark()
{
  QSignalSpy spy(m, SIGNAL(comparingComplete()));
  QBENCHMARK {
    m->addItems(QDir::currentPath(), NULL);
    m->m_folder = QDir::homePath();

    m->on_pbGo_clicked();
    spy.wait();
  }
}
