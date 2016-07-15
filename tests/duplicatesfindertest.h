#ifndef DUPLICATESFINDERTEST_H
#define DUPLICATESFINDERTEST_H

#include <QtTest/QtTest>

class DuplicatesFinderTest : public QObject
{
  Q_OBJECT
public:
  explicit DuplicatesFinderTest(QObject *parent = 0);

signals:

private slots:

//  void benchmarkFileSearch_data();
//  void benchmarkFileSearch();

//  void benchmarkRecursive();
//  void benchmarkRegexp();

  void benchmarkAll();

};

#endif // DUPLICATESFINDERTEST_H
