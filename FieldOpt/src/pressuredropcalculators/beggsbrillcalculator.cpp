/*
 * This file is part of the FieldOpt project.
 *
 * Copyright (C) 2015-2015 Einar J.M. Baumann <einarjba@stud.ntnu.no>
 *
 * The code is largely based on ResOpt, created by  Aleksander O. Juell <aleksander.juell@ntnu.no>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */

#include "beggsbrillcalculator.h"
#include <math.h>


BeggsBrillCalculator::BeggsBrillCalculator()
    : m_sg_gas(0.0),
      m_den_oil(0.0),
      m_den_wat(0.0),
      m_vis_oil(0.0),
      m_vis_wat(0.0),
      m_diameter(0.0),
      m_length(0.0),
      m_angle(0.0),
      m_temperature(0.0)
{
}

double BeggsBrillCalculator::superficialGasVelocity(Stream *s, double p, double z)
{
    double gas_rate_surface = s->gasRate(Stream::FIELD) / 86.4;  // Convert Mscf/day -> scf/s

    /*
     * Calculate \f$ 1/B_g \f$
     *
     * 288.71 is the temperature in Kelvin at standard conditions (15 degrees C).
     * 1.01 is the pressure in bar at standard conditions.
     * 273.15 converts the temperature from C to K.
     */
    double b_g = p * 288.71 / 1.01 / (temperature() + 273.15) / z;

    double gas_rate = gas_rate_surface / b_g;       // Calculate the gas rate at pipe conditions (scf/s)
    double r_ft = diameter() / 0.3048 / 2;          // Calculate pipe radius in ft. 0.3048 converts m->ft.
    return gas_rate / (3.14159265 * pow(r_ft,2));   // return gas velocity in ft/s
}

double BeggsBrillCalculator::superficialLiquidVelocity(Stream *s)
{
    double liquid_rate = s->oilRate(Stream::FIELD) + s->waterRate(Stream::FIELD);  // Calculate liquid rate in bbl/d
    double liquid_rate_ft = 5.61458333 * liquid_rate / 86400.0;  // Convert the liquid rate from bbl/day -> ft^3/s
    double r_ft = diameter() / 0.3048 / 2.0;  // pipe radius from m -> ft
    return liquid_rate_ft / (3.14159265 * pow(r_ft,2)); // return liquid rate in ft/s
}

double BeggsBrillCalculator::liquidDensity(Stream *s)
{
    double oil_rate = s->oilRate(Stream::METRIC);
    double water_rate = s->waterRate(Stream::METRIC);
    if((oil_rate + water_rate) < 1e-6) oil_rate = 1;
    double den_metric = (oil_rate * oilDensity() + water_rate * waterDensity()) / (oil_rate + water_rate);  // Liquid density in kg/m^3
    return 0.0624279606 * den_metric;   // converting to lb / ft^3
}

double BeggsBrillCalculator::gasZFactor(double yg, double t, double p)
{
    // t  - degrees Celcius
    // p  - bara
    // yg - specific gravity

    // Calculating pseudocritical properties
    double t_pc = 169.2 +349.5*yg - 74*pow(yg,2);
    double p_pc = 756.8 - 131*yg - 3.6*pow(yg,2);

    // unit conversion
    t = 9.0/5.0 * t + 32;  // C -> F
    p = 14.5*p;  // bar -> psi

    // calculating pseudo reduced properties
    double t_pr = (t + 460) / t_pc;
    double p_pr = p / p_pc;

     t = 1 / t_pr;
     double a = 0.06125 * t * exp(-1.2*pow((1-t),2));
//          a = 0.06125 * T * Exp(-1.2 * (1# - T) ^ 2)

     double y = 0.001;
     int i = 0;

     double fy = 1;

     do
     {
        fy = -a * p_pr + (y + pow(y,2) + pow(y,3) - pow(y,4)) / pow((1 - y),3) - (14.76 * t - 9.76 * pow(t,2) + 4.58 * pow(t,3)) * pow(y,2) + (90.7 * t - 242.2 * pow(t,2) + 42.4 * pow(t,3)) * pow(y,(2.18 + 2.82 * t));
//      fy = -a * Ppr +  (y +  y ^ 2   +   y ^ 3  -   y ^ 4)  /   (1 - y) ^ 3  - (14.76 * T - 9.76 * T ^ 2 +    4.58 *  T ^ 3) *    y ^ 2 +   (90.7 * T - 242.2 *  T ^ 2 +   42.4 *  T ^ 3)   *   y ^ (2.18 + 2.82 * T)

        double dfY = (1 + 4 * y + 4 *pow(y,2) - 4 *pow(y,3) + pow(y,4)) / pow((1 - y),4) - (29.52 * t - 19.52 * pow(t,2) + 9.16 * pow(t,3)) * y + (2.18 + 2.82 * t) * (90.7 * t - 242.2 * pow(t,2) + 42.4 * pow(t,3)) * pow(y,(1.18 + 2.82 * t));
//             dfY = (1 + 4 * y + 4 * y ^ 2   - 4 * y ^ 3   + y ^ 4   ) /   (1 - y) ^ 4  - (29.52 * T - 19.52 *   T ^ 2  + 9.16 *   T ^ 3)  * y + (2.18 + 2.82 * T) * (90.7 * T - 242.2 *   T ^ 2  + 42.4 *   T ^ 3 ) *   y ^ (1.18 + 2.82 * T)
        y = y - fy / dfY;

        i++;

     }while(fy > 1e-8 && i < 200);


     return a * p_pr / y;
}

double BeggsBrillCalculator::gasDensity(double t, double p, double z)
{
    double Mg = gasSpecificGravity()*28.97;     // molecular weight of gas in g/mol
    double den_gas_metric = (p * Mg) / (83.143 * z * (t + 273.15) );    // in kg/m^3
    return 0.0624279606 * den_gas_metric;  // return in lb/ft^3
}

double BeggsBrillCalculator::surfaceTension(double gas_density, double liquid_density)
{
    double dg = liquid_density - gas_density;
    return 15.0 + 0.91 * dg;
}

double BeggsBrillCalculator::gasViscosity(double p, double z)
{

    double t_r = (temperature() + 273.15) * 1.8;
    double Mg = gasSpecificGravity()*28.97;

    double den_gas = (p * Mg) / (z * 83.143 * (temperature() + 273.15)) / 1000;

    double A1 = ((9.379 + 0.01607*Mg) * pow(t_r, 1.5)) / (209.2 + 19.26*Mg + t_r);
    double A2 = (3.448 + 986.4/t_r + 0.01009*Mg);
    double A3 = 2.447 - 0.2224*A2;

    return 1E-4*A1 * exp(A2* pow(A2*den_gas, A3));
}

double BeggsBrillCalculator::liquidViscosity(Stream *s)
{
    double oil_rate = s->oilRate(Stream::FIELD);
    double water_rate = s->waterRate(Stream::FIELD);
    if((oil_rate + water_rate) < 1e-6){
        oil_rate = 1.0;
    }
    return (oil_rate * oilViscosity() + water_rate * waterViscosity()) / (oil_rate + water_rate);
}

double BeggsBrillCalculator::pressureDrop(Stream *s, double p, Stream::units unit)
{
    // Check if the rates are zero
    double total_rate = s->gasRate(Stream::FIELD) + s->oilRate(Stream::FIELD) + s->waterRate(Stream::FIELD);
    if(total_rate <= 0) return 0.0;
    if(s->oilRate() < 0) return 0.0;
    if(s->gasRate() < 0) return 0.0;
    if(s->waterRate() < 0) return 0.0;
    if(p <= 0) return 0.0;

    double p_psi = p;
    if(unit == Stream::METRIC) p_psi = p * 14.5037738;  // pressure in psi

    double d_in = diameter() * 39.3700787;              // pipe diameter in inches
    double g = 32.2;                                    // gravitational constant (ft / s^2)
    double z_fac = gasZFactor(gasSpecificGravity(), temperature(), p);      // gas z-factor
    double vsl = superficialLiquidVelocity(s);          // superficial liquid velocity
    double vsg = superficialGasVelocity(s, p, z_fac);   // superficial gas velocity
    double vm = vsl + vsg;                              // superficial two phase velocity
    double froude_no = pow(vm, 2) / d_in / g;  // froude number

    double liquid_content = vsl / vm;
    if(liquid_content < 1e-8) liquid_content = 1e-8; // if the liquid content is 0, changing it to something small

    // Determine flow regime
    flow_regime regime;
    // constants for determining the flow regime
    double l1 = 316 * pow(liquid_content, 0.302);
    double l2 = 0.0009252 * pow(liquid_content, -2.4684);
    double l3 = 0.10 * pow(liquid_content, -1.4516);
    double l4 = 0.5 * pow(liquid_content, -6.738);

    // checking the flow regime
    if(liquid_content < 0.01 && froude_no < l1) regime = SEGREGATED;
    else if(liquid_content >= 0.01 && froude_no < l2) regime = SEGREGATED;
    else if(liquid_content >= 0.01 && froude_no >= l2 && froude_no <= l3) regime = TRANSITION;
    else if(liquid_content >= 0.01 && liquid_content < 0.4 && froude_no > l3 && froude_no <= l1) regime = INTERMITTENT;
    else if(liquid_content >= 0.4 && froude_no > l3 && froude_no <= l4) regime = INTERMITTENT;
    else if(liquid_content < 0.4 && froude_no >= l1) regime = DISTRIBUTED;
    else if(liquid_content >= 0.4 && froude_no > l4) regime = DISTRIBUTED;
    else    // the current conditions are not covered by Beggs & Brill...
    {
//        cout << endl << "### Warning ###" << endl
//             << "From: Beggs & Brill 1973" << endl
//             << "The flow regime could not be determined..." << endl
//             << "Assuming INTERMITTENT flow..." << endl
//             << "For the current stream:" << endl << endl;
        s->printToCout();

        regime = INTERMITTENT;
    }

    // calculating the horizontal liquid holdup, hl(0)
    double hz_holdup = 0.0;

    if(regime == SEGREGATED)
    {
        hz_holdup = (0.98 * pow(liquid_content, 0.4846)) / pow(froude_no, 0.0868);
    }
    else if(regime == INTERMITTENT)
    {
        hz_holdup = (0.845 * pow(liquid_content, 0.5351)) / pow(froude_no, 0.0173);

        // cor = (1 - liquid_content) * log(2.96 * pow(liquid_content, -0.305) * pow(nlv, -0.4473) * pow(froude_no, -0.0978));

        //cout << "intermittent hz_holdup = " << hz_holdup << endl;
        //cout << "liquid_content         = " << liquid_content << endl;
    }
    else if(regime == DISTRIBUTED)
    {
        hz_holdup = (1.065 * pow(liquid_content, 0.5824)) / pow(froude_no, 0.0609);
    }



    // sets the horizontal holdup to the liquid content if smaller
    if(hz_holdup < liquid_content) hz_holdup = liquid_content;


    // calculating correction factor
    double den_l = liquidDensity(s);                        // liquid density
    double den_g = gasDensity(temperature(), p, z_fac);         // gas density
    double surface_tens = surfaceTension(den_g, den_l);     // gas - liquid surface tension

    double nlv = vsl * pow(den_l / (g * surface_tens), 0.25);        // liquid velocity number
    double cor = 0.0;
    double payne_cor = 0.924;       // Payne correction factor to holdup


    if(angle() < 0)     //downhill pipe, all flow regimes are the same
    {
        payne_cor = 0.685;
        cor = (1 - liquid_content) * log(4.7 * pow(liquid_content, -0.3692) * pow(nlv, 0.1244) * pow(froude_no, -0.5056));
    }
    else if(regime == SEGREGATED)
    {
        cor = (1 - liquid_content) * log(0.011 * pow(liquid_content, -3.768) * pow(nlv, 3.539) * pow(froude_no, -1.614));
    }
    else if(regime == INTERMITTENT)
    {
        cor = (1 - liquid_content) * log(2.96 * pow(liquid_content, -0.305) * pow(nlv, -0.4473) * pow(froude_no, -0.0978));

        //cout << "intermittent cor = " << cor << endl;
    }
    else if(regime == DISTRIBUTED)
    {
        cor = 0.0;
    }

    // checking to see if correction is >= 0
    if(cor < 0)
    {
//        cout << endl << "### Warning ###" << endl
//             << "From: Beggs & Brill 1973" << endl
//             << "The calculated correction factor, C, is negative..." << endl
//             << "Resetting to 0.0..." << endl << endl;
        cor = 0.0;


    }



    double phi = 1 + cor * (sin(3.14159265 * 1.8*angle() / 180) - 0.333 * pow(sin(3.14159265 * 1.8*angle() / 180), 3));
    double holdup = payne_cor * hz_holdup*phi;      // liquid holdup corrected for inclination



    // if transition regime, the liquid holdup is a mix of segregated and intermittent
    if(regime == TRANSITION)
    {
        double frac = (l3 - froude_no) / (l3 -l2);

        // horizontal holdups
        double hz_holdup_seg = frac * (0.98 * pow(liquid_content, 0.4846)) / pow(froude_no, 0.0868);
        double hz_holdup_int = (1 - frac) * (0.845 * pow(liquid_content, 0.5351)) / pow(froude_no, 0.0173);

        // sets the horizontal holdup to the liquid content if smaller¨
        if(hz_holdup_seg < liquid_content) hz_holdup_seg = liquid_content;
        if(hz_holdup_int < liquid_content) hz_holdup_int = liquid_content;

        //correction factors
        double cor_seg;
        double cor_int;

        if(angle() < 0)
        {
            cor_seg = cor_int = (1 - liquid_content) * log(4.7 * pow(liquid_content, -0.3692) * pow(nlv, 0.1244) * pow(froude_no, -0.5056));
        }
        else
        {
            cor_seg = (1 - liquid_content) * log(0.011 * pow(liquid_content, -3.768) * pow(nlv, 3.539) * pow(froude_no, -1.614));
            cor_int = (1 - liquid_content) * log(2.96 * pow(liquid_content, -0.305) * pow(nlv, -0.4473) * pow(froude_no, -0.0978));
        }

        double phi_seg = 1 + cor_seg * (sin(3.14159265 * 1.8*angle() / 180) - 0.333 * pow(sin(3.14159265 * 1.8*angle() / 180), 3));
        double phi_int = 1 + cor_int * (sin(3.14159265 * 1.8*angle() / 180) - 0.333 * pow(sin(3.14159265 * 1.8*angle() / 180), 3));

        holdup = payne_cor * (frac * (hz_holdup_seg * phi_seg) + (1 - frac) * (hz_holdup_int * phi_int));
    }

    // done calculating the holdup

    if(holdup > 1.0) holdup = 1.0;


    // calculating pressure drop due to elevation change

    double den_s = den_l * holdup + den_g * (1 - holdup);  // two phase density

    double dp_el = den_s * sin(angle(true)) / 144;     // pressure drop due to elevation change


    // calculating friction factor
    double vis_g = gasViscosity(p, z_fac);

    double den_ns = den_l * liquid_content + den_g * (1 - liquid_content);      // no-slip density
    double vis_ns = liquidViscosity(s) * liquid_content + vis_g * (1 - liquid_content);       // no-slip viscosity

    double re_ns = 124*(den_ns * vm * d_in) / vis_ns;     // no-slip reynolds number

    double fn = 1 / (2 * log10(pow(re_ns / (4.5223 * log10(re_ns) - 3.8215), 2)));    // no-slip friction factor


    double y = liquid_content / pow(holdup, 2);


    double s_term;

    if(y > 1.0 && y < 1.2)
    {
        s_term = log(2.2*y -1.2);
    }
    else
    {
        s_term = log(y) / (-0.0523 + 3.182 * log(y) - 0.8725 * pow(log(y), 2) + 0.01853 * pow(log(y), 4));
    }


    double ftp = fn * exp(s_term);      // the friction factor



    // calculating pressure drop due to friction

    double dp_f = 5.176e-3 * (ftp * den_ns * pow(vm,2)) / (d_in);


    // calculating acceleration term

    double ek = 2.16e-4 * (den_ns * vm * vsg) / p_psi;



    // calculating total pressure drop

    double dp_tot = (dp_f + dp_el) / (1 - ek);  // total pressure drop per length of pipe (in psi / ft)

    // double dp_tot_bar = dp_tot / 14.5038 / 0.3048;       // total pressure drop in bar / m


    // converting length from m to ft

    double length_ft = length() * 3.28;

    // total pressure drop in psi
    double dp_psi_tot = dp_tot * length_ft;


    if(unit == Stream::FIELD) return dp_psi_tot;
    else return dp_psi_tot / 14.5037738;

}

double BeggsBrillCalculator::angle(bool rad)
{
    if(!rad) return m_angle;
    else return 3.14159265 * m_angle / 180;
}

