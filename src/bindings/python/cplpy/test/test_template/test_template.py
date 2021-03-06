#!/usr/bin/env python2
import pytest
from cplpy import run_test, prepare_config, get_test_dir
import os

# -----MAPPING TESTS-----

# EXPLANATION: These tests fail due to no_procs(MD) != k*no_procs(CFD),
#              k in [1,2,3,...] in one direction.

MD_FNAME = "md_test.py"
MD_ARGS = MD_FNAME
MD_EXEC = "python2"
CFD_FNAME = "cfd_test.py"
CFD_ARGS = CFD_FNAME
CFD_EXEC = "python2"
TEST_TEMPLATE_DIR = os.path.join(get_test_dir(), "templates")
TEST_DIR = os.path.dirname(os.path.realpath(__file__))


@pytest.fixture()
def prepare_config_fix(tmpdir):
    prepare_config(tmpdir, TEST_DIR, MD_FNAME, CFD_FNAME)


@pytest.mark.parametrize("cfdprocs, mdprocs, err_msg", 
                         [((1, 1, 1), (1, 1, 1), "")])
def test_CPL_map_coord2cell(prepare_config_fix, cfdprocs, mdprocs, err_msg):
    MD_PARAMS = {"lx": 24.0, "ly": 24.0, "lz": 24.0,
                 "which_test": "coord_test"}
    MD_PARAMS["npx"], MD_PARAMS["npy"], MD_PARAMS["npz"] = mdprocs

    CFD_PARAMS = {"lx": 24.0, "ly": 24.0, "lz": 24.0,
                  "ncx": 24, "ncy": 24, "ncz": 24,
                  "which_test": "coord_test"}
    CFD_PARAMS["npx"], CFD_PARAMS["npy"], CFD_PARAMS["npz"] = cfdprocs

    CONFIG_PARAMS = {"cfd_bcx": 1, "cfd_bcy": 1, "cfd_bcz": 1,
                     "olap_xlo": 1, "olap_xhi": 24,
                     "olap_ylo": 1, "olap_yhi": 4,
                     "olap_zlo": 1, "olap_zhi": 24,
                     "cnst_xlo": 1, "cnst_xhi": 1,
                     "cnst_ylo": 1, "cnst_yhi": 1,
                     "cnst_zlo": 1, "cnst_zhi": 1,
                     "tstep_ratio": 50, }

    run_test(TEST_TEMPLATE_DIR, CONFIG_PARAMS, MD_EXEC, MD_FNAME, MD_ARGS,
             CFD_EXEC, CFD_FNAME, CFD_ARGS, MD_PARAMS, CFD_PARAMS, err_msg)

