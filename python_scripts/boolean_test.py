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


def patterns_to_matrix(patterns_mols, dimensions):
    bool_matrix = np.zeros(dimensions)

    for i, p in enumerate(patterns_mols):
        bool_matrix.T[i, p] = 1

    return bool_matrix


def get_patterns_points(patterns_points_file):
    return loadtxt(patterns_points_file)


def get_patterns_mols(patterns_mols_file):
    patterns_pat = np.loadtxt(patterns_mols_file, dtype=str)
    return np.array([name2mol(pat) for pat in patterns_pat])


def get_x_train_test(x_train_file, x_test_file,
                     patterns_points_file, patterns_mols_file, y_size):

    point_mol = np.loadtxt(x_train_file)
    mol_out = np.loadtxt(x_test_file)

    patterns_points = get_patterns_points(patterns_points_file)
    patterns_mols = get_patterns_mols(patterns_mols_file)
    thresholds = get_threshold(point_mol, patterns_points, patterns_mols)

    dimensions = (y_size, len(patterns_mols))
    x_train = patterns_to_matrix(patterns_mols, dimensions)

    interaction = np.array([np.mean(mol_out[points])
                            for points in patterns_points])
    x_test = np.array([(interaction > thresholds)]).astype(int)

    return x_train, x_test


def predict(x_train_file, x_test_file, y_train_file, k):
    base = 'X_train'

    y_train = np.loadtxt(y_train_file)

    patterns_points_file = base + '_points_' + str(k)
    patterns_mols_file = base + '_mols_' + str(k)

    x_train, x_test = get_x_train_test(x_train_file, x_test_file,
                                       patterns_points_file,
                                       patterns_mols_file,
                                       len(y_train))

    # Fit regression model
    regr_tree = tree.DecisionTreeRegressor(min_samples_leaf=int(k))
    regr_tree.fit(x_train, y_train)

    y_predicted = regr_tree.predict(x_test)
    return y_predicted


def main():
    x_train_file = 'X_train.txt'
    y_train_file = 'Y_train.txt'
    x_test_file = 'X_test.txt'
    k = 5

    predicted = []
    for x_train_file, x_test_file, y_train_file in files:
        predicted.append(predict(x_train_file, x_test_file, y_train_file, k))


if __name__ == '__main__':
    main()
