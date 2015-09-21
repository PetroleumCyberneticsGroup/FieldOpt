#ifndef ADJOINTSCOUPLEDMODEL_H
#define ADJOINTSCOUPLEDMODEL_H

#include <QVector>
#include "coupledmodel.h"
#include "optimizers/adjoint.h"
#include "optimizers/adjointcollection.h"
#include "stream.h"
#include "optimizers/case.h"

/*!
 * \brief Model that uses adjoints from a reservoir simulator (e.g. MRST) to obtain derivatives.
 *
 * This model type is at the moment only used in combination with MRST.
 * Adjoints are read from MRST and used to calculate numerical derivatives for the production network.
 *
 * \todo Document this.
 */
class AdjointsCoupledModel : public CoupledModel
{
private:
    double m_perturbation;
    Case *p_results;
    QVector<AdjointCollection*> m_adjoint_collections;
    Case* processPerturbation(shared_ptr<RealVariable> v);
    Case* processBaseCase();

public:
    AdjointsCoupledModel();
    AdjointsCoupledModel(const AdjointsCoupledModel &m);
    ~AdjointsCoupledModel();

    virtual Model* clone() const {return new AdjointsCoupledModel(*this);}
    virtual void initialize();
    virtual void process();

    AdjointCollection* adjointCollection(shared_ptr<RealVariable> v);  //!< Get the Adjoints asociated with variable v.

    void setPerturbationSize(double eps) {m_perturbation = eps;}

    double perturbationSize() {return m_perturbation;}
    Case* results() {return p_results;}

    bool applyCaseVariables(Case* c);
};

#endif // ADJOINTSCOUPLEDMODEL_H
