
#include <algorithm>
#include <iostream>
#include <fstream>
#include <cmath>
#include <itpp/base/vec.h>
#include <itpp/itbase.h>
#include <itpp/itsignal.h>
#include <complex>
#include <itpp/comm/modulator.h>


 /*
 This function prepares a vector of NCarriers to be transmitted with OFDM.
 */
using namespace std;
using namespace itpp;

extern void tx_funct(short output[]);
extern void tx_funct_hcrs(short output[]);

cvec dataAlloc (cvec data_qam, std::complex<double> data_pilot[], double pilot_pattern[], double data_pattern[], int N){

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
     data_alloc.set(i,data_qam.get(n));   
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


cvec serial2serial (cvec data_qam,std::complex<double> data_pil[],double data_pattern[], double pilot_pattern[],int N, int nUsedC, int nPilotC, int pre, int post){

    cvec data_alloc=dataAlloc(data_qam,data_pil,pilot_pattern,data_pattern,N);
    
    cvec data_time=itpp::ifft(data_alloc);//IFFT
    //std::cout << "Prefix and postfix added" << std::endl;
 
    cvec out=prePost(data_time,pre,post);
      return(out);
 }


void tx_funct(short output[]){

  int nCar=128;
  int nUsedC=89;
  int nPilotC=10;
  int pre=18;
  int post=1;
  int nDataBin=10000;
  int nPilotData=2000;
  int nBits=4;

  //Load data and initialization parameters
  //Read data from a file
  double data_bin[nDataBin];
  std::ifstream ifs( "dataBinNPaq.dat" , std::ifstream::in );
  ifs.read((char * )data_bin,nDataBin*sizeof(double));
  ifs.close();
  
  //Read pilots from a file;
  double data_pil[nPilotData];
  std::ifstream ifs2( "dataPilotN.dat" , std::ifstream::in );
  ifs2.read((char * )data_pil,nPilotData*sizeof(double));
  ifs2.close();
  
  //Read data pattern from a file
  double data_pattern[nUsedC];
  std::ifstream ifs3( "dataPattern.dat" , std::ifstream::in );
  ifs3.read((char * )data_pattern,nUsedC*sizeof(double));
  ifs3.close();
  
  //Read pilot pattern from a file
  double pilot_pattern[nPilotC];
  std::ifstream ifs4( "pilotPattern.dat" , std::ifstream::in );
  ifs4.read((char * )pilot_pattern,nPilotC*sizeof(double));
  ifs4.close();

  cvec outBuffer(0);

 //M-QAM mapping
  bvec dataBvec(nDataBin);
  int nQAM=nDataBin/(log2(nBits)); 
  QAM md(nBits);
  cvec dataMapped(nQAM);
  for(int i=0;i<nDataBin;i++){
    dataBvec.set(i,(bin) data_bin[i] );
  }
  std::complex<double> *pilotComp=(std::complex<double> *) data_pil;
  std::cout << "- mapping - check!" << std::endl;
  md.modulate_bits(dataBvec,dataMapped);
  // for(int i=0;i<nQAM;i++){
  // std::cout << dataMapped.get(i) << std::endl;
  // }

  //Serial2Serial
  cvec shortBuffer(nUsedC);
  std::complex<double> shortPilot[nPilotC];
  for(int i=0;i<nQAM;){
    for(int j=0;j<nUsedC;j++){
      if(i>=nQAM){
	shortBuffer.set(j,0);//Zero padding --> need of random sequence instead of zeros;
	   i++;  
      }         
      else {shortBuffer.set(j,dataMapped.get(i++));}
    }
    for(int j=0;j<nPilotC;j++){
      shortPilot[j]=pilotComp[(i/nUsedC-1)*nPilotC+j];
    }
    cvec shortFrame=serial2serial(shortBuffer, shortPilot,data_pattern,pilot_pattern,nCar, nUsedC, nPilotC,pre,post);
    outBuffer.ins(outBuffer.length(),shortFrame);
  }
  std::cout << "- serial2serial - check!" << std::endl;
  std::cout << outBuffer.length()<< std::endl;
  
  //Training sequence + upsample(and store it in a vec)
    double amp=0.5;
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
    fill_n(&output[w+nGuard],nGuard,0);
    std::cout << "- done - check!" << std::endl;

}



