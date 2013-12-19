#include <set>
#include <vector>
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

// vectorをVectorに変換
template<typename T>
Vector<T> extendVector(const std::vector<T>& v){
  Vector<T> V;
  V.resize(v.size());
  for (size_t i = 0; i < v.size(); i++)
    V[i] = v[i];
  return V;
}

double accuracyRate(const Vector<double>& v1, const Vector<double>& v2) {
  int n_errors = 0;
  for(int i=0; i<v1.size(); ++i) {
    if(v1[i] != v2[i])
      n_errors++;
  }
  return n_errors/double(v1.size());
}

double CrossValid::calcAccuracyRate(int n){
  Matrix<double> learningVectors, evalVectors;
  Vector<double> learningAnswerVector, evalAnswerVector;
  std::set<unsigned int> lerningIndexes, evalIndexes;
  std::vector<double> result;
  Vector<double> R;

  double ar = 0;

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
    learningAnswerVector = y.extract(lerningIndexes);
    evalVectors = x.extractRows(evalIndexes);
    evalAnswerVector = y.extract(evalIndexes);

    SVM svm(learningVectors, learningAnswerVector, kernel);

    result.clear();
    for (int j=0; j<evalVectors.nrows(); ++j)
      result.push_back(svm.discriminate(evalVectors.extractRow(j)));
    R = extendVector(result);

    ar = accuracyRate(y, R);
    std::cout << "ar: " << ar << std::endl;
  }
  return ar/n;
}