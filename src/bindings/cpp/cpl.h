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

    Declares functions that wrap the C-bindings from cpl/src/bindings/c/CPLC.h
    in the namespace CPL, such that CPL::function_name corresponds to
    CPLC_function_name.

Author(s)

    David Trevelyan

*/

#ifndef CPL_H_INCLUDED
#define CPL_H_INCLUDED
#include "CPL_ndArray.h"
#include <string>
#include <vector>
#include "CPLC.h"
#include <iostream>
#include "common.h"

// CPL namespace for C++ bindings
namespace CPL
{

    // Realm identifiers, also found in coupler_module.f90
    static const int cfd_realm = 1;
    static const int md_realm = 2;



#ifdef JSON_SUPPORT
	/** MODULE IO **/
	void load_param_file(const std::string fname);
	void close_param_file();
	void get_file_param(const std::string section, const std::string param_name, double& real_param);
	void get_file_param(const std::string section, const std::string param_name, std::vector<double> &real_param_array);
	void get_file_param(const std::string section, const std::string param_name, int& int_param);
	void get_file_param(const std::string section, const std::string param_name, std::vector<int> &int_param_array);
	void get_file_param(const std::string section, const std::string param_name, bool& boolean_param);
	void get_file_param(const std::string section, const std::string param_name, std::vector<bool> &boolean_param_array);
	void get_file_param(const std::string section, const std::string param_name, std::string& string_param);
	void get_file_param(const std::string section, const std::string param_name, std::vector<std::string> &string_param_array);
#endif 

    void init (int  calling_realm, int& returned_realm_comm); 
    void finalize(); 

    void setup_cfd (int icomm_grid, const DoubVector& xyzL, const DoubVector& xyz_orig, 
                    const IntVector& ncxyz);
    void setup_md (int icomm_grid, const CPL::DoubVector& xyzL, 
                   const CPL::DoubVector& xyz_orig);
    void set_timing (int initialstep, int nsteps, double dt);

    bool send (const DoubNdArray& asend, const CPL::IntVector& asend_shape, 
               const CPL::IntVector& limits);
    bool recv (CPL::DoubNdArray& arecv,	const CPL::IntVector& arecv_shape, 
               const CPL::IntVector& limits);

    void scatter (CPL::DoubVector& scatterarray, const CPL::IntVector& scatter_shape, 
                  const CPL::IntVector& limits, CPL::DoubVector& recvarray,
                  const CPL::IntVector& recv_shape);

    void gather (CPL::DoubVector& gatherarray, const CPL::IntVector& gather_shape,
                 const CPL::IntVector& limits, CPL::DoubVector& recvarray,
                 const CPL::IntVector& recv_shape);

    void proc_extents (const CPL::IntVector& coord, int realm, CPL::IntVector& extents);
    void my_proc_extents (CPL::IntVector& extents);
    void proc_portion (const CPL::IntVector& coord, int realm,
                       const CPL::IntVector& limits, CPL::IntVector& portion);
    void my_proc_portion (const CPL::IntVector& limits, CPL::IntVector& portion);
    void map_cell2coord (int i, int j, int k, CPL::DoubVector& coord_xyz);
    bool map_coord2cell (double x, double y, double z, CPL::IntVector& cell_ijk);
    void get_no_cells (const CPL::IntVector& limits, CPL::IntVector& no_cells);
    bool map_glob2loc_cell (const CPL::IntVector& limits, const CPL::IntVector& glob_cell,
                            CPL::IntVector& loc_cell);
    void get_olap_limits (CPL::IntVector& limits);
    void get_cnst_limits (CPL::IntVector& limits);
    void get_bnry_limits (CPL::IntVector& limits);
    bool map_cfd2md_coord (const CPL::DoubVector& cfd_coord, CPL::DoubVector& md_coord);
    bool map_md2cfd_coord (const CPL::DoubVector& md_coord, CPL::DoubVector& cfd_coord);
    bool overlap (); 
    bool is_proc_inside(const CPL::IntVector& region);
    void set_output_mode (int mode);

    // Getters
    template<class T>
    T get(std::string name)
    {
        T (*fp)();
        if (name == "nsteps_md") fp = reinterpret_cast<T(*)()> (&CPLC_nsteps_md); 
        else if (name == "nsteps_cfd") fp = reinterpret_cast<T(*)()> (&CPLC_nsteps_cfd); 
        else if (name == "nsteps_coupled") fp = reinterpret_cast<T(*)()> (&CPLC_nsteps_coupled); 
        else if (name == "ncx") fp = reinterpret_cast<T(*)()> (&CPLC_ncx);
        else if (name == "ncy") fp = reinterpret_cast<T(*)()> (&CPLC_ncy);
        else if (name == "ncz") fp = reinterpret_cast<T(*)()> (&CPLC_ncz);
        else if (name == "icmin_olap") fp = reinterpret_cast<T(*)()> (&CPLC_icmin_olap);
        else if (name == "jcmin_olap") fp = reinterpret_cast<T(*)()> (&CPLC_jcmin_olap);
        else if (name == "kcmin_olap") fp = reinterpret_cast<T(*)()> (&CPLC_kcmin_olap);
        else if (name == "icmax_olap") fp = reinterpret_cast<T(*)()> (&CPLC_icmax_olap);
        else if (name == "jcmax_olap") fp = reinterpret_cast<T(*)()> (&CPLC_jcmax_olap);
        else if (name == "kcmax_olap") fp = reinterpret_cast<T(*)()> (&CPLC_kcmax_olap);
        else if (name == "icmin_cnst") fp = reinterpret_cast<T(*)()> (&CPLC_icmin_cnst);
        else if (name == "jcmin_cnst") fp = reinterpret_cast<T(*)()> (&CPLC_jcmin_cnst);
        else if (name == "kcmin_cnst") fp = reinterpret_cast<T(*)()> (&CPLC_kcmin_cnst);
        else if (name == "icmax_cnst") fp = reinterpret_cast<T(*)()> (&CPLC_icmax_cnst);
        else if (name == "jcmax_cnst") fp = reinterpret_cast<T(*)()> (&CPLC_jcmax_cnst);
        else if (name == "kcmax_cnst") fp = reinterpret_cast<T(*)()> (&CPLC_kcmax_cnst);
        else if (name == "icmin_bnry") fp = reinterpret_cast<T(*)()> (&CPLC_icmin_bnry);
        else if (name == "jcmin_bnry") fp = reinterpret_cast<T(*)()> (&CPLC_jcmin_bnry);
        else if (name == "kcmin_bnry") fp = reinterpret_cast<T(*)()> (&CPLC_kcmin_bnry);
        else if (name == "icmax_bnry") fp = reinterpret_cast<T(*)()> (&CPLC_icmax_bnry);
        else if (name == "jcmax_bnry") fp = reinterpret_cast<T(*)()> (&CPLC_jcmax_bnry);
        else if (name == "kcmax_bnry") fp = reinterpret_cast<T(*)()> (&CPLC_kcmax_bnry);
        else if (name == "timestep_ratio") fp = reinterpret_cast<T(*)()> (&CPLC_timestep_ratio);
        else if (name == "dx") fp = reinterpret_cast<T(*)()> (&CPLC_dx);
        else if (name == "dy") fp = reinterpret_cast<T(*)()> (&CPLC_dy);
        else if (name == "dz") fp = reinterpret_cast<T(*)()> (&CPLC_dz);
        else if (name == "comm_style") fp = reinterpret_cast<T(*)()> (&CPLC_comm_style); 
        else if (name == "comm_style_gath_scat") fp = reinterpret_cast<T(*)()> (&CPLC_comm_style_gath_scat); 
        else if (name == "comm_style_send_recv") fp = reinterpret_cast<T(*)()> (&CPLC_comm_style_send_recv); 
        else if (name == "cpl_cfd_bc_x") fp = reinterpret_cast<T(*)()> (&CPLC_cpl_cfd_bc_x);
        else if (name == "cpl_cfd_bc_y") fp = reinterpret_cast<T(*)()> (&CPLC_cpl_cfd_bc_y);
        else if (name == "cpl_cfd_bc_z") fp = reinterpret_cast<T(*)()> (&CPLC_cpl_cfd_bc_z);
        else if (name == "cpl_md_bc_slice") fp = reinterpret_cast<T(*)()> (&CPLC_cpl_md_bc_slice);
        else if (name == "cpl_cfd_bc_slice") fp = reinterpret_cast<T(*)()> (&CPLC_cpl_cfd_bc_slice);
        else if (name == "cpl_x_orig_cfd") fp = reinterpret_cast<T(*)()> (&CPLC_x_orig_cfd);
        else if (name == "cpl_y_orig_cfd") fp = reinterpret_cast<T(*)()> (&CPLC_y_orig_cfd);
        else if (name == "cpl_z_orig_cfd") fp = reinterpret_cast<T(*)()> (&CPLC_z_orig_cfd);
        else if (name == "cpl_x_orig_md") fp = reinterpret_cast<T(*)()> (&CPLC_x_orig_md);
        else if (name == "cpl_y_orig_md") fp = reinterpret_cast<T(*)()> (&CPLC_y_orig_md);
        else if (name == "cpl_z_orig_md") fp = reinterpret_cast<T(*)()> (&CPLC_z_orig_md);

        T got = fp();

        return got;

    }

	/**
	 template<class T>
		const T& get_file_param(std::string section, std::string param_name)
		{
			T (*fp)(std::string, std::string, T(*));
			T got;
			if (std::is_same<T, int>::value) {
				std::cout << "Entra int" << std::endl;
//				CPLC_get_int_param(section.c_str(), param_name.c_str(), &got);
				fp = reinterpret_cast<T(*)(std::string, std::string, T&)> (&get_int_param); 
			}
			else if (std::is_same<T, std::vector<int>>::value) {
				std::cout << "Entra vector int" << std::endl;
				fp = reinterpret_cast<T(*)(std::string, std::string, T*)> (&get_int_array_param); 
				//std::vector<int> got;
				int array_len = 0;
				int* int_param_array_c;
				std::cout << "section: " << section << " paramname:" << param_name<< std::endl;
				CPLC_get_int_array_param(section.c_str(), param_name.c_str(), &int_param_array_c, &array_len);
				got.assign(int_param_array_c, int_param_array_c + array_len);
			}
			else if (std::is_same<T, double>::value) {
				std::cout << "Entra double" << std::endl;
				fp = reinterpret_cast<T(*)(std::string, std::string, T(*))> (&get_real_param); 
			}
			else if (std::is_same<T, std::vector<double>>::value) {
				std::cout << "Entra vector double" << std::endl;
				fp = reinterpret_cast<T(*)(std::string, std::string, T(*))> (&get_int_array_param); 
			}
			else
				std::cout << "NONE" << std::endl;


			fp(section, param_name, got);
			return got;
		}
**/
    double density_cfd();

}

#endif // CPL_H_INCLUDED
