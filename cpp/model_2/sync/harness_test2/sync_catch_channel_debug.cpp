#include <iostream>
#include <complex>
#include <vector>

#include <itpp/itbase.h>
#include <itpp/itcomm.h> 
#include <itpp/stat/misc_stat.h>


using namespace std;
using namespace itpp;
#define DispVal(X) std::cout << #X << " = " << X<< std::endl

extern cvec  synchCatchChannel(cvec dataC, cvec trainCUp, int Post, int Pre , double *phiHat, double *AHat, int *delay, double *sigmaSqrNoise );



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
  DispVal(crossCorr.length()-dataC.length()+1);
  for(int i=dataC.length()-1;i<crossCorr.length();i++){
    tmp[count]=abs(crossCorr[i]);
   
    //DispVal(tmp[count]);
    //DispVal(count);
    if (tmp[count]> max && count<(crossCorr.length()-dataC.length()+1)/2  ){
      max=tmp[count];
      index=i;
      DispVal(index-dataC.length()+1);
    }
     count++;
  }
  
  index=index-dataC.length()+1;
  DispVal(index);
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
   
  //std::cout<<"SigmaYx=\n"<<SigmaYx<<"\n";

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

