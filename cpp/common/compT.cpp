//#include <itpp/itbase.h>
//#include <iostream>
using namespace std;
using namespace itpp;



/** Convert an array of complex short into an array of complex double
 * @pre:
 *    - array: the pointer to the array of complex short
 *    - nElem: the number of elem in array
 *    - out: a pointer to an complex double array of size nElem
 *
 * @post:
 *    - out is now filled with complex double
 */
void compShortToCompDouble(complex<short> *array, int nElem, complex<double>* out){
  double arrayS[nElem*2];
  int c=0;
  // Copy the datas to an array of double
  for (int i=0; i<nElem;i++){
    arrayS[c]=(double) array[i].real();
    arrayS[c+1]=(double) array[i].imag();
    //cout<< arrayS[c] << ".." << arrayS[c+1]<< "\n";
    c=c+2;
  }
  /*  out= (complex<double> *) arrayS;
  for (int i =0; i<nElem;i++){
  std::cout << "datas received IN:" << out[i]<<".." << '\n';
  }
  */
  c=0;
  // Fill out with objects
  for (int i =0; i<nElem;i++){   
    out[i]= complex<double>( arrayS[c], arrayS[c+1]);
    c=c+2;
    //std::cout << "datas received IN:" << arrayS[i] << '\n';
  }
  //  return out;
}

/** Convert an array of complex double into an array of complex short
 * @pre:
 *    - array: the pointer to the array of complex double
 *    - nElem: the number of elem in array
 *    - out: a pointer to an complex short array of size nElem
 *                                               --------------
 * @post:
 *    - out is now filled with complex short
 */
void compDoubleToCompShort(complex<double> *array, int nElem, complex<short>* out){
  short arrayS[nElem*2];
  int c=0;
  // Copy the data to an array of short
  for (int i=0; i<nElem;i++){
    arrayS[c]=(short) array[i].real();
    arrayS[c+1]=(short) array[i].imag();
    //cout<< arrayS[c] << ".." << arrayS[c+1]<< "\n";
    c=c+2;
  }
  /*  out= (complex<double> *) arrayS;
  for (int i =0; i<nElem;i++){
  std::cout << "datas received IN:" << out[i]<<".." << '\n';
  }
  */
  c=0;
  //Fill out
  for (int i =0; i<nElem;i++){   
    out[i]= complex<short>( arrayS[c], arrayS[c+1]);
    c=c+2;
    //std::cout << "datas received IN:" << arrayS[i] << '\n';
  }
  //  return out;

}
