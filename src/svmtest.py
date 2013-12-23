#coding:utf-8
import sys
import numpy as np
from scipy.linalg import norm
import cvxopt
import cvxopt.solvers
from pylab import *

def gaussian(x, y, sigma=10):
  return exp(-np.dot(x-y, x-y) / (2*sigma**2))

class SVM(object):
  def __init__(self, X, Y, kernel=np.dot):
    self.X = X
    self.Y = Y
    self.kernel = kernel

    # self.alpha
    # self.S
    # self.w
    # self.theta

    self.calc()

  def calc(self):
    n = len(self.X)
    d = len(self.X[0])
    g = np.zeros((n, n))
    for i in range(n):
      for j in range(n):
        g[i, j] = self.Y[i] * self.Y[j] * self.kernel(X[i], X[j])

    G = cvxopt.matrix(g)
    g0 = cvxopt.matrix(-np.ones(n))
    CE = cvxopt.matrix(self.Y, (1, n))
    ce0 = cvxopt.matrix(0.0)
    CI = cvxopt.matrix(np.diag([-1.0]*n))
    ci0 = cvxopt.matrix(np.zeros(n))

    sol = cvxopt.solvers.qp(G, g0, CI, ci0, CE, ce0)
    alpha = array(sol['x']).reshape(n)
    self.alpha = alpha

    S = []
    for i, a in enumerate(alpha):
      if a > 0.00001:
        S.append(i)
    self.S = S

    w = np.zeros(d)
    for i in S:
      w += alpha[i] * self.Y[i] * self.X[i]
    self.w = w

    theta = 0
    for i in S:
      temp = 0
      for j in S:
        temp += alpha[j] * self.Y[j] * self.kernel(self.X[i], self.X[j])
      theta += (self.Y[i] - temp)
    theta /= len(S)
    self.theta = theta

  def plot(self):
    if len(self.X[0])!=2:
      print 'can plot only 2-D data'
      return

    def f(x1, w, b):
      return - (w[0] / w[1]) * x1 - (b / w[1])

    for i in range(len(self.X)):
      if Y[i] == 1:
        scatter(X[i][0], X[i][1], c='r', marker='x')
      else:
        scatter(X[i][0], X[i][1], c='b', marker='x')
    for i in self.S:
      scatter(X[i, 0], X[i, 1], s=80, c='c', marker='o')
    x1 = np.linspace(0, 50, 1000)
    x2 = [f(x, self.w, self.theta) for x in x1]
    plot(x1, x2, 'g-')
    xlim(0, 50)
    ylim(0, 50)
    show()

if __name__ == '__main__':
  if len(sys.argv)<=1:
    print 'input file name required'
    sys.exit(1)

  D = np.loadtxt(sys.argv[1])
  n = len(D)
  d = len(D[0])-1
  X = D[:, :d]
  Y = np.reshape(D[:, d:], n)

  svm = SVM(X, Y)

  svm.plot()

  