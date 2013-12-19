#include "../lib/QuadProg++/QuadProg++.hh"

using namespace QuadProgPP;

class CrossValid {
public:
  CrossValid(Matrix<double> _x,
             Vector<double> _y,
             Kernel* _kernel);
  calcAccuracyRate(int n);
private:
  Kernel* kernel;
  Matrix<double> x;
  Vector<double> y;
};