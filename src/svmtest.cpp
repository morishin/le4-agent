#include <vector>
#include "SVM.h"

#define BUF_LEN 256

using namespace QuadProgPP;

// Vectorを標準出力する
template<typename T>
void printVector(const Vector<T>& v) {
  for (int i = 0; i < v.size(); ++i) {
    std::cout << v[i] << " ";
  }
  std::cout << std::endl;
}

// vectorをVectorに変換
template<typename T>
Vector<T> extendVector(const std::vector<T>& v){
  Vector<T> V;
  V.resize(v.size());
  for (size_t i = 0; i < v.size(); i++)
    V[i] = v[i];
  return V;
}

// カーネル名からKernelオブジェクト生成
Kernel *kernelWithName(const char *name){
  if (strcmp(name, "DotProd") == 0){
    return new DotProd();
  } else if (strcmp(name, "Gaussian") == 0){
    return new Gaussian(10);
  } else if (strcmp(name, "Polynomial") == 0){
    return new Polynomial(2);
  } else if (strcmp(name, "Sigmoid") == 0){
    return new Sigmoid(0.01, -3);
  } else {
    throw 1;
  }
}

int main(int argc, char *argv[]) {
  std::vector<std::vector<double> > x;
  std::vector<double> *x_row;
  std::vector<double> y;

  // コマンドライン引数のカーネル名kernel_nameから、Kernelを生成
  Kernel *kernel;
  const char *kernel_name;  
  try {
    if(argc == 1) {
      kernel_name = "DotProd";
    } else if(argc == 3){
      char *p;
      p = argv[1];
      if(*p == '-'){
        p++;
        switch(*p){
          case 'k':
            kernel_name = argv[2];
            break;
          default:
            throw 0;
        }
      } else {
        throw 0;
      }
    } else {
      throw 0;
    }
    
    kernel = kernelWithName(kernel_name);

  } catch(int n){
    switch(n){
      case 0:
        std::cout << "Invalid arguments" << std::endl;
        break;
      case 1:
        std::cout << "Invalid kernel name" << std::endl;
        break;
    }
    return 1;
  }

  // サンプルデータを標準入力から受け、vectorに格納
  char buf[BUF_LEN];
  char *tp;
  while(fgets(buf, BUF_LEN, stdin) != NULL) {
    x_row = new std::vector<double>();

    tp = strtok(buf, " ");
    x_row->push_back(atof(tp));

    while(tp != NULL) {
      tp = strtok(NULL, " ");
      if (tp != NULL) {
        x_row->push_back(atof(tp));
      } else {
        y.push_back(x_row->back());
        x_row->pop_back();
      }
    }

    x.push_back(*x_row);
  }

  // サンプルデータ点をXに格納
  Matrix<double> X;
  X.resize(x.size(), x[0].size());
  for (size_t i = 0; i < x.size(); i++)
    for (size_t j = 0; j < x[0].size(); j++)
      X[i][j] = x[i][j];
  // サンプルデータ点に対するクラスをYに格納
  Vector<double> Y = extendVector(y);

  // SVMオブジェクトを生成 コンストラクタ内で学習をする
  SVM svm(X, Y, kernel);

  // パラメータを出力
  svm.printAlpha();
  svm.printTheta();

  /*
  std::vector<double> r;
  for (int i = 0; i < X.extractColumn(0).size(); ++i) {
    r.push_back(svm.discriminate(X.extractRow(i)));
  }
  Vector<double> R = extendVector(r);

  std::cout << "result:" << std::fixed << std::setprecision(0) << std::endl;
  printVector(Y);
  */

  return 0;
}
