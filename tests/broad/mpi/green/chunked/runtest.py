#!/usr/bin/env python3
#
# Verify that SOFT generates the same output when
# run with MPI as when run without MPI.
# ####################

import sys
if __name__ == '__main__':
    sys.path.append('../../..')

import broadutil
import h5py
import numpy as np
import os.path

TOLERANCE  = 5e-7
NPROCESSES = 4

def run(soft):
    """
    Run this test

    soft: Path to SOFT executable
    """
    global TOLERANCE, NPROCESSES

    # Run with MPI
    print('Running SOFT with MPI...')
    broadutil.runSOFT_mpi("<pi>\nnum_threads=1;", NPROCESSES)

    # Load output
    gfval_mpi = 0.0
    for i in range(0, NPROCESSES):
        gf = broadutil.Green("green{0}.mat".format(i))
        gfval_mpi += np.sum(gf.func)

    # Run without MPI
    print('Running SOFT without MPI...')
    broadutil.runSOFT("<pi>\n@RadiationOutput green { mpi_mode = contiguous; }")

    # Load all output
    gf = broadutil.Green("green.mat")
    gfval = np.sum(gf.func)

    dgf = np.abs((gfval - gfval_mpi)/gfval)

    success = True
    if dgf > TOLERANCE:
        print("ERROR: Green's function was not computed properly when using MPI. Delta = {0:.5e}".format(dgf))
        success = False

    return success


if __name__ == '__main__':
    if len(sys.argv) == 1:
        broadutil.init()
        sys.exit(0 if run('../../../../build/src/soft') else -1)
    else:
        print('ERROR: Unrecognized number of arguments.')
        sys.exit(-1)

