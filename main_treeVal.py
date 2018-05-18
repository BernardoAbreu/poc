import sys
import numpy as np
from treeLOO import regTreeLOO
from sklearn import preprocessing
from sklearn.cross_decomposition import PLSRegression
import matplotlib.pyplot as plt

datasetPath = sys.argv[1]
responsePath = sys.argv[2]
selFeatPath = sys.argv[3]
opsFilePath = sys.argv[4]
outputPath = sys.argv[5]

X = np.loadtxt(datasetPath, delimiter='\t')
Y = np.loadtxt(responsePath, delimiter='\t')
selFeat = np.loadtxt(selFeatPath, delimiter=',')
metrics = np.loadtxt(opsFilePath, delimiter=',')

X = preprocessing.scale(X)
selFeat = list(map(int, selFeat))
numLV = int(metrics[7])

X = X[:, selFeat]
mdl = PLSRegression(n_components=numLV).fit(X, Y)
X = mdl.transform(X)

nSamples, nFeatures = X.shape
results = np.empty([0, 7])

for minSamples in range(nSamples):
    q2, r2, rmsecv, rmse, corrcv, corrmdl = regTreeLOO(X, Y, minSamples + 1)
    output = np.array([minSamples, q2, r2, rmsecv, rmse, corrcv, corrmdl])
    results = np.vstack((results, output))
    print('minSamples', minSamples + 1, 'Q2', q2)

maxPerformance = results[np.argmax(results, axis=0)[1], :]
training = results[:, 2]
testing = results[:, 1]

x = list(map(lambda x: x + 1, list(range(nSamples))))
plt.plot(x, testing, 'bs-', label='Testing', markersize=3)
y = testing
xmax = x[np.argmax(y)]
ymax = y.max()
text = "X={:.0f}, R²={:.3f}, nLV={:.0f}".format(xmax, ymax, numLV)
ax = plt.gca()
bbox_props = dict(boxstyle="square,pad=0.3", fc="w", ec="k", lw=0.72)
arrowprops = dict(arrowstyle="->",
                  connectionstyle="angle,angleA=0,angleB=60")
kw = dict(xycoords='data', textcoords="axes fraction",
          arrowprops=arrowprops, bbox=bbox_props, ha="right", va="top")
ax.annotate(text, xy=(xmax, ymax), xytext=(0.94, 0.96), **kw)

plt.plot(x, training, 'ro-', label='Training', markersize=3)
plt.ylim((-1, 1))
plt.xlabel('Min Samples Leaf')
plt.ylabel('R²')
titleName = 'Regression tree with ' + outputPath
plt.title(titleName)
plt.legend(loc=4)
plt.grid()
plt.xlim((min(x), max(x)))
plt.ylim((0, 1))
# plt.show()
plt.savefig(outputPath + '.eps')
np.savetxt(outputPath + '.txt', np.reshape(maxPerformance, (1, -1)),
           delimiter=',', fmt='%f')
