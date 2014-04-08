#include "freq_off.cpp"
#include "hard_detect.cpp"
#include "sync.cpp"
#include "filter_phase.cpp"
#include <complex>
#include <iostream>
#include "arrayToCvec.cpp"
#include <cmath>
#include "filter.cpp"


using namespace std;
using namespace itpp;

void matchedFilter(std::complex<double> data_compl[],int dataLength,std::complex<double> output[], int outLength){
  double a[] = {1.0,0.0,0.0,0.0};
  double b[] = {1.0,1.0,1.0,1.0};
  int nElemA = 4;
  int nElemB = 4;
  filter(b,nElemB,a,nElemA,data_compl,output,dataLength);
}

void receiverSignalProcessing(short buff_short[],int buffersize,short data_bin[]){

double freqOffset=0.0;

  double buff_double[buffersize];
  for(int k=0;k<buffersize;k++){
    buff_double[k] = buff_short[k];
  }
  cvec buff_cvec = arrayToCvec(buff_double,buffersize);

  freqOffset=freqOff(buff_cvec);
  //std::cout << "... " << freqOffset << std::endl;

  const double pi = std::acos(-1);

  // multiplying elementwise and getting x_complD out!
  double complExp[buffersize];
  double x[buffersize];
  std::complex<double> x_complD[buffersize/2];
  int counter = 0;

  for(int f=0;f<buffersize;f=f+2)
    {
      complExp[f] = cos(-2.00*pi*freqOffset*counter);
      complExp[f+1] = sin(-2.00*pi*freqOffset*counter);
      /* Multiplying the complex arrays elementwise */
      x[f]=complExp[f]*buff_double[f] - complExp[f+1]*buff_double[f+1]; // real part
      x[f+1]=complExp[f]*buff_double[f+1] + complExp[f+1]*buff_double[f]; //imag part
      x_complD[counter]=std::complex<double>(x[f],x[f+1]);
      counter++;
    };

  // for(int k=0;k<=(buffersize/2);k++){
  // std::cout << x_complD[k] << std::endl;
  // }

  /* Debugging Outputs */
  // std::cout << "length buff_short  : " << sizeof(buff_short)/sizeof(buff_short[0]) << std::endl;
  // std::cout << "length buff_double : " << sizeof(buff_double)/sizeof(buff_double[0]) << std::endl;
  // std::cout << "length x           : " << sizeof(x)/sizeof(x[0]) << std::endl;
  // std::cout << "length x_complD    : " << sizeof(x_complD)/sizeof(x_complD[0]) << std::endl;
  // std::cout << "length complExp    : " << sizeof(complExp)/sizeof(complExp[0]) << std::endl;
  // std::cout << "counter: " << counter << std::endl;
  // std::cout << buff_cvec << std::endl;
  /* ----------------  */

  std::cout << " Frequency Offset Removed! " << std::endl;

  // matched filtering
  std::complex<double> x_matchedFilt [buffersize/2];
  matchedFilter(x_complD,buffersize/2,x_matchedFilt,buffersize/2);
  
  std::cout << " Matched Filtered! " << std::endl;

  // synchronization
  int nTrainSeq = 100;
  std::complex<double> trainSeq[nTrainSeq];
  double phase=0;
  
  // Loading the training sequenz
  std::ifstream ifs3( "train_norm.dat" , std::ifstream::in );
  ifs3.read((char * )trainSeq,nTrainSeq*sizeof(double));
  ifs3.close();

  // for(int k=0;k<100;k++){
  // std::cout << trainSeq[k] << std::endl;
  // }
  // for(int k=0;k<100;k++){
  // std::cout << x_matchedFilt[k] << std::endl;
  // }

  int delay = synch(x_matchedFilt,(buffersize/2),trainSeq, nTrainSeq, &phase);
  //std::cout << " " << std::endl;
  //std::cout << delay << std::endl;
  //std::cout << phase << std::endl;

  std::cout << " Synchronized! " << std::endl;

  // downsampling
  std::complex<double> x_downsampled[buffersize/8];
  int iter = 0;
  for(int i=0;i<=buffersize;i=i+4){
    x_downsampled[iter]=x_matchedFilt[delay+i];
    iter++;
  }

  std::cout << " Downsampled! " << std::endl;

 // for(int k=0;k<buffersize/8;k++){
 //  std::cout << x_downsampled[k] << std::endl;
 //  }
 // std::cout << "length x_downsampled    : " << sizeof(x_downsampled)/sizeof(x_downsampled[0]) << std::endl;


  // filter the phase offset
  complex<double> * res;
  int res_size=nTrainSeq;
  res= new complex<double>[res_size];  

  res_size= filter_phase( x_downsampled, sizeof(x_downsampled)/sizeof(x_downsampled[0]),  phase, trainSeq, nTrainSeq, 0.01,0.01, 0.01, res);

  std::cout << " Phase Filteres! " << std::endl;

  // detect
  //short data_bin[buffersize/8];
  hardDetect(res,data_bin,buffersize/8);

  std::cout << " Detected! " << std::endl;

  std::cout << " " << std::endl;
  std::cout << " Done! " << std::endl;
}

//cvec complExp(double freqOffset,


int main(){

  // creating some testing data
  int buffersize = 1000;
  short buff_short[buffersize];
  // creating data
  for(int i=0;i<buffersize-1; i=i+2){
    buff_short[i]=(short) 3*cos(3.14159*0.62*i) +1* cos(3.14159*0.56*i);//+cos(2*3.14159*0.2*i);
    buff_short[i+1]=(short) 10*sin(3.14159*0.6*i);
    //std::cout << buff_short[i] << " " << buff_short[i+1] << std::endl;
  };

  short data_bin[buffersize/8];
  receiverSignalProcessing(buff_short,buffersize,data_bin);


// for(int k=0;k<buffersize/8;k++){
//   std::cout << data_bin[k] << std::endl;
//   }
//  std::cout << "length data_bin    : " << sizeof(data_bin)/sizeof(data_bin[0]) << std::endl;  

  
  return 0;
}
