
#include <algorithm>
#include <iostream>
#include <fstream>
#include <cmath>
#include <itpp/itbase.h>
#include <itpp/itsignal.h>
#include <complex>


 /*
 This function prepares a vector of NCarriers to be transmitted with OFDM.
 */
using namespace std;
using namespace itpp;

extern void tx_funct(  int N,int nUsedC,int nPilotC,int pre,int post,int nDataBin, int nPilotData,int nBits,double data_pattern[],double data_bin[],double data_pil[],double pilot_pattern[],short output[]);

cvec dataAlloc (std::complex<double> data_qam[], std::complex<double> data_pilot[], double pilot_pattern[], double data_pattern[], int N){

  int n=0;
  int m=0;
  int u=0;
  std::complex<double> zeroC(0,0);
  cvec data_alloc(N);

  for(int i=0;i<N;i++){;
   if (pilot_pattern[m]-1==i){
     data_alloc.set(i,data_pilot[m]);   
      m++;
     
   }
   else if (data_pattern[n]-1==i){
     data_alloc.set(i,data_qam[n]);   
      n++;
   }
   else{
     data_alloc.set(i,zeroC);
     u++;
   }
  
 }

  return(data_alloc);

}








cvec prePost (cvec data_time, int pre, int post){

   data_time.ins(0, data_time.right(pre));
   data_time.ins(data_time.length(), data_time.mid(pre,post));
   return(data_time);

}







cvec serial2serial (std::complex<double> data_qam[],std::complex<double> data_pil[],double data_pattern[], double pilot_pattern[],int N, int nUsedC, int nPilotC, int pre, int post, bool firstDone){

    cvec data_alloc=dataAlloc(data_qam,data_pil,pilot_pattern,data_pattern,N);
    
    cvec data_time=itpp::ifft(data_alloc);//IFFT
    //std::cout << "Prefix and postfix added" << std::endl;
 
    cvec out=prePost(data_time,pre,post);
      return(out);
 }

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
void qpsk(double data_bin[],std::complex<double> data_qpsk[], int nElem){
  for (int i=0;i<nElem;i=i+2){
    if(itpp::round(data_bin[i])==0){ 
      // 00 mapped to 1+j
      if (itpp::round(data_bin[i+1])==0){data_qpsk[i/2].real()=1/1.4142 ; data_qpsk[i/2].imag()=1/1.4142;}
      // 01 mapped to 1-j
      else if(itpp::round(data_bin[i+1])==1){data_qpsk[i/2].real()=1/1.4142 ; data_qpsk[i/2].imag()=-1/1.4142;}
      else {cerr << "The input data_bin is not composed of 01"<<"\n";}
    } 
    else if(itpp::round(data_bin[i])==1){
      // 10 mapped to -1+j
      if (itpp::round(data_bin[i+1])==0){data_qpsk[i/2].real()=-1/1.4142 ; data_qpsk[i/2].imag()=1/1.4142;}
      // 11 mapped to -1-j
      else if(itpp::round(data_bin[i+1])==1){data_qpsk[i/2].real()=-1/1.4142 ; data_qpsk[i/2].imag()=-1/1.4142;}
      else {cerr << "The input data_bin is not composed of 01"<<"\n";;}
    }
    else {cerr << "The input data_bin is not composed of 01"<<"\n";;}
  }

}

void mqam(double data_bin[],std::complex<double> data_mqam[], int nElem, int nBits){
  int counter = 0;
  //int nBits = nBitsPerAxis-1;
  for(int i=0;i<nElem;i=i+2*nBits){
    double summation1 = 0.0;
    double summation2 = 0.0;
    for(int k=1;k<=nBits;k++){
      double sumBits1 = 0.0;
      double sumBits2 = 0.0;
      for(int j=0;j<k;j++){sumBits1 = sumBits1 + data_bin[i+j];sumBits2 = sumBits2 + data_bin[i+j+nBits];};
      summation1 = summation1 + pow(2.0,nBits-k)*pow(-1.0,sumBits1);
      summation2 = summation2 + pow(2.0,nBits-k)*pow(-1.0,sumBits2);
      data_mqam[counter].real(summation1/sqrt(2));
      data_mqam[counter].imag(summation2/sqrt(2)); 
      //  std::cout << data_mqam[counter].real() << ", " << data_mqam[counter].imag() << std::endl;
      //std::cout << sumBits2 << std::endl;
    }
    counter++;
  }
}

void tx_funct(  int N,int nUsedC,int nPilotC,int pre,int post,int nDataBin, int nPilotData,int nBits,double data_pattern[],double data_bin[],double data_pil[],double pilot_pattern[],short output[]){

  cvec outBuffer(0);
 //M-QAM mapping
  int nQAM=nDataBin/(nBits*2); 
     std::cout << "Number of symbols" <<nQAM <<std::endl;
     std::complex<double> dataMapped[nQAM];
     if(nBits==1){
     qpsk(data_bin,dataMapped, nDataBin);
     }
     else if(nBits>1){
     mqam(data_bin,dataMapped, nDataBin, nBits);
     }
     else{cerr << "Not known modulation for!" <<"\n";;}
     std::complex<double> *pilotComp=(std::complex<double> *) data_pil;
     std::cout << "- mapping - check!" << std::endl;

  //Serial2Serial
     std::complex<double> shortBuffer[nUsedC];
     std::complex<double> shortPilot[nPilotC];
     bool firstDone = false;
     for(int i=0;i<nQAM;){
       for(int j=0;j<nUsedC;j++){
	 if(i>=nQAM){
	   shortBuffer[j]=0;//Zero padding --> need of random sequence instead of zeros;
	   i++;  
	 }         
	 else {shortBuffer[j]=dataMapped[i++];}
       }
       for(int j=0;j<nPilotC;j++){
	 shortPilot[j]=pilotComp[(i/nUsedC-1)*nPilotC+j];
       }
       cvec shortFrame=serial2serial(shortBuffer, shortPilot,data_pattern,pilot_pattern,N, nUsedC, nPilotC,pre,post,firstDone);
       outBuffer.ins(outBuffer.length(),shortFrame);
     }
     std::cout << "- serial2serial - check!" << std::endl;

     //std::cout << outBuffer.length()<< std::endl;
  //Training sequence + upsample(and store it in a vec)
    int amp=1;
    int nTrain=20;
    int Q=4;
    double train[2*nTrain];
    cvec cvecTrain(0);
    
    std::ifstream ifs5( "dataTrain.dat" , std::ifstream::in );
    ifs5.read((char * )train,2*nTrain*sizeof(double));
    ifs5.close();
    std::complex<double> *trainTmp=(std::complex<double> *) &train[0];

    for(int m=0;m<nTrain;m++){
      for(int n=0;n<Q;n++){
      cvecTrain.ins(cvecTrain.length(),amp*trainTmp[m]);
      }
    }
    outBuffer.ins(0,cvecTrain);
    std::cout << "- training sequence added - check!" << std::endl;
  //Insert guard bits & convert to short
     int nGuard=20;
     int w=0;
     int ampTotal=7000;
    fill_n(&output[0],nGuard,0);
    for(w=0;w<2*outBuffer.length();w++){
      std::complex<double> tmp= outBuffer.get(floor(w/2));
      output[w+nGuard]=(short) ampTotal*std::real(tmp);
      output[++w+nGuard]=(short) ampTotal*std::imag(tmp);
    }
    fill_n(&output[w+nGuard-1],nGuard,0);
    std::cout << "- done - check!" << std::endl;

}
