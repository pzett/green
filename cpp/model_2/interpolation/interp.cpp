#include <iostream>
#include <cmath>
//#include "arrayToCvec.cpp"
#include <itpp/itbase.h>
#include <itpp/itsignal.h>
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

void interp(vec pilotFFT, double pilot_pos[], int nPilot, int nElem){
  // Search the highest frequency pilot
  int lPilot=0;
  for (; lPilot<nPilot && pilot_pos[lPilot]< (nElem/2); lPilot++){  std::cout << pilot_pos[lPilot]<< std::endl;}
  lPilot--;


  // Create cyclic extension of pilot position
  vec pilotP( pilot_pos, nPilot);
  pilotP.ins(0, pilotP.right(1)-nElem);
  std::cout << pilotP << std::endl;
  //Create a cyclic extension of pilotFFT
  vec pilotFFTp=pilotFFT; 
  pilotFFTp.ins(0, pilotFFTp.right(1));

  // Evaluate the delta amplitude
  pilotFFT.ins(pilotFFT.length(),pilotFFT.left(1));
  std::cout << pilotFFT << std::endl;
  std::cout << pilotFFTp << std::endl;
  vec pilotFFTd=pilotFFT-pilotFFTp;
  std::cout << pilotFFTd << std::endl;

  // Evaluate the delta index


  // Create the ouput vector
  vec out(nElem);
  for (int i=0; i<lPilot+1; i++){//+1 because of the periodisation
    int deltaI=pilotP(i+1)-pilotP(i);
    std::cout <<i<<  " delta " <<deltaI << std::endl;
    double step=pilotFFTd(i)/deltaI;
    std::cout << step << std::endl;
    for (int ii=0;ii<deltaI; ii++ ){
      out.set(mod((pilotP(i)+ii),(double) nElem), pilotFFTp(i)+ii*step);
      std::cout << out << std::endl;
    }
  }
  std::cout << out<<std::endl;
  for(int i=pilotP(lPilot+1);i<nElem/2;i++ ){
out.set(i, pilotFFTp(lPilot+1));
  std::cout << out<<std::endl;
  }
  for(int i=pilotP(lPilot+2);i>=nElem/2;i-- ){
out.set(i, pilotFFTp(lPilot+2));
  std::cout << out<<std::endl;
  }
  for (int i=lPilot+2;i<nPilot; i++){
    int deltaI=pilotP(i+1)-pilotP(i);
    std::cout <<i<<  " delta " <<deltaI << std::endl;
    double step=pilotFFTd(i)/deltaI;
    std::cout << step << std::endl;
    for (int ii=0;ii<deltaI; ii++ ){
      out.set(pilotP(i)+ii, pilotFFTp(i)+ii*step);
      std::cout << out<<std::endl;
    }  
  }
}

int main (){
  int nElem = 18;
  double pilot_pos[]={ 4 , 11 , 13, 15,17};
  int nPilot=5;
  double pilot[]={ 1,2,3,4,5};
  vec pilotFFT(pilot, nPilot);

 std::cout << pilotFFT << std::endl;
 interp(pilotFFT, pilot_pos, nPilot, nElem);
  return 0;

}
