#include <iostream>

using namespace std;

/** Transfer the information bits to a QPSK constellation
 * @pre:
 *     - data_bin: an array of 0 and 1
 *     - data_qpsk: complex array of short. output. Length nElem 
 *     - nElem: length of data_bin
 *
 * @post:
 *     - data_qpsk is filled with -1 and 1 according to
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
void qpsk(int data_bin[], short data_qpsk[], int nElem){
  for (int i=0;i<nElem;i=i+2){
    if(data_bin[i]==0){ 
      // 00 mapped to 1+j
      if (data_bin[i+1]==0){data_qpsk[i]=1 ; data_qpsk[i+1]=1;}
      // 01 mapped to 1-j
      else if(data_bin[i+1]==1){data_qpsk[i]=1 ; data_qpsk[i+1]=-1;}
      else {cerr << "The input data_bin is not composed of 01"<<"\n";}
    } 
    else if(data_bin[i]==1){
      // 10 mapped to -1+j
      if ( data_bin[i+1]==0){data_qpsk[i]=-1 ; data_qpsk[i+1]=1;}
      // 11 mapped to -1-j
      else if(data_bin[i+1]==1){data_qpsk[i]=-1 ; data_qpsk[i+1]=-1;}
      else {cerr << "The input data_bin is not composed of 01"<<"\n";;}
    }
    else {cerr << "The input data_bin is not composed of 01"<<"\n";;}
  }
}
/*
int main(){
  int nElem=8;
  int data_bin[]={0, 2, 0, 1, 1, 0, 1, 1};
  short data_qpsk[nElem];
  qpsk(data_bin, data_qpsk, nElem);
    for (int i=0; i<nElem;i++){
      cout<< i << " : " << data_qpsk[i]<<"\n";
    }
  return 0;
}
*/
