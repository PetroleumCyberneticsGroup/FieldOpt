#include <gtest/gtest.h>
#include "AdgprsResultsReader/adgprs_results_reader.h"
#include "Utilities/tests/test_resource_example_file_paths.h"

namespace {

class AdgprsResultsReaderTest : public ::testing::Test {
protected:
    AdgprsResultsReaderTest() {
        reader_ = new AdgprsResultsReader::AdgprsResultsReader(TestResources::ExampleFilePaths::gprs_smry_hdf5_5spot_);
    }
    virtual ~AdgprsResultsReaderTest() {}
    virtual void SetUp() {}
    AdgprsResultsReader::AdgprsResultsReader *reader_;
};

TEST_F(AdgprsResultsReaderTest, Global) {
    EXPECT_EQ(5, reader_->results()->GetNumberOfWells());
}

}


