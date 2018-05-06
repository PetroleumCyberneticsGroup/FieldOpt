#ifndef FIELDOPT_TEST_RESOURCE_CASES_H
#define FIELDOPT_TEST_RESOURCE_CASES_H

#include "Optimization/case.h"
#include "Model/properties/continous_property.h"
#include "Model/tests/test_resource_variable_property_container.h"
#include "Utilities/math.hpp"
#include <QHash>
#include <QUuid>
#include <QList>

namespace TestResources {
class TestResourceCases : public TestResources::TestResourceVariablePropertyContainer {
 public:
  TestResourceCases() {
      test_case_1_3i_ = new Optimization::Case(QHash<QUuid, bool>(), integer_variables_3d_, QHash<QUuid, double>());
      test_case_2_3r_ = new Optimization::Case(QHash<QUuid, bool>(), QHash<QUuid, int>(), real_variables_3d_);
      test_case_2_3r_->set_objective_function_value(100.0);
      test_case_3_4b3i3r_ = new Optimization::Case(binary_variables_4d_, integer_variables_3d_, real_variables_3d_);
      test_case_3_4b3i3r_->set_objective_function_value(-50.0);
      test_case_3_4b3i3r_->SetWICTime(5);
      test_case_4_4b3i3r = new Optimization::Case(binary_variables_4d_, integer_variables_3d_, real_variables_3d_);
      test_case_4_4b3i3r->set_objective_function_value(-50.0);
      trivial_cases_ << test_case_1_3i_ << test_case_2_3r_ << test_case_3_4b3i3r_ << test_case_4_4b3i3r;

      test_case_two_well_splines_ = new Optimization::Case(varcont_two_spline_wells_->GetBinaryVariableValues(),
                                                           varcont_two_spline_wells_->GetDiscreteVariableValues(),
                                                           varcont_two_spline_wells_->GetContinuousVariableValues());
      test_case_spline_ = new Optimization::Case(varcont_prod_spline_->GetBinaryVariableValues(),
                                                 varcont_prod_spline_->GetDiscreteVariableValues(),
                                                 varcont_prod_spline_->GetContinuousVariableValues());

      test_case_2r_ = new Optimization::Case(QHash<QUuid, bool>(), QHash<QUuid, int>(), real_variables_2d_);

      auto gen = get_random_generator();
      std::vector<double> rand_reals_30 = random_doubles(gen, -5.12, 5.12, 30);
      for (double rand : rand_reals_30)
          real_variables_sph_rand_30d_.insert(QUuid::createUuid(), rand);
      test_case_ga_spherical_30r_ = new Optimization::Case(QHash<QUuid, bool>(), QHash<QUuid, int>(), real_variables_sph_rand_30d_);



      std::vector<double> rand_reals_6 = random_doubles(gen, -5.12, 5.12, 6);
      varcont_6r_ = new VariablePropertyContainer();
      QString base_name = "BHP#PRODUCER#";
      for (int i = 0; i < rand_reals_6.size(); ++i) {
          ContinousProperty *prop = new ContinousProperty(rand_reals_6[i]);
          prop->setName(base_name + QString::number(i));
          varcont_6r_->AddVariable(prop);
          real_variables_sph_rand_6d_.insert(QUuid::createUuid(), rand_reals_6[i]);
      }
      test_case_ga_spherical_6r_ = new Optimization::Case(QHash<QUuid, bool>(), QHash<QUuid, int>(),
                                                          varcont_6r_->GetContinuousVariableValues());
  }

  QList<Optimization::Case *> trivial_cases_;

  /* Case 1:
   * Only integer variables.
   * OBjective function not evaluated.
   */
  Optimization::Case *test_case_1_3i_;

  /* Case 2:
   * Only real variables.
   * Objective function evaluated.
   */
  Optimization::Case *test_case_2_3r_;

  /* Case 3:
   * All variable types.
   * Objective function evaluated.
   */
  Optimization::Case *test_case_3_4b3i3r_;

  /* Case 4:
   * Identical to case 3.
   */
  Optimization::Case *test_case_4_4b3i3r;

  /* Case 5:
   * Spline well case
   */
  Optimization::Case *test_case_spline_;

  /* Case:
   * Two real variables.
   */
  Optimization::Case *test_case_2r_;

  /*Case:
   * Two well splines, parallel and 20 meters apart.
   */
  Optimization::Case *test_case_two_well_splines_;

  /*!
   * Case:
   * Test case with 30 random real variables between +- 5.12.
   * Primarily intended for testing GA with the spherical function.
   */
  Optimization::Case *test_case_ga_spherical_30r_;

  Optimization::Case *test_case_ga_spherical_6r_;

 private:
  const QHash<QUuid, bool> binary_variables_4d_{
      {QUuid::createUuid(), true},
      {QUuid::createUuid(), true},
      {QUuid::createUuid(), false},
      {QUuid::createUuid(), false}
  };
  const QHash<QUuid, int> integer_variables_3d_{
      {QUuid::createUuid(), 1},
      {QUuid::createUuid(), 2},
      {QUuid::createUuid(), 5}
  };
  const QHash<QUuid, double> real_variables_3d_{
      {QUuid::createUuid(), 1.0},
      {QUuid::createUuid(), 4.0},
      {QUuid::createUuid(), 2.5}
  };

  const QHash<QUuid, double> real_variables_2d_{
      {QUuid::createUuid(), 2.0},
      {QUuid::createUuid(), 3.0}
  };

  QHash<QUuid, double> real_variables_sph_rand_30d_;

  QHash<QUuid, double> real_variables_sph_rand_6d_;

};

}
#endif //FIELDOPT_TEST_RESOURCE_CASES_H
