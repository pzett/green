#include <iostream>
//#include "arrayToCvec.cpp"
#include <itpp/itbase.h>
#include <itpp/itsignal.h>
using namespace itpp;
/* 
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
*/

/** Return the frequency offset (1st max- 2nd max) in the range [0.55:0.65[ 
 * @pre:
 *   - data: The data to analyse
 *
 *@post:
 *   - nu: the normalised frequency offset
 */
double freqOff(cvec data){
  // assure to do fft on enough points and power of 2
  int a=0;
  if (data.length()>pow2(16)){a=pow2(ceil(log2(data.length())));}
  else {a=pow2(16); std::cout << "Warning! data too small in freqOff" << std::endl;}

  cvec fftOUT=itpp::fft(data,a);
  //std::cout << "a= " <<a << '\n';
  vec fftABS=abs(fftOUT);
  //std::cout << "fft abs= " << floor(fftABS) << '\n';
  int fftSIZE=fftABS.size();
  //std::cout << "size= " << fftSIZE << '\n';

  // Select the range wherer to look for LO leakage
  vec fftFILT=fftABS.get(floor(0.55*fftSIZE), floor(0.65*fftSIZE));
  //std::cout << "fft filt= " << floor(fftFILT) << '\n';

  //1st Maximum
  int i1=0;
  double maxFFT1=max(fftFILT,i1);
  //std::cout << "max 1= " << i1 <<"->" << maxFFT1 << '\n';
  int range=(int)((double) (fftSIZE/data.length()));
  //std::cout << "range: " << range << std::endl;

  // Put the first max and neighbours to 0
  fftFILT.set_subvector(i1-range,i1+range,0.0);
  //std::cout << "fft Filt 2= " << floor(fftFILT) << '\n';

  // Second max search
  int i2=0;
  double maxFFT2=max(fftFILT, i2);
  //std::cout << "max 2= " << i2 <<"->" << maxFFT2 << '\n';

  // normalised frequency
  double nu= (double) (i2-i1)/fftSIZE;
  return nu;
}

// double freqOff(short buff_short[], int nElem){
//   cvec fftIN=arrayToCvec(buff_short, nElem);
//   return freqOff(fftIN);
// }
/*
int main () {
 std::cout << "here";
  itpp::vec n="0:1:40000";
  // std::cout << "n= " << n << "\n";
  itpp::vec datas=10*cos(2*3.14159*0.6*n)+50*cos(2*3.14159*0.62*n);
  //std::cout << "datas to be sent:" << datas << '\n';
  double nu =freqOff(to_cvec(datas));
  std::cout << "delta nu= " << nu << "real" << 0.62-0.6 << '\n';
}
*/
