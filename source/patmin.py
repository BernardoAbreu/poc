#! /usr/bin/env python2


import sys
import patmin
import numpy as np


def read_csv(filename, sep=None):
    with open(filename, 'r') as f:
        return [list(map(float, line.split(sep))) for line in f]


def rev_tup(t):
    return (t[1], t[0])


def build_matrix(matrix, max=True):
    return [sorted(list(enumerate(row)), key=rev_tup, reverse=max)
            for row in matrix]


filename = sys.argv[1]

# matrix = build_matrix(read_csv(filename, sep=';'))
matrix = build_matrix(np.loadtxt(filename, delimiter=';').T)
results = patmin.mine_patterns(matrix, 10)

for mol in results[0]:
    print(mol)

print('\n')

for point in results[1]:
    print(point)
