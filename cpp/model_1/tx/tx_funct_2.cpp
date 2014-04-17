//Functions of the transmitter 

#include <algorithm>
#include <iostream>
#include <fstream>
#include <cmath>

#include "tx_funct_2.hpp"

using namespace std;

/** Transfer the information bits to a QPSK constellation
 * @pre:
 *     - data_bin: an array of 0 and 1
 *     - data_qpsk: complex array of double. output. Length nElem 
 *     - nElem: length of data_bin
 *
 * @post:
 *     - data_qpsk is filled with -1/sqrt(2) and 1/sqrt(2) according to
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
void qpsk(double data_bin[], double data_qpsk[], int nElem){
  for (int i=0;i<nElem;i=i+2){
    if( round(data_bin[i])==0){ 
      // 00 mapped to 1+j
      if (round( data_bin[i+1])==0){data_qpsk[i]=1/1.4142 ; data_qpsk[i+1]=1/1.4142;}
      // 01 mapped to 1-j
      else if( round(data_bin[i+1])==1){data_qpsk[i]=1/1.4142 ; data_qpsk[i+1]=-1/1.4142;}
      else {cerr << "The input data_bin is not composed of 01"<<"\n";}
    } 
    else if( round(data_bin[i])==1){
      // 10 mapped to -1+j
      if ( round(data_bin[i+1])==0){data_qpsk[i]=-1/1.4142 ; data_qpsk[i+1]=1/1.4142;}
      // 11 mapped to -1-j
      else if(round(data_bin[i+1])==1){data_qpsk[i]=-1/1.4142 ; data_qpsk[i+1]=-1/1.4142;}
      else {cerr << "The input data_bin is not composed of 01"<<"\n";;}
    }
    else {cerr << "The input data_bin is not composed of 01"<<"\n";;}
  }
}


//int main (){
void imp_1(short allUp[]){
// QPSK, 1st imp

int nData=6250;
int nTrain=100*2; // To store complex part *2
int nPref=500*2;
int nGuard=10*2;
short amp=7000;


 double guard[nGuard]; fill_n(guard,nGuard,0);

// Create storage for Train //////////////////////////
double train[nTrain];

// Read data from file
std::ifstream ifs( "train_norm.dat" , std::ifstream::in );
ifs.read((char * )train,nTrain*sizeof(double));
ifs.close();

 // for (int i=0;i<2*100;i++){
 //   cout << "Train["<<i<<"] = " << train[i]<<"\n";
 // }


// Create storage for Data //////////////////////////////
double Data[nData];


// Read data from file
std::ifstream ifs2( "data.dat" , std::ifstream::in );
ifs2.read((char * )Data,nData*sizeof(double));
ifs2.close();
/*
// Read data from file
std::ifstream ifs2( "data.bin" , std::ifstream::in );
ifs2.read((char * )Data,nData*sizeof(double));
ifs2.close();
*/
/*
 for (int i=0;i<2*4;i++){
   cout << "Data["<<i<<"] = " << Data[i]<<"\n";
 }
*/

// Create storage for prefix ////////////////////////////
double pref[nPref];

// Read data from file
std::ifstream ifs3( "pref_bin.dat" , std::ifstream::in );
ifs3.read((char * )pref,nPref*sizeof(double));
ifs3.close();

// Convert data to QPSK /////////////////////////////////
double data_qpsk[nData];
qpsk(Data, data_qpsk, nData);

// Convert prefix to QPSK ///////////////////////////////
double pref_qpsk[nPref];
qpsk(pref,pref_qpsk,nPref);

// Put all together ////////////////////////////////////
int nAll=nPref+nGuard+nTrain+nData+nTrain+nData;
double all[nAll];
copy(pref, &pref[nPref], all);
copy(guard, &guard[nGuard], &all[nPref]);
copy(train,&train[nTrain], &all[nPref+nGuard]);
copy(data_qpsk, &data_qpsk[nData],&all[nPref+nGuard+nTrain]);
copy(train,&train[nTrain], &all[nPref+nGuard+nTrain+nData]);
copy(data_qpsk, &data_qpsk[nData],&all[nPref+nGuard+nTrain+nTrain+nData]);

// Upsample ///////////////////////////////////////////////
 int Q=4;
 //short allUp[Q*nAll];
 double* p=all;
 short* q=allUp;
 for (int i =0; i<nAll;i=i+2){
   for (int ii=0;ii<Q; ii++){
     *q=(short) (amp*(*p));
     *(q+1)=(short) (amp*(*(p+1)));
     q=q+2;
   }
   p=p+2;
 }
 /* 
 cout<<nAll;
 for (int i=(nPref+nGuard+nTrain-1)*4;i<(nPref+nGuard+nTrain+10)*4;i++){
   cout << "allUp["<<i<<"] = " << allUp[i]<<"\n";
 }
 */
 //return 0;
 //return allUp;
}
