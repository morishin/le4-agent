#include "SVM.h"

using namespace QuadProgPP;

SVM::SVM(Matrix<double> _x,
        Vector<double> _y,
        Kernel* _kernel):kernel(_kernel), x(_x), y(_y) {
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

  /*
  w.resize(0, x.extractRow(0).size());
  for (int i = 0; i < n; ++i) {
    w += alpha[i] * y[i] * x.extractRow(i);
  }

  double t = 0.0;
  for (int i = 0; i < n; ++i) {
    t += dot_prod(w, x.extractRow(i)) - y[i];
  }
  theta = t / n;
  std::cout << "theta: " << theta << std::endl;
  */
};

void SVM::printAlpha() {
  for (int i = 0; i < x.extractColumn(0).size(); ++i) {
    std::cout << "alph[" << i << "] = " << std::fixed << std::setprecision(6) << alpha[i] << std::endl;
  }
}

void SVM::printWeight() {
  std::cout << "weight: ";
  for (int i = 0; i < w.size(); ++i) {
    std::cout << w[i] << " ";
  }
  std::cout << std::endl;
}

double SVM::discriminate(Vector<double> v){
  double sum = 0;
  for (int i = 0; i < y.size(); ++i) {
    sum += alpha[i] * y[i] * (*kernel)(x.extractRow(i), v);
  }
  if (sum - theta >= 1) {
    return 1;
  } else {
    return -1;
  }
}