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


def get_threshold(point_mol, points_max, mols_max, points_min, mols_min):

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
    y_file_test = 'Y_test.txt'
    matrix_file = 'X_train.txt'
    base = 'X_train'
    base_max = base + '_max'
    base_min = base + '_min'
    k = str(5)

    # patterns_limit_max = np.loadtxt(base_max + '_limit_' + k)
    # patterns_limit_min = np.loadtxt(base_min + '_limit_' + k)

    # print(len(patterns_limit_max))
    # print(len(patterns_limit_min))
    point_mol = np.loadtxt(matrix_file)
    print(point_mol.shape)

    patterns_points_max = loadtxt(base_max + '_points_' + k)
    patterns_points_min = loadtxt(base_min + '_points_' + k)

    print(len(patterns_points_max))
    print(len(patterns_points_min))

    patterns_mols_max = np.loadtxt(base_max + '_patterns_' + k, dtype=str)
    patterns_mols_min = np.loadtxt(base_min + '_patterns_' + k, dtype=str)

    print(len(patterns_mols_max))
    print(len(patterns_mols_min))

    mols_max = np.array([name2mol(pat) for pat in patterns_mols_max])
    mols_min = np.array([name2mol(pat) for pat in patterns_mols_min])

    print(mols_max)
    print(len(mols_max))

    thresholds = get_threshold(point_mol,
                               patterns_points_max, mols_max,
                               patterns_points_min, mols_min)

    print(thresholds)
    # y = np.array([6.4, 7.2, 6.5, 3.8])

    y = np.loadtxt(y_train_file)
    # print(len(y))

    sh = (len(y), len(mols_max))
    bool_matrix = np.zeros(sh)

    for i, p in enumerate(mols_max):
        bool_matrix.T[i, p] = 1

    x = bool_matrix
    # print(x.shape)
    # print(y.shape)

    # # Fit regression model
    # # regr_1 = tree.DecisionTreeRegressor(max_depth=2)
    # regr_2 = tree.DecisionTreeRegressor(max_depth=5)
    # # regr_1.fit(X, y)
    # regr_2.fit(X, y)

    # X_test = np.loadtxt('X_test.txt')
    # y_2 = regr_2.predict(X_test)

    # # # Predict
    # # X_test = np.arange(0.0, 5.0, 0.01)[:, np.newaxis]
    # # y_1 = regr_1.predict(X_test)
    # # y_2 = regr_2.predict(X_test)

    # # # Plot the results
    # # plt.figure()
    # # plt.scatter(X, y, s=20, edgecolor="black",
    # #             c="darkorange", label="data")
    # # plt.plot(X_test, y_1, color="cornflowerblue",
    # #          label="max_depth=2", linewidth=2)
    # # plt.plot(X_test, y_2, color="yellowgreen", label="max_depth=5", linewidth=2)
    # # plt.xlabel("data")
    # # plt.ylabel("target")
    # # plt.title("Decision Tree Regression")
    # # plt.legend()
    # # plt.show()

    # dot_data = tree.export_graphviz(regr_2, out_file='gg',
    #                                 filled=True, rounded=True,)
    #                                 # special_characters=True)
    # graph = graphviz.Source(dot_data)
    # # graph.render("iris")


main()
