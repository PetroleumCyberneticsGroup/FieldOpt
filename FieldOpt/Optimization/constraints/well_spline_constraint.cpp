#include "well_spline_constraint.h"

namespace Optimization {
    namespace Constraints {

        WellSplineConstraint::Well WellSplineConstraint::initializeWell(QList<Model::Properties::ContinousProperty *> vars) {
            if (vars.length() != 6)
                throw std::runtime_error("Incorrect number of variables (" + std::to_string(vars.length())
                                         + ")passed to the initialize well method.");

            Well well;
            foreach (auto var, vars) {
                QStringList nmcomps = var->name().split("#"); // Should be {SplinePoint, WELLNAME, heel/toe, x/y/z}
                if (QString::compare("heel", nmcomps[2]) == 0) {
                    if (QString::compare("x", nmcomps[3]) == 0)
                        well.heel.x = var->id();
                    else if (QString::compare("y", nmcomps[3]) == 0)
                        well.heel.y = var->id();
                    else if (QString::compare("z", nmcomps[3]) == 0)
                        well.heel.z = var->id();
                    else throw std::runtime_error("Unable to parse variable " + var->name().toStdString());
                }
                else if (QString::compare("toe", nmcomps[2]) == 0) {
                    if (QString::compare("x", nmcomps[3]) == 0)
                        well.toe.x = var->id();
                    else if (QString::compare("y", nmcomps[3]) == 0)
                        well.toe.y = var->id();
                    else if (QString::compare("z", nmcomps[3]) == 0)
                        well.toe.z = var->id();
                    else throw std::runtime_error("Unable to parse variable " + var->name().toStdString());
                }
                else throw std::runtime_error("Unable to parse variable " + var->name().toStdString());
            }
            return well;
        }


    }
}
