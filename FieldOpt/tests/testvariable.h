#ifndef TESTVARIABLE_H
#define TESTVARIABLE_H

#include "autotest.h"
#include "variables/intvariable.h"
#include "variables/realvariable.h"
#include "variables/binaryvariable.h"

class TestVariable : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void testInteger();
    void testIntegerBounds();
    void testReal();
    void testRealBounds();
    void testBinary();
    void testBinaryBounds();
    void cleanupTestCase();

private:
    IntVariable* intVariable;
    IntVariable* intVariable2;
    RealVariable* realVariable;
    RealVariable* realVariable2;
    BinaryVariable* binaryVariable;
    BinaryVariable* binaryVariable2;
};

DECLARE_TEST(TestVariable)

#endif // TESTVARIABLE_H
