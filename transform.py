#!/usr/bin/env python3

import numpy as np
import sys


def rev_tup(t):
    return (t[1], t[0])


def save_matrix(matrix, filename):
    rows, cols, _ = matrix.shape
    with open(filename, 'w') as f:
        f.write(f'{rows},{cols}\n')
        for i in range(rows):
            for j in range(cols):
                # print(f'{matrix[i][j][0]},{matrix[i][j][1]} ')
                f.write(f'{matrix[i][j][0]},{matrix[i][j][1]} ')
            f.write('\n')


def main(filename, output_name):
    matrix = np.loadtxt(filename)

    output = np.array([sorted(list(enumerate(row)), key=rev_tup, reverse=True)
                       for row in matrix])

    print('Matrix built')
    save_matrix(np.transpose(output, (1, 0, 2)), output_name)
    print('File saved')


if __name__ == '__main__':
    filename = sys.argv[1]
    output_name = sys.argv[2]
    main(filename, output_name)
