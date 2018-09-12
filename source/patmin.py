#! /usr/bin/env python
#  Copyright Joel de Guzman 2002-2007. Distributed under the Boost
#  Software License, Version 1.0. (See accompanying file LICENSE_1_0.txt
#  or copy at http://www.boost.org/LICENSE_1_0.txt)
#  Hello World Example from the tutorial

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

matrix = read_csv(filename)
print(matrix)
matrix = build_matrix(matrix)
for row in matrix:
    print(' '.join(map(str, row)))


print(patmin.greet())
l = [(1, 1), (2, 4), (3, 9), (4, 16), (5, 25)]
# print(hello_ext.sum(l))
# print(hello_ext.create())
print(patmin.mine_patterns(matrix))
