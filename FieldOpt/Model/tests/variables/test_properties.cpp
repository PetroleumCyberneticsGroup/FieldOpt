#include <gtest/gtest.h>
#include <QtCore/QStringList>
#include "Reservoir/grid/xyzcoordinate.h"
#include "Model/properties/property.h"
#include "Model/properties/binary_property.h"
#include "Model/properties/discrete_property.h"
#include "Model/properties/continous_property.h"

using namespace Model::Properties;

namespace {

    class PropertiesTest : public ::testing::Test {
    protected:
        PropertiesTest () { }
        virtual ~PropertiesTest () { }
    };

    TEST_F(PropertiesTest , BinaryPropertyConstructor) {
        BinaryProperty *binvar = new BinaryProperty(true);
        EXPECT_TRUE(binvar->value());
        EXPECT_FALSE(binvar->IsLocked());
        EXPECT_TRUE(binvar->type() == Property::Type::Binary);
    }

    TEST_F(PropertiesTest, DiscretePropertyConstructor) {
        DiscreteProperty *intvar = new DiscreteProperty(4);
        EXPECT_TRUE(intvar->value() == 4);
        EXPECT_FALSE(intvar->IsLocked());
        EXPECT_TRUE(intvar->type() == Property::Type::Discrete);
    }

    TEST_F(PropertiesTest, ContinousPropertyConstructor) {
        ContinousProperty *realvar = new ContinousProperty(2.0);
        EXPECT_TRUE(realvar->value() == 2.0);
        EXPECT_FALSE(realvar->IsLocked());
        EXPECT_TRUE(realvar->type() == Property::Type::Continous);
    }

    TEST_F(PropertiesTest, Locking) {
        BinaryProperty *binvar = new BinaryProperty(true);
        DiscreteProperty *intvar = new DiscreteProperty(4);
        ContinousProperty *realvar = new ContinousProperty(2.0);

        // Lock and check
        binvar->Lock(); intvar->Lock(); realvar->Lock();
        EXPECT_TRUE(binvar->IsLocked());
        EXPECT_TRUE(intvar->IsLocked());
        EXPECT_TRUE(realvar->IsLocked());

        // Attempt modifications;
        EXPECT_THROW(binvar->setValue(false), PropertyLockedException);
        EXPECT_THROW(intvar->Add(1), PropertyLockedException);
        EXPECT_THROW(intvar->setValue(1), PropertyLockedException);
        EXPECT_THROW(realvar->Add(2.0), PropertyLockedException);
        EXPECT_THROW(realvar->setValue(0.0), PropertyLockedException);

        // Verify values not changed
        EXPECT_TRUE(binvar->value());
        EXPECT_TRUE(intvar->value() == 4);
        EXPECT_TRUE(realvar->value() == 2.0);
    }

    TEST_F(PropertiesTest, DiscretePropertyAddition) {
        DiscreteProperty *intvar = new DiscreteProperty(5);
        intvar->Add(2);
        EXPECT_TRUE(intvar->value() == 7);
    }

    TEST_F(PropertiesTest, ContinousPropertyAddition) {
        ContinousProperty *realvar = new ContinousProperty(7.0);
        realvar->Add(-2.5);
        EXPECT_TRUE(realvar->value() == 4.5);
    }

    TEST_F(PropertiesTest, DiscretePropertyEquals) {
        DiscreteProperty *intvar_1 = new DiscreteProperty(1);
        DiscreteProperty *intvar_2 = new DiscreteProperty(1);
        DiscreteProperty *intvar_3 = new DiscreteProperty(2);
        EXPECT_TRUE(intvar_1->Equals(intvar_2));
        EXPECT_FALSE(intvar_1->Equals(intvar_3));
    }

    TEST_F(PropertiesTest, ContinousPropertyEquals) {
        ContinousProperty *realvar_1 = new ContinousProperty(2.0);
        ContinousProperty *realvar_2 = new ContinousProperty(2.0);
        ContinousProperty *realvar_3 = new ContinousProperty(3.0);
        ContinousProperty *realvar_4 = new ContinousProperty(2.1);
        EXPECT_TRUE(realvar_1->Equals(realvar_2));
        EXPECT_FALSE(realvar_1->Equals(realvar_3));
        EXPECT_FALSE(realvar_1->Equals(realvar_4, 0.09)); // Should be outside epsilon
        EXPECT_TRUE(realvar_1->Equals(realvar_4, 0.11)); // Should be inside epsilon
    }


    TEST_F(PropertiesTest, VariableProperties) {
        QStringList pnames;
        pnames << "SplinePoint#PROD#heel#x" << "SplinePoint#PROD#toe#z"
               << "WellBlock#PROD#4#i" << "WellBlock#PROD#2#j"
               << "Transmissibility#PROD#6"
               << "Rate#PROD#1" << "BHP#PROD#50";

        ContinousProperty sp_h_x = ContinousProperty(1.0);
        ContinousProperty sp_t_z = ContinousProperty(1.0);
        DiscreteProperty wb_4_x = DiscreteProperty(1);
        DiscreteProperty wb_2_y = DiscreteProperty(1);
        ContinousProperty tr_6 = ContinousProperty(2.0);
        ContinousProperty ra_1 = ContinousProperty(1.0);
        ContinousProperty pr_50 = ContinousProperty(1.0);

        EXPECT_FALSE(sp_h_x.isVariable());
        EXPECT_THROW(sp_t_z.propertyInfo(), std::runtime_error);
        EXPECT_THROW(wb_4_x.SetVariable(), std::runtime_error);

        sp_h_x.setName(pnames[0]);
        sp_t_z.setName(pnames[1]);
        wb_4_x.setName(pnames[2]);
        wb_2_y.setName(pnames[3]);
        tr_6.setName(pnames[4]);
        ra_1.setName(pnames[5]);
        pr_50.setName(pnames[6]);

        sp_h_x.SetVariable();
        sp_t_z.SetVariable();
        wb_4_x.SetVariable();
        wb_2_y.SetVariable();
        tr_6.SetVariable();
        ra_1.SetVariable();
        pr_50.SetVariable();

        EXPECT_EQ(Property::PropertyType::SplinePoint, sp_h_x.propertyInfo().prop_type);
        EXPECT_STREQ("PROD", sp_h_x.propertyInfo().parent_well_name.toLatin1().constData());
        EXPECT_EQ(Property::SplineEnd::Heel, sp_h_x.propertyInfo().spline_end);
        EXPECT_EQ(Property::Coordinate::x, sp_h_x.propertyInfo().coord);
        EXPECT_EQ(Property::SplineEnd::Toe, sp_t_z.propertyInfo().spline_end);
        EXPECT_EQ(Property::Coordinate::z, sp_t_z.propertyInfo().coord);
        EXPECT_EQ(Property::PropertyType::WellBlock, wb_4_x.propertyInfo().prop_type);
        EXPECT_EQ(4, wb_4_x.propertyInfo().index);
        EXPECT_EQ(Property::Coordinate::i, wb_4_x.propertyInfo().coord);
        EXPECT_EQ(Property::Coordinate::j, wb_2_y.propertyInfo().coord);
        EXPECT_EQ(Property::PropertyType::Transmissibility, tr_6.propertyInfo().prop_type);
        EXPECT_EQ(Property::PropertyType::BHP, pr_50.propertyInfo().prop_type);
        EXPECT_EQ(50, pr_50.propertyInfo().index);
        EXPECT_EQ(Property::PropertyType::Rate, ra_1.propertyInfo().prop_type);
    }
}
