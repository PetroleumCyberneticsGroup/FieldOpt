#ifndef ADGPRSRESULTS_H
#define ADGPRSRESULTS_H

#include "results.h"
#include <QHash>
#include "Hdf5SummaryReader/hdf5_summary_reader.h"

namespace Simulation { namespace Results {

/*!
 * \brief AdgprsResults provides access to the results of an ADGPRS simulation.
 *
 * TODO: Support well properties. This is problematic because in the summary
 * the wells only have a number that I can't reliably map to a name.
 */
class AdgprsResults : public Results
{
 public:
  AdgprsResults();

  double GetValue(int well_nr, Property prop);
  double GetValue(int well_nr, Property prop, int time_index);

  // Results interface
 public:
  void ReadResults(QString file_path);
  void ReadResults(QString file_path, std::vector<int> verb_vector);
  void DumpResults();
  double GetValue(Property prop);
  double GetValue(Property prop, QString well);
  double GetValue(Property prop, int time_index);
  double GetValue(Property prop, QString well, int time_index);
  std::vector<double> GetValueVector(Property prop);

 private:
  QString file_path_;
  Hdf5SummaryReader *summary_reader_;
};

}}

#endif // ADGPRSRESULTS_H
