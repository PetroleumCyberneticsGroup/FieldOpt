#include "adjointcollection.h"

AdjointCollection::AdjointCollection(const AdjointCollection &ac)
{
    p_var = ac.p_var;
    for(int i = 0; i < ac.m_adjoints.size(); ++i)
        m_adjoints.push_back(new Adjoint(*ac.m_adjoints.at(i)));
}


Adjoint* AdjointCollection::adjoint(Well *w, int time)
{
    for(int i = 0; i < m_adjoints.size(); ++i)
    {
        if((w->id() == m_adjoints.at(i)->well()->id()) && (time == m_adjoints.at(i)->time()))
            return m_adjoints.at(i);
    }
    return 0;
}


bool AdjointCollection::perturbStreams(double eps_x)
{
    for(int i = 0; i < m_adjoints.size(); ++i)
    {
        if(!m_adjoints.at(i)->perturbStream(eps_x))  // perturbing each adjoint stream, returning false if unsuccessfull.
            return false;
    }
    return true;
}
