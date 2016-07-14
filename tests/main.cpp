#include <QtTest/QtTest>
#include <iostream>
#include "mainwindowtest.h"
#include "duplicatesfindertest.h"

int main(int argc, char *argv[])
{
  Q_UNUSED(argc)
  Q_UNUSED(argv)

  QList<QObject*> tests{new MainWindowTest(),
                        new DuplicatesFinderTest()};

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
  return 0;
}
