#include "oneoff_runner.h"
#include "Optimization/constraints/constraint_handler.h"
#include <iostream>

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
}

void OneOffRunner::applyWellPositionFromArguments()
{
    QList<QPair<QUuid, QString>> prod_vars = model_->variables()->GetContinousVariableNamesAndIdsMatchingSubstring("PRODUCER");
    QList<QPair<QUuid, QString>> inje_vars = model_->variables()->GetContinousVariableNamesAndIdsMatchingSubstring("INJECTOR");
    if (prod_vars.length() != 6 || inje_vars.length() != 6)
        throw std::runtime_error("Found an incorrect number of variables for the wells.");

    QUuid phx, phy, phz, ihx, ihy, ihz, ptx, pty, ptz, itx, ity, itz;
    for (int i = 0; i < prod_vars.length(); ++i)
    {
        if (QString::compare(prod_vars[i].second.split("#").end()[-2], "heel") == 0)
        {
            if (QString::compare(prod_vars[i].second.split("#").end()[-1], "x") == 0)
                phx = prod_vars[i].first;
            else if (QString::compare(prod_vars[i].second.split("#").end()[-1], "y") == 0)
                phy = prod_vars[i].first;
            else if (QString::compare(prod_vars[i].second.split("#").end()[-1], "z") == 0)
                phz = prod_vars[i].first;
        }
        else if (QString::compare(prod_vars[i].second.split("#").end()[-2], "toe") == 0)
        {
            if (QString::compare(prod_vars[i].second.split("#").end()[-1], "x") == 0)
                ptx = prod_vars[i].first;
            else if (QString::compare(prod_vars[i].second.split("#").end()[-1], "y") == 0)
                pty = prod_vars[i].first;
            else if (QString::compare(prod_vars[i].second.split("#").end()[-1], "z") == 0)
                ptz = prod_vars[i].first;
        }
    }
    for (int i = 0; i < inje_vars.length(); ++i)
    {
        if (QString::compare(inje_vars[i].second.split("#").end()[-2], "heel") == 0)
        {
            if (QString::compare(inje_vars[i].second.split("#").end()[-1], "x") == 0)
                ihx = inje_vars[i].first;
            else if (QString::compare(inje_vars[i].second.split("#").end()[-1], "y") == 0)
                ihy = inje_vars[i].first;
            else if (QString::compare(inje_vars[i].second.split("#").end()[-1], "z") == 0)
                ihz = inje_vars[i].first;
        }
        else if (QString::compare(inje_vars[i].second.split("#").end()[-2], "toe") == 0)
        {
            if (QString::compare(inje_vars[i].second.split("#").end()[-1], "x") == 0)
                itx = inje_vars[i].first;
            else if (QString::compare(inje_vars[i].second.split("#").end()[-1], "y") == 0)
                ity = inje_vars[i].first;
            else if (QString::compare(inje_vars[i].second.split("#").end()[-1], "z") == 0)
                itz = inje_vars[i].first;
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

    auto constraint_handler_ = new Optimization::Constraints::ConstraintHandler(settings_->optimizer()->constraints(), model_->variables());
    constraint_handler_->SnapCaseToConstraints(base_case_);
    model_->ApplyCase(base_case_);
}

}
