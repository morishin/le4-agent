import sys
import numpy as np

from SVM import SVM

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

  print [svm.discriminate(x) for x in X]

  