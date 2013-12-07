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

int main() {
  std::vector<std::vector<double> > tmp;
  std::vector<double> *x;
  std::vector<double> y;

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

  int n = X.extractColumn(1).size();
  int p = n, m = 1;

  Matrix<double> G(n, n), CE(n, m), CI(n, p);
  Vector<double> g0(-1.0, n), ce0(0.0, m), ci0(0.0, p), alpha;
  Kernel* kernel = new DotProd();

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
    std::cout << "alph[" << i << "] = " << std::fixed << std::setprecision(5) << alpha[i] << std::endl;
  }

  return 0;
}
