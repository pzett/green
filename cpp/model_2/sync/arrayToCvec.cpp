#include <itpp/itbase.h>
#include <iostream>

using namespace std;
using namespace itpp;

cvec arrayToCvec(std::complex<double> *array, int nElem){
  //std::cout<< b << "\n";
  cvec out(nElem);
  for (int i=0; i<nElem;i++){
    out.set(i,array[i]);
    //std::cout<< i << "\n";
  }
  //std::cout << "power= " << out.get(0, b-1) << '\n';
  return out;
}
/*
cvec arrayToCvec( complex<short> * array,int nElem){
  complex<double> *arrayD= compShortToCompDouble(array, nElem);
  return arrayToCvec(arrayD, nElem);
  }
*/
cvec arrayToCvec(double *array, int nElem){
  std::complex<double> *arrayC=(std::complex<double> *) array; // Cast the double to complex double
  return arrayToCvec(arrayC, nElem/2);

}

cvec arrayToCvec(short *array, int nElem){
  double arrayD[nElem];
  for (int i=0; i<nElem ; i++){
    arrayD[i]=(double) array[i]; // cast the short to double
  }
  cvec out= arrayToCvec(arrayD,nElem);
  return out;
}
/*
int main () {
  int nElem=8;
  short data[]={0, 0, 1, 1, -1, -1, 0.1, -0.3};
  //std::cout << "datas to be sent:" << data << '\n';
  cvec out; 
  // cvec a(8);
  // std::cout <<"a size. "<< a.size()<< "\n";
  out=arrayToCvec(data, nElem);
  std::cout << "power= " << out.get(0, out.size()-1) << '\n';
}
*/
