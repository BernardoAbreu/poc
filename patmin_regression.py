#!/usr/bin/env python3

from sklearn.metrics import mean_squared_error
from sklearn.metrics import r2_score
from sklearn.model_selection import LeaveOneOut

import numpy as np
import sys
import patmin

# Import the necessary modules and libraries
from sklearn import tree
import matplotlib.pyplot as plt


def correlation(x, y):

    my = np.mean(y)
    mx = np.mean(x)
    c = (y - my).T.dot(x - mx)
    sigy = np.sqrt(np.sum((y - my)**2))
    sigx = np.sqrt(np.sum((x - mx)**2))
    if sigy == 0 or sigx == 0:
        return 0.0
    p = c / (sigy * sigx)
    return p


def loadtxt(filename, dtype=int, delimiter=','):
    with open(filename, 'r') as f:
        return [list(map(dtype, line.split(delimiter))) for line in f]


def plot(x, training, testing, label, file_type, outputPath, min_samples):
    plt.clf()
    plt.plot(x, testing, 'bs-', label='Testing', markersize=3)
    plt.plot(x, training, 'ro-', label='Training', markersize=3)

    plt.xlabel('K')
    plt.ylabel(label)
    titleName = 'Regression tree with ' + str(min_samples) + ' min_samples'
    plt.title(titleName)
    plt.legend(loc=4)
    plt.grid()
    plt.xlim((min(x), max(x)))
    # plt.ylim((0, 1))
    # plt.show()
    plt.savefig(outputPath + '_' + file_type + '.eps')


def get_threshold(point_mol, points_max, mols_max, top=False):

    mols_mask = np.zeros(point_mol.shape, dtype=bool)

    for i, mols in enumerate(mols_max):
        mols_mask[i][mols] = True

    max_threshold = []
    min_threshold = []

    for i, points in enumerate(points_max):
        pat = point_mol[np.ix_(points, mols_mask[i])]
        pat_max = point_mol[np.ix_(points, ~mols_mask[i])]
        if top:
            pat, pat_max = pat_max, pat
        min_threshold.append(np.mean(np.amax(pat, axis=0)))
        max_threshold.append(np.mean(np.amin(pat_max, axis=0)))

    max_threshold = np.array(max_threshold)
    min_threshold = np.array(min_threshold)

    thresholds = (max_threshold + min_threshold) / 2

    return thresholds


def patterns_to_matrix(patterns_mols, dimensions):
    bool_matrix = np.zeros(dimensions)

    for i, p in enumerate(patterns_mols):
        bool_matrix.T[i, p] = 1

    return bool_matrix


def rev_tup(t):
    return (t[1], t[0])


def build_matrix(matrix, max=True):
    return [sorted(list(enumerate(row)), key=rev_tup, reverse=max)
            for row in matrix]


def reg_tree_LOO(x_data, y_data, k, min_samples):
    nmols = y_data.shape[0]

    yFitCV = np.zeros(nmols)
    rmseModel = np.zeros(nmols)
    rmseCV = np.zeros(nmols)
    r2Model = np.zeros(nmols)
    corrModel = np.zeros(nmols)

    loo = LeaveOneOut()
    for i, (train_index, test_index) in enumerate(loo.split(x_data, y_data)):
        X_train, X_test = x_data[train_index].T, x_data[test_index].T
        y_train, y_test = y_data[train_index], y_data[test_index]

        print(f'Mining leaving out {i}')
        matrix = build_matrix(X_train)
        patterns_mols, patterns_points = patmin.mine_patterns(matrix, k)
        print('Done')

        thresholds = get_threshold(x_data.T, patterns_points, patterns_mols)

        dimensions = (len(y_train), len(patterns_mols))
        x_train_matrix = patterns_to_matrix(patterns_mols, dimensions)

        interaction = np.array([np.mean(X_test[points])
                                for points in patterns_points])
        x_test_matrix = np.array([(interaction < thresholds)]).astype(int)

        # Fit regression model
        regr_tree = tree.DecisionTreeRegressor(min_samples_leaf=min_samples)
        regr_tree.fit(x_train_matrix, y_train)

        train_fit = regr_tree.predict(x_train_matrix)
        test_fit = regr_tree.predict(x_test_matrix)

        yFitCV[i] = test_fit
        rmseModel[i] = mean_squared_error(y_train, train_fit)
        rmseCV[i] = mean_squared_error(y_test, test_fit)
        r2Model[i] = r2_score(y_train, train_fit)
        corrModel[i] = correlation(y_train, train_fit)

    q2 = r2_score(y_data, yFitCV)
    r2 = np.mean(r2Model)
    rmsecv = np.mean(rmseCV)
    rmse = np.mean(rmseModel)
    corrcv = correlation(y_data, np.reshape(yFitCV, yFitCV.size))
    corrmdl = np.mean(corrModel)
    return q2, r2, rmsecv, rmse, corrcv, corrmdl


def regression(x_data, y_data, cols, min_samples, out_path):
    results = np.empty([0, 7])
    for k in range(1, cols):
        q2, r2, rmsecv, rmse, corrcv, corrmdl = reg_tree_LOO(x_data, y_data, k,
                                                             min_samples)

        output = np.array([k, q2, r2, rmsecv, rmse, corrcv, corrmdl])
        results = np.vstack((results, output))
        print('k', k, 'Q2', q2)

    maxPerformance = results[np.argmax(results, axis=0)[1], :]
    training = results[:, 2]
    testing = results[:, 1]
    training_error = results[:, 4]
    testing_error = results[:, 3]
    x = list(range(1, cols))

    plot(x, training, testing, 'R²', 'score', out_path, min_samples)
    plot(x, training_error, testing_error, 'Mean squared error', 'error',
         out_path, min_samples)
    np.savetxt(out_path + '.txt', np.reshape(maxPerformance, (1, -1)),
               delimiter=',', fmt='%f')


def main(input_file, output_file, out_path):

    x_data = np.loadtxt(input_file, delimiter=';')
    y_data = np.loadtxt(output_file)

    cols = int((y_data.shape[0] - 1) / 2)

    for min_samples in range(1, cols):
        print('Min Samples ' + str(min_samples))
        regression(x_data, y_data, cols, min_samples, out_path)


if __name__ == '__main__':
    infile = sys.argv[1]
    outfile = sys.argv[2]
    out_path = sys.argv[3]
    main(infile, outfile, out_path)
