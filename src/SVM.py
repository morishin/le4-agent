#coding:utf-8
import numpy as np
import cvxopt
import cvxopt.solvers
from pylab import *
cvxopt.solvers.options['show_progress'] = False

# SVMクラス
class SVM(object):
  # コンストラクタ
  def __init__(self, X, Y, kernel=np.dot):
    self.X = X            # データ点
    self.Y = Y            # データ点の属するクラス
    self.kernel = kernel  # カーネルトリック

    self.alpha = []       # 二次計画問題の解
    self.S = []           # サポートベクタのインデックスの配列
    self.w = 0.0          # 重みベクタ
    self.theta = 0.0      # 閾値

    self.calc()           # 二次計画問題の計算

  # 二次計画問題の計算
  def calc(self):
    n = len(self.X)
    d = len(self.X[0])
    g = np.zeros((n, n))
    for i in range(n):
      for j in range(n):
        g[i, j] = self.Y[i] * self.Y[j] * self.kernel(self.X[i], self.X[j])
        if i==j:
          g[i, j] += 1.0e-9

    # 二次計画問題のパラメータ設定
    G = cvxopt.matrix(g)
    g0 = cvxopt.matrix(-np.ones(n))
    CE = cvxopt.matrix(self.Y, (1, n))
    ce0 = cvxopt.matrix(0.0)
    CI = cvxopt.matrix(np.diag([-1.0]*n))
    ci0 = cvxopt.matrix(np.zeros(n))

    # CVXOPTライブラリによる二次計画問題の解の算出
    sol = cvxopt.solvers.qp(G, g0, CI, ci0, CE, ce0)
    alpha = array(sol['x']).reshape(n)
    self.alpha = alpha

    # サポートベクターのインデックスの配列
    S = []
    for i, a in enumerate(alpha):
      if a > 0.00001:
        S.append(i)
    self.S = S

    # w: 重みベクタ
    w = np.zeros(d)
    for i in S:
      w += alpha[i] * self.Y[i] * self.X[i]
    self.w = w

    # θ: 閾値
    theta = 0
    for i in S:
      temp = 0
      for j in S:
        temp += alpha[j] * self.Y[j] * self.kernel(self.X[i], self.X[j])
      theta += (self.Y[i] - temp)
    theta /= len(S)
    self.theta = theta

  # 識別関数
  def discriminate(self, v):
    # result = np.dot(self.w, v) + self.theta

    result = 0
    for i in range(len(self.Y)):
      result += self.alpha[i] * self.Y[i] * self.kernel(self.X[i], v)
    result += self.theta

    if result >= 0:
      return 1
    else:
      return -1

  # pylabによるプロット関数(入力が二次元の時のみ動作)
  def plot(self):
    if len(self.X[0])!=2:
      print 'can plot only 2D-type data'
      return

    def f(x1, w, b):
      return - (w[0] / w[1]) * x1 - (b / w[1])

    for i in range(len(self.X)):
      if self.Y[i] == 1:
        scatter(self.X[i][0], self.X[i][1], c='r', marker='x')
      else:
        scatter(self.X[i][0], self.X[i][1], c='b', marker='x')
    for i in self.S:
      scatter(self.X[i, 0], self.X[i, 1], s=80, c='c', marker='o')
    x1 = np.linspace(0, 50, 1000)
    x2 = [f(x, self.w, self.theta) for x in x1]
    plot(x1, x2, 'g-')
    xlim(0, 50)
    ylim(0, 50)
    show()