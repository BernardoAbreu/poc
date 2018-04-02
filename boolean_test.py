#!/usr/bin/env python3

import numpy as np

# Import the necessary modules and libraries
from sklearn import tree
import matplotlib.pyplot as plt

import graphviz


def name2mol(x):
    return [int(mol) for mol in x.split((','))]


def loadtxt(filename, dtype=int, delimiter=','):
    with open(filename, 'r') as f:
        return [list(map(dtype, line.split(delimiter))) for line in f]


def get_threshold(point_mol, points_max, mols_max):

    mols_mask = np.zeros(point_mol.shape, dtype=bool)

    for i, mols in enumerate(mols_max):
        mols_mask[i][mols] = True

    max_threshold = []
    min_threshold = []

    for i, points in enumerate(points_max):
        pat = point_mol[np.ix_(points, mols_mask[i])]
        pat_min = point_mol[np.ix_(points, ~mols_mask[i])]
        max_threshold.append(np.mean(np.amin(pat, axis=0)))
        min_threshold.append(np.mean(np.amax(pat_min, axis=0)))

    max_threshold = np.array(max_threshold)
    min_threshold = np.array(min_threshold)

    thresholds = (max_threshold + min_threshold) / 2

    return thresholds


def main():
    y_train_file = 'Y_train.txt'
    matrix_file = 'X_train.txt'
    base = 'X_train'
    base_max = base + '_max'
    k = 5

    point_mol = np.loadtxt(matrix_file)

    patterns_points_max = loadtxt(base_max + '_points_' + str(k))

    patterns_mols_max = np.loadtxt(base_max + '_patterns_' + str(k), dtype=str)

    mols_max = np.array([name2mol(pat) for pat in patterns_mols_max])

    thresholds = get_threshold(point_mol, patterns_points_max, mols_max)

    y = np.loadtxt(y_train_file)

    sh = (len(y), len(mols_max))
    bool_matrix = np.zeros(sh)

    for i, p in enumerate(mols_max):
        bool_matrix.T[i, p] = 1

    x_train = bool_matrix

    mol_out = np.loadtxt('X_test.txt')

    interaction = np.array([np.mean(mol_out[points])
                            for points in patterns_points_max])

    x_test = np.array([(interaction > thresholds)]).astype(int)

    # Fit regression model
    regr_tree = tree.DecisionTreeRegressor(min_samples_leaf=k)
    regr_tree.fit(x_train, y)

    y_2 = regr_tree.predict(x_test)
    print(y_2)


main()
