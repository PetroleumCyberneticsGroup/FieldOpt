/***********************************************************
 Copyright (C) 2015-2017
 Einar J.M. Baumann <einar.baumann@gmail.com>

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
#include "constraint_handler.h"
#include <iostream>

// ---------------------------------------------------------
namespace Optimization {
namespace Constraints {

// =========================================================
ConstraintHandler::ConstraintHandler(
    QList<Settings::Optimizer::Constraint> constraints,
    Model::Properties::VariablePropertyContainer *variables,
    Reservoir::Grid::Grid *grid,
    Settings::Optimizer *settings,
    RICaseData *ricasedata) {

  if(ricasedata == nullptr) {
    cout << "RICaseData is null" << endl;
  }

  // -------------------------------------------------------
  for (Settings::Optimizer::Constraint constraint : constraints) {

    // -----------------------------------------------------
    switch (constraint.type) {

      // ---------------------------------------------------
      // WSPLINE_LENGTH + INTERW_DIST
      case Settings::Optimizer::ConstraintType::
        CombinedWellSplineLengthInterwellDistance:
        // c_wspln_lnght-interw-dist

        constraints_.append(
            new CombinedSplineLengthInterwellDistance(constraint,
                                                      variables));
        break;

        // -------------------------------------------------
        // WSPLINE_LENGTH + INTERW_DIST + IJK_RES_BOX
      case Settings::Optimizer::ConstraintType::
        CombinedWellSplineLengthInterwellDistanceReservoirBoundary:
        // c_wspln_lnght-interw-dist_res_ijk_box

        constraints_.append(
            new CombinedSplineLengthInterwellDistanceReservoirBoundary(
                constraint,
                variables,
                grid));
        break;

        // -------------------------------------------------
        // STANDALONE CONSTRAINT-HANDLING BY ADGPRS OPTIMIZER
      case Settings::Optimizer::ConstraintType::ADG:
        // adgprs_optimizer

        constraints_.append(new ADGConstraint(settings,
                                              variables,
                                              grid,
                                              ricasedata));
        break;

        // -------------------------------------------------
        // RES_IJK_BOX
      case Settings::Optimizer::ConstraintType::
        ReservoirBoundary:
        // res_ijk_box

        for (auto wname : constraint.wells) {
          auto cons = Settings::Optimizer::Constraint(constraint);
          cons.well = wname;
          constraints_.append(
              new ReservoirBoundary(cons,
                                    variables,
                                    grid));
        }
        break;

        // -------------------------------------------------
        // WCNTRL_BHP
      case Settings::Optimizer::ConstraintType::BHP:
        // wcntrl_bhp

        constraints_.append(new BhpConstraint(constraint,
                                              variables));
        break;

        // -------------------------------------------------
        // WCNTRL_RATE
      case Settings::Optimizer::ConstraintType::Rate:
        // wcntrl_rate

        constraints_.append(new RateConstraint(constraint,
                                               variables));
        break;

        // -------------------------------------------------
        // WSPLINE_INTERW_DIST_ANL
      case Settings::Optimizer::ConstraintType::
        WellSplineInterwellDistance:
        // wspln_interw_dist_anl

        constraints_.append(new InterwellDistance(constraint,
                                                  variables));
        break;

        // -------------------------------------------------
        // WSPLINE_INTERW_DIST_OPT (SNOPT)
      case Settings::Optimizer::ConstraintType::IWD:
        // wspln_interw_dist_opt

        constraints_.append(new IWDConstraint(settings,
                                              variables,
                                              grid,
                                              ricasedata));
        break;

        // -------------------------------------------------
        // WSPLINE_LENGTH
      case Settings::Optimizer::ConstraintType::
        WellSplineLength:
        // wspln_lngth

        for (auto wname : constraint.wells) {
          auto cons = Settings::Optimizer::Constraint(constraint);
          cons.well = wname;
          constraints_.append(new WellSplineLength(cons,
                                                   variables));
        }
        break;

        // -------------------------------------------------
        // WVERT_PSEUDO_CONT_2D_IJK_BOX
      case Settings::Optimizer::ConstraintType::
        PseudoContBoundary2D:
        // wvert_pseudo_cont_2d_ijk_box

        for (auto wname : constraint.wells) {
          auto cons = Settings::Optimizer::Constraint(constraint);
          cons.well = wname;
          constraints_.append(
              new PseudoContBoundary2D(cons,
                                       variables,
                                       grid));
        }
        break;

#ifdef WITH_EXPERIMENTAL_CONSTRAINTS
        // Cases for constraints in the experimental_constraints directory go here
#endif

      default:
        break;
    }
  }

// ---------------------------------------------------------
#ifdef WITH_EXPERIMENTAL_CONSTRAINTS
  std::cout << "Using experimental constraints" << std::endl;
#else
  std::cout << "Not using experimental constraints" << std::endl;
#endif
}

// =========================================================
bool ConstraintHandler::CaseSatisfiesConstraints(Case *c) {

  // -------------------------------------------------------
  // Check status wrt constraint, set to infeasible if true
  for (Constraint *constraint : constraints_) {
    if (!constraint->CaseSatisfiesConstraint(c)) {
      c->state.cons = Case::CaseState::ConsStatus::C_INFEASIBLE;
      return false;
    }
  }

  // -------------------------------------------------------
  // Set status to feasible if not infeasible
  c->state.cons = Case::CaseState::ConsStatus::C_FEASIBLE;
  return true;
}

// =========================================================
void ConstraintHandler::SnapCaseToConstraints(Case *c) {

  // -------------------------------------------------------
  // The following applies the different constraint types
  // from the constraint set onto the variable vector in
  // a sequential manner.

  // OK to apply the constraints to different parts of
  // the variable vector as long as the same constraint
  // tyoe applies to different groups of wells, e.g.,
  // two interwell distance constraints apply to different
  // sets of wells, or across all wells as long as using
  // different constraint types. For example:
  //
  // (i)   wbhp constraint on all wells
  // (ii)  interw_dist constraint a on a subset of wells
  // (iii) interw_dist constraint b on a another subset of wells
  //
  // Note: is interw_dist constraints a and b overlap on some
  // wells, then they may hinder each other and case convergence
  // issues
  // (\todo create a check that ensures that the entire constraint
  // \todo set defined is consistent, in the sense that no two
  // \todo constraints function on the same components of the
  // \todo variable vector.)

  // Each constraint type enforces alternating sequentiality
  // according to its type.

  // -------------------------------------------------------
  auto vec_before = c->GetRealVarVector();
  for (Constraint *constraint : constraints_) {
    constraint->SnapCaseToConstraints(c);
  }

  // -------------------------------------------------------
  if (vec_before != c->GetRealVarVector()) {
    c->state.cons = Case::CaseState::ConsStatus::C_PROJECTED;

  } else {
    c->state.cons = Case::CaseState::ConsStatus::C_FEASIBLE;
  }

}

// =========================================================
bool ConstraintHandler::HasBoundaryConstraints() const {

  // -------------------------------------------------------
  for (auto con : constraints_) {
    if (con->IsBoundConstraint())
      return true;
  }
  return false;
}

// =========================================================
Eigen::VectorXd
ConstraintHandler::GetLowerBounds(QList<QUuid> id_vector) const {

  // -------------------------------------------------------------
  Eigen::VectorXd lbounds(id_vector.size());
  lbounds.fill(0);

  // -------------------------------------------------------
  for (auto con : constraints_) {
    if (con->IsBoundConstraint()) {
      lbounds = lbounds + con->GetLowerBounds(id_vector);
    }
  }
  return lbounds;
}

// =========================================================
Eigen::VectorXd
ConstraintHandler::GetUpperBounds(QList<QUuid> id_vector) const {

  // -------------------------------------------------------
  Eigen::VectorXd ubounds(id_vector.size());
  ubounds.fill(0);

  // -------------------------------------------------------
  for (auto con : constraints_) {
    if (con->IsBoundConstraint()) {
      ubounds = ubounds + con->GetUpperBounds(id_vector);
    }
  }
  return ubounds;
}

// =========================================================
void ConstraintHandler::InitializeNormalizers(QList<Case *> cases) {

  // -------------------------------------------------------
  cout << "ConstraintHandler Initializing constraint normalizers" << endl;
  for (auto con : constraints_) {
    con->InitializeNormalizer(cases);
  }
}

// =========================================================
long double
ConstraintHandler::GetWeightedNormalizedPenalties(Case *c) {

  // -------------------------------------------------------
  long double wnp = 0.0L;
  for (auto con : constraints_) {
    long double pen = con->PenaltyNormalized(c);
    wnp += pen * con->GetPenaltyWeight();
  }
  return wnp;
}

}
}
