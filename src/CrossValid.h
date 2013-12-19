#include "../lib/QuadProg++/QuadProg++.hh"
#include "SVM.h"

using namespace QuadProgPP;

class CrossValid {
public:
  CrossValid(Matrix<double> _x,
             Vector<double> _y,
             Kernel* _kernel);
  double calcAccuracyRate(int n);
private:
  Kernel* kernel;
  Matrix<double> x;
  Vector<double> y;
};