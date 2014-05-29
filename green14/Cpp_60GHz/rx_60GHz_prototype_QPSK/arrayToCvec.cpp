#include <itpp/itbase.h>
#include <iostream>

using namespace std;
using namespace itpp;

// Convert an array of nElem complex<double> to a cvec
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

// Convert an array of nElem double (Real-Imag-Real-...) to a cvec
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
