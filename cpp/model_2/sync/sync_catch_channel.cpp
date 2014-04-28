#include <iostream>
#include <complex>
#include <vector>

#include <itpp/itbase.h>
#include <itpp/itcomm.h> 
#include <itpp/stat/misc_stat.h>


using namespace std;
using namespace itpp;
#define DispVal(X) std::cout << #X << " = " << X<< std::endl


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

/*
int main(){
  
  int nElem=12;
  double data[]={1,1,1,-1,1,-1,1,-1,0,0,0,0};
  int nElemT=6;
  double train[]={1,1,1,1,1,1};
  
  int delay, Post=3, Pre=1;
  double phiHat, AHat, sigmaSqrNoise;
  cvec thetaEst;
  
  cvec dataC=arrayToCvec(data, nElem); 
  cvec trainC=arrayToCvec(train, nElemT); 
  
  thetaEst = synchCatchChannel(dataC, trainC, Post, Pre , &phiHat,  &AHat, &delay, &sigmaSqrNoise );

  std::cout<<"Channel="<<thetaEst<<"\n";
  std::cout <<" tsamp="<<delay<<"\n";
  std::cout<<" phase_hat="<<phiHat<<"\n";
  std::cout<<" A_hat="<<AHat<<"\n";
  std::cout<<" sigma_square_noise="<<sigmaSqrNoise<<"\n";
  return 0;
}
*/
