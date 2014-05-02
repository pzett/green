#include <complex>
#include <iostream>
#include <vector>
#include <queue>      

#include <itpp/itbase.h>
#include <itpp/itcomm.h> 
#include <itpp/stat/misc_stat.h>

#include "arrayToCvec.cpp"

using namespace std;
using namespace itpp;

#define DispVal(X) std::cout << #X << " = " << X<< std::endl

#define M_PI 3.14159265358979323846

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


/** Return the channel FIR estimation based on trainSeq and fill phiHat, Ahat, delay, sigmaSqrNoise
 *
 * @pre:
 *    - dataC: cvec of the received data
 *    - trainC: pointer to array of the training sequence
 *             !!!  length assume to be < than dataC.length()!!!
 *    - Post and Pre: FIR filter channel number of past and future elements
 *    - phiHat: pointer to a double with phase estimation
 *    - delay: pointer to a int with synchornization point
 *    - sigmaSqrNoise: pointer to a double with FIR estimator variance 
 *
 * @post:
 *    - Estimators are now filled!
 *    - returned the channel FIR estimation
 */
cvec synchCatchChannel(cvec dataC, cvec trainCUp, int Post, int Pre , double *phiHat, double *AHat, int *delay, double *sigmaSqrNoise ){

  if(dataC.length()<trainCUp.length()){
    cerr << "The training sequence is too big compare to datas!\n";
    exit(1);
  }
  if(Pre<0||Post<0){
    cerr << "Not possible to compute filter!\n";
    exit(1);
  }

  //std::cout<<"Data="<<dataC<<"\n";
  //std::cout<<"Train="<<trainCUp<<"\n";
  
  //Computes crosscorrelation:

  cvec crossCorr=itpp::xcorr(dataC, trainCUp);

  //std::cout<<"Cross="<<crossCorr<<"\n";

  //Search for the maximum
  double max=0.0;
  int index=0;
  for(int i=dataC.length()-1;i<crossCorr.length();i++){
    double tmp=abs(crossCorr[i]);
    //DispVal(tmp);
    if (tmp> max){
      max=tmp;
      index=i;
    }
  }
  index=index-dataC.length()+1;
  //DispVal(index);
  *delay=index;
 //Computes Estimate of phase at the peak:
  *phiHat=arg(dataC(index));

   //std::cout<<"tsamp="<<index<<"\n";
  //Removes received train train from data

  cvec trainRecC(trainCUp.length());
  int i=index;
  for(int count=0;count<trainCUp.length();count++){
    trainRecC.set(count,dataC[i+count]);
    //DispVal(dataC[i]);
    //DispVal(count);
  }
  
  std::complex< double > meanTrain=itpp::mean(trainRecC);
  //DispVal(meanTrain);
  
  trainRecC=trainRecC-meanTrain;

  //std::cout<<"TrainRec"<<trainRecC<<"\n";

  //Channel estimator using training sequence:

  //autoCorr of known training sequence:
  cvec autoCorr=itpp::xcorr(trainCUp);
 
  //DispVal(autoCorr.length());
  //DispVal(autoCorr(trainCUp.length()-1));
  //std::cout<<"Auto="<<autoCorr<<"\n";
  
  *AHat=abs(crossCorr(index+dataC.length()-1))/abs(autoCorr(trainCUp.length()-1));
 

  //SigmaYY:
  cvec aux(Pre+Post+1);
  int start=trainCUp.length()-1;
  for(int count=0; count<Pre+Post+1; count++){
    if(count<=2*trainCUp.length()-1){
      aux.set(count,autoCorr[start+count]);
    }else{
      aux.set(count,complex<double>(0.0,0.0));
     }
  }
 
  cmat SigmaYY=toeplitz(aux);
  //std::cout<<"SigmaYY=\n"<<SigmaYY<<"\n";


  cvec aux2 =itpp::xcorr(trainRecC,trainCUp);
  //SigmaYx:
  int ini=trainCUp.length()-Pre-1;
  cvec SigmaYx(Pre+Post+1);
  for(int count=0; count<SigmaYx.length(); count++){
    SigmaYx.set(count, aux2[ini+count]);
    //std::cout<<"xcorr=\n"<<crossCorr[i]<<"\n";
  }
   
  //std::cout<<"SigmaYx=\n"<<SigmaYx<<"\n";

    //Estimate of the channel:
    cmat invSigmaYY=itpp::inv(SigmaYY);
    cvec theta_est=itpp::operator*(invSigmaYY,SigmaYx);
   
    //std::cout<<"invSigmaYY=\n"<<invSigmaYY<<"\n";
    // std::cout<<"Channel="<<theta_est<<"\n";
    
    //Compute the variance of the estimation:
    
    cvec trainCEst=filter( theta_est, 1, trainCUp);
    //std::cout<<"trainCEst="<<trainCEst<<"\n";

    cvec crossEst=itpp::xcorr(trainRecC-trainCEst);
    //std::cout<<"crossEst="<<crossEst<<"\n";
    *sigmaSqrNoise=abs(crossEst(trainRecC.length()-1));

    return theta_est;
}



void print(std::queue<cvec> s)
{
  int sizeQueue=s.size();
  DispVal(sizeQueue);
 
    while(sizeQueue!=0){
      cvec x= s.front();
      s.pop();
      std::cout << x << "\n";
      sizeQueue--;
    }
    return;
}


/** Removes the cyclic prefix, sufix. Corrects also the frequency shift and puts 
 *    the OFMD data in a queue   
 *
 * @pre:
 *    - data: vector with received data
 *    - tIni: first point of the the ODFM symbols
 *    - nCarries: number of carriers used in OFDM system
 *    - nOFDM_symbols: number of OFDM symbols
 *    - Pre and Post: size of used cyclic prefix and sufix
 *    - OFDM_data: queue with OFDM data processed
 *
 * @post:
 *    - CP is now removed. Data in queue!
 */


void CPremove(cvec dataC, int tIni, int nCarriers, int nOFDM_symbols, int Pre, int Post, std::queue<cvec> &OFDM_data){


  int pt=tIni;
  cvec aux(nCarriers), Pre1(Pre), Pre2(Pre);
  cvec Pre1conj(Pre), mult(Pre);
  cvec data(nCarriers);
  //cycle to Remove CP:
  
  for(int i1=0;i1<nOFDM_symbols;i1++){

    //cycle to get Prefix, OFDM sysmbol and Post fix:
    for(int i2=0; i2<Pre; i2++){
      Pre1.set(i2,dataC[pt+i2]);
    }
    //std::cout<<"Pre1="<<Pre1<<"\n";

    pt=pt+Pre;

    for(int i2=0;i2<nCarriers;i2++){
      aux.set(i2,dataC[i2+pt]);
    }
    //std::cout<<"aux="<<aux<<"\n";

    pt=pt+nCarriers-Pre;
    
    
    for(int i2=0; i2<Pre ;i2++){
      Pre2.set(i2,dataC[i2+pt]);
    }
    
    //std::cout<<"Pre2="<<Pre2<<"\n";

    pt=pt+Post+Pre;

    //Process data and store it in the queue:
   
    Pre1conj =itpp::conj(Pre1);
       
    mult= itpp::elem_mult(Pre1conj,Pre2); 	
  
    complex<double> sum=itpp::sum(mult);
    double nuOff=arg(sum)/nCarriers;
    DispVal(nuOff);
    
    complex<double> arg1;
   
    for(int i3=0; i3<nCarriers; i3++){
      arg1=complex<double>(0,-1*nuOff);
      aux[i3]=aux[i3]*exp(arg1);
    }
    //DispVal(aux);
    data=itpp::fft(aux, nCarriers);
    //DispVal(data);
      //Inserte data in Processing queue:
   
    OFDM_data.push(data);

   DispVal(i1);
   DispVal(pt);
   
  }
  
  return;
}

/** Computes one iteration of a scalar kalman filter:
 *
 * @pre:
 *    - xhat_filt_p: Previous filtered data -> x(n|n)
 *    - y: data measured -> y(n+1)
 *    - F, G and H: state model definiton
 *    - R1 and R2: User parameters of the Kalman -> covariance of the noise
 *    - Q: error covariance matrix -> Q(n)
 *    - xhat_pred: data predicted -> x(n+1|n)
 *    - yhat: predicted measured data -> y(n+1|n)
 *    - xhat_filt: data filtered -> x(n+1|n+1)
 *    - Q_n: computed error covariance -> Q(n+1)
 *
 * @post:
 *    - Iteration done!
 */

void kalmanGainIteration (double xhat_filt_p, double y, double Q, double R1, double R2, double F,double H, double G, double *xhat_pred, double *yhat, double *xhat_filt, double *Q_n ){
  
  double P, L;

  //Time update of the filter:
  *xhat_pred=(F*xhat_filt_p);
  *yhat=H*(*xhat_pred);
  P=F*Q*F+G*R1*G;

  //Measurement update:
  L=P*H/(H*P*H+R2);
  *xhat_filt=(*xhat_pred)+L*(y-(*yhat));
  *Q_n=P-L*H*P;
  
  return;
}


/** Return a queue with the Pilot Gain filtered based on a scalar Kalman filter
 *
 * @pre:
 *    - Pilot: queue of vec's with the received pilots
 *    - nPilot: number of pilots per OFDM symbol
 *    - F, G and H: state model definiton
 *    - R1 and R2: User parameters of the Kalman -> covariance of the noise
 *    - x0: vec with the initialization of the kalman
 *    - Q0: initialization of the error covariance matrix 
 *
 * @post:
 *    - Pilots are now filter!
 */
std::queue<itpp::vec> kalmanGain (std::queue<itpp::vec> Pilot, int nPilot, double F, double G, double H, double R1, double R2, vec x0, double Q0){
  
  double xhat_pred,  yhat,  xhat_filt, Q_n;

  std::queue<itpp::vec> filtPilot; 
  
  int PilotSizeIni=Pilot.size();
  itpp::vec xhat_filt_p(nPilot), Q(nPilot);
  //kalman first iterartion:
  
  itpp::vec y=Pilot.front(); //Take measurements from data
  Pilot.pop();
   for(int i=0; i<nPilot;i++){
      //one iteration of the kalman
     kalmanGainIteration(x0(i), y(i), Q0,R1,R2,F,H,G, &xhat_pred, &yhat, &xhat_filt, &Q_n );
      //update xhatfilt and Q
      xhat_filt_p(i)=xhat_filt;
      Q(i)=Q_n;
    }
    //store new filtered pilot vector into queue:
    filtPilot.push(xhat_filt_p);
  
  //Kalman loop:
  for(int i1=1; i1<PilotSizeIni;i1++){

    itpp::vec y=Pilot.front(); //Take measurements from data
    Pilot.pop();
    for(int i2=0; i2<nPilot;i2++){
      //one iteration of the kalman
      kalmanGainIteration(xhat_filt_p(i2), y(i2), Q(i2) ,R1,R2,F,H,G, &xhat_pred, &yhat, &xhat_filt, &Q_n );
      //update xhatfilt and Q
      xhat_filt_p(i2)=xhat_filt;
      Q(i2)=Q_n;
    }
    //store new filtered pilot vector into queue:
    filtPilot.push(xhat_filt_p);
    
  }

  //DispVal(Pilot.size());
  //DispVal(filtPilot.size());
  if(PilotSizeIni!=(int)filtPilot.size()){
    std::cout<<"Error in filtering!\n";
    exit(1);
  }

  return filtPilot;

}

double modP(double a, double b){
  if (a<0){
  double rem=std::ceil(a/b);
  rem=a-rem*b;
  //std::cout<<"rem "<<rem<<std::endl;
  rem=b+rem;
  return rem;
  }else{
  double rem=std::floor(a/b);
  rem=a-rem*b;
 return rem;
  }
}

/** Computes one iteration of a kalman filter:
 *
 * @pre:
 *    - x0: Previous filtered data -> x(n|n)
 *    - y_1: data measured -> y(n+1)
 *    - F, G and H: state model definiton
 *    - R1 and R2: User parameters of the Kalman -> covariance of the noise
 *    - Q0: error covariance matrix -> Q(n)
 *    - xhat_pred0: data predicted -> x(n+1|n)
 *    - y_pred0: predicted measured data -> y(n+1|n)
 *    - x_hat_10: data filtered -> x(n+1|n+1)
 *    - Q_n0: computed error covariance -> Q(n+1)
 *    - P_10: computed a posteriori error covariance
 *
 * @post:
 *    - Iteration done!
 */

void kalmanPhaseIteration (double y_1, itpp::mat R1,itpp::mat R2,itpp::mat x0, itpp::mat Q0, itpp::mat F, itpp::mat G, itpp::mat H,itpp::mat *x_predO, itpp::mat *y_predO, itpp::mat *P_1O, itpp::mat *x_hat_1O, itpp::mat *Q_1O ){

 
  //Time Updata:
  itpp::mat x_pred= F*x0;
  itpp::mat y_pred= H*x_pred;
  itpp::mat P_1=F*Q0*transpose(F)+G*R1*transpose(G);


  //measurement update
  itpp::mat L_1=P_1*transpose(H)*inv(H*P_1*transpose(H)+R2);
  itpp::mat x_hat_1=x_pred+L_1*(modP((y_1-y_pred.get(0,0))+M_PI,2*M_PI)-M_PI);
  itpp::mat Q_1=P_1-P_1*transpose(H)*inv(H*P_1*transpose(H)+R2)*H*P_1;

   // Save the output
  x_predO->set_submatrix(0,0,x_pred);
  y_predO->set_submatrix(0,0,y_pred);
  P_1O->set_submatrix(0,0,P_1);
  x_hat_1O->set_submatrix(0,0,x_hat_1);
  Q_1O->set_submatrix(0,0,Q_1);

  return;
}

/** Return a queue with the Pilot Phase filtered based on a scalar Kalman filter
 *
 * @pre:
 *    - Pilot: queue of vec's with the received pilots
 *    - nPilot: number of pilots per OFDM symbol
 *    - F, G and H: state model definiton
 *    - R1 and R2: User parameters of the Kalman -> covariance of the noise
 *    - x0: vector of matrix with the initialization of the kalman
 *    - Q0: initialization of the error covariance matrix 
 *
 * @post:
 *    - Pilots are now filter!
 */
std::queue<itpp::vec> kalmanPhase (std::queue<itpp::vec> Pilot, int nPilot, itpp::mat F, itpp::mat G, itpp::mat H, itpp::mat R1, itpp::mat R2, std::vector<itpp::mat> x0 , itpp::mat Q0){
  
 
  itpp::mat x_pred(2,1);
  itpp::mat y_pred(1,1);
  itpp::mat P_1(2,2);
  itpp::mat x_hat_1(2,1);
  itpp::mat Q_1(2,2); 

  std::queue<itpp::vec> filtPilot; 
  
  int PilotSizeIni=Pilot.size();
  std::vector<itpp::mat> xhat_filt_p(nPilot), Q(nPilot);
  //kalman first iterartion:
  
  itpp::vec y=Pilot.front(); //Take measurements from data
  Pilot.pop();
   for(int i=0; i<nPilot;i++){
      //one iteration of the kalman
      kalmanPhaseIteration ( y(i), R1, R2,  x0[i], Q0, F, G, H, &x_pred, &y_pred, &P_1, &x_hat_1, &Q_1 );
      //update xhatfilt and Q
      xhat_filt_p[i]=x_hat_1;
      Q[i]=Q_1;
    }
    //store new filtered pilot vector into queue:
   itpp::vec aux(nPilot);
   for(int i=0;i<nPilot;i++){
     aux.set(i,xhat_filt_p[i].get(0,0));
   }
   filtPilot.push(aux);
  
  //Kalman loop:
  for(int i1=1; i1<PilotSizeIni;i1++){
    itpp::vec y=Pilot.front(); //Take measurements from data
    Pilot.pop();
    for(int i2=0; i2<nPilot;i2++){
      //one iteration of the kalman
      kalmanPhaseIteration (y(i2),R1,R2, xhat_filt_p[i2], Q[i2], F, G, H, &x_pred, &y_pred, &P_1, &x_hat_1, &Q_1 );
      //update xhatfilt and Q
      xhat_filt_p[i2]=x_hat_1;
      Q[i2]=Q_1;
    }
    //store new filtered pilot vector into queue:
   itpp::vec aux(nPilot);
   for(int i=0;i<nPilot;i++){
     aux.set(i,xhat_filt_p[i].get(0,0));
   }
   filtPilot.push(aux);

  }

  //DispVal(Pilot.size());
  //DispVal(filtPilot.size());
  if(PilotSizeIni!=(int)filtPilot.size()){
    std::cout<<"Error in filtering!\n";
    exit(1);
  }

  return filtPilot;

}

template<class T>
void hold(T data[], T out[], int Q, int nElem){
  int c=0;
  for (int i=0; i<nElem;i++){
    for (int d=0;d<Q;d++){
    
      out[c+d]=data[i];
      
    }
    c=c+Q;
  }
}

///////////////////////////////////////MAIN FUNCTION////////////////////////////////////////////////////////////

void receiverSignalProcessing(short buff_short[], int buffersize,short data_bin[], int nDataB){

  //Conversion to double -> More precision in processing
  double buff_double[buffersize];
  for(int k=0;k<buffersize;k++){
    buff_double[k] =(double) buff_short[k];
  }
  cvec buff_cvec = arrayToCvec(buff_double,buffersize);

  // Loading the training sequence
  int nTrainSeq=12;
  complex<double> * trainSeq;
  trainSeq= new complex<double>[nTrainSeq];
  std::ifstream ifs( "train_norm.dat" , std::ifstream::in );
  if(ifs.is_open()){
    ifs.read((char * )trainSeq,2*nTrainSeq*sizeof(double));
    ifs.close();
  }else{
    std::cerr<<"Error downloading train!\n";
    exit(1);
  }
  
  //Upsampling training sequence:
  int Q=4;
  std::complex<double> trainSeqUp[Q*nTrainSeq];
  hold(trainSeq, trainSeqUp, Q, nTrainSeq);


  // synchronization and channel coarse estimation:
  cvec trainC=arrayToCvec(trainSeqUp, Q*nTrainSeq); 
  cvec dataCvec=arrayToCvec(buff_double, buffersize);
  
  int FIR_pre=1, FIR_post=4;
  double phiHat, AHat, sigmaSqrNoise;
  int delay;

    itpp::cvec thetaEst = synchCatchChannel(dataCvec, trainC, FIR_pre, FIR_post , &phiHat,  &AHat, &delay, &sigmaSqrNoise);

    std::cout<<"Channel="<<thetaEst<<"\n";
    std::cout <<" tsamp="<<delay<<"\n";
    std::cout<<" phase_hat="<<phiHat<<"\n";
    std::cout<<" A_hat="<<AHat<<"\n";
    std::cout<<" sigma_square_noise="<<sigmaSqrNoise<<"\n";
    std::cout<<"Synchronization and channel estimation done!\n\n";

  

 //CP removal of OFDM symbols and FFT to get OFDM data:
    int tIni=delay+Q*nTrainSeq; //Beginnig of OFDM data
    int PostFix=1, PreFix=18;
    int nCarriers=128;
    int nSymbolsOFDM=235;
    
    std::queue<cvec> dataOFDM;

    CPremove(dataCvec, tIni, nCarriers, nSymbolsOFDM, PreFix, PostFix, dataOFDM);
    std::cout<<" Cyclic Prefix removed and OFDM data obtained!\n\n";

  // Save data to file
    //  int sizeQueue=dataOFDM.size();
    //  complex<double> result[sizeQueue*nCarriers];
    //  int count=0;
    // while(sizeQueue!=0){
    //   itpp::cvec x= dataOFDM.front();
    //   dataOFDM.pop();
    //   for(int i=0;i<nCarriers;i++){
    // 	result[count]=x[i];
    //   }
    //   sizeQueue--;
    // }

    //  // Write data to file
    //  std::ofstream ofs( "OFDM_data.dat" , std::ifstream::out );
    //  ofs.write((char * )result, 2*sizeQueue*nCarriers*sizeof(double));
    //  ofs.close();



 //Pick data corresponding to the different pattern
     
     //Read Pilot Patern from file: 
  int nUsedPilot=10;
  double * pilotPattern;
  pilotPattern= new double[nUsedPilot];
  std::ifstream ifs1( "pilotPattern.dat" , std::ifstream::in );
  if(ifs1.is_open()){
    ifs1.read((char * )pilotPattern, nUsedPilot*sizeof(double));
    ifs1.close();
  }else{
    std::cerr<<"Error downloading train!\n";
    exit(1);
  }

     //Read data Patern from file: 
  int nUsedCarrier=89;
  double * dataPattern;
  dataPattern= new double[nUsedCarrier];
  std::ifstream ifs2( "dataPattern.dat" , std::ifstream::in );
  if(ifs2.is_open()){
    ifs2.read((char * )dataPattern, nUsedCarrier*sizeof(double));
    ifs2.close();
  }else{
    std::cerr<<"Error downloading train!\n";
    exit(1);
  }

  //Take OFDM data:

  for(int i=0;i<nSymbolsOFDM;i++){
    
    
    
  }
  

  
 //  // for(int k=0;k<=(10);k++){
 //    // std::cout << x[k] << std::endl;
 //   // }

 //  // Debugging Outputs 
 //  // std::cout << "length buff_short  : " << sizeof(buff_short)/sizeof(buff_short[0]) << std::endl;
 //  // std::cout << "length buff_double : " << sizeof(buff_double)/sizeof(buff_double[0]) << std::endl;
 //  // std::cout << "length x           : " << sizeof(x)/sizeof(x[0]) << std::endl;
 //  // std::cout << "length x_complD    : " << sizeof(x_complD)/sizeof(x_complD[0]) << std::endl;
 //  // std::cout << "length complExp    : " << sizeof(complExp)/sizeof(complExp[0]) << std::endl;
 //  // std::cout << "counter: " << counter << std::endl;
 //  // std::cout << buff_cvec << std::endl;
 //  // ----------------  
  
 //  std::cout << " Frequency Offset Removed! " << std::endl;



 //  /////////////////////////// matched filtering//////////////////////
 //  double x_matchedFilt [buffersize];

 //  matchedFilter(x, buffersize, x_matchedFilt, buffersize);

  
 //  std::cout << " Matched Filtered! " << std::endl;

 //  // synchronization
 //  int nTrainSeq = 100;
 //  std::complex<double> trainSeq[nTrainSeq];
 //  double phase=0.0;
  


  

 //  // for(int i=0; i<nTrainSeq; i++){
 //  //   DispVal(trainSeq[i]);
 //  // }


 //  // for(int k=0;k<100;k++){
 //  // std::cout << x_matchedFilt[k] << std::endl;
 //  // }

 //   std::complex<double> x_matchedFiltC[buffersize/2];
 //   for(int i=0,count=0;i<buffersize;i=i+2){
 //     x_matchedFiltC[count]=std::complex<double>(x_matchedFilt[i],x_matchedFilt[i+1]);
 //     count++;
 //   }

 //    // Save data to file
 //     std::ofstream ofs1( "x_matched.dat" , std::ifstream::out );
 //     ofs1.write((char * ) x_matchedFiltC, buffersize*sizeof(double));
 //     ofs1.close();

   
 //   int Q=4;

 //   std::complex<double> trainSeqUp[Q*nTrainSeq];
   
 //   hold_zeros(trainSeq, trainSeqUp, Q, nTrainSeq);

 //  int delay = synch(x_matchedFiltC,buffersize/2,trainSeqUp, Q*nTrainSeq, &phase);
  
  
 //  DispVal(phase);
 //  DispVal(delay);

 //  std::cout << " Synchronized! " << std::endl;

 //  int nDataC=nDataB/2;
 //  int nDataTrain=nDataC+nTrainSeq;
 //  DispVal(nDataTrain);
 //  //DispVal(nDataTrain);
 //  // downsampling
 //  std::complex<double> x_downsampled[nDataTrain];
 //  for(int iter=0;iter<nDataTrain;iter++){
 //    x_downsampled[iter]=x_matchedFiltC[delay+iter*4];
 //  }

 //  // Save data to file
 //     std::ofstream ofs2( "x_downsamp.dat" , std::ifstream::out );
 //     ofs2.write((char * ) x_downsampled, 2*(nDataTrain)*sizeof(double));
 //     ofs2.close();
  
  

 //  std::cout << " Downsampled! " << std::endl;

 // // for(int k=0;k<buffersize/8;k++){
 // //  std::cout << x_downsampled[k] << std::endl;
 // //  }
 // // std::cout << "length x_downsampled    : " << sizeof(x_downsampled)/sizeof(x_downsampled[0]) << std::endl;


 //  // filter the phase offset
 //  complex<double> * DataFilt;
  
 //  DataFilt= new complex<double>[nDataC];  
  
  
 //  int nDataCConf = filter_phase( x_downsampled, nDataTrain, phase, trainSeq, nTrainSeq, 0.01, 0.01, 0.01, DataFilt);

 //  // for(int k=0;k<nDataC;k++){
 //  //     std::cout << DataFilt[k] << std::endl;
 //  // }

 //   // Save data to file
 //     std::ofstream ofs3( "x_filt.dat" , std::ifstream::out );
 //     ofs3.write((char * ) DataFilt, 2*nDataC*sizeof(double));
 //     ofs3.close();


 //  if(nDataCConf!=nDataC){
 //    std::cerr<<"Error in filtering the Phase!\n";
 //    exit(1);
 //  }
    


 //  std::cout << " Phase Filtered! " << std::endl;

 //  // detect
  
 //  // Error to be solved
 //  // for(int i=0;i<nDataC;i++){
 //  //   DataFilt[i]=conj(DataFilt[i]);
 //  // }

 //  hardDetect(DataFilt, data_bin, nDataC);


 //  std::cout << " Detected! " << std::endl;

 //  std::cout << " " << std::endl;
 //  std::cout << " Done! " << std::endl;

}

