#include <complex>
#include <iostream>
#include <vector>
#include <queue> 
#include <cmath>     
#include <string>

#include <ctime>
#include <stdlib.h> 

#include <itpp/itbase.h>
#include <itpp/itcomm.h> 
#include <itpp/stat/misc_stat.h>

#include "arrayToCvec.cpp"

using namespace std;
using namespace itpp;

#define DispVal(X) std::cout << #X << " = " << X<< std::endl

#define M_PI 3.14159265358979323846

//extern void receiverSignalProcessing(short buff_short[], int buffersize,short data_bin[], int nDataC);


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
  int index=0, count=0;
  double tmp[crossCorr.length()-dataC.length()+1];
  for(int i=dataC.length()-1;i<crossCorr.length();i++){
    tmp[count]=abs(crossCorr[i]);
   
    //DispVal(tmp[count]);
    //DispVal(count);
    if (tmp[count]> max && count<(crossCorr.length()-dataC.length()+1)/2  ){
      max=tmp[count];
      index=i;
     
    }
     count++;
  }
  
  index=index-dataC.length()+1;
  *delay=index;
 //Computes Estimate of phase at the peak:
  *phiHat=arg(dataC(index));

  // Save data to file
     std::ofstream ofs( "xcorr.dat" , std::ifstream::out );
     ofs.write((char * ) tmp, (crossCorr.length()-dataC.length()+1)*sizeof(double));
     ofs.close();


   //std::cout<<"tsamp="<<index<<"\n";
  //Removes received train from data

  cvec trainRecC(trainCUp.length());
  int i=index;
  for(int count=0;count<trainCUp.length();count++){
    trainRecC.set(count,dataC[i+count]);
    //DispVal(dataC[i]);
    //DispVal(count);
  }
  
  //std::complex< double > meanTrain=itpp::mean(trainRecC);
  //DispVal(meanTrain);
  
  //trainRecC=trainRecC-meanTrain;

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
   //Estimate of the channel:
    cmat invSigmaYY=itpp::inv(SigmaYY);
    cvec theta_est=itpp::operator*(invSigmaYY,SigmaYx);
   
    //std::cout<<"invSigmaYY=\n"<<invSigmaYY<<"\n";
    // std::cout<<"Channel="<<theta_est<<"\n";
    
    //Compute the variance of the estimation:

    trainCUp.ins(trainCUp.length(),complex<double>(0,0));
    
    cvec trainCEst=filter( theta_est, 1, trainCUp);
    //std::cout<<"trainCEst="<<trainCEst<<"\n";
    
    trainCEst.del(0);
    cvec crossEst=itpp::xcorr(trainRecC-trainCEst);
    //std::cout<<"crossEst="<<crossEst<<"\n";
    *sigmaSqrNoise=abs(crossEst(trainRecC.length()-1));

    return theta_est;
 
}

 
void saveQueueComplex(std::queue<cvec> s, int vecSize, char filename[])
{
  
     int sizeQueue=s.size();
     int tmpSize=vecSize*sizeQueue*2;
     double tmp[tmpSize];
     int count=0;
    while(sizeQueue!=0){
      itpp::cvec x= s.front();
      s.pop();
      for(int i=0;i<vecSize;i++){
	//DispVal(count);
	tmp[count]=x[i].real();
	tmp[count+1]=x[i].imag();
	
	  count=count+2;
      }
      sizeQueue--;
    }
    
    //DispVal(tmpSize);
     std::ofstream ofs( filename , std::ifstream::out );
     ofs.write((char * ) tmp, tmpSize*sizeof(double));
     ofs.close();

    
    
    return;
}

void printComplex(std::queue<cvec> s)
{
  int sizeQueue=s.size();
  //DispVal(sizeQueue);
 
    while(sizeQueue!=0){
      cvec x= s.front();
      s.pop();
      std::cout << x << "\n";
      sizeQueue--;
    }
    return;
}
void saveQueue(std::queue<vec> s, int vecSize, char filename[])
{
  
     int sizeQueue=s.size();
     int tmpSize=vecSize*sizeQueue;
     double tmp[tmpSize];
     int count=0;
    while(sizeQueue!=0){
      itpp::vec x= s.front();
      s.pop();
      for(int i=0;i<vecSize;i++){
	//DispVal(count);
	tmp[count]=x[i];
		
	  count=count+1;
      }
      sizeQueue--;
    }
    

     std::ofstream ofs( filename , std::ifstream::out );
     ofs.write((char * ) tmp, tmpSize*sizeof(double));
     ofs.close();

    
    
    return;
}

void print(std::queue<vec> s)
{
  int sizeQueue=s.size();
  //DispVal(sizeQueue);
 
    while(sizeQueue!=0){
      vec x= s.front();
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
    double nuOff=arg(sum)/(2*M_PI*nCarriers);
    //DispVal(nuOff);
    
    complex<double> arg1;
   
    for(int i3=0; i3<nCarriers; i3++){
      arg1=complex<double>(0,-1*2*M_PI*nuOff*i3);
      aux[i3]=aux[i3]*exp(arg1);
    }
    //DispVal(aux);
    data=itpp::fft(aux);
    //DispVal(data);
      //Inserte data in Processing queue:
   
    OFDM_data.push(data);

    //DispVal(i1);
    //DispVal(pt);
   
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



/** Interpolation on the amplitude
 *      - PilotFFT: the amplitude at the different pilots
 *      - pilot_pos[]: The index of the different pilot.
 *      - nPilot: The number of pilot 
 *      - nElem: The number of point in the FFT
 *  EXPL: if one pilot is at 0.25 in normalised freq and nElem=80, pilot_pos=20
 *                           0                                =80           =0
 * 
 */
vec interp(vec pilotFFT, double pilot_pos[], int nPilot, int nElem){
  // Search the highest frequency pilot
  int lPilot=0;
  for (; lPilot<nPilot && pilot_pos[lPilot]< (nElem/2); lPilot++){  //std::cout << pilot_pos[lPilot]<< std::endl;
}
  lPilot--;


  // Create cyclic extension of pilot position
  vec pilotP( pilot_pos, nPilot);
  pilotP.ins(0, pilotP.right(1)-nElem);
  //std::cout << pilotP << std::endl;
  //Create a cyclic extension of pilotFFT
  vec pilotFFTp=pilotFFT; 
  pilotFFTp.ins(0, pilotFFTp.right(1));

  // Evaluate the delta amplitude
  pilotFFT.ins(pilotFFT.length(),pilotFFT.left(1));
  //std::cout << pilotFFT << std::endl;
  //std::cout << pilotFFTp << std::endl;
  vec pilotFFTd=pilotFFT-pilotFFTp;
  //std::cout << pilotFFTd << std::endl;

  // Create the ouput vector
  vec out(nElem);
  // Fill the higher frequencies
  for (int i=0; i<lPilot+1; i++){//+1 because of the periodisation
    int deltaI=pilotP(i+1)-pilotP(i);
    //std::cout <<i<<  " delta " <<deltaI << std::endl;
    double step=pilotFFTd(i)/deltaI;
    //std::cout << step << std::endl;
    for (int ii=0;ii<deltaI; ii++ ){
      out.set(modP((pilotP(i)+ii),(double) nElem), pilotFFTp(i)+ii*step);
      //std::cout << out << std::endl;
    }
  }
  //std::cout << out<<std::endl;
  // Fill the frequencies around DC
  for(int i=pilotP(lPilot+1);i<nElem/2;i++ ){
    out.set(i, pilotFFTp(lPilot+1));
    //std::cout << out<<std::endl;
  }
  for(int i=pilotP(lPilot+2);i>=nElem/2;i-- ){
    out.set(i, pilotFFTp(lPilot+2));
    //std::cout << out<<std::endl;
  }
  // Fill the low freq
  for (int i=lPilot+2;i<nPilot; i++){
    int deltaI=pilotP(i+1)-pilotP(i);
    //std::cout <<i<<  " delta " <<deltaI << std::endl;
    double step=pilotFFTd(i)/deltaI;
    //std::cout << step << std::endl;
    for (int ii=0;ii<deltaI; ii++ ){
      out.set(pilotP(i)+ii, pilotFFTp(i)+ii*step);
      //std::cout << out<<std::endl;
    }  
  }
  
  return out;
}

/** IDEM interp but with periodicity of the phase taking into account.
 *
 */
vec interpPhase(vec pilotFFT, double pilot_pos[], int nPilot, int nElem){
  // Search the highest frequency pilot
  int lPilot=0;
  for (; lPilot<nPilot && pilot_pos[lPilot]< (nElem/2); lPilot++){ }
  lPilot--;


  // Create cyclic extension of pilot position
  vec pilotP( pilot_pos, nPilot);
  pilotP.ins(0, pilotP.right(1)-nElem);
  //std::cout << pilotP << std::endl;
  //Create a cyclic extension of pilotFFT
  vec pilotFFTp=pilotFFT; 
  pilotFFTp.ins(0, pilotFFTp.right(1));

  // Evaluate the delta amplitude
  pilotFFT.ins(pilotFFT.length(),pilotFFT.left(1));
  //std::cout << pilotFFT << std::endl;
  //std::cout << pilotFFTp << std::endl;
  vec pilotFFTd=pilotFFT-pilotFFTp;
  //std::cout << pilotFFTd << std::endl;

  // Create the ouput vector
  vec out(nElem);
  // Fill the higher frequencies
  for (int i=0; i<lPilot+1; i++){//+1 because of the periodisation
    int deltaI=pilotP(i+1)-pilotP(i);// Difference in index
    //std::cout <<i<<  " delta " <<deltaI << std::endl;
 
    double step=(modP(pilotFFTd(i)+M_PI,2*M_PI)-M_PI)/deltaI;// Difference in phase
  
    //std::cout << step << std::endl;
    for (int ii=0;ii<deltaI; ii++ ){
      out.set(modP((pilotP(i)+ii),(double) nElem), modP(pilotFFTp(i)+ii*step+M_PI,2*M_PI)-M_PI);
      //std::cout << out << std::endl;
    }
  }
  //std::cout << out<<std::endl;
  // Fill the frequencies around DC
  for(int i=pilotP(lPilot+1);i<nElem/2;i++ ){
    out.set(i, modP(pilotFFTp(lPilot+1)+M_PI,2*M_PI)-M_PI);
    //std::cout << out<<std::endl;
  }
  for(int i=pilotP(lPilot+2);i>=nElem/2;i-- ){
    out.set(i, modP(pilotFFTp(lPilot+2)+M_PI,2*M_PI)-M_PI);
    //std::cout << out<<std::endl;
  }
  // Fill the low freq
  for (int i=lPilot+2;i<nPilot; i++){
    int deltaI=pilotP(i+1)-pilotP(i);
    //std::cout <<i<<  " delta " <<deltaI << std::endl;
    double step=(modP(pilotFFTd(i)+M_PI,2*M_PI)-M_PI)/deltaI;// Difference in phase
    //std::cout << step << std::endl;
    for (int ii=0;ii<deltaI; ii++ ){
      out.set(pilotP(i)+ii,modP(pilotFFTp(i)+ii*step+M_PI,2*M_PI)-M_PI );
      //std::cout << out<<std::endl;
    }  
  }
  return out;
}



///////////////////////////////////////MAIN FUNCTION////////////////////////////////////////////////////////////

void receiverSignalProcessing(short buff_short[], int buffersize,short data_bin[], int nDataB){

  std::cout<<"Running Receiver!\n";

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
  std::ifstream ifs( "dataTrain.dat" , std::ifstream::in );
  if(ifs.is_open()){
    ifs.read((char * )trainSeq,2*nTrainSeq*sizeof(double));
    ifs.close();
  }else{
    std::cerr<<"Error loading data!\n";
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


    itpp::cvec thetaEst = synchCatchChannel(dataCvec, trainC, FIR_post, FIR_pre , &phiHat,  &AHat, &delay, &sigmaSqrNoise);

    std::cout<<"Channel="<<thetaEst<<"\n";
    std::cout <<"tsamp="<<delay<<"\n";
    std::cout<<"phase_hat="<<phiHat<<"\n";
    std::cout<<"A_hat="<<AHat<<"\n";
    std::cout<<"sigma_square_noise="<<sigmaSqrNoise<<"\n\n";
    std::cout<<" Synchronization and channel estimation done - checked!\n\n";
    

 //CP removal of OFDM symbols and FFT to get OFDM data:
    int tIni=delay+Q*nTrainSeq; //Beginnig of OFDM data
    //DispVal(tIni);
    int PostFix=1, PreFix=18;
    int nCarriers=128;
    int nSymbolsOFDM=200;
    
    std::queue<cvec> dataOFDM;

    CPremove(dataCvec, tIni, nCarriers, nSymbolsOFDM, PreFix, PostFix, dataOFDM);
    std::cout<<" Cyclic Prefix removed and OFDM data obtained - checked!\n\n";

    saveQueueComplex(dataOFDM, nCarriers, (char*)"ofdm_cp.dat");

    //printComplex(dataOFDM);




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
    std::cerr<<"Error loading data!\n";
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
    std::cerr<<"Error loading data!\n";
    exit(1);
  }

  

   //Read Pilots from file: 
  int nPilotData=nSymbolsOFDM*nUsedPilot;
  complex<double> * dataPilot;
  dataPilot= new complex<double>[nPilotData];
  std::ifstream ifs3( "dataPilot.dat" , std::ifstream::in );
  if(ifs3.is_open()){
    ifs3.read((char * )dataPilot, 2*nPilotData*sizeof(double));
    ifs3.close();
  }else{
    std::cerr<<"Error loading data!\n";
    exit(1);
  }

  // for(int i=0;i<11;i++){
  //   DispVal(dataPilot[i]);
  //   DispVal(i);
  // }
  
 

  //Take OFDM data and Pilots:
  //Create 3 queues: OFDM data, channel gain and channel phase:
  std::queue<itpp::cvec> constOFDM;
  std::queue<itpp::vec> PilotGain, PilotPhase;

  itpp::cvec dataAux(nUsedCarrier);
  itpp::vec gainAux(nUsedPilot);
  itpp::vec phaseAux(nUsedPilot);
  int countPilot=0;//Assuming that pilots are inserted by order
  for(int i1=0;i1<nSymbolsOFDM;i1++){
    itpp::cvec aux=dataOFDM.front();
    dataOFDM.pop();
    int m=0,n=0;
    double gain, phase;
    for(int i2=0;i2<nCarriers;i2++){
      if (pilotPattern[m]-1==i2){	
	gain=std::abs(aux[i2]/dataPilot[countPilot]);
	phase=std::arg(aux[i2]/dataPilot[countPilot]);
	gainAux.set(m, gain);
	phaseAux.set(m, phase);
	m++;
	countPilot++;
      }
      if (dataPattern[n]-1==i2){
	dataAux.set(n, aux[i2]);   
	n++;
      }

     
     
    }
    
    dataOFDM.push(aux); //not to lose dataOFDM -> put again in queue
    constOFDM.push(dataAux);
    PilotGain.push(gainAux);
    PilotPhase.push(phaseAux);

    
  }
  
  
  
  //print(PilotGain);
  saveQueue(PilotGain, nUsedPilot, (char*) "rx_pilot_gain.dat");
  saveQueue(PilotPhase, nUsedPilot, (char*) "rx_pilot_phase.dat");
  saveQueueComplex(constOFDM, nUsedCarrier , (char*) "cons_ofdm.dmakeat");
  std::cout<<" Data and Pilots obtained - checked!\n\n";



   //Filtering pilots with Kalman filter:
   
   //Initialization of the kalman with channel coarse estimation:
   
   cvec thetaEstAux(nCarriers);

   for(int i=0,count=0;i<nCarriers;i++){
     if(i<=FIR_post){
       thetaEstAux.set(i,thetaEst[i+FIR_pre]);
     }
     if(i>FIR_post && i<nCarriers-FIR_pre){
       thetaEstAux.set(i,complex<double>(0,0));
     }
     if(i>=nCarriers-FIR_pre){
       thetaEstAux.set(i,thetaEst[count]);
       count++;
     }

   }
   
   cvec fftTheta=itpp::fft(thetaEstAux);
   vec x0Gain(nUsedPilot);
   std::vector<itpp::mat> x0Phase(nUsedPilot);
   
  
   itpp::mat aux(2,1);

   int m=0;
   double gain, phase;
   for(int i=0;i<nCarriers;i++){
    if (pilotPattern[m]-1==i){	
      gain=std::abs(fftTheta[i]);
      phase=std::arg(fftTheta[i]);
	x0Gain.set(m, gain);
	aux.set(0,0,0.0);
	aux.set(1,0,0.0);
	x0Phase[m]=aux;
	m++;
    }
   }


   //Kalman filter on the Gain:
   std::queue<itpp::vec> filtPilotGain=PilotGain;//kalmanGain (PilotGain,  nUsedPilot,  1, 1, 1, 0.001, 1,  x0Gain, 10);

   //print(filtPilotGain);
   saveQueue(filtPilotGain, nUsedPilot, (char*)"filt_pilot_gain.dat");


   //Kalman filter in the Phase:
   std::queue<itpp::vec> filtPilotPhase=PilotPhase;//kalmanPhase(PilotPhase, nUsedPilot, ("1 6.28318 ; 0 1"), ("1 0 ; 0 0"),("1 0"),("0.1 0; 0 0"), ("0.01"), x0Phase,("300000 0 ; 0 90000"));

   saveQueue(filtPilotPhase, nUsedPilot, (char*)"filt_pilot_phase.dat");
   
   std::cout<<" Pilot Gain and Phase filtered - checked!\n\n";

 

   //Channel Interpolation:
   
   itpp::vec gainInterp(nUsedCarrier), phaseInterp(nUsedCarrier);

   double pilotPat[nUsedPilot];

   for(int i=0;i<nUsedPilot;i++)pilotPat[i]=pilotPattern[i]-1;


   for(int i1=0;i1<nSymbolsOFDM;i1++){

     gainAux=filtPilotGain.front();
     filtPilotGain.pop();
     phaseAux=filtPilotPhase.front();
     filtPilotPhase.pop();
    
     gainInterp=interp( gainAux, pilotPat,nUsedPilot, nCarriers);
     phaseInterp=interpPhase(phaseAux, pilotPat, nUsedPilot, nCarriers);
     

     filtPilotGain.push(gainInterp);
     filtPilotPhase.push(phaseInterp);

   }

   saveQueue(filtPilotGain, nCarriers, (char*)"int_pilot_gain.dat");
   saveQueue(filtPilotPhase, nCarriers, (char*)"int_pilot_phase.dat");

   std::cout<<" Pilot Gain and Phase interpolation done - checked!\n\n";
   
  

   //Channel Correction
   complex<double> correct, phaseCorrect;

   for(int i1=0;i1<nSymbolsOFDM;i1++){
     gainInterp=filtPilotGain.front();
     filtPilotGain.pop();
     phaseInterp=filtPilotPhase.front();
     filtPilotPhase.pop();

     dataAux=constOFDM.front();
     constOFDM.pop();

     //Correction of channel impulse responce:
     int n=0;
     for(int i2=0; i2<nCarriers; i2++){
       
       if (dataPattern[n]-1==i2){
	
	 phaseCorrect=std::exp(complex<double>(0,phaseInterp[i2]));
	 correct=dataAux[n]/(gainInterp[i2]*phaseCorrect);
	 

	 dataAux.set(n,correct);
	  n++;
       }
     }

     constOFDM.push(dataAux);
   }

   saveQueueComplex(constOFDM, nUsedCarrier, (char*)"qpsk_data.dat");
   
  
    std::cout<<" Channel Impulse response corrected - checked!\n\n";
 

   //Detection
   
   
   complex<double> DataFilt[nUsedCarrier*nSymbolsOFDM]; 
   int count=0;
    for(int i1=0;i1<nSymbolsOFDM;i1++){
   
     dataAux=constOFDM.front();
     constOFDM.pop();

     //Correction of channel impulse responce:
     for(int i2=0; i2<nUsedCarrier; i2++){
       DataFilt[count]=dataAux[i2];
       count++;
     }

     constOFDM.push(dataAux);

   }

    cvec toDemo=arrayToCvec(DataFilt, nDataB);

    QAM qam(16);
    bvec bits=qam.demodulate_bits(toDemo);
    //hardDetect(DataFilt, data_bin, nDataB/2);

   
   //for(int i=0; i<nDataB; i++)DispVal(data_bin[i]);
    for(int i=0; i<nDataB;i++){
      data_bin[i]=(short)bits[i];
    }

   // hardDetect(DataFilt, data_bin, nDataB);

   //for(int i=0; i<nDataB; i++)DispVal(data_bin[i]);

   // Save data to file
     std::ofstream ofs1( "dataBin.dat" , std::ifstream::out );
     ofs1.write((char * )data_bin, nDataB*sizeof(short));
     ofs1.close();


   std::cout << " Detected - checked! " << std::endl;

   std::cout << " " << std::endl;
   std::cout << " Processing Done! " << std::endl;

   return;

}

