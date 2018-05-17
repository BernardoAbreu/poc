import sys
import re
import numpy as np
from sklearn.model_selection import LeaveOneOut
from sklearn.metrics import mean_squared_error
from sklearn.metrics import r2_score
from sklearn.tree import DecisionTreeRegressor


class Reprinter:
    def __init__(self):
        self.text = ''

    def moveup(self, lines):
        for _ in range(lines):
            sys.stdout.write("\x1b[A")

    def reprint(self, text):
        # Clear previous text by overwritig non-spaces with spaces
        self.moveup(self.text.count("\n"))
        sys.stdout.write(re.sub(r"[^\s]", " ", self.text))

        # Print new text
        lines = min(self.text.count("\n"), text.count("\n"))
        self.moveup(lines)
        sys.stdout.write(text)
        self.text = text


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


def regTreeLOO(X, Y, minSamplesLeaf):
    nSamples, nFeatures = np.shape(X)
    yFitCV = np.zeros((nSamples, 1))
    rmseModel = np.zeros((nSamples, 1))
    rmseCV = np.zeros((nSamples, 1))
    r2Model = np.zeros((nSamples, 1))
    corrModel = np.zeros((nSamples, 1))

    cv = LeaveOneOut()
    reprinter = Reprinter()
    countLOO = 1
    for train_index, test_index in cv.split(X):
        out = 'Cross Validating ' + str(countLOO) + '/' + str(nSamples) + '\n'
        reprinter.reprint(out)
        countLOO = countLOO + 1

        X_train, X_test = X[train_index], X[test_index]
        Y_train, Y_test = Y[train_index], Y[test_index]

        tree = DecisionTreeRegressor(min_samples_leaf=minSamplesLeaf)
        mdl = tree.fit(X_train, Y_train)
        trainFit = mdl.predict(X_train)
        testFit = mdl.predict(X_test)

        yFitCV[test_index] = testFit
        rmseModel[test_index] = mean_squared_error(Y_train, trainFit)
        rmseCV[test_index] = mean_squared_error(Y_test, testFit)
        r2Model[test_index] = r2_score(Y_train, trainFit)
        corrModel[test_index] = correlation(Y_train, trainFit)

    q2 = r2_score(Y, yFitCV)
    r2 = np.mean(r2Model)
    rmsecv = np.mean(rmseCV)
    rmse = np.mean(rmseModel)
    corrcv = correlation(Y, np.reshape(yFitCV, yFitCV.size))
    corrmdl = np.mean(corrModel)
    return q2, r2, rmsecv, rmse, corrcv, corrmdl
