#! /usr/bin/env python


import sys
import patmin


def read_csv(filename):
    with open(filename, 'r') as f:
        return [list(map(float, line.split())) for line in f]


def rev_tup(t):
    return (t[1], t[0])


def build_matrix(matrix, max=True):
    return [sorted(list(enumerate(row)), key=rev_tup, reverse=max)
            for row in matrix]


filename = sys.argv[1]

matrix = build_matrix(read_csv(filename))

print(patmin.mine_patterns(matrix, 10))
