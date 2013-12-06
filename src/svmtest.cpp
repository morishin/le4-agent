#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#import <numeric>
#include "../lib/QuadProg++/QuadProg++.hh"

#define BUF_LEN 256

using namespace QuadProgPP;

int main() {
  std::vector<std::vector<double> > X;
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

    X.push_back(*x);
  }

  int n = X.size();
  int p = n, m = 1;

  Matrix<double> G(n, n), CE(n, m), CI(n, p);
  Vector<double> g0(-1.0, n), ce0(0.0, m), ci0(0.0, p), alpha;

  for (int i = 0; i < n; i++) {
    for (int j = 0; j < n; j++) {
      G[i][j] = y[i] * y[j] * inner_product(X[i].begin(), X[i].end(), X[j].begin(), 0);
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
