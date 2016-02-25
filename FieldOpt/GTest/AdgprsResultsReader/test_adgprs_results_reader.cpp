#include "GTest/Simulation/results/test_fixture_adgprs.h"
#include "AdgprsResultsReader/adgprs_results_reader.h"

namespace {

class AdgprsResultsReaderTest : public AdgprsTestFixture {
protected:
    AdgprsResultsReaderTest() {
        reader_ = new AdgprsResultsReader::AdgprsResultsReader(hdf5_summary_path_);
    }
    virtual ~AdgprsResultsReaderTest() {}
    virtual void SetUp() {}
    AdgprsResultsReader::AdgprsResultsReader *reader_;
};

TEST_F(AdgprsResultsReaderTest, Global) {
    EXPECT_EQ(5, reader_->results()->GetNumberOfWells());
}

}


