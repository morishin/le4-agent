import sys
import numpy as np
from scipy.linalg import norm

from SVM import SVM

def gaussianKernel(x, y, sigma=10):
  return np.exp(-norm(x-y)**2 / (2*sigma**2))

def crossValidation(X, Y, k, kernel=np.dot):
  def accuracyRate(result, answer):
    return 1.0 - sum(abs(result - answer)/2) / float(len(result))

  n = len(X)
  l = n / k
  ac = 0.0
  for i in range(k):
    testVectors = X[l*i:l*(i+1)]
    answerForTestVectors = Y[l*i:l*(i+1)]
    learningVectors = np.vstack((X[:l*i], X[l*(i+1):]))
    answerForlearningVectors = np.hstack((Y[:l*i], Y[l*(i+1):]))

    svm = SVM(learningVectors, answerForlearningVectors, kernel)
    if i==1: svm.plot()
    result = [svm.discriminate(t) for t in testVectors]

    ac += accuracyRate(result, answerForTestVectors)
  return ac / k

if __name__ == '__main__':
  if len(sys.argv)<=1:
    print 'input file name required'
    sys.exit(1)
  elif len(sys.argv)==2:
    kernel = np.dot
    D = np.loadtxt(sys.argv[1])
  elif len(sys.argv)==4 and sys.argv[1]=='-k':
    if sys.argv[2]=='Gaussian':
      kernel = gaussianKernel
    elif sys.argv[2]=='Sigmoid':
      kernel = sigmoidKernel
    elif sys.argv[2]=='Polynomial':
      kernel = polynomialKernel
    else:
      print 'invalid kernel name'
      sys.exit(1)
    D = np.loadtxt(sys.argv[3])
  else:
    print 'invalid arguments (must be the following style)'
    print '\'python '+__file__+' [-k <kernel name>] <input file path>\''
    sys.exit(1)

  n = len(D)
  d = len(D[0])-1
  X = D[:, :d]
  Y = np.reshape(D[:, d:], n)

  print crossValidation(X, Y, 10, kernel)

  