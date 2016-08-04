//
// Created by bellout on 8/4/16.
//

#include "test_resource_wic_diff_functions.h"
//#include "test_resource_wic_widata.h"
#include <iostream>

// Variables:
//Matrix<int, Dynamic, 4> IJK;
//Matrix<double,Dynamic,1> WCF;

bool TestResources::WIBenchmark::DiffVectorLength(WIData va, WIData vb){

    bool vector_diff = true;
    if (va.IJK.rows() != vb.IJK.rows()){
        vector_diff = false;
    }
    return vector_diff;
}


bool TestResources::WIBenchmark::CompareIJK(WIData va, WIData vb){

    auto vdiff = va.IJK - vb.IJK;

    std::cout << "WIDataPCG.IJK " << vdiff << std::endl;

    return true;
}

