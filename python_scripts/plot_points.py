#!/usr/bin/env python

from mpl_toolkits.mplot3d import Axes3D
import matplotlib.pyplot as plt
import numpy as np
from sys import argv
from mpl_toolkits.axes_grid1 import ImageGrid


def loadtxt(file, delimiter=','):
    with open(file, 'r') as f:
        return [list(map(int, line[:-1].split(delimiter))) for line in f]


def plot(coord, points):

    # toPlot = points_coord[points_list, :]

    fig = plt.figure()
    ax = fig.add_subplot(111, projection='3d')
    for s in points:
        toPlot = coord[s, :]
        ax.scatter(toPlot[:, 0], toPlot[:, 1], toPlot[:, 2])
    # ax.scatter(toPlot[:, 0], toPlot[:, 1], toPlot[:, 2])
    # ax.scatter(points_coord[:, 0], points_coord[:, 1], points_coord[:, 2])
    ax.set_xlabel('X Label')
    ax.set_ylabel('Y Label')
    ax.set_zlabel('Z Label')

    plt.show()


def plot2d(coord, pat_points, mols, pat_mols):
    toPlot = coord[points[0], :]
    # print(mols.shape)
    # print(len(pat_mols))
    index = 3
    s = pat_points[index]
    m = pat_mols[index]

    plt.figure(1)

    for n in range(0, len(m), 4):
        plt.clf()
        # for s in pat_points:
        for i in range(4):
            plt.subplot(221 + i)
            toPlot = coord[s, :]
            colors = mols[s, :][:, m][:, i + n]
            # print(toPlot)
            # plt.scatter(toPlot[:, 0], toPlot[:, 1], c=colors)
            plt.scatter(toPlot[:, 0], toPlot[:, 2], c=colors)
            # plt.scatter(toPlot[:, 1], toPlot[:, 2], c=colors)
            # plt.plot(toPlot[:, 1], toPlot[:, 2])

        plt.show()


def plot2d1(coord, pat_points, mols, pat_mols):
    toPlot = coord[points[0], :]
    print(mols.shape)
    print(len(pat_mols))
    index = 3
    s = pat_points[index]
    m = pat_mols[index]
    i = 0
    n = 3
    for s in pat_points:
        toPlot = coord[s, :]
        colors = mols[s, :][:, m][:, i + n]
        # print(toPlot)
        plt.scatter(toPlot[:, 0], toPlot[:, 1])
        # plt.scatter(toPlot[:, 0], toPlot[:, 2])
        # plt.scatter(toPlot[:, 1], toPlot[:, 2])
        # plt.plot(toPlot[:, 1], toPlot[:, 2])

    plt.show()


def plot2d2(coord, pat_points, mols, pat_mols):
    toPlot = coord[points[0], :]
    print(mols.shape)
    np.random.seed(1)
    index = 0
    s = pat_points[index]
    m = pat_mols[index]

    clrs = np.random.rand(len(pat_mols))
    print(clrs)
    color_i = 0
    print(len(pat_mols))
    colors = [0 for i in range(mols.shape[0])]
    for s in pat_points:
        for i in s:
            colors[i] = clrs[color_i]
        color_i += 1

    plt.scatter(list(range(mols.shape[0])), mols[:, index], c=colors, s=10)

    plt.tight_layout()
    plt.show()


if __name__ == '__main__':

    potential = 'L'
    distance = 10
    k = 11
    coord_file = 'TP_points_%d_1' % distance
    points_file = 'loo_output/T_%s_X_TP_t%d/%d/T_%s_X_TP_t%d_Xtrain_1_points_%d' \
        % (potential, distance, k, potential, distance, k)
    mols_file = 'loo_data/T_%s_X_TP_t%d/T_%s_X_TP_t%d_Xtrain_1' \
        % (potential, distance, potential, distance)
    pat_mols_file = 'loo_output/T_%s_X_TP_t%d/%d/T_%s_X_TP_t%d_Xtrain_1_mols_%d' \
        % (potential, distance, k, potential, distance, k)

    if len(argv) > 4:
        pat_mols_file = argv[4]

    if len(argv) > 3:
        mols_file = argv[3]

    if len(argv) > 2:
        points_file = argv[2]

    if len(argv) > 1:
        coord_file = argv[1]

    coord = np.loadtxt(coord_file, delimiter=',')
    points = loadtxt(points_file, delimiter=',')
    mols = np.loadtxt(mols_file, delimiter=' ')
    pat_mols = loadtxt(pat_mols_file, delimiter=',')

    # plot2d(coord, points, mols, pat_mols)
    # plot2d2(coord, points, mols, pat_mols)
    plot2d1(coord, points, mols, pat_mols)
