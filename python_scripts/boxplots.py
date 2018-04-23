#!/usr/bin/env python3


import numpy as np
import matplotlib.pyplot as plt
import pandas as pd
from pandas import DataFrame
from sys import argv

from matplotlib.legend_handler import HandlerLine2D


params = {
    'legend.fontsize': 56,
    'figure.figsize': (22, 18),
    'axes.labelsize': 56,
    'axes.titlesize': 56,
    'xtick.labelsize': 56,
    'ytick.labelsize': 56,
}
plt.rcParams.update(params)


def loadtxt(file, delimiter=',', dtype=float):
    with open(file, 'r') as f:
        return [np.array(list(map(dtype, line[:-1].split(delimiter))))
                for line in f]


def line_mean_plot(files, labels, y_label, save='', log=False):
    mm = []

    for file in files:
        # print(i)
        a = np.loadtxt(file, delimiter=',')
        means = []
        for column in a.T:
            means.append(np.mean(column))
        if len(means) == 1:
            means = means * 500
        mm.append(np.array(means))

    fig = plt.figure(figsize=(19, 11))
    ax = fig.add_subplot(111)
    # plt.grid(True)

    title = y_label.replace('_', ' ').title()
    plt.xlabel('Iterations')
    plt.ylabel(title)
    plt.title(title + ' x Iterations')

    line1 = None
    for m, d in zip(mm, labels):
        print(len(m))
        line1, = plt.plot(m, label=str(d).replace('_', ' ').title())

    if log:
        ax.set_yscale('log')

    if len(labels) > 0:
        plt.legend(handler_map={line1: HandlerLine2D(numpoints=4)})

    if save:
        print('saving to ' + save)
        plt.savefig(save + '_' + y_label + '_mean.png', dpi=fig.dpi)
    else:
        plt.show()


def line_plot(a, y_label, save=''):
    title = y_label.replace('_', ' ').title()
    plt.plot(np.arange(len(a)), a)
    plt.xlabel('Iterations')
    plt.ylabel(title)
    plt.title(title + ' x Iterations')
    # plt.grid(True)

    if save:
        print('saving to ' + save)
        plt.savefig(save + '_line.png', dpi=300)
    plt.show()


def boxplot(a, y_label, save=''):
    fig = plt.figure(figsize=(19, 5))
    plt.locator_params(axis='x', nticks=10)
    df = DataFrame(a)
    plt.figure()
    df.boxplot().set_xticklabels(
        [str(i) if i % 5 == 0 else '' for i in range(101)])

    plt.xlabel('Iterations')
    plt.ylabel(y_label.title())
    plt.title(y_label.title() + ' x Iterations')
    # plt.grid(True)

    if save:
        print('saving to ' + save)
        plt.savefig(save + '_boxplot.png', dpi=fig.dpi)


def main(patterns_file, bio_file, max_patt):
    fig = plt.figure(figsize=(19, 5))

    patterns = loadtxt(patterns_file, delimiter=',', dtype=int,)
    bio_act = np.loadtxt(bio_file)

    num_mol = len(bio_act)

    all_mol = np.arange(num_mol)

    for i in range(5):
        patt = patterns[i]
        non_patt = np.setdiff1d(all_mol, patt)

        d1 = pd.Series(bio_act[patt])
        d2 = pd.Series(bio_act[non_patt])

        df = DataFrame({'Dentro do padrão': d1, 'Fora do padrão': d2})
        plt.figure()
        plt.ylabel("Atividade biológica")
        df.boxplot(fontsize=56)
        # plt.show()
        plt.savefig(str(i) + '_bio_boxplot.eps', format='eps', dpi=fig.dpi)


if __name__ == '__main__':
    patt_file = argv[1]
    bio_file = argv[2]
    max_patt = 1
    main(patt_file, bio_file, max_patt)
