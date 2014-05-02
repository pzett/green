#include <iostream>
#include <cmath>
#include <complex>
#include <vector>
#include <itpp/itbase.h>
#include <itpp/itcomm.h> 
#include <itpp/stat/misc_stat.h>
#include <itpp/itsignal.h>
#include <itpp/itcomm.h>
#include "arrayToCvec.cpp"
#include <queue>         


#define DispVal(X) std::cout << #X << " = " << X<< std::endl

void print(std::queue<itpp::vec> s)
{
  int sizeQueue=s.size();
  DispVal(sizeQueue);
 
    while(sizeQueue!=0){
      itpp::vec x= s.front();
      s.pop();
      std::cout << x << "\n";
      sizeQueue--;
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

/*
int main (){
  
   double noisevar = 0.01;
   AWGN_Channel awgn_channel(noisevar);

  //create data to test
  std::queue<vec> Pilot;
  int nPilot=8;
  itpp::vec p;
  p.set("1.01,1.02,0.98,0.79,1.3,0.87,1.11,1.002");
  
  for(int i=0;i<20; i++ ){
    itpp::vec noise = awgn_channel(p);
    Pilot.push(noise);
  }
  print(Pilot);
  
  itpp::vec x0;
  x0.set("1,1,1,1,1,1,1,1");

  std::queue<vec> filtPilot= kalmanGain (Pilot,  nPilot,  1, 1, 1, 0.1, 0.1,  x0, 0.1);
  
  print(filtPilot);
  
  return 0;
}
*/
