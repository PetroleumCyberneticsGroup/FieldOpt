#include "wellpath.h"

WellPath::WellPath() :
    m_lock_heel_i(false),
    m_lock_heel_j(false),
    m_lock_heel_k(false)
{}

WellPath::WellPath(const WellPath &wp)
{
    setToeI(shared_ptr<IntVariable>(new IntVariable(*wp.p_toe_i)));
    setToeJ(shared_ptr<IntVariable>(new IntVariable(*wp.p_toe_j)));
    setToeK(shared_ptr<IntVariable>(new IntVariable(*wp.p_toe_k)));

    if(wp.m_lock_heel_i)
        p_heel_i = p_toe_i;
    else
        setHeelI(shared_ptr<IntVariable>(new IntVariable(*wp.p_heel_i)));

    if(wp.m_lock_heel_j)
        p_heel_j = p_toe_j;
    else
        setHeelJ(shared_ptr<IntVariable>(new IntVariable(*wp.p_heel_j)));

    if(wp.m_lock_heel_k)
        p_heel_k = p_toe_j;
    else
        setHeelK(shared_ptr<IntVariable>(new IntVariable(*wp.p_heel_k)));

    for(int i = 0; i < wp.m_options_var.size(); ++i)
    {
        addOptionVariable(shared_ptr<IntVariable>(new IntVariable(*wp.m_options_var.at(i))));
    }

    m_options_const = wp.m_options_const;
    m_lock_heel_i = wp.m_lock_heel_i;
    m_lock_heel_j = wp.m_lock_heel_j;
    m_lock_heel_k = wp.m_lock_heel_k;
}

bool WellPath::initialize()
{
    if(p_toe_i == 0 || p_toe_j == 0 || p_toe_k == 0)
        return false;

    // Set names
    Well *w = dynamic_cast<Well*>(p_toe_i->parent());  // Parent well. Its name will be used for the variable names.
    p_toe_i->setName("WELLPATH TOE I for well: " + w->name());
    p_toe_j->setName("WELLPATH TOE J for well: " + w->name());
    p_toe_k->setName("WELLPATH TOE K for well: " + w->name());


    // locking, naming
    if(p_heel_i == 0) {
        p_heel_i = p_toe_i;
        m_lock_heel_i = true;
    }
    else {
        p_heel_i->setName("WELLPATH HEEL I for well: " + w->name());
    }

    if(p_heel_j == 0) {
        p_heel_j = p_toe_j;
        m_lock_heel_j = true;
    }
    else {
        p_heel_j->setName("WELLPATH HEEL J for well: " + w->name());
    }

    if(p_heel_k == 0) {
        p_heel_k = p_toe_k;
        m_lock_heel_k = true;
    }
    else {
        p_heel_k->setName("WELLPATH HEEL K for well: " + w->name());
    }

    // naming options
    for(int i = 0; i < m_options_var.size(); ++i)
    {
        m_options_var.at(i)->setName("WELLPATH " + m_options_var.at(i)->name() + " for well: " + w->name());
    }

    return true;
}

void WellPath::updateConstantOption(int i, double value)
{
    QPair<QString, double> opt_new(m_options_const.at(i).first, value);
    m_options_const.replace(i, opt_new);
}

QVector<shared_ptr<IntVariable> > WellPath::variables()
{
    QVector<shared_ptr<IntVariable>> vars;
    vars += m_options_var;
    vars.push_back(p_toe_i);
    vars.push_back(p_toe_j);
    vars.push_back(p_toe_k);
    if(!heelILocked()) vars.push_back(p_heel_i);
    if(!heelJLocked()) vars.push_back(p_heel_j);
    if(!heelKLocked()) vars.push_back(p_heel_k);
    return vars;
}
