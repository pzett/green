#include <iostream>
#include <cmath>
//#include "arrayToCvec.cpp"
#include <itpp/itbase.h>
#include <itpp/itsignal.h>
#include <math.h>
using namespace itpp;


cvec arrayToCvec(std::complex<double> *array, int nElem){
  //std::cout<< b << "\n";
  cvec out(nElem);
  for (int i=0; i<nElem;i++){
    out.set(i,array[i]);
    //std::cout<< i << "\n";
  }
  //std::cout << "power= " << out.get(0, b-1) << '\n';
  return out;
}

cvec arrayToCvec(double *array, int nElem){
  std::complex<double> *arrayC=(std::complex<double> *) array; // Cast the double to complex double
  return arrayToCvec(arrayC, nElem/2);

}

double mod(double a, double b){
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
      out.set(mod((pilotP(i)+ii),(double) nElem), pilotFFTp(i)+ii*step);
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
 
    double step=(mod(pilotFFTd(i)+M_PI,2*M_PI)-M_PI)/deltaI;// Difference in phase
  
    //std::cout << step << std::endl;
    for (int ii=0;ii<deltaI; ii++ ){
      out.set(mod((pilotP(i)+ii),(double) nElem), mod(pilotFFTp(i)+ii*step+M_PI,2*M_PI)-M_PI);
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
    double step=(mod(pilotFFTd(i)+M_PI,2*M_PI)-M_PI)/deltaI;// Difference in phase
    //std::cout << step << std::endl;
    for (int ii=0;ii<deltaI; ii++ ){
      out.set(pilotP(i)+ii,mod(pilotFFTp(i)+ii*step+M_PI,2*M_PI)-M_PI );
      //std::cout << out<<std::endl;
    }  
  }
  return out;
}

int main (){
  int nElem = 18;
  double pilot_pos[]={ 2 , 5 , 13, 15,17};
  int nPilot=5;
  double pilot[]={ 3,-3,-2,3,1.3};
  vec pilotFFT(pilot, nPilot);

 std::cout << pilotFFT << std::endl;
 interp(pilotFFT, pilot_pos, nPilot, nElem);
  return 0;

}
