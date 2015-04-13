#include <QString>
#include <QtTest>

class GSSOptimizerTestsTest : public QObject
{
    Q_OBJECT

public:
    GSSOptimizerTestsTest();

private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();
    void testCase1();
};

GSSOptimizerTestsTest::GSSOptimizerTestsTest()
{
}

void GSSOptimizerTestsTest::initTestCase()
{
}

void GSSOptimizerTestsTest::cleanupTestCase()
{
}

void GSSOptimizerTestsTest::testCase1()
{
    QVERIFY2(true, "Failure");
}

QTEST_APPLESS_MAIN(GSSOptimizerTestsTest)

#include "tst_gssoptimizerteststest.moc"
