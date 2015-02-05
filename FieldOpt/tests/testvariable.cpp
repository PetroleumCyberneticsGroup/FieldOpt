#include "testvariable.h"
#include <QDebug>

void TestVariable::initTestCase()
{
    intVariable = new IntVariable;
    realVariable = new RealVariable;
    binaryVariable = new BinaryVariable;
}

void TestVariable::testInteger()
{
    QVERIFY(intVariable->id() == 0);
    QVERIFY(intVariable->name() == "no_name");

    intVariable->setName("integer variable");
    QVERIFY(intVariable->name() == "integer variable");

    intVariable->setValue(24);
    QVERIFY(intVariable->value() == 24);
}

void TestVariable::testReal()
{
    QVERIFY(realVariable->id() == 1);
    QVERIFY(realVariable->name() == "no_name");

    realVariable->setName("real variable");
    QVERIFY(realVariable->name() == "real variable");

    realVariable->setValue(25.0);
    QVERIFY(realVariable->value() == 25.0);
}

void TestVariable::testBinary()
{
    QVERIFY(binaryVariable->id() == 2);
    QVERIFY(binaryVariable->name() == "no_name");

    binaryVariable->setName("binary variable");
    QVERIFY(binaryVariable->name() == "binary variable");

    binaryVariable->setValue(0.0);
    QVERIFY(binaryVariable->value() == 0.0);
}

void TestVariable::cleanupTestCase()
{
}
