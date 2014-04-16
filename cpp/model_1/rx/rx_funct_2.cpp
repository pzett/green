#include <complex>
#include <iostream>

#include <itpp/itbase.h>
#include <itpp/itsignal.h>


#include "common.cpp"
#include "filter_phase.cpp"
#include "arrayToCvec.cpp"

using namespace std;
using namespace itpp;

extern void receiverSignalProcessing(short buff_short[], int buffersize,short data_bin[], int nDataC);


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

  // int norm=0;
  // for(int i;i<nElemT;i++){
  //   norm=norm+abs(trainC[i]);
  // }
  // norm=norm/nElemT;

  // Takes the conjugate - > Problem at the receiver
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
  int search=10180;
  for(int i=0;i<search;i++){
    double tmp=abs(xcorr[i]);
    if (tmp> max){
      max=tmp;
      index=i;//Some problem over here also
    }
  }
  *phase=arg(trainC[index]);

   // Save data to file
     std::ofstream ofs4( "xcorr.dat" , std::ifstream::out );
     ofs4.write((char * ) xcorr,2*nElem*sizeof(double));
     ofs4.close();

  return index;
}

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

void matchedFilter(double data_compl[],int dataLength,double output[], int outLength){
  double a[] = {1.0};
  double b[] = {1.0,1.0,1.0,1.0};
  int nElemA = 1;
  int nElemB = 4;
  filterComp(b,nElemB,a,nElemA,data_compl, output, dataLength);
}

template<class T>
void hold_zeros(T data[], T out[], int Q, int nElem){
  int c=0;
  for (int i=0; i<nElem;i++){
    for (int d=0;d<Q;d++){
    
      if(d==0) out[c+d]=data[i];
      else out[c+d]=0.0;
    }
    c=c+Q;
  }
}


void receiverSignalProcessing(short buff_short[], int buffersize,short data_bin[], int nDataB){

double freqOffset=0.0;
 
  //Conversion to double -> More precision in processing
  double buff_double[buffersize];
  for(int k=0;k<buffersize;k++){
    buff_double[k] =(double) buff_short[k];
  }
  cvec buff_cvec = arrayToCvec(buff_double,buffersize);

  //Frequency Offset computation
  freqOffset=freqOff(buff_cvec);
  std::cout << "freqOffset= " << freqOffset << std::endl;


  const double pi = std::acos(-1);

  // multiplying elementwise and getting x_complD 
  double complExp[buffersize];
  double x[buffersize];
 
  int counter = 0;

  for(int f=0;f<buffersize;f=f+2)
    {
      complExp[f] = cos(2.00*pi*freqOffset*counter);
      complExp[f+1] = sin(-2.00*pi*freqOffset*counter);
      // Multiplying the complex arrays elementwise 
      x[f]=complExp[f]*buff_double[f] - complExp[f+1]*buff_double[f+1]; // real part
      x[f+1]=complExp[f]*buff_double[f+1] + complExp[f+1]*buff_double[f]; //imag part

      //just debugging
      // x[f]=buff_double[f];
      // x[f+1]=buff_double[f+1];
     
      counter++;
    };

  // Save data to file
     std::ofstream ofs( "x_freq.dat" , std::ifstream::out );
     ofs.write((char * ) x, buffersize*sizeof(double));
     ofs.close();

 

  
  // for(int k=0;k<=(10);k++){
    // std::cout << x[k] << std::endl;
   // }

  // Debugging Outputs 
  // std::cout << "length buff_short  : " << sizeof(buff_short)/sizeof(buff_short[0]) << std::endl;
  // std::cout << "length buff_double : " << sizeof(buff_double)/sizeof(buff_double[0]) << std::endl;
  // std::cout << "length x           : " << sizeof(x)/sizeof(x[0]) << std::endl;
  // std::cout << "length x_complD    : " << sizeof(x_complD)/sizeof(x_complD[0]) << std::endl;
  // std::cout << "length complExp    : " << sizeof(complExp)/sizeof(complExp[0]) << std::endl;
  // std::cout << "counter: " << counter << std::endl;
  // std::cout << buff_cvec << std::endl;
  // ----------------  
  
  std::cout << " Frequency Offset Removed! " << std::endl;




  /////////////////////////// matched filtering//////////////////////
  double x_matchedFilt [buffersize];

  matchedFilter(x, buffersize, x_matchedFilt, buffersize);

   
  

 
  std::cout << " Matched Filtered! " << std::endl;

  // synchronization
  int nTrainSeq = 100;
  std::complex<double> trainSeq[nTrainSeq];
  double phase=0.0;
  
  // Loading the training sequence
  std::ifstream ifs3( "train_norm.dat" , std::ifstream::in );
  ifs3.read((char * )trainSeq,2*nTrainSeq*sizeof(double));
  ifs3.close();

  // for(int i=0; i<nTrainSeq; i++){
  //   DispVal(trainSeq[i]);
  // }


  // for(int k=0;k<100;k++){
  // std::cout << x_matchedFilt[k] << std::endl;
  // }

   std::complex<double> x_matchedFiltC[buffersize/2];
   for(int i=0,count=0;i<buffersize;i=i+2){
     x_matchedFiltC[count]=std::complex<double>(x_matchedFilt[i],x_matchedFilt[i+1]);
     count++;
   }

    // Save data to file
     std::ofstream ofs1( "x_matched.dat" , std::ifstream::out );
     ofs1.write((char * ) x_matchedFiltC, buffersize*sizeof(double));
     ofs1.close();

   
   int Q=4;

   std::complex<double> trainSeqUp[Q*nTrainSeq];
   
   hold_zeros(trainSeq, trainSeqUp, Q, nTrainSeq);

  int delay = synch(x_matchedFiltC,buffersize/2,trainSeqUp, Q*nTrainSeq, &phase);
  
  
  DispVal(phase);
  DispVal(delay);

  std::cout << " Synchronized! " << std::endl;

  int nDataC=6250/2;
  int nDataTrain=nDataC+nTrainSeq;
  //DispVal(nDataTrain);
  // downsampling
  std::complex<double> x_downsampled[nDataTrain];
  for(int iter=0;iter<nDataTrain;iter++){
    x_downsampled[iter]=x_matchedFiltC[delay+iter*4];
  }

  // Save data to file
     std::ofstream ofs2( "x_downsamp.dat" , std::ifstream::out );
     ofs2.write((char * ) x_downsampled, 2*(nDataTrain)*sizeof(double));
     ofs2.close();
  
  

  std::cout << " Downsampled! " << std::endl;

 // for(int k=0;k<buffersize/8;k++){
 //  std::cout << x_downsampled[k] << std::endl;
 //  }
 // std::cout << "length x_downsampled    : " << sizeof(x_downsampled)/sizeof(x_downsampled[0]) << std::endl;


  // filter the phase offset
  complex<double> * DataFilt;
  
  DataFilt= new complex<double>[nDataC];  
  
  
  int nDataCConf = filter_phase( x_downsampled, nDataTrain, phase, trainSeq, nTrainSeq, 0.01, 0.01, 0.01, DataFilt);

  // for(int k=0;k<nDataC;k++){
  //     std::cout << DataFilt[k] << std::endl;
  // }

   // Save data to file
     std::ofstream ofs3( "x_filt.dat" , std::ifstream::out );
     ofs3.write((char * ) DataFilt, 2*nDataC*sizeof(double));
     ofs3.close();


  if(nDataCConf!=nDataC){
    std::cerr<<"Error in filtering the Phase!\n";
    exit(1);
  }
    


  std::cout << " Phase Filtered! " << std::endl;

  // detect
  
  // Error to be solved
  // for(int i=0;i<nDataC;i++){
  //   DataFilt[i]=conj(DataFilt[i]);
  // }

  hardDetect(DataFilt, data_bin, nDataC);


  std::cout << " Detected! " << std::endl;

  std::cout << " " << std::endl;
  std::cout << " Done! " << std::endl;

}
