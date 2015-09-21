#include "adjoint.h"

Adjoint::Adjoint() :
    m_dp_dx(0),
    m_dqo_dx(0),
    m_dqg_dx(0),
    m_dqw_dx(0),
    m_time(-1),
    p_well(0)
{}

Adjoint::Adjoint(Well *w, int time) :
    m_dp_dx(0),
    m_dqo_dx(0),
    m_dqg_dx(0),
    m_dqw_dx(0),
    m_time(time),
    p_well(w)
{}

bool Adjoint::perturbStream(double eps_x)
{
    if(p_well != 0 && m_time >= 0)
    {
        Stream *p_stream = p_well->stream(m_time);

        double q_o = p_stream->oilRate() + m_dqo_dx*eps_x;
        double q_g = p_stream->gasRate() + m_dqg_dx*eps_x;
        double q_w = p_stream->waterRate() + m_dqw_dx*eps_x;
        double p = p_stream->pressure() + m_dp_dx*eps_x;

        p_stream->setOilRate(q_o);
        p_stream->setGasRate(q_g);
        p_stream->setWaterRate(q_w);
        p_stream->setPressure(p);
        return true;
    }
    else return false;
}
