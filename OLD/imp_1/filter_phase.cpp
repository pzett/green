#include <iostream>
#include <cmath>
#include <complex>
#include <cstdio>
#include <stdio.h>      
#include <stdlib.h>

using namespace std;


extern int  filter_phase(complex<double> * data, int data_size, double phi_hat, complex<double> * train, int train_size, double Q0, double sigma_phi_sqr, double sigma_m_sqr, complex<double> * out );

template <class T >
extern T mod(T x, T y);

#define PI  3.14159265359

#define DispVal(X) std::cout << #X << " = " << X<< std::endl


template <class T >
T mod(T x, T y){
  
  T n=floor(x/y);
  
  T result=x-n*y;

  return result;

}


int filter_phase(complex<double> * data, int data_size, double phi_hat, complex<double> * train, int train_size, double Q0, double sigma_phi_sqr, double sigma_m_sqr, complex<double> * out ){
  
  double b=1+(Q0/sigma_phi_sqr);
  double a=sigma_m_sqr/sigma_phi_sqr;

  

  int total_samps=data_size-train_size;
  
  double e, phi_mes_estimated, phi;

  double res_mod ,mod1, mod2, angle;
  
   

  phi=phi_hat;

  
  for(int i1=0; i1<train_size; i1++){
    e=b+a;
    phi_mes_estimated=arg(data[i1]*conj(train[i1]));
    mod1=phi_mes_estimated-phi;
    mod2 = 2*PI;
    res_mod=mod(mod1,mod2);
   
    phi=(a/e)*(res_mod)+phi;
    b=1+b/e;
   
    
  }
  //Attention, for good result this angle should be defined between -pi and pi

  complex<double> arg1;

 

  for(int i2=train_size; i2<data_size ; i2++ ){
    e=b+a;
    angle=arg(data[i2]);
    mod1=angle-phi;
    mod2 = PI/2;
    res_mod=mod(mod1,mod2);
    phi_mes_estimated = res_mod-PI/4;
    phi=(a/e)*(phi_mes_estimated)+phi;
    b=b/e+1;
    arg1=complex<double>(0,-1*phi);
    out[i2-train_size]=data[i2]*exp(arg1);
    
  }
  //attention, here we assume the QPSK mapping and this formula is not very good, we should change it

  

  return total_samps;

};



