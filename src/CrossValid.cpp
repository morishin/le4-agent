#include <stdio.h>
#include <set>
#include <vector>
#include "CrossValid.h"

using namespace QuadProgPP;

CrossValid::CrossValid(Matrix<double> _x,
                       Vector<double> _y,
                       Kernel* _kernel):kernel(_kernel), x(_x), y(_y) {};

// Vectorを標準出力する
template<typename T>
void printVector(const Vector<T>& v) {
  for (int i = 0; i < v.size(); ++i) {
    std::cout << v[i] << " ";
  }
  std::cout << std::endl;
}

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
  if(v1.size() != v2.size()){
    std::cout << "Vector sizes are different" << std::endl;
    return -1;
  }
  int n_errors = 0;
  for(int i=0; i<v1.size(); ++i) {
    if(v1[i] != v2[i]) {
      n_errors++;
    }
  }
  return 1 - n_errors/double(v1.size());
}

double CrossValid::calcAccuracyRate(int n){
  Matrix<double> learningVectors, evalVectors;
  Vector<double> learningAnswerVector, evalAnswerVector;
  std::set<unsigned int> lerningIndexes, evalIndexes;
  std::vector<double> result;
  Vector<double> R;
  SVM *svm;

  double ar = 0;

  int n_indexes = int(x.nrows() / float(n));
  std::cout << "n_indexes: " << n_indexes << std::endl;
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
  
    // FILE *gp = popen("gnuplot -persist", "w");
    // fprintf(gp, "set multiplot\n");
    // fprintf(gp, " set xrange [0:50]\n");
    // fprintf(gp, "set yrange [0:50]\n");
    // fprintf(gp, "plot '-' w p pt 5 lc 1\n");
    // for(int j=0; j<learningAnswerVector.size(); ++j) {
    //   if(learningAnswerVector[j] == 1){
    //     fprintf(gp, "%f\t%f\n", learningVectors[j][0], learningVectors[j][1]);
    //   }
    // }
    // fprintf(gp, "e\n");
    // fprintf(gp, "plot '-' w p pt 5 lc 3\n");
    // for(int j=0; j<learningAnswerVector.size(); ++j) {
    //   if(learningAnswerVector[j] == -1){
    //     fprintf(gp, "%f\t%f\n", learningVectors[j][0], learningVectors[j][1]);
    //   }
    // }
    // fprintf(gp, "e\n");
    // fprintf(gp, "set nomultiplot\n");
    // fprintf(gp, "exit\n");
    // fflush(gp);
    // pclose(gp);
    
    svm = new SVM(learningVectors, learningAnswerVector, kernel);

    result.clear();
    for (int j=0; j<evalVectors.nrows(); ++j)
      result.push_back(svm->discriminate(evalVectors.extractRow(j)));
    R = extendVector(result);

    printVector(R);
    printVector(evalAnswerVector);
    double tmp = accuracyRate(R, evalAnswerVector);
    std::cout << i << ": " << tmp << std::endl;
    std::cout << "----------" << std::endl;
    ar += tmp;
  }

  return ar/n;
}