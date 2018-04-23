#!/usr/bin/env  python3

import numpy as np
from sys import argv


def split(input_file):
    a = np.loadtxt(input_file)

    a = np.loadtxt(input_file)

    coulomb, lennard = np.split(a, 2, axis=1)
    np.savetxt('T_C_' + input_file.split('/')[-1], coulomb.T)
    np.savetxt('T_L_' + input_file.split('/')[-1], lennard.T)


if __name__ == '__main__':
    input_file = argv[1]
    split(input_file)
