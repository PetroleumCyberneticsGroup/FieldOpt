#ifndef PERTURBATION_H
#define PERTURBATION_H

#include <vector>
#include "model/model.h"
#include "optimizers/case.h"
#include "variables/binaryvariable.h"
#include "variables/intvariable.h"
#include "variables/realvariable.h"

class Perturbation
{
private:
    const int perturbation_id;
    const int binaryLength;
    const int integerLength;
    const int realLength;
    std::vector<double> binaryVariables;
    std::vector<int> integerVariables;
    std::vector<double> realVariables;
    Case* c;

public:
    Perturbation(Case* c, int id);

    Case* getCase(Model* m);  //!< Creates a Case object by combining information from the variables stored in this class and the information in a Model object.

    int getPerturbation_id() const { return perturbation_id; }

    int getBinaryLength() const { return binaryLength; }
    int getIntegerLength() const { return integerLength; }
    int getRealLength() const { return realLength; }

    std::vector<int> getSendHeader() const;
    std::vector<double> getBinaryVariables() const { return binaryVariables; }
    std::vector<int> getIntegerVariables() const { return integerVariables; }
    std::vector<double> getRealVariables() const { return realVariables; }
};

#endif // PERTURBATION_H
