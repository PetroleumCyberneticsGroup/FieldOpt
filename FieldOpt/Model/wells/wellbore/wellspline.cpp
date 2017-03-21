#include "wellspline.h"
#include <iostream>

namespace Model {
namespace Wells {
namespace Wellbore {
    using namespace Reservoir::WellIndexCalculation;

WellSpline::WellSpline(Settings::Model::Well well_settings,
                       Properties::VariablePropertyContainer *variable_container,
                       Reservoir::Grid::Grid *grid)
{
    grid_ = grid;
    well_settings_ = well_settings;

    heel_x_ = new Model::Properties::ContinousProperty(well_settings.spline_heel.x);
    heel_y_ = new Model::Properties::ContinousProperty(well_settings.spline_heel.y);
    heel_z_ = new Model::Properties::ContinousProperty(well_settings.spline_heel.z);
    toe_x_ = new Model::Properties::ContinousProperty(well_settings.spline_toe.x);
    toe_y_ = new Model::Properties::ContinousProperty(well_settings.spline_toe.y);
    toe_z_ = new Model::Properties::ContinousProperty(well_settings.spline_toe.z);

    if (well_settings.spline_heel.is_variable) {
        heel_x_->setName(well_settings.spline_heel.name + "#x");
        heel_y_->setName(well_settings.spline_heel.name + "#y");
        heel_z_->setName(well_settings.spline_heel.name + "#z");
        variable_container->AddVariable(heel_x_);
        variable_container->AddVariable(heel_y_);
        variable_container->AddVariable(heel_z_);
    }
    if (well_settings.spline_toe.is_variable) {
        toe_x_->setName(well_settings.spline_toe.name + "#x");
        toe_y_->setName(well_settings.spline_toe.name + "#y");
        toe_z_->setName(well_settings.spline_toe.name + "#z");
        variable_container->AddVariable(toe_x_);
        variable_container->AddVariable(toe_y_);
        variable_container->AddVariable(toe_z_);
    }
}

QList<WellBlock *> *WellSpline::GetWellBlocks()
{
    auto heel = Eigen::Vector3d(heel_x_->value(), heel_y_->value(), heel_z_->value());
    auto toe = Eigen::Vector3d(toe_x_->value(), toe_y_->value(), toe_z_->value());

    auto wic = WellIndexCalculator(grid_);

    vector<WellDefinition> welldefs;
    welldefs.push_back(WellDefinition());
    welldefs[0].wellname = well_settings_.name.toStdString();
    welldefs[0].radii.push_back(well_settings_.wellbore_radius);
    welldefs[0].heels.push_back(heel);
    welldefs[0].toes.push_back(toe);

    auto block_data = wic.ComputeWellBlocks(welldefs)[well_settings_.name.toStdString()];
    QList<WellBlock *> *blocks = new QList<WellBlock *>();
    for (int i = 0; i < block_data.size(); ++i) {
        blocks->append(getWellBlock(block_data[i]));
    }
    return blocks;
}

WellBlock *WellSpline::getWellBlock(Reservoir::WellIndexCalculation::IntersectedCell block_data)
{
    auto wb = new WellBlock(block_data.ijk_index().i()+1, block_data.ijk_index().j()+1, block_data.ijk_index().k()+1);
    auto comp = new Completions::Perforation();
    comp->setTransmissibility_factor(block_data.cell_well_index());
    wb->AddCompletion(comp);
    return wb;
}

}
}
}
