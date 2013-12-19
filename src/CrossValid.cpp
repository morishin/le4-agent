#include <set>
#include "CrossValid.h"

using namespace QuadProgPP;

CrossValid::CrossValid(Matrix<double> _x,
                       Vector<double> _y,
                       Kernel* _kernel):kernel(_kernel), x(_x), y(_y) {};

void printMatrix(Matrix<double>& A) {
  for(int i=0; i<A.nrows(); ++i) {
    for(int j=0; j<A.ncols(); ++j) {
      std::cout << A[i][j] << " ";
    }
    std::cout << "\n";
  }
  std::cout << std::endl;
}

void CrossValid::calcAccuracyRate(int n){
  Matrix<double> learningVectors, evalVectors;
  std::set<unsigned int> lerningIndexes, evalIndexes;
  int n_indexes = int(x.nrows() / n);
  for(int i=0; i<n; ++i) {
    lerningIndexes.clear();
    evalIndexes.clear();
    for(int j=0; j<x.nrows(); ++j) {
      if(i*n_indexes <= j && j < (i+1)*n_indexes)
        evalIndexes.insert(j);
      else
        lerningIndexes.insert(j);
    }
    learningVectors = x.extractRows(lerningIndexes);
    evalVectors = x.extractRows(evalIndexes);

    printMatrix(evalVectors);
    printMatrix(learningVectors);    
  }
}