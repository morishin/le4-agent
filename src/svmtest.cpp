#include <iostream>
#include <algorithm>
#include <set>
#include <cstdio>
#include <exception>
#include <cmath>
#include <vector>
#include "../lib/QuadProg++/QuadProg++.hh"

#define BUF_LEN 256

using namespace QuadProgPP;

//カーネル - 関数オブジェクト
class Kernel {
public:
  virtual double operator() (const Vector<double> x, const Vector<double> y) = 0;
};

class DotProd : public Kernel {
public:
  double operator() (const Vector<double> x, const Vector<double> y) {
    return dot_prod(x, y);
  };
};

class Gaussian : public Kernel {
public:
  Gaussian(double sigma):sigma(sigma){};
  double operator() (const Vector<double> x, const Vector<double> y) {
    return exp(-dot_prod(x - y, x - y) / 2.0 / sigma / sigma);
  };
private:
  double sigma;
};

class Polynomial : public Kernel {
public:
  Polynomial(double d):d(d){};
  double operator() (const Vector<double> x, const Vector<double> y) {
    return pow(1.0 + dot_prod(x, y), d); 
  };
private:
  double d;
};

class Sigmoid : public Kernel {
public:
  Sigmoid(double a, double b):a(a), b(b){};
  double operator() (const Vector<double> x, const Vector<double> y) {
    return tanh(a * dot_prod(x, y) - b); 
  };
private:
  double a, b;
};

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
  std::vector<std::vector<double> > tmp;
  std::vector<double> *x;
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
    x = new std::vector<double>();

    tp = strtok(buf, " ");
    x->push_back(atof(tp));

    while(tp != NULL) {
      tp = strtok(NULL, " ");
      if (tp != NULL) {
        x->push_back(atof(tp));
      } else {
        y.push_back(x->back());
        x->pop_back();
      }
    }

    tmp.push_back(*x);
  }

  Matrix<double> X;
  X.resize(tmp.size(), tmp[0].size());
  for (size_t i = 0; i < tmp.size(); i++)
    for (size_t j = 0; j < tmp[0].size(); j++)
      X[i][j] = tmp[i][j];

  int n = X.extractColumn(0).size();
  int p = n, m = 1;

  Matrix<double> G(n, n), CE(n, m), CI(n, p);
  Vector<double> g0(-1.0, n), ce0(0.0, m), ci0(0.0, p), alpha;

  for (int i = 0; i < n; i++) {
    for (int j = 0; j < n; j++) {
      G[i][j] = y[i] * y[j] * (*kernel)(X.extractRow(i), X.extractRow(j));
      if(i==j) G[i][j]+=1.0e-9;
    }
  }

  for (int i = 0; i < n; ++i) {
    CE[i][0] = y[i];
    for (int j = 0; j < p; ++j) {
      if (i==j) {
        CI[i][j] = 1.0;
      } else {
        CI[i][j] = 0.0;
      }
    }
  }

  solve_quadprog(G, g0, CE, ce0, CI, ci0, alpha);

  for (int i = 0; i < n; ++i) {
    std::cout << "alph[" << i << "] = " << std::fixed << std::setprecision(6) << alpha[i] << std::endl;
  }

  return 0;
}
