#ifndef MAINWINDOWTEST_H
#define MAINWINDOWTEST_H

#include <QtTest/QtTest>

#include "mainwindow.h"

class MainWindowTest : public QObject
{
  Q_OBJECT
public:
  explicit MainWindowTest(QObject *parent , MainWindow *am);
  MainWindow *m;
signals:

private slots:
  void initTestCase();
  void theBenchmark();
  void cleanupTestCase();
};

#endif // MAINWINDOWTEST_H
