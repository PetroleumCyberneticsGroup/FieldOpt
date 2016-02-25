#include "GTest/Simulation/results/test_fixture_adgprs.h"
#include "AdgprsResultsReader/json_summary_reader.h"

namespace {

class JsonSummaryReaderTest : public AdgprsTestFixture {
protected:
    JsonSummaryReaderTest() {
        reader_ = new AdgprsResultsReader::JsonSummaryReader(json_summary_path_);
    }
    virtual ~JsonSummaryReaderTest() {}
    virtual void SetUp() {}
    AdgprsResultsReader::JsonSummaryReader *reader_;
};

TEST_F(JsonSummaryReaderTest, Global) {
    EXPECT_EQ(reader_->GetNumberOfWells(), 5);
    EXPECT_EQ(reader_->GetTimeVector()->size(), 122);
}

TEST_F(JsonSummaryReaderTest, Field) {
    EXPECT_EQ(reader_->GetAvalableFieldPropertyNames().length(), 12);
    foreach (QString key, reader_->GetAvalableFieldPropertyNames()) {
        EXPECT_EQ(reader_->GetFieldProperty(key)->length(), 122);
    }
    EXPECT_ANY_THROW(reader_->GetFieldProperty("NOTAPROP"));
}

TEST_F(JsonSummaryReaderTest, Wells) {
    EXPECT_EQ(reader_->GetAvailableWellPropertyNames().size(), 7);

    EXPECT_TRUE(reader_->IsInjector(0));
    for (int i = 1; i < 5; ++i) {
        EXPECT_FALSE(reader_->IsInjector(i));
    }

    for (int i = 0; i < reader_->GetNumberOfWells(); ++i) {
        EXPECT_EQ(reader_->GetNumberOfPerforations(i), 1);
        foreach (QString key, reader_->GetAvailableWellPropertyNames()) {
            EXPECT_EQ(reader_->GetWellProperty(i, key)->length(), 122);
        }
    }
}

}

