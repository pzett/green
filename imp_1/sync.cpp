#include <iostream>
#include <complex>
using namespace std;
/** Return the index of the start of the training sequence and fill phase with its phase
 *
 * @pre:
 *    - dataC: array of the received data
 *    - nElem: size of dataC[]
 *    - trainC: pointer to array of the training sequence
 *             !!!  length assume to be < than nElem!!!
 *    - nElemT: size of trainC[]
 *    - phase: pointer to a double
 *
 * @post:
 *    - phase is now filled with the phase of trainC[index]
 *    - return the index of the beginning of the training sequence
 */
int synch(complex<double> dataC[], int nElem, complex<double> trainC[], int nElemT, double *phase){

  if(nElem<nElemT){cerr << "The training sequence is too big compare to datas";}

  //Takes the conjugate
  complex<double> trainCC[nElemT];
  for (int i=0; i<nElemT; i++){
    trainCC[i]=conj(trainC[i]);
  }

  complex<double> xcorr[nElem];
  // Do the multiplication element wise
  for (int i=0;i<nElem-nElemT+1;i++){
    xcorr[i]=complex<double> (0,0);
    for (int ii=0;ii<nElemT;ii++){
      xcorr[i]=xcorr[i]+dataC[i+ii]*trainCC[ii];
    }
    //cout << "xcorr " << i << "= " << xcorr[i]<<"\n";
  }

  //Search for the maximum
  double max=0.0;
  int index=0;
  for(int i=0;i<nElem;i++){
    double tmp=abs(xcorr[i]);
    if (tmp> max){
      max=tmp;
      index=i;
    }
  }
  *phase=arg(trainC[index]);
  return index;
}
/*
int main(){
  int nElem=8;
  double data[]={-1,1,1,1,1,1,-1,1};
  int nElemT=4;
  double train[]={1,1,-1,1};
  int delay=0;
  double phase=0;
  complex<double>* dataC=(complex<double> *) data;  
  complex<double>* trainC= (complex<double> *) train;
  delay=synch(dataC,nElem/2,trainC, nElemT/2, &phase);
  cout <<"   "<<delay<<"\n";
  return 0;
}
*/
