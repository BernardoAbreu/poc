#!/usr/bin/env python3

import numpy as np
import matplotlib
import matplotlib.pyplot as plt
from matplotlib.widgets import Slider
from matplotlib.widgets import Button
from matplotlib import colors
from vertical_slider import VertSlider


def reshape(data, delta):
    # print(data.shape)
    mols = np.zeros((data.shape[1], 7, 180 // delta, 360 // delta))
    # print(mols.shape)
    for cur_mol in range(data.shape[1]):
        index = 0
        found_last = False
        for i in range(0, 179, delta):
            for j in range(0, 359, delta):
                for r in range(7):
                    if (j == 90 or j == 270) and found_last:
                        mols[cur_mol][r][i // delta][j // delta] = \
                            mols[cur_mol][r][0][j // delta]
                    else:
                        # print(i, j, r, index)
                        mols[cur_mol][r][i // delta][j // delta] = \
                            data[index, cur_mol]
                        index += 1
                if j == 270:
                    found_last = True
    return mols


def point_dict(delta):
    total_points = (7 * (180 // delta) * (360 // delta)) -\
        (7 * ((180 // delta) - 1) * 2)
    d_points = [0 for i in range(total_points)]

    found_last = False
    index = 0
    for i in range(0, 179, delta):
        for j in range(0, 359, delta):
            for r in range(7):
                if (j == 90 or j == 270) and found_last:
                    pass
                else:
                    # print(i, j, r, index)
                    d_points[index] = (r, i, j)
                    index += 1
            if j == 270:
                found_last = True
    return d_points


def reshape_pat(mols, points, total_mols, delta):
    # print(data.shape)
    # print(len(mols))
    # pats = np.zeros((len(mols), total_mols, 7, 180 // delta, 360 // delta),
    #                 dtype=int)
    pats = np.zeros((total_mols, 7, 180 // delta, 360 // delta), dtype=int)
    # print(mols.shape)

    d = point_dict(delta)

    for pat, (pat_m, pat_p) in enumerate(zip(mols, points)):
        for m in pat_m:
            for p in pat_p:
                r, i, j = d[p]
                # pats[pat, m, r, i // delta, j // delta] = 1
                pats[m, r, i // delta, j // delta] = pat + 1
    return pats


def heatmap(data, row_labels=None, col_labels=None, ax=None, arange=None,
            cbar_kw={}, cbarlabel="", **kwargs):
    """
    Create a heatmap from a numpy array and two lists of labels.

    Arguments:
        data       : A 2D numpy array of shape (N,M)
        row_labels : A list or array of length N with the labels
                     for the rows
        col_labels : A list or array of length M with the labels
                     for the columns
    Optional arguments:
        ax         : A matplotlib.axes.Axes instance to which the heatmap
                     is plotted. If not provided, use current axes or
                     create a new one.
        cbar_kw    : A dictionary with arguments to
                     :meth:`matplotlib.Figure.colorbar`.
        cbarlabel  : The label for the colorbar
    All other arguments are directly passed on to the imshow call.
    """

    if not ax:
        ax = plt.gca()

    if not row_labels:
        row_labels = range(data.shape[0])

    if not col_labels:
        col_labels = range(data.shape[1])

    # Plot the heatmap
    im = ax.imshow(data, **kwargs)
    if arange:
        im.set_norm(colors.Normalize(vmin=arange[0], vmax=arange[1]))

    # Create colorbar
    cbar = ax.figure.colorbar(im, ax=ax, **cbar_kw)
    cbar.ax.set_ylabel(cbarlabel, rotation=-90, va="bottom")

    # We want to show all ticks...
    ax.set_xticks(np.arange(0, data.shape[1], 10.))
    ax.set_yticks(np.arange(0, data.shape[0], 10.))
    # ... and label them with the respective list entries.
    ax.set_xticklabels(col_labels)
    ax.set_yticklabels(row_labels)

    # Let the horizontal axes labeling appear on top.
    # ax.tick_params(top=True, bottom=False,
    #                labeltop=False, labelbottom=True)

    # Rotate the tick labels and set their alignment.
    # plt.setp(ax.get_xticklabels(), rotation=-30, ha="right",
    #          rotation_mode="anchor")

    # Turn spines off and create white grid.
    for edge, spine in ax.spines.items():
        spine.set_visible(False)

    ax.set_xticks(np.arange(data.shape[1] + 1) - .5, minor=True)
    ax.set_yticks(np.arange(data.shape[0] + 1) - .5, minor=True)
    # ax.grid(which="minor", color="w", linestyle='-', linewidth=3)
    ax.tick_params(which="minor", bottom=False, left=False)

    return im, cbar


def loadtxt(filename, dtype=int, delimiter=','):
    with open(filename, 'r') as f:
        return [list(map(dtype, line.split(delimiter))) for line in f]


def update(val):
    current_mol = int(smol.val)
    radius = int(sradius.val)
    im.set_data(data[current_mol, radius])
    im.set_norm(colors.Normalize(vmin=np.amin(data[current_mol, radius]),
                                 vmax=np.amax(data[current_mol, radius])))
    # im2.set_data(pats[current_pat, current_mol, radius])
    im2.set_data(data2[current_mol, radius])
    # im.set_norm(colors.Normalize(vmin=vmin, vmax=vmax))
    fig.canvas.draw_idle()


def reset(event):
    # sfreq.reset()
    sradius.reset()
    smol.reset()
    # spat.reset()
    im.set_norm(colors.Normalize(vmin=np.amin(data[current_mol, radius]),
                                 vmax=np.amax(data[current_mol, radius])))


potential = 'L'
base = 'TP'
delta = 2
k = 13

mols = np.loadtxt('split_data/T_%s_X_%s_t%d.txt' % (potential, base, delta))
mols2 = reshape(mols, delta)

pat_points = loadtxt('output/%s_%s_t%d_points_%d' % (base, potential, delta, k))
pat_mols = loadtxt('output/%s_%s_t%d_mols_%d' % (base, potential, delta, k))

pats = reshape_pat(pat_mols, pat_points, mols.shape[1], delta)

print('%d patterns.' % len(pat_mols),
      'Max mols: %d,' % max(map(len, pat_mols)),
      'Max points: %d' % max(map(len, pat_points))
      )

current_mol = 0
radius = 0
current_pat = 0
color_palette = 'plasma'


data = mols2
data2 = pats
print(mols2.shape, data.shape)
# data = mols2.T
vmin = np.amin(mols)
vmax = np.amax(mols)
arange = (vmin, vmax)


# fig, ax = plt.subplots()
fig, ax = plt.subplots(2, 1)
# fig, ax = plt.subplots(1, 2)
# fig.tight_layout()

# plt.subplots_adjust(left=0.25)

plt.ylabel('Theta')
plt.xlabel('Phi')
im, cbar = heatmap(data[current_mol, radius],
                   range(0, 180, 20),
                   range(0, 360, 20),
                   ax=ax[0],
                   # ax=ax,
                   arange=arange,
                   cmap=color_palette,
                   cbarlabel="potential value")

im.set_norm(colors.Normalize(vmin=np.amin(data[current_mol, radius]),
                             vmax=np.amax(data[current_mol, radius])))


my_cmap = matplotlib.cm.get_cmap('rainbow')
my_cmap.set_under('w')
im2 = ax[1].imshow(data2[current_mol, radius], cmap=my_cmap, vmin=.001)

# We want to show all ticks...
ax[1].set_xticks(np.arange(0, data2[current_mol, radius].shape[1], 10.))
ax[1].set_yticks(np.arange(0, data2[current_mol, radius].shape[0], 10.))
# ... and label them with the respective list entries.
ax[1].set_xticklabels(range(0, 360, 20))
ax[1].set_yticklabels(range(0, 180, 20))

axcolor = 'lightgoldenrodyellow'

axradius = plt.axes([0.25, 0.02, 0.65, 0.02], facecolor=axcolor)
sradius = Slider(axradius, 'Radius', 0., 6., valinit=radius, valfmt='%0.0f')

axmol = plt.axes([0.05, 0.15, 0.02, 0.75], facecolor=axcolor)
smol = VertSlider(axmol, 'Mol', 0., mols.shape[1] - 1,
                  valinit=current_mol, valfmt='%0.0f')

resetax = plt.axes([0.05, 0.025, 0.1, 0.04])
button = Button(resetax, 'Reset', color=axcolor, hovercolor='0.975')


smol.on_changed(update)
sradius.on_changed(update)
button.on_clicked(reset)


plt.show()
