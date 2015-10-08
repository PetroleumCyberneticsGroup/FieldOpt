#ifndef ADJOINT_H
#define ADJOINT_H

#include "Library/model/stream.h"
#include "Library/variables/realvariable.h"
#include "Library/model/components/well.h"

/*!
 * \brief Holds the adjoints for one variable with respect to a single Stream.
 *
 * \todo Document this. Ask Mathias: What is an adjoint?
 */
class Adjoint
{
private:
    double m_dp_dx;
    double m_dqo_dx;
    double m_dqg_dx;
    double m_dqw_dx;
    int m_time;
    Well *p_well;

public:
    Adjoint();
    Adjoint(Well *w, int time);

    /*!
     * \brief Perturbes the Stream asociated with these adjoints.
     *
     * All the Stream quantities (oil, gas, water, pressure) will be set to: current_value + adjoint * eps_x
     * \param eps_x The size of the perturbation
     * \return True if everything went ok, false if the asociated Stream could not be found.
     */
    bool perturbStream(double eps_x);

    void setDpDx(double d) {m_dp_dx = d;}
    void setDqoDx(double d) {m_dqo_dx = d;}
    void setDqgDx(double d) {m_dqg_dx = d;}
    void setDqwDx(double d) {m_dqw_dx = d;}
    void setWell(Well *w) {p_well = w;}

    double dpDx() const {return m_dp_dx;}
    double dqoDx() const {return m_dqo_dx;}
    double dqgDx() const {return m_dqg_dx;}
    double dqwDx() const {return m_dqw_dx;}
    Stream* stream() {return p_well->stream(m_time);}
    int time() {return m_time;}
    Well* well() {return p_well;}
};

#endif // ADJOINT_H
