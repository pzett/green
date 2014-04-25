#include <iostream>
#include <complex>
#include <vector>

#include <itpp/itbase.h>
#include <itpp/itcomm.h> 

#include "arrayToCvec.cpp"

using namespace std;
#define DispVal(X) std::cout << #X << " = " << X<< std::endl

/*
Function that catch tsamp an estiametes the channel;
  return tsamp
  and phi_hat, A_hat, theta_est, 

  Train sequence should be upsampled!!!

  Since we use matrices-> ITPP

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

  std::cout<<"Data="<<dataC<<"\n";
  std::cout<<"Train="<<trainCUp<<"\n";
  
  //Computes crosscorrelation:

  cvec crossCorr=itpp::xcorr(dataC, trainCUp);

  std::cout<<"Cross="<<crossCorr<<"\n";

  //Search for the maximum
  double max=0.0;
  int index=0;
  for(int i=dataC.length()-1;i<crossCorr.length();i++){
    double tmp=abs(crossCorr[i]);
    DispVal(tmp);
    if (tmp> max){
      max=tmp;
      index=i;
    }
  }
  index=index-dataC.length()+1;
  DispVal(index);
  delay=&index;
 //Computes Estimate of phase at the peak:
   *phiHat=arg(dataC(index));

   //std::cout<<"tsamp="<<index<<"\n";
  //Removes received train train from data

  cvec trainRecC(trainCUp.length());
  for(int i=index;i<trainCUp.length();i++){
    trainRecC.set(i,dataC[i]);
  }
  //std::cout<<"TrainRec"<<trainRecC<<"\n";

  //Channel estimator using training sequence:

  //autoCorr of known training sequence:
  cvec autoCorr=itpp::xcorr(trainCUp, trainCUp);
  std::cout<<"Auto="<<autoCorr<<"\n";
  
  *AHat=abs(crossCorr(index+dataC.length()))/abs(autoCorr(trainCUp.length()-1));
 

  //SigmaYY:
  cvec aux(Pre+Post+1);
  int count=0;
  for(int i=trainCUp.length()-1; i<=Pre+Post+1; i++){
    if(i<=2*trainCUp.length()-1){
      aux.set(count,autoCorr[i]);
    }else{
      aux.set(count,complex<double>(0.0,0.0));
     }
    count++;
  }
  cmat SigmaYY=toeplitz(aux);
  std::cout<<"SigmaYY=\n"<<SigmaYY<<"\n";

  //SigmaYx:
  count=0;
  cvec SigmaYx(Pre+Post+1);
   for(int i=dataC.length()-Pre-1+index; i<dataC.length()+Post+index; i++){
    SigmaYx.set(count, crossCorr[i]);
    //std::cout<<"xcorr=\n"<<crossCorr[i]<<"\n";
    count++;
  }
   
   std::cout<<"SigmaYx=\n"<<SigmaYx<<"\n";

    //Estimate of the channel:
    cmat invSigmaYY=itpp::inv(SigmaYY);
    cvec theta_est=itpp::operator*(invSigmaYY,SigmaYx);
   
    //std::cout<<"invSigmaYY=\n"<<invSigmaYY<<"\n";
    // std::cout<<"Channel="<<theta_est<<"\n";
    
    //Compute the variance of the estimation:
    
    cvec trainCEst=filter( theta_est, 1, trainCUp);
    std::cout<<"trainCEst="<<trainCEst<<"\n";

    cvec crossEst=itpp::xcorr(trainRecC- trainCEst);
    //std::cout<<"crossEst="<<crossEst<<"\n";
    *sigmaSqrNoise=abs(crossEst(trainRecC.length()-1));

    return theta_est;
}

int main(){
  
  int nElem=8;
  double data[]={-1,1,1,+1,1,-1,1,-1};
  int nElemT=4;
  double train[]={1,1,1,1};
  
  int delay, Post=1, Pre=0;
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

