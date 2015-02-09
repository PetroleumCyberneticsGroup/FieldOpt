#include "testvariable.h"
#include <QDebug>

void TestVariable::initTestCase()
{
    intVariable = new IntVariable;
    realVariable = new RealVariable;
    binaryVariable = new BinaryVariable;
    intVariable2 = new IntVariable;
    realVariable2 = new RealVariable;
    binaryVariable2 = new BinaryVariable;
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

void TestVariable::testIntegerBounds()
{
    intVariable2->setMax(30);
    intVariable2->setMin(10);
    intVariable2->setValue(28);
    QVERIFY(intVariable2->value() == 28);

    intVariable2->setValue(31);
    QVERIFY(intVariable2->value() == 31);
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

void TestVariable::testRealBounds()
{
    realVariable2->setMax(200.0);
    realVariable2->setMin(1.0);
    realVariable2->setValue(100.0);
    QVERIFY(realVariable2->value() == 100.0);

    realVariable2->setValue(360.0);
    QVERIFY(realVariable2->value() == 360.0);
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

void TestVariable::testBinaryBounds()
{
    binaryVariable2->setValue(0.0);
    QVERIFY(binaryVariable2->value() == 0.0);

    binaryVariable2->setValue(-0.1);
    QVERIFY(binaryVariable2->value() == -0.1);

    binaryVariable2->setValue(1.1);
    QVERIFY(binaryVariable2->value() == 1.1);
}

void TestVariable::cleanupTestCase()
{
}
