//
// Created by bellout on 2/10/18.
//

#ifndef FIELDOPT_SNOPTSOLVER_H
#define FIELDOPT_SNOPTSOLVER_H

#include "FieldOpt-3rdPartySolvers/include/snopt/snopt.hpp"
#include "FieldOpt-3rdPartySolvers/handlers/SNOPTHandler.h"

#ifdef __cplusplus
extern "C" {
#endif

int SNOPTusrFG_( integer    *Status, integer *n,    doublereal x[],
                 integer    *needF,  integer *neF,  doublereal F[],
                 integer    *needG,  integer *neG,  doublereal G[],
                 char       *cu,     integer *lencu,
                 integer    iu[],    integer *leniu,
                 doublereal ru[],    integer *lenru);

#ifdef __cplusplus
}
#endif

#include "Optimization/optimizer.h"

namespace Optimization {
namespace Optimizers {

class SNOPTSolver : public Optimizer {

 public:
  SNOPTSolver();
  ~SNOPTSolver();

  void callSNOPT();
  void setOptionsForSNOPT(SNOPTHandler& optimizeWithSNOPT);

  bool loadSNOPT(string libname = "libsnopt-7.2.12.2.so");

};

}
}

#endif //FIELDOPT_SNOPTSOLVER_H
