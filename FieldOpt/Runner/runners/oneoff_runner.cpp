#include "oneoff_runner.h"

namespace Runner {

OneOffRunner::OneOffRunner(RuntimeSettings *runtime_settings)
    : AbstractRunner(runtime_settings)
{
    std::cout << "Initializing one-off runner." << std::endl;
    InitializeSettings();
    InitializeModel();
    InitializeSimulator();
    //EvaluateBaseModel();
    InitializeObjectiveFunction();
    InitializeBaseCase();
    //InitializeOptimizer();
    //InitializeBookkeeper();
    InitializeLogger();
}

void OneOffRunner::Execute()
{
    applyWellPositionFromArguments();
    EvaluateBaseModel();
    // Write objective function value to file
    Utilities::FileHandling::WriteLineToFile(QString("%1").arg(objective_function_->value()),settings_->output_directory() + "/f.out");
//    std::cout << objective_function_->value() << std::endl;
}

void OneOffRunner::applyWellPositionFromArguments()
{
    QList<Model::Properties::ContinousProperty *> prod_vars = model_->variables()->GetWellSplineVariables("PRODUCER");
    QList<Model::Properties::ContinousProperty *> inje_vars = model_->variables()->GetWellSplineVariables("INJECTOR");
    if (prod_vars.length() != 6 || inje_vars.length() != 6)
        throw std::runtime_error("Found an incorrect number of variables for the wells.");

    QUuid phx, phy, phz, ihx, ihy, ihz, ptx, pty, ptz, itx, ity, itz;
    for (int i = 0; i < prod_vars.length(); ++i)
    {
        if (prod_vars[i]->propertyInfo().spline_end == Model::Properties::Property::SplineEnd::Heel)
        {
            if (prod_vars[i]->propertyInfo().coord == Model::Properties::Property::Coordinate::x)
                phx = prod_vars[i]->id();
            else if (prod_vars[i]->propertyInfo().coord == Model::Properties::Property::Coordinate::y)
                phy = prod_vars[i]->id();
            else if (prod_vars[i]->propertyInfo().coord == Model::Properties::Property::Coordinate::z)
                phz = prod_vars[i]->id();
        }
        else if (prod_vars[i]->propertyInfo().spline_end == Model::Properties::Property::SplineEnd::Toe)
        {
            if (prod_vars[i]->propertyInfo().coord == Model::Properties::Property::Coordinate::x)
                ptx = prod_vars[i]->id();
            else if (prod_vars[i]->propertyInfo().coord == Model::Properties::Property::Coordinate::y)
                pty = prod_vars[i]->id();
            else if (prod_vars[i]->propertyInfo().coord == Model::Properties::Property::Coordinate::z)
                ptz = prod_vars[i]->id();
        }
    }
    for (int i = 0; i < inje_vars.length(); ++i)
    {
        if (inje_vars[i]->propertyInfo().spline_end == Model::Properties::Property::SplineEnd::Heel)
        {
            if (inje_vars[i]->propertyInfo().coord == Model::Properties::Property::Coordinate::x)
                ihx = inje_vars[i]->id();
            if (inje_vars[i]->propertyInfo().coord == Model::Properties::Property::Coordinate::y)
                ihy = inje_vars[i]->id();
            if (inje_vars[i]->propertyInfo().coord == Model::Properties::Property::Coordinate::z)
                ihz = inje_vars[i]->id();
        }
        if (inje_vars[i]->propertyInfo().spline_end == Model::Properties::Property::SplineEnd::Toe)
        {
            if (inje_vars[i]->propertyInfo().coord == Model::Properties::Property::Coordinate::x)
                itx = inje_vars[i]->id();
            if (inje_vars[i]->propertyInfo().coord == Model::Properties::Property::Coordinate::y)
                ity = inje_vars[i]->id();
            if (inje_vars[i]->propertyInfo().coord == Model::Properties::Property::Coordinate::z)
                itz = inje_vars[i]->id();
        }
    }

    base_case_->set_real_variable_value(phx, runtime_settings_->prod_coords().first[0]);
    base_case_->set_real_variable_value(phy, runtime_settings_->prod_coords().first[1]);
    base_case_->set_real_variable_value(phz, runtime_settings_->prod_coords().first[2]);
    base_case_->set_real_variable_value(ptx, runtime_settings_->prod_coords().second[0]);
    base_case_->set_real_variable_value(pty, runtime_settings_->prod_coords().second[1]);
    base_case_->set_real_variable_value(ptz, runtime_settings_->prod_coords().second[2]);
    base_case_->set_real_variable_value(ihx, runtime_settings_->inje_coords().first[0]);
    base_case_->set_real_variable_value(ihy, runtime_settings_->inje_coords().first[1]);
    base_case_->set_real_variable_value(ihz, runtime_settings_->inje_coords().first[2]);
    base_case_->set_real_variable_value(itx, runtime_settings_->inje_coords().second[0]);
    base_case_->set_real_variable_value(ity, runtime_settings_->inje_coords().second[1]);
    base_case_->set_real_variable_value(itz, runtime_settings_->inje_coords().second[2]);

    auto constraint_handler_ = new Optimization::Constraints::ConstraintHandler(settings_->optimizer()->constraints(),
                                                                                model_->variables(), model_->grid());
    constraint_handler_->SnapCaseToConstraints(base_case_);
    model_->ApplyCase(base_case_);
}

}
