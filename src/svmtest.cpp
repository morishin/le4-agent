#include <iostream>
#include <algorithm>
#include <set>
#include <cstdio>
#include <exception>
#include <cmath>
#include <vector>
#include "SVM.h"

#define BUF_LEN 256

using namespace QuadProgPP;

template<typename T>
void print_vector(Vector<T> v) {
  for (int i = 0; i < v.size(); ++i) {
    std::cout << v[i] << " ";
  }
  std::cout << std::endl;
}

Kernel *kernelWithName(const char *name){
  if (strcmp(name, "DotProd") == 0){
    return new DotProd();
  } else if (strcmp(name, "Gaussian") == 0){
    return new Gaussian(10);
  } else if (strcmp(name, "Polynomial") == 0){
    return new Polynomial(2);
  } else if (strcmp(name, "Sigmoid") == 0){
    return new Sigmoid(1, 4);
  } else {
    throw 1;
  }
}

int main(int argc, char *argv[]) {
  std::vector<std::vector<double> > x;
  std::vector<double> *x_row;
  std::vector<double> y;

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

  Matrix<double> X;
  X.resize(x.size(), x[0].size());
  for (size_t i = 0; i < x.size(); i++)
    for (size_t j = 0; j < x[0].size(); j++)
      X[i][j] = x[i][j];

  Vector<double> Y;
  Y.resize(y.size());
  for (size_t i = 0; i < y.size(); i++)
    Y[i] = y[i];

  SVM svm(X, Y, kernel);

  std::vector<double> result;
  for (int i = 0; i < X.extractColumn(0).size(); ++i) {
    result.push_back(svm.discriminate(X.extractRow(i)));
  }
  Vector<double> R;
  R.resize(result.size());
  for (size_t i = 0; i < result.size(); i++)
    R[i] = result[i];

  print_vector(Y-R);
  return 0;
}
