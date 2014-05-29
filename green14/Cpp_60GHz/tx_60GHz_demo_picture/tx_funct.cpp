
#include <algorithm>
#include <iostream>
#include <fstream>
#include <cmath>

#include <itpp/base/vec.h>
#include <itpp/itsignal.h>
#include <itpp/itbase.h>
#include <itpp/itcomm.h> 
#include <itpp/stat/misc_stat.h>
#include <itpp/signal/transforms.h>
#include <itpp/comm/interleave.h>

#include <complex>
#include <itpp/comm/modulator.h>
#include <stdio.h> 
#include <math.h>  


 /*
 This function prepares a vector of NCarriers to be transmitted with OFDM.
 */
using namespace std;
using namespace itpp;

extern void tx_funct(int16_t output[],bvec data_bin,int nDataBin);
extern bvec prepairPic(int16_t picRaw[],int nPicRaw);

/** Allocated the data
 * @pre:
 *    - data_qam     : modulated data
 *    - data_pilot   : complex pilot data
 *    - pilot_pattern: pilot pattern
 *    - data_pattern : pattern how the data is stored
 *    - N            : number of subcarriers
 *
 * @post:
 *    - cvec return  : output vector with data and pilots properly placed inside
 */
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

/** Adds the Cyclicprefix and Suffix
 * @pre:
 *    - data_time : data in the time domain
 *    - pre       : cyclic prefix
 *    - post      : suffix
 *
 * @post:
 *    - cvec return : vector of the data in time domain with cyclic prefix and suffix added
 */

cvec prePost (cvec data_time, int pre, int post){

   data_time.ins(0, data_time.right(pre));
   data_time.ins(data_time.length(), data_time.mid(pre,post));
   return(data_time);

}

/** Includes the serial-to-parallel, data allocation, ifft, adding of cyclic prefix and suffix and the parallel-to-serial
 * @pre:
 *    - data_qam     : modulated data
 *    - data_pilot   : complex pilot data
 *    - pilot_pattern: pilot pattern
 *    - data_pattern : pattern how the data is stored
 *    - N            : number of subcarriers
 *    - nUsedC       : number of used carriers
 *    - nPilotC      : number of the complex pilots
 *    - pre          : number of cyclic prefix
 *    - post         : number of suffix
 *
 * @post:
 *    - cvec return  : complex vector filled with the actual ofdm symbols
 */

cvec serial2serial (cvec data_qam,std::complex<double> data_pil[],double data_pattern[], double pilot_pattern[],int N, int nUsedC, int nPilotC, int pre, int post){

    cvec data_alloc=dataAlloc(data_qam,data_pil,pilot_pattern,data_pattern,N);

    cvec data_time=itpp::ifft(data_alloc);//IFFT
 
    cvec out=prePost(data_time,pre,post);
      return(out);
 }

/** Includes the serial-to-parallel, data allocation, ifft, adding of cyclic prefix and suffix and the parallel-to-serial
 * @pre:
 *    - output  : empty array 
 *
 * @post:
 *    - output  : filled array filled with the processed data
 */

void tx_funct(int16_t output[],bvec data_bin,int nDataBin){

  int nCar=32;
  int nUsedC=17;
  int nPilotC=6;
  int pre=10;
  int post=4;
  int nPilotData=6*2000;
  int modulationOrder=4;

  //Load data and initialization parameters

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

 
  //Read pilots from a file;
  double data_pil[2*nPilotData];
  std::ifstream ifs1( "dataPilotN.dat" , std::ifstream::in );
  ifs1.read((char * )data_pil,2*nPilotData*sizeof(double));
  ifs1.close();
  std::cout << "- loading files - check!" << std::endl; 

  cvec outBuffer(0);

 //M-QAM mapping
  bvec dataBvec(nDataBin);
  int nQAM=nDataBin/(log2(modulationOrder));
  QAM md(modulationOrder);

  cvec dataMapped(nQAM);

  for(int i=0;i<nDataBin;i++){
    dataBvec.set(i,(bin) itpp::round(data_bin[i]) );
  }
 
  std::complex<double> *pilotComp=(std::complex<double> *) data_pil;

  md.modulate_bits(dataBvec,dataMapped);

  std::cout << "- mapping - check!" << std::endl;

  //Serial2Serial
  cvec shortBuffer(nUsedC);
  std::complex<double> shortPilot[nPilotC];
  for(int i=0;i<nQAM;){
    for(int j=0;j<nUsedC;j++){   
      
      shortBuffer.set(j,dataMapped.get(i++));
    }
    for(int j=0;j<nPilotC;j++){
      shortPilot[j]=pilotComp[(i/nUsedC-1)*nPilotC+j];
    }
    cvec shortFrame=serial2serial(shortBuffer, shortPilot,data_pattern,pilot_pattern,nCar, nUsedC, nPilotC,pre,post);
    outBuffer.ins(outBuffer.length(),shortFrame);
  }

  std::cout << "- serial2serial - check!" << std::endl;  

  //Training sequence + upsample(and store it in a vec)
  double amp=0.25;
  int nTrain=12;
  int Q=2;
  double train[2*nTrain];
  
  cvec cvecTrain(0);
    
  std::ifstream ifs6( "dataTrain.dat" , std::ifstream::in );
  ifs6.read((char * ) train,2*nTrain*sizeof(double));
  ifs6.close();


  std::complex<double> *trainTmp=(std::complex<double> *) &train[0];

  for(int m=0;m<nTrain;m++){
    for(int n=0;n<Q;n++){
      cvecTrain.ins(cvecTrain.length(),amp*trainTmp[m]);
    }
  }
  //adding guard bits (pseudonoise) at ending of train seq
  std::complex<double> guardTrain;
  for(int i=0;i<46*4;i++){
    guardTrain = itpp::randn_c()/10;
    cvecTrain.ins(nTrain*Q,guardTrain);
  }
  
  outBuffer.ins(0,cvecTrain);
  std::cout << "- training sequence added - check!" << std::endl;
  
  //Insert guard bits & convert to short
  int nGuard=20;
  int w=0;
  
  // RMS computation and correction
  double rms = 0.0;
  double sum = 0.0;
  double norm = 0.0;
  for(int i=0;i<outBuffer.length();i++){
    sum = sum + abs(outBuffer[i]*outBuffer[i]);
    norm++;
  }
  sum = sum/norm;
  rms = sqrt(sum);
  //std::cout << "rms: " << rms << std::endl;
  double ampTotal = 10000/rms;
  //std::cout << "ampl: " << ampTotal << std::endl;
  
  fill_n(&output[0],nGuard,0);
  for(w=0;w<2*outBuffer.length();w++){
    std::complex<double> tmp= outBuffer.get(floor(w/2));
    output[w+nGuard]=(int16_t) (ampTotal*(std::real(tmp)));
    output[++w+nGuard]=(int16_t)( ampTotal*(std::imag(tmp)));
  }
  fill_n(&output[w+nGuard],nGuard,0);
  std::cout << "- doneProcessing - check!" << std::endl;
  std::cout << " " << std::endl;
  
}

/** converts the grayscale picture to a binary data stream and XORs it with pseudonoise
 * @pre:
 *    - picRaw  : grayscale array
 *    - nPicRaw : length of the grayscale array
 *
 * @post:
 *    - return  : converted binary vector
 */

bvec prepairPic(int16_t picRaw[],int nPicRaw){

  bvec temp(8);
  bvec picBin,picBinInter,picBinCompare;
  for(int i=0;i<nPicRaw;i++){
    temp = itpp::dec2bin(8,picRaw[i]);
    picBin.ins(picBin.length(),temp);

  }

  bvec pseudoNoise;
  it_file my_file2("pseudoNoise.it");
  my_file2 >> pseudoNoise;
  my_file2.flush();
  my_file2.close();
  
  picBinInter =  itpp::operator-(pseudoNoise, picBin);

  return picBinInter;

}



