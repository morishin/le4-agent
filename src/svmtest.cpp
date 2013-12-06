#include <iostream>
#include <numeric>
#include <vector>
#include "../lib/QuadProg++/QuadProg++.hh"

#define BUF_LEN 256

using namespace std;

int main() {
  vector<vector<double> > X;
  vector<double> *x;
  vector<double> y;

  char buf[BUF_LEN];
  char *tp;
  while(fgets(buf, BUF_LEN, stdin) != NULL) {
    x = new vector<double>();

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
  int p = 1, m = 1;

  double G[MATRIX_DIM][MATRIX_DIM], g0[MATRIX_DIM], 
    CE[MATRIX_DIM][MATRIX_DIM], ce0[MATRIX_DIM], 
    CI[MATRIX_DIM][MATRIX_DIM], ci0[MATRIX_DIM], 
    alpha[MATRIX_DIM];

  ce0[0] = 0.0;
  ci0[0] = 0.0;
  for (int i = 0; i < n; i++) {
    g0[i] = -1.0;
    CE[i][0] = y[i];
    CI[i][0] = 1.0;
    for (int j = 0; j < n; j++) {
      G[i][j] = inner_product(X[i].begin(), X[i].end(), X[j].begin(), 0);
    }
  }

  solve_quadprog(G, g0, n, CE, ce0, p, CI, ci0, m, alpha);

  for (int i = 0; i < n; ++i) {
    cout << alpha[i] << endl;
  }

  return 0;
}