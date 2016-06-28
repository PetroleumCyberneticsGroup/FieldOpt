#include <Utilities/settings/simulator.h>
#include "adgprs_results_reader.h"
#include "Utilities/unix/execution.h"
#include "Utilities/file_handling/filehandling.h"

namespace AdgprsResultsReader {

AdgprsResultsReader::AdgprsResultsReader(QString summary_path)
{
    hdf5_path_ = summary_path;
    json_path_ = summary_path.split(".SIM.H5").first() + ".json";
    build_path_ = Utilities::FileHandling::GetBuildDirectoryPath();
    convertHdfSummaryToJson(build_path_);
    json_reader_ = new JsonSummaryReader(json_path_);
}

AdgprsResultsReader::AdgprsResultsReader(QString summary_path, QString build_path)
{
    hdf5_path_ = summary_path;
    json_path_ = summary_path.split(".SIM.H5").first() + ".json";
    convertHdfSummaryToJson(build_path);
    json_reader_ = new JsonSummaryReader(json_path_);
}

AdgprsResultsReader::~AdgprsResultsReader()
{
    delete json_reader_;
}

void AdgprsResultsReader::convertHdfSummaryToJson(QString build_path)
{
    QString conversion_script_path = build_path + "/AdgprsSummaryConverter/AdgprsSummaryConverter.py";
    QStringList params = {hdf5_path_, json_path_};
    Utilities::Unix::ExecShellScript(conversion_script_path, params);
}
}
