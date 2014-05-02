#include <iostream>

#include <cmath>
//#include "arrayToCvec.cpp"
#include <itpp/itbase.h>
#include <itpp/itsignal.h>
#include <string>
using namespace itpp;

void fillMatrix(itpp::mat *A){
  A->set_submatrix(0,0, itpp::mat("1 10 ; 3 4"));
  std::cout << "A in fillMatrix: " << *A << std::endl;
}

int main (){
  itpp::mat A(2,2);
  fillMatrix(&A);
  std::cout << "A in main: " << A << std::endl;
  return 0;
}
