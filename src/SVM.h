#include <iostream>
#include <algorithm>
#include <set>
#include <cstdio>
#include <exception>
#include <cmath>
#include <vector>
#include "../lib/QuadProg++/QuadProg++.hh"

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


class SVM {
public:
  SVM(Matrix<double> x,
      Vector<double> y,
      Kernel* kernel):kernel(kernel), x(x), y(y) {

    int n = x.extractColumn(0).size();
    int p = n, m = 1;

    Matrix<double> G(n, n), CE(n, m), CI(n, p);
    Vector<double> g0(-1.0, n), ce0(0.0, m), ci0(0.0, p);

    for (int i = 0; i < n; i++) {
      for (int j = 0; j < n; j++) {
        G[i][j] = y[i] * y[j] * (*kernel)(x.extractRow(i), x.extractRow(j));
        if(i==j)
          G[i][j]+=1.0e-9;
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

    dump_alpha();

    w.resize(0, x.extractRow(0).size());
    for (int i = 0; i < n; ++i) {
      w += alpha[i] * y[i] * x.extractRow(i);
    }

    dump_weight();

    double t = 0.0;
    for (int i = 0; i < n; ++i) {
      t += dot_prod(w, x.extractRow(i)) - y[i];
    }
    theta = t / n;
    std::cout << "theta: " << theta << std::endl;
  };

  void dump_alpha() {
    for (int i = 0; i < x.extractColumn(0).size(); ++i) {
      std::cout << "alph[" << i << "] = " << std::fixed << std::setprecision(6) << alpha[i] << std::endl;
    }
  }

  void dump_weight() {
    std::cout << "weight: ";
    for (int i = 0; i < w.size(); ++i) {
      std::cout << w[i] << " ";
    }
    std::cout << std::endl;
  }

  double discriminate(Vector<double> v){
    if (dot_prod(w, v) - theta >= 1) {
      return 1;
    } else {
      return -1;
    }
  }
private:
  Kernel* kernel;
  Vector<double> alpha;
  Vector<double> w;
  double theta;
  Matrix<double> x;
  Vector<double> y;
};