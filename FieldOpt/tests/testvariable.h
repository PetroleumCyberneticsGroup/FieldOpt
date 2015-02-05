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
    void testReal();
    void testBinary();
    void cleanupTestCase();

private:
    IntVariable* intVariable;
    RealVariable* realVariable;
    BinaryVariable* binaryVariable;
};

DECLARE_TEST(TestVariable)

#endif // TESTVARIABLE_H
