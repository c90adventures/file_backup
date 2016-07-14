#include <QtTest/QtTest>
#include <iostream>
#include "mainwindow.h"
#include "mainwindowtest.h"

int main(int argc, char *argv[])
{
  QApplication a(argc, argv);
  MainWindow m;

  QList<QObject*> tests{new MainWindowTest(NULL, &m)};

  bool failed = false;

  foreach (QObject* qobj, tests)
    if (QTest::qExec(qobj)) {
      failed = true;
      std::cout << qobj->metaObject()->className() << " failed." << std::endl;
      break;
    }

  if (!failed) {
    std::cout << std::endl << "All tests passed!" << std::endl << std::endl;
  }

  qDeleteAll(tests);
  tests.clear();
  return a.exec();
}
