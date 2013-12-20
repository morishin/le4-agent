svmtest:
	g++ lib/QuadProg++/QuadProg++.cc src/SVM.cpp src/svmtest.cpp -o svmtest.out
cv:
	g++ lib/QuadProg++/QuadProg++.cc src/SVM.cpp src/CrossValid.cpp src/svmtest.cpp -o svmtest.out
