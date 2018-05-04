/***********************************************************
 Copyright (C) 2017
 Mathias C. Bellout <mathias.bellout@ntnu.no>
 Oleg Volkov <ovolkov@stanford.edu>

 Created by bellout 10/2/18.

 This file is part of the FieldOpt project.

 FieldOpt is free software: you can redistribute it
 and/or modify it under the terms of the GNU General
 Public License as published by the Free Software
 Foundation, either version 3 of the License, or (at
 your option) any later version.

 FieldOpt is distributed in the hope that it will be
 useful, but WITHOUT ANY WARRANTY; without even the
 implied warranty of MERCHANTABILITY or FITNESS FOR
 A PARTICULAR PURPOSE.  See the GNU General Public
 License for more details.

 You should have received a copy of the GNU
 General Public License along with FieldOpt.
 If not, see <http://www.gnu.org/licenses/>.
***********************************************************/

// ---------------------------------------------------------
#ifndef FIELDOPT_SNOPTSOLVER_H
#define FIELDOPT_SNOPTSOLVER_H

// ---------------------------------------------------------
#include "Optimization/optimizer.h"

// ---------------------------------------------------------
#include "FieldOpt-3rdPartySolvers/handlers/SNOPTLoader.h"
#include "FieldOpt-3rdPartySolvers/handlers/SNOPTHandler.h"

// ---------------------------------------------------------
namespace Optimization {
namespace Optimizers {

// =========================================================
#ifdef __cplusplus
extern "C" {
#endif
int Rosenbrock_( integer    *Status, integer *n,    doublereal x[],
                 integer    *needF,  integer *neF,  doublereal F[],
                 integer    *needG,  integer *neG,  doublereal G[],
                 char       *cu,     integer *lencu,
                 integer    iu[],    integer *leniu,
                 doublereal ru[],    integer *lenru);

int SNOPTusrFG_( integer    *Status, integer *n,    doublereal x[],
                 integer    *needF,  integer *neF,  doublereal F[],
                 integer    *needG,  integer *neG,  doublereal G[],
                 char       *cu,     integer *lencu,
                 integer    iu[],    integer *leniu,
                 doublereal ru[],    integer *lenru);
#ifdef __cplusplus
}
#endif

// =========================================================
class SNOPTSolverC : public Optimizer
{
 public:

  // -------------------------------------------------------
  SNOPTSolverC(Settings::Optimizer *settings,
              Case *base_case,
              ::Reservoir::Grid::Grid *grid,
              RICaseData *ricasedata);

  // -------------------------------------------------------
  SNOPTSolverC(Settings::Optimizer *settings,
              Case *base_case,
              Model::Properties::VariablePropertyContainer *variables,
              Reservoir::Grid::Grid *grid,
              Logger *logger);

  // -------------------------------------------------------
//  ~SNOPTSolverC();

  // -------------------------------------------------------
  TerminationCondition IsFinished();

  // -------------------------------------------------------
   void handleEvaluatedCase(Case *c){};

  // -------------------------------------------------------
  // QString GetStatusStringHeader() const {};
  // QString GetStatusString() const {};

  // -------------------------------------------------------
 private:

  int n_;       // Number of variables
  int m_;       // Number of nonlinear constraints
  integer neF_; // Number of element in F
  integer neG_;
  integer lenG_;
  integer objRow_;
  double objAdd_;

  integer *iAfun_ = nullptr;
  integer *jAvar_ = nullptr;
  double *A_ = nullptr;
  integer lenA_;
  integer neA_;

  integer *iGfun_ = nullptr;
  integer *jGvar_ = nullptr;

  double *x_;

  // -------------------------------------------------------
  // Controls lower and upper bounds
  double *xlow_ = nullptr;
  double *xupp_ = nullptr;

  // -------------------------------------------------------
  // Initial guess for Lagrange multipliers
  double *xmul_ = nullptr;;

  // -------------------------------------------------------
  // State of the variables (whether the optimal is likely
  // to be on the boundary or not)
  integer *xstate_ = nullptr;

  // -------------------------------------------------------
  double *F_ = nullptr;
  double *Flow_ = nullptr;
  double *Fupp_ = nullptr;
  double *Fmul_ = nullptr;
  integer *Fstate_ = nullptr;
  char *xnames_ = nullptr;
  char *Fnames_ = nullptr;

  integer nxnames_;
  integer nFnames_;

  const double infinity_ = 1e20;

  // -------------------------------------------------------
  bool loadSNOPT(string libname = "libsnopt-7.2.12.2.so");
  void initSNOPTHandler();
  void setOptionsForSNOPT();
  void callSNOPT();

  // -------------------------------------------------------
  SNOPTHandler *SNOPTHandler_;

  string opt_prob_;

  Settings::Optimizer *settings_opt_;
  Settings::Optimizer::Constraint settings_con_;

  // -------------------------------------------------------
  RICaseData* ricasedata_;
  Reservoir::Grid::Grid *grid_;
  RIGrid* rigrid_;

  // Set in settings
  cvf::BoundingBox bbgrid_;

  void dbg_bbgrid();

  // static Eigen::VectorXd xinit_;
  Eigen::VectorXd xinit_;

  // -------------------------------------------------------
  void iterate() {};
  bool is_successful_iteration(){};

 public:
  // -------------------------------------------------------
  Case *wplccase_;
  // Eigen::VectorXd static get_xinit(){ return xinit_; }

 protected:

};

}
}

#endif //FIELDOPT_SNOPTSOLVER_H
