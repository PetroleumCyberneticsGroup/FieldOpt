#include <gtest/gtest.h>
#include "test_resource_cases.h"
#include <Optimization/case_transfer_object.h>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/lexical_cast.hpp>
#include <sstream>

namespace {
    using namespace Optimization;
    using namespace boost::archive;
    class CaseTransferObjectTest : public ::testing::Test, public TestResources::TestResourceCases {
    protected:
        CaseTransferObjectTest() {}
        virtual void SetUp() {}
        virtual void TearDown() {}

        boost::uuids::uuid qUuidToBoostUuid(const QUuid quuid) const {
            boost::uuids::uuid uuid(string_generator()(quuid.toString().toStdString()));
            return uuid;
        }
    };

    TEST_F(CaseTransferObjectTest, BaseConstructor) {
        CaseTransferObject cto();
    }


    TEST_F(CaseTransferObjectTest, CaseConstructor) {
        CaseTransferObject cto(test_case_3_4b3i3r_);
        EXPECT_STREQ(    test_case_3_4b3i3r_->id_stdstr().c_str(),        cto.id_stdstr().c_str());
        EXPECT_FLOAT_EQ( test_case_3_4b3i3r_->objective_function_value(), cto.objective_function_value());
        EXPECT_EQ(       test_case_3_4b3i3r_->binary_variables().size(),  cto.binary_variables().size());
        EXPECT_EQ(       test_case_3_4b3i3r_->integer_variables().size(), cto.integer_variables().size());
        EXPECT_EQ(       test_case_3_4b3i3r_->real_variables().size(),    cto.real_variables().size());
        EXPECT_EQ(       test_case_3_4b3i3r_->GetWICTime(),               cto.wic_time_secs());
    }

    TEST_F(CaseTransferObjectTest, SerializationAndDeserializationAndGeneratedCase) {
        auto cto1 = CaseTransferObject(test_case_3_4b3i3r_); // create a populated cto

        // Serialize
        std::stringstream stream; // create a stream
        binary_oarchive oa(stream); // create an archive using the stream
        oa << cto1; // store the cto in the archive

        // Deserialize
        auto cto2 = CaseTransferObject(); // create an empty cto
        binary_iarchive ia(stream); // create an archive using the stream
        ia >> cto2; // deserialize the archive into cto2

        // Regenerate case object
        auto c = cto2.CreateCase();

        // Check that the generated case matches with the original test_case_3
        EXPECT_TRUE(test_case_3_4b3i3r_->Equals(c));
        EXPECT_STREQ(    test_case_3_4b3i3r_->id_stdstr().c_str(),        c->id_stdstr().c_str());
        EXPECT_FLOAT_EQ( test_case_3_4b3i3r_->objective_function_value(), c->objective_function_value());
        EXPECT_EQ(       test_case_3_4b3i3r_->binary_variables().size(),  c->binary_variables().size());
        EXPECT_EQ(       test_case_3_4b3i3r_->integer_variables().size(), c->integer_variables().size());
        EXPECT_EQ(       test_case_3_4b3i3r_->real_variables().size(),    c->real_variables().size());
        EXPECT_EQ(       test_case_3_4b3i3r_->GetWICTime(),               c->GetWICTime());

        for (auto id : test_case_3_4b3i3r_->integer_variables().keys())
            EXPECT_EQ(      test_case_3_4b3i3r_->integer_variables()[id], c->integer_variables()[id]);
        for (auto id : test_case_3_4b3i3r_->binary_variables().keys())
            EXPECT_EQ(      test_case_3_4b3i3r_->binary_variables()[id],  c->binary_variables()[id]);
        for (auto id : test_case_3_4b3i3r_->real_variables().keys())
            EXPECT_FLOAT_EQ(test_case_3_4b3i3r_->real_variables()[id],    c->real_variables()[id]);


    }
}
