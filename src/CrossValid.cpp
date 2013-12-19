#include <set>
#include "CrossValid.h"

using namespace QuadProgPP;

// Vectorを標準出力する
template<typename T>
void printVector(const Vector<T>& v) {
  for (int i = 0; i < v.size(); ++i) {
    std::cout << v[i] << " ";
  }
  std::cout << std::endl;
}

CrossValid::CrossValid(Matrix<double> _x,
                       Vector<double> _y,
                       Kernel* _kernel):kernel(_kernel), x(_x), y(_y) {};

void CrossValid::calcAccuracyRate(int n){
  Matrix<double> learningVectors, evalVectors;
  std::set<int> lerningIndexes, evalIndexes;
  int n_indexes = int(x.nrows() / n);
  for(int i=0; i<n; ++i) {
    lerningIndexes.clear();
    evalIndexes.clear();
    for(int j=0; j<x.rows(); ++j) {
      if(i*n_indexes <= j && j < (i+1)*n_indexes)
        evalIndexes.insert(j);
      else
        lerningIndexes.insert(j);
    }
    learningVectors = x.extractRows(lerningIndexes);
    evalVectors = x.extractRows(evalIndexes);
    printVector(learningVectors)
    printVector(evalVectors)
  }
}