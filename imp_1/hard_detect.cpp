#include <complex>
#include <iostream>

using namespace std;


/** Make hard decision based on QPSK constellation
 * @pre:
 *     - data_bin: an array to be filled with 0 1. Length nElem*2
 *     - data_qpsk: complex with the datas. Length nElem 
 *     - nElem: length of data_qpsk
 *
 * @post:
 *     - data_bin is filled with 0 and 1 according to
 *
 * complex part (Q channel)
 *         ^
 *         |
 *  10 x   |   x 00   (odd bit, even bit)
 *         |
 *  -------+------->  real part (I channel)
 *         |
 *  11 x   |   x 01
 *         |
 */
void hardDetect(complex<double> data_qpsk[], short data_bin[], int nElem){
  int c=0;
  for (int i=0;i<nElem;i++){
    if(data_qpsk[i].real()>=0){ 
      // 00 mapped to 1+j
      if (data_qpsk[i].imag()>=0){data_bin[c]=0 ; data_bin[c+1]=0;}
      // 01 mapped to 1-j
      else if(data_qpsk[i].imag()<0){data_bin[c]=0 ; data_bin[c+1]=1;}
      else {cerr << "Error in hard detect!"<<"\n";}
    } 
    else if(data_qpsk[i].real()<0){
      // 10 mapped to -1+j
      if ( data_qpsk[i].imag()>=0){data_bin[c]=1  ; data_bin[c+1]=0;}
      // 11 mapped to -1-j
      else if(data_qpsk[i].imag()<0){data_bin[c]=1 ; data_bin[c+1]=1;}
      else {cerr << "Error in harDetect!"<<"\n";;}
    }
    else {cerr << "The input data_bin is not composed of 01"<<"\n";;}
    c=c+2;
  }
}

/*
int main(){
  int nElem=8;
  double data[]={1, 1, -1, -1, -1, 1, 1, -1};
  complex<double> *data_qpsk= (complex<double> *) data;
  short data_bin[nElem];
  hardDetect(data_qpsk, data_bin, nElem/2);
    for (int i=0; i<nElem;i++){
      cout<< i << " : " << data_bin[i]<<"\n";
    }
  return 0;
}

*/
