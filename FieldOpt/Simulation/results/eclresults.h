#ifndef ECLRESULTS_H
#define ECLRESULTS_H

#include "results.h"
#include "ERTWrapper/eclsummaryreader.h"
#include <QHash>

namespace Simulation {
namespace Results {

/*!
 * \brief The ECLResults class uses the ECLSummaryReader class in the ERTWrapper library
 * to read properties from ECLIPSE summary files.
 */
class ECLResults : public Results
{
 public:
  ECLResults();

  void ReadResults(QString file_path);
  void ReadResults(QString file_path, QString build_dir);
  void DumpResults();


  double GetValue(Property prop);
  double GetValue(Property prop, int time_index);
  double GetValue(Property prop, QString well);
  double GetValue(Property prop, QString well, int time_index);
  std::vector<double> GetValueVector(Property prop);
  std::vector<double> GetValueVector(Property prop, QString well_name);

 private:
  QString file_path_;
  ERTWrapper::ECLSummary::ECLSummaryReader *summary_reader_;
};

}
}

#endif // ECLRESULTS_H
