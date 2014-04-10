using namespace std;
using namespace itpp;


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

void matchedFilter(std::complex<double> data_compl[],int dataLength,std::complex<double> output[], int outLength){
  double a[] = {1.0};
  double b[] = {1.0,1.0,1.0,1.0};
  int nElemA = 1;
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

  //cout << " buff_cvec: " << buff_cvec << endl;
  freqOffset=freqOff(buff_cvec);
  std::cout << "freqOffset " << freqOffset << std::endl;
  
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
      // Multiplying the complex arrays elementwise 
      x[f]=complExp[f]*buff_double[f] - complExp[f+1]*buff_double[f+1]; // real part
      x[f+1]=complExp[f]*buff_double[f+1] + complExp[f+1]*buff_double[f]; //imag part
      x_complD[counter]=std::complex<double>(x[f],x[f+1]);
      counter++;
    };


  
  // for(int k=0;k<=(buffersize/2);k++){
  // std::cout << x_complD[k] << std::endl;
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

  // matched filtering
  std::complex<double> x_matchedFilt [buffersize/2];
  matchedFilter(x_complD,buffersize/2,x_matchedFilt,buffersize/2);


  // FILE * xFilt;
  // xFilt = fopen("multiplyer.bin","wb");
  // fwrite(x_matchedFilt,sizeof(double), buffersize, xFilt);
  // fclose(xFilt);


 
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
  
  /*error message


*** glibc detected *** ./green_rx_signal_processing: free(): corrupted unsorted chunks: 0x0000000000c24ff0 ***
*** glibc detected *** ./green_rx_signal_processing: malloc(): memory corruption: 0x0000000000c3e4b0 ***

  */

  /*
  res_size = filter_phase( x_downsampled, sizeof(x_downsampled)/sizeof(x_downsampled[0]),  phase, trainSeq, nTrainSeq, 0.01,0.01, 0.01, res);

  //std::cout << " Phase Filteres! " << std::endl;

  // detect
  short data_bin[buffersize/8];
  hardDetect(res,data_bin,buffersize/8);

  std::cout << " Detected! " << std::endl;

  std::cout << " " << std::endl;
  std::cout << " Done! " << std::endl;
  */
}
