/*

    ________/\\\\\\\\\__/\\\\\\\\\\\\\____/\\\_____________
     _____/\\\////////__\/\\\/////////\\\_\/\\\_____________
      ___/\\\/___________\/\\\_______\/\\\_\/\\\_____________
       __/\\\_____________\/\\\\\\\\\\\\\/__\/\\\_____________
        _\/\\\_____________\/\\\/////////____\/\\\_____________
         _\//\\\____________\/\\\_____________\/\\\_____________
          __\///\\\__________\/\\\_____________\/\\\_____________
           ____\////\\\\\\\\\_\/\\\_____________\/\\\\\\\\\\\\\\\_
            _______\/////////__\///______________\///////////////__


                         C P L  -  L I B R A R Y

           Copyright (C) 2012-2015 Edward Smith & David Trevelyan

License

    This file is part of CPL-Library.

    CPL-Library is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    CPL-Library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with CPL-Library.  If not, see <http://www.gnu.org/licenses/>.


Description

    Utility class that provides Fortran-like N-dimensional arrays (up to
    4D) to C++ programs that are to interface with CPL-Library. The ordering
    of the internal data is stored in Fortran column-major order, so objects
    of this class may be communicated directly with CPL-Library without any
    need for re-ordering.

    The () operator is overloaded to provide a simple interface that is
    identical to Fortran-array element access (except that indices count
    from 0, rather than 1).

        constexpr arrayDims = 3;
        int arrayShape[arrayDims] = {3, 5, 8};
        CPL::ndArray<int> A(arrayDims, arrayShape)
        A(1, 0, 7) = 314; // Sets element 1,0,7 to 314

Author(s)
    
    David Trevelyan, Eduardo Ramos

*/

#ifndef CPL_NDARRAY_H_INCLUDED
#define CPL_NDARRAY_H_INCLUDED

#include <exception>
#include <string>
#include <sstream>
#include <vector>
#include "common.h"

namespace CPL
{


    // N-dimensional array class where the data is guaranteed to be contiguous
    template<class T>
    class ndArray
    {

    public:

        // Constructor with no arguments, don't allocate
        ndArray();

        // Constructor taking in number of dimensions, shape and optional order
        ndArray(const CPL::IntVector& shape, char ordering=defaultOrder);
    
        // Construct from pointer to existing data, number of dimensions and
        // shape, with optional ordering
        ndArray(const T* existingData, const CPL::IntVector& shape,
                char ordering=defaultOrder);

        // Destructor automatically generated by compiler
        //~ndArray();
        // Deallocate data, resizes to 0
        void clear();

//        //Need value type, begin, end and iterator here
//        typedef T value_type;
//        std::begin();
//        std::end();
//        std::const_iterator();

//        iterator begin() { return 0; }
//        iterator end() { return this.size(); }
//        iterator iter() {
//            for( iter it = this.begin(); it != this.end(); ++i ) {
//                // use *i ...
//            }
//        }

        // Number of elements of the ndArray along the dim axis
        int shape (int dim) const;


        // Pointer to the front of the shapeVector array
        const CPL::IntVector& shape() const;

        // Pointer to front of internal array
        T* data();
        const T* data() const;

        // Number of elements
        int size();

        // Min/max functions
        double min();
        double max();

        // Allocation after empty construction
        void resize (const CPL::IntVector& shape);

        // Element access - support up to 4 dimensions
        // 1D
        T operator ()  (int i0) const;
        T& operator () (int i0);

        // 2D
        T operator ()  (int i0, int i1) const;
        T& operator () (int i0, int i1);

        // 3D
        T operator ()  (int i0, int i1, int i2) const;
        T& operator () (int i0, int i1, int i2);

        // 4D
        T operator ()  (int i0, int i1, int i2, int i3) const;
        T& operator () (int i0, int i1, int i2, int i3);

        // Assignment operator from single value
        ndArray& operator= (const T &rhs);

        // Arithmetic operators from single value
        ndArray& operator+= (const T &rhs);
        ndArray& operator-= (const T &rhs);
        ndArray& operator*= (const T &rhs);
        ndArray operator+ (const T &rhs);
        ndArray operator- (const T &rhs);
        ndArray operator* (const T &rhs);

        // Check dimensionality is a certain value
        bool checkDimsEquals (int dims) const;

        // Return string information about the object
        std::string info();

        // Return string all the elements of the internal data
        std::string infoData();

        // Print the array prettily
        std::string infoArray();


    private:

        // Internal 1D data array
        std::vector<T> ndArrayData;

        CPL::IntVector shapeVector;

        // Number of dimensions
        int nDims;

        // C or Fortran ordering
        static const char defaultOrder = 'F';
        char order;
        
        // Shape (size in each dimension)

        // Number of elements
        int nElements;

        // Exceptions
        class wrongShape: public std::exception
        {
            const char* what() const throw()
            {
                return "Wrong number of arguments to ndArray.operator()";
            }
        } shapeEx;
        
        class invalidOrder: public std::exception
        {
            const char* what() const throw()
            {
                return "CPL::ndArray ordering may only be 'F' or 'C'";
            }
        } orderEx;

        // Convert nd indices to 1D index position
        int flatIndex(int i0) const;
        int flatIndex(int i0, int i1) const;
        int flatIndex(int i0, int i1, int i2) const;
        int flatIndex(int i0, int i1, int i2, int i3) const;

    };

    // Typedefs
    typedef ndArray<double> DoubNdArray;

}

#endif // CPL_NDARRAY_H_INCLUDED
