#ifndef MAINWINDOWTEST_H
#define MAINWINDOWTEST_H

#include <QtTest/QtTest>

class MainWindowTest : public QObject
{
  Q_OBJECT
public:
  explicit MainWindowTest(QObject *parent = 0);

signals:

private slots:
  void initTestCase();
  void cleanupTestCase();
};

#endif // MAINWINDOWTEST_H
