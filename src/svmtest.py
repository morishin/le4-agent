#coding:utf-8
import sys
import numpy as np
from scipy.linalg import norm
from SVM import SVM

# カーネルトリック
def gaussianKernel(x, y, sigma=10):
  return np.exp(-norm(x-y)**2 / (2*sigma**2))
def sigmoidKernel(x, y, a=0.002, b=-1.5):
  return np.tanh(a * np.dot(x, y) - b)
def polynomialKernel(x, y, d=2):
  return (1 + np.dot(x, y))**d

### 交差検定
# X: データ点の配列
# Y: データ点の属するクラスの配列
# k: 分割数
# kernel: カーネルトリック (デフォルトは内積)
def crossValidation(X, Y, k, kernel=np.dot):
  # answerを正解としてresultの正解率を計算
  def accuracyRate(result, answer):
    return 1.0 - sum(abs(result - answer)/2) / float(len(result))

  n = len(X)  # データ点の個数
  l = n / k   # k分割したデータ点の個数
  ac = 0.0    # 正解率の初期化

  # k-交差検定を行い正解率を計算
  for i in range(k):
    # l個の評価ベクトルとそのクラス
    testVectors = X[l*i:l*(i+1)]
    classForTestVectors = Y[l*i:l*(i+1)]
    # n-l個の訓練ベクトルとそのクラス
    learningVectors = np.vstack((X[:l*i], X[l*(i+1):]))
    classForlearningVectors = np.hstack((Y[:l*i], Y[l*(i+1):]))

    # 訓練ベクトルからサポートベクターを計算
    svm = SVM(learningVectors, classForlearningVectors, kernel)
    # 学習した識別関数で評価ベクトルを識別
    result = [svm.discriminate(t) for t in testVectors]
    # 評価結果の正解率を算出
    ac += accuracyRate(result, classForTestVectors)

  # 正解率の平均を返す
  return ac / k

if __name__ == '__main__':
  # コマンドライン引数処理
  if len(sys.argv)==1:
    kernel = np.dot
    D = np.loadtxt(sys.stdin)
  elif len(sys.argv)==3 and sys.argv[1]=='-k':
    if sys.argv[2]=='Gaussian':
      kernel = gaussianKernel
    elif sys.argv[2]=='Sigmoid':
      kernel = sigmoidKernel
    elif sys.argv[2]=='Polynomial':
      kernel = polynomialKernel
    else:
      print 'invalid kernel name (Gaussian/Sigmoid/Polynomial)'
      sys.exit(1)
    D = np.loadtxt(sys.stdin)
  else:
    print 'invalid arguments (must be the following style)'
    print '\'python '+__file__+' [-k KERNEL_NAME] < INPUT_FILE_PATH\''
    sys.exit(1)

  n = len(D)        # データ点の個数
  d = len(D[0])-1   # データ点の次元
  X = D[:, :d]      # データ点の配列
  Y = np.reshape(D[:, d:], n) #データ点の属するクラスの配列

  # サポートベクタ, 識別関数のプロット
  # svm = SVM(X, Y, kernel)
  # svm.plot()

  # 交差検定
  ar = crossValidation(X, Y, 10, kernel)
  # 正解率を出力
  print 'Accuracy Rate: ' + str(ar)

  