SET(MODEL_HEADERS
	cpp-spline/src/BSpline.h
	cpp-spline/src/Bezier.h
	cpp-spline/src/CatmullRom.h
	cpp-spline/src/Curve.h
	cpp-spline/src/Vector.h
	model.h
	model_synchronization_object.h
	properties/binary_property.h
	properties/continous_property.h
	properties/discrete_property.h
	properties/property.h
	properties/property_exceptions.h
	properties/variable_property_container.h
	wells/compartment.h
	wells/control.h
	wells/segment.h
	wells/well.h
	wells/well_exceptions.h
	wells/wellbore/completions/completion.h
	wells/wellbore/completions/icd.h
	wells/wellbore/completions/packer.h
	wells/wellbore/completions/perforation.h
	wells/wellbore/completions/segmented_completion.h
	wells/wellbore/pseudo_cont_vert.h
	wells/wellbore/trajectory.h
	wells/wellbore/wellblock.h
	wells/wellbore/wellspline.h
    wells/wellbore/polar_spline.h
)

SET(MODEL_SOURCES
	cpp-spline/src/BSpline.cpp
	cpp-spline/src/Bezier.cpp
	cpp-spline/src/CatmullRom.cpp
	cpp-spline/src/Curve.cpp
	cpp-spline/src/Vector.cpp
	model.cpp
	model_synchronization_object.cpp
	properties/binary_property.cpp
	properties/continous_property.cpp
	properties/discrete_property.cpp
	properties/property.cpp
	properties/variable_property_container.cpp
	wells/compartment.cpp
	wells/control.cpp
	wells/segment.cpp
	wells/well.cpp
	wells/wellbore/completions/completion.cpp
	wells/wellbore/completions/icd.cpp
	wells/wellbore/completions/packer.cpp
	wells/wellbore/completions/perforation.cpp
	wells/wellbore/completions/segmented_completion.cpp
	wells/wellbore/pseudo_cont_vert.cpp
	wells/wellbore/trajectory.cpp
	wells/wellbore/wellblock.cpp
	wells/wellbore/wellspline.cpp
	wells/wellbore/polar_spline.cpp
)

SET(MODEL_TESTS
	tests/test_resource_model.h
	tests/test_resource_model_setting_snippets.hpp
	tests/test_resource_variable_property_container.h
	tests/test_model.cpp
	tests/variables/test_properties.cpp
	tests/variables/test_variable_property_container.cpp
	tests/wells/test_control.cpp
	tests/wells/test_perforation.cpp
	tests/wells/test_pseudo_cont_vert.cpp
	tests/wells/test_segmented_well.cpp
	tests/wells/test_trajectory.cpp
	tests/wells/test_trajectory_curve.cpp
	tests/wells/test_well.cpp
	tests/wells/test_wellblock.cpp
	tests/wells/test_polar_spline.cpp
)


