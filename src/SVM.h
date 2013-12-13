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
  Gaussian(double _sigma):sigma(_sigma){};
  double operator() (const Vector<double> x, const Vector<double> y) {
    return exp(-dot_prod(x - y, x - y) / (2.0 * sigma * sigma));
  };
private:
  double sigma;
};

class Polynomial : public Kernel {
public:
  Polynomial(double _d):d(_d){};
  double operator() (const Vector<double> x, const Vector<double> y) {
    return pow(1.0 + dot_prod(x, y), d); 
  };
private:
  double d;
};

class Sigmoid : public Kernel {
public:
  Sigmoid(double _a, double _b):a(_a), b(_b){};
  double operator() (const Vector<double> x, const Vector<double> y) {
    return tanh(a * dot_prod(x, y) - b); 
  };
private:
  double a, b;
};


class SVM {
public:
  SVM(Matrix<double> _x,
      Vector<double> _y,
      Kernel* _kernel);
  void dump_alpha();
  void dump_weight();
  double discriminate(Vector<double> v);
  void printAlpha();
  void printWeight();
private:
  Kernel* kernel;
  Vector<double> alpha;
  Vector<double> w;
  double theta;
  Matrix<double> x;
  Vector<double> y;
};