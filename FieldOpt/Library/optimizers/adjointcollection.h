#ifndef ADJOINTCOLLECTION_H
#define ADJOINTCOLLECTION_H

#include "adjoint.h"
#include "Library/variables/realvariable.h"
#include "Library/model/components/well.h"
#include "Library/model/stream.h"

/*!
 * \brief A Collection of all the Adjoints for a given variable.
 *
 * \todo Document this.
 */
class AdjointCollection
{
private:
    QVector<Adjoint*> m_adjoints;
    shared_ptr<RealVariable> p_var;

public:
    AdjointCollection(){}
    AdjointCollection(const AdjointCollection &ac);

    bool perturbStreams(double eps_x);  //!< Perturbs all Streams asociated with this adjoint collection. See Adjoint::perturbStream(double).

    void addAdjoint(Adjoint *a) {m_adjoints.push_back(a);}

    void setVariable(shared_ptr<RealVariable> v) {p_var = v;}

    shared_ptr<RealVariable> variable() {return p_var;}
    Adjoint* adjoint(int i) {return m_adjoints.at(i);}
    Adjoint* adjoint(Well *w, int time);  //!< Returns the Adjoint for the Stream from a given Well at the specified timestep, with respect to the variable variable().
    int numberOfAdjoints() {return m_adjoints.size();}  //!< Returns the numer of adjoints in this collection.
};

#endif // ADJOINTCOLLECTION_H
