#include "model.h"

namespace Model {

    Model::Model(Settings::Model settings)
    {
        grid_ = new Reservoir::Grid::ECLGrid(settings.reservoir().path.toStdString());
        variable_container_ = new Properties::VariablePropertyContainer();

        wells_ = new QList<Wells::Well *>();
        for (int well_nr = 0; well_nr < settings.wells().size(); ++well_nr) {
            wells_->append(new Wells::Well(settings, well_nr, variable_container_, grid_));
        }

        variable_container_->CheckVariableNameUniqueness();
    }

    void Model::ApplyCase(Optimization::Case *c)
    {
        for (QUuid key : c->binary_variables().keys()) {
            variable_container_->SetBinaryVariableValue(key, c->binary_variables()[key]);
        }
        for (QUuid key : c->integer_variables().keys()) {
            variable_container_->SetDiscreteVariableValue(key, c->integer_variables()[key]);
        }
        for (QUuid key : c->real_variables().keys()) {
            variable_container_->SetContinousVariableValue(key, c->real_variables()[key]);
        }
        for (Wells::Well *w : *wells_) {
            w->Update();
        }
        verify();
    }

    void Model::verify()
    {
        verifyWells();
    }

    void Model::verifyWells()
    {
        for (Wells::Well *well : *wells_) {
            verifyWellTrajectory(well);
        }
    }

    void Model::verifyWellTrajectory(Wells::Well *w)
    {
        for (Wells::Wellbore::WellBlock *wb : *w->trajectory()->GetWellBlocks()) {
            verifyWellBlock(wb);
        }
    }

    void Model::verifyWellBlock(Wells::Wellbore::WellBlock *wb)
    {
        if (wb->i() < 1 || wb->i() > grid()->Dimensions().nx ||
            wb->j() < 1 || wb->j() > grid()->Dimensions().ny ||
            wb->k() < 1 || wb->k() > grid()->Dimensions().nz)
            throw std::runtime_error("Invalid well block detected: ("
                                     + std::to_string(wb->i()) + ", "
                                     + std::to_string(wb->j()) + ", "
                                     + std::to_string(wb->k()) + ")"
            );
    }

}

