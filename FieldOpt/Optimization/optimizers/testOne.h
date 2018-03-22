//
// Created by joakim on 06.03.18.
//

#ifndef FIELDOPT_TESTONE_H
#define FIELDOPT_TESTONE_H


#include "Optimization/optimizer.h"
#include "FieldOpt-3rdPartySolvers/handlers/SNOPTLoader.h"
#include "FieldOpt-3rdPartySolvers/handlers/SNOPTHandler.h"
#include <Eigen/Dense>


namespace Optimization {
    namespace Optimizers {

        /*
        typedef struct what what_t;
#ifdef __cplusplus
        extern "C" {
#endif

        struct what{
            int var1 = 0;
            int (*func)(integer    *Status, integer *n,    doublereal x[],
                                  integer    *needF,  integer *neF,  doublereal F[],
                                  integer    *needG,  integer *neG,  doublereal G[],
                                  char       *cu,     integer *lencu,
                                  integer    iu[],    integer *leniu,
                                  doublereal ru[],    integer *lenru) ;
            void (*setGradient)(int a);
        };


#ifdef __cplusplus
        }
#endif
*/

#ifdef __cplusplus
        extern "C" {
#endif
        int mySNOPTusrFG_( integer    *Status, integer *n,    doublereal x[],
                         integer    *needF,  integer *neF,  doublereal F[],
                         integer    *needG,  integer *neG,  doublereal G[],
                         char       *cu,     integer *lencu,
                         integer    iu[],    integer *leniu,
                         doublereal ru[],    integer *lenru);
#ifdef __cplusplus
        }
#endif



        class testOne {

        private:
            SNOPTHandler initSNOPTHandler();
            void setOptionsForSNOPT(SNOPTHandler& snoptHandler_);



            Settings::Optimizer *settings_;
            SNOPTHandler snoptHandler_;



        public:
            bool loadSNOPT(string libname = "libsnopt-7.2.12.2.so");
            void seta1(int a);
            void init();
            void seta2(int a);
            void setQuadraticModel(double c, Eigen::VectorXd gradient, Eigen::MatrixXd hessian);
            void setGradient(Eigen::VectorXd gradient);
            void setHessian(Eigen::MatrixXd hessian);
            void setConstant(double constant);
            void callSNOPT();
            testOne(Settings::Optimizer *settings);
            ~testOne();

        };


    }
}


#endif //FIELDOPT_TESTONE_H
