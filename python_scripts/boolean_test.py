#!/usr/bin/env python3

# import pydot
# from sklearn.externals.six import StringIO

from sklearn.metrics import mean_squared_error
from sklearn.metrics import r2_score

import numpy as np
import sys

# Import the necessary modules and libraries
from sklearn import tree
import matplotlib.pyplot as plt

# import graphviz


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


def get_threshold2(point_mol, points_max, mols_max):

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
    # print(patterns_mols.shape)
    thresholds = get_threshold(point_mol, patterns_points, patterns_mols)

    dimensions = (y_size, len(patterns_mols))
    x_train = patterns_to_matrix(patterns_mols, dimensions)

    interaction = np.array([np.mean(mol_out[points])
                            for points in patterns_points])
    x_test = np.array([(interaction > thresholds)]).astype(int)

    return x_train, x_test


def predict(x_train_file, x_test_file, y_train,
            patterns_points_file,
            patterns_mols_file, k, m, min_samples):
    # base = 'X_train'

    # y_train = np.loadtxt(y_train_file)
    # x_train_file = x_train_file
    # patterns_points_file = base + 'Xtrain_points_' + str(k)
    # patterns_mols_file = base + 'Xtrain_mols_' + str(k)

    x_train, x_test = get_x_train_test(x_train_file, x_test_file,
                                       patterns_points_file,
                                       patterns_mols_file,
                                       len(y_train))

    # Fit regression model
    # regr_tree = tree.DecisionTreeRegressor(min_samples_leaf=int(k))
    regr_tree = tree.DecisionTreeRegressor(min_samples_leaf=min_samples)
    regr_tree.fit(x_train, y_train)

    # dot_data = StringIO()
    # tree.export_graphviz(regr_tree, out_file=dot_data)
    # graph = pydot.graph_from_dot_data(dot_data.getvalue())
    # graph[0].write_pdf(str(m) + 'tree.pdf')

    train_fit = regr_tree.predict(x_train)
    test_fit = regr_tree.predict(x_test)

    return train_fit, test_fit


def reg_tree_LOO(Y, o_dir, pat_dir, potential_type, data_type, distance, k,
                 min_samples):
    # x_train_file = 'X_train.txt'
    # y_train_file = 'Y_train.txt'
    # x_test_file = 'X_test.txt'

    base = 'T_%s_X_%s_t%d' % (potential_type, data_type, distance)

    o_dir += base + '/'
    pat_dir += base + '/'
    nmols = Y.shape[0]
    y_train_data = np.loadtxt('Y_data/Y_%s.txt' % data_type)
    # print(y_train_data[1:].shape)
    # predicted = []

    yFitCV = np.zeros(nmols)
    rmseModel = np.zeros(nmols)
    rmseCV = np.zeros(nmols)
    r2Model = np.zeros(nmols)
    corrModel = np.zeros(nmols)

    # for x_train_file, x_test_file, y_train_file in files:
    for m in range(1, nmols + 1):
        x_train_file = o_dir + base + '_Xtrain_' + str(m)
        x_test_file = o_dir + base + '_Xtest_' + str(m)

        pat_file = '%s%d/%s_Xtrain_%d_%s_%d' % (pat_dir, k, base, m, '%s', k)
        pat_points_file = pat_file % 'points'
        pat_mols_file = pat_file % 'mols'

        y_train = np.hstack((y_train_data[:m - 1], y_train_data[m:]))
        y_test = np.array([y_train_data[m - 1]])
        train_fit, test_fit = predict(x_train_file,
                                      x_test_file,
                                      y_train,
                                      pat_points_file,
                                      pat_mols_file, k, m, min_samples)

        yFitCV[m - 1] = test_fit
        rmseModel[m - 1] = mean_squared_error(y_train, train_fit)
        rmseCV[m - 1] = mean_squared_error(y_test, test_fit)
        r2Model[m - 1] = r2_score(y_train, train_fit)
        corrModel[m - 1] = correlation(y_train, train_fit)

    q2 = r2_score(Y, yFitCV)
    r2 = np.mean(r2Model)
    rmsecv = np.mean(rmseCV)
    rmse = np.mean(rmseModel)
    corrcv = correlation(Y, np.reshape(yFitCV, yFitCV.size))
    corrmdl = np.mean(corrModel)
    return q2, r2, rmsecv, rmse, corrcv, corrmdl


def plot(x, training, testing, label, file_type, outputPath, min_samples):
    plt.clf()
    plt.plot(x, testing, 'bs-', label='Testing', markersize=3)
    plt.plot(x, training, 'ro-', label='Training', markersize=3)

    plt.xlabel('Min Samples Leaf')
    plt.ylabel(label)
    titleName = 'Regression tree with ' + str(min_samples) + ' min_samples'
    plt.title(titleName)
    plt.legend(loc=4)
    plt.grid()
    plt.xlim((min(x), max(x)))
    # plt.ylim((0, 1))
    # plt.show()
    plt.savefig(outputPath + '_' + file_type + '.eps')


def regression(Y, o_dir, pat_dir, outputPath, potential_type, data_type,
               distance, cols, min_samples):
    results = np.empty([0, 7])
    for k in range(1, cols):
        q2, r2, rmsecv, rmse, corrcv, corrmdl = reg_tree_LOO(Y, o_dir, pat_dir,
                                                             potential_type,
                                                             data_type,
                                                             distance, k,
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

    plot(x, training, testing, 'R²', 'score', outputPath, min_samples)
    plot(x, training_error, testing_error, 'Mean squared error', 'error',
         outputPath, min_samples)
    np.savetxt(outputPath + '.txt', np.reshape(maxPerformance, (1, -1)),
               delimiter=',', fmt='%f')


def main(o_dir, pat_dir, outputPath, potential_type, data_type, distance):

    # X = np.loadtxt('data/%s/X_%s_t%d.txt' % (data_type, data_type, distance))
    Y = np.loadtxt('data/%s/Y_%s.txt' % (data_type, data_type))

    cols = int((Y.shape[0] - 1) / 2)

    for min_samples in range(1, cols):
        print('Min Samples ' + str(min_samples))
        regression(Y, o_dir, pat_dir, '%s_%d_' % (outputPath, min_samples),
                   potential_type, data_type, distance, cols, min_samples)


if __name__ == '__main__':
    # o_dir = sys.argv[1]
    # pat_dir = sys.argv[2]
    # min_samples = int(sys.argv[1])
    potential_type = 'C'
    data_type = 'TP'
    distance = 10
    potential_type = sys.argv[1]
    data_type = sys.argv[2]
    distance = int(sys.argv[3])
    outfile = '%s_%s_%d' % (potential_type, data_type, distance)
    print(outfile)
    outfile = 'plots/outfile_' + outfile
    o_dir = 'loo_data/'
    pat_dir = 'loo_output/'

    main(o_dir, pat_dir, outfile, potential_type, data_type, distance)
