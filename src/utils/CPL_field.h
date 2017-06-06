
//=====================================================
// Class of type CPLField
//    In the interest of dependency injection, a field 
//    class with dxyz, min, max, interpolation and get 
//    cell. Build with CPL_ndArray as dependency and 
//    use as input to constructor of CPLField.
//=====================================================

#ifndef CPLField_H_
#define CPLField_H_

#include <vector>
#include <iostream>

#include "CPL_ndArray.h"

namespace CPL{

class CPLField{

public:

    //Constructors
    CPLField(int nd, int icell, int jcell, int kcell);
    CPLField(CPL::ndArray<double> arrayin);

    //Getters and setters
    void set_array(CPL::ndArray<double> arrayin);
    void set_minmax(double min_in[], double max_in[]);
    void set_dxyz();

    CPL::ndArray<double> get_array();

    //Get cell values
    std::vector<int> get_cell(double r[]);
    std::vector<double> get_dA();    
    std::vector<double> interpolate(double r[]);    

    //Destructor
    virtual ~CPLField() {}

    //Variables
    CPL::ndArray<double> array;

//private:

    double min[3], max[3], dxyz[3], dA[3], dV;

};

}

#endif  // CPLField_H_