#ifndef FIELDOPT_FLOWDRIVERFILEWRITER_H
#define FIELDOPT_FLOWDRIVERFILEWRITER_H

#include "Settings/settings.h"
#include "Settings/simulator.h"
#include "Model/model.h"

namespace Simulation {
class FlowSimulator;
}

namespace Simulation {
class FlowDriverFileWriter {
  friend class FlowSimulator;
 private:
  friend class ::Simulation::FlowSimulator;
  FlowDriverFileWriter(::Settings::Settings *settings, Model::Model *model);
  void WriteDriverFile(QString output_dir);

  Model::Model *model_;
  ::Settings::Settings *settings_;
  QString output_driver_file_name_; //!< Path to the driver file to be written.
  QString GetCompdatString();
};
}

#endif // FIELDOPT_FLOWDRIVERFILEWRITER_H
