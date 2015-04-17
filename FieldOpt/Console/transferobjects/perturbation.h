#ifndef PERTURBATION_H
#define PERTURBATION_H

#include <vector>
#include "model/model.h"
#include "optimizers/case.h"
#include "variables/binaryvariable.h"
#include "variables/intvariable.h"
#include "variables/realvariable.h"

/*!
 * \brief The Perturbation class is a convenience class used for handling the variables of
 * perturbed Case objects before and after transfering them using MPI.
 */
class Perturbation
{
private:
    const int perturbation_id;              //!< A unique ID for the perturbation. After evaluation a Result object with the same ID should be created.
    const int binaryLength;                 //!< The number of binary variables.
    const int integerLength;                //!< The number of integer variables.
    const int realLength;                   //!< The number of real variables.
    std::vector<double> binaryVariables;    //!< List containing all the binary variables from a case.
    std::vector<int> integerVariables;      //!< List containing all the integer variables from a case.
    std::vector<double> realVariables;      //!< List containing all the real variables from a case.
    Case* c;

public:
    Perturbation(Case* c, int id);  //!< Create a Pertrubation object from a Case and an id.
    Perturbation(std::vector<int> &header,  std::vector<double> &binaries, std::vector<int> &integers, std::vector<double> &reals);

    Case* getCase(Model* m);  //!< Creates a Case object by combining information from the variables stored in this class and the information in a Model object.

    int getPerturbation_id() const { return perturbation_id; }  //!< Get the id.

    int getBinaryLength() const { return binaryLength; }    //!< Get the number of binary variables.
    int getIntegerLength() const { return integerLength; }  //!< Get the number of integer variables.
    int getRealLength() const { return realLength; }        //!< Get the number of real variables.

    /*!
     * \brief Get the "send header".
     *
     * The send should be sent before the rest of the contents of a Perturbation object.
     * It describes what is to be received: the id of the perturbation, as well as the number
     * of variables of each type.
     * \return Vector containing: perturbation_id, binaryLength, integerLength, realLength
     */
    std::vector<int> getSendHeader() const;
    std::vector<double> getBinaryVariables() const { return binaryVariables; }  //!< Get a vector containing all binary variables.
    std::vector<int> getIntegerVariables() const { return integerVariables; }   //!< Get a vector containing all integer variables.
    std::vector<double> getRealVariables() const { return realVariables; }      //!< Get a vector containing all real variables.
};

#endif // PERTURBATION_H
