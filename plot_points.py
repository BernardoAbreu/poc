#!/usr/bin/env  python

from mpl_toolkits.mplot3d import Axes3D
import matplotlib.pyplot as plt
import numpy as np
from sys import argv


def loadtxt(file, delimiter=','):
    with open(file, 'r') as f:
        return [list(map(int, line[:-1].split(delimiter))) for line in f]


def plot(coord_file, points_file):
    points_coord = np.loadtxt(coord_file, delimiter=',')
    selected = loadtxt(points_file, delimiter=',')

    # points_list = [int(x) for x in selected if int(x) < points_coord.shape[0]]

    # toPlot = points_coord[points_list, :]

    fig = plt.figure()
    ax = fig.add_subplot(111, projection='3d')
    for s in selected:
        plist = list(map(int, s))
        toPlot = points_coord[plist, :]
        ax.scatter(toPlot[:, 0], toPlot[:, 1], toPlot[:, 2])
    # ax.scatter(toPlot[:, 0], toPlot[:, 1], toPlot[:, 2])

    ax.set_xlabel('X Label')
    ax.set_ylabel('Y Label')
    ax.set_zlabel('Z Label')

    plt.show()


if __name__ == '__main__':

    coord = 'TP_coordPoints_10_1_7.txt'
    points = 'points'

    if len(argv) > 2:
        coord = argv[1]
        points = argv[2]

    plot(coord, points)
