#include <iostream>
#include <cmath>
#include <complex>
#include <cstdio>
#include <stdio.h>      
#include <stdlib.h>

using namespace std;


#define PI  3.14159265359

#define DispVal(X) std::cout << #X << " = " << X<< std::endl


/** Modulus after division
 *
 * @pre:
 *    - x: Numerator
 *    - y: Denominator
 *                            
 *
 * @post:
 *    - modulus after division computed
 */
template <class T >
T mod(T x, T y){
  
  T n=floor(x/y);
  
  T result=x-n*y;

  return result;

}

/** Function is supposed to filter the phase offset using state space model,
* it is a Kalman filter in a particular case with known phi_hat:
*
*         B(n)A+B(n)+A
* B(n+1)=--------------- 
*            B(n)+A
*   
*             A                     B(n) 
* phi(n+1)=--------phi(n)  +    ---------*phi(n+1)
*           B(n)+A                 B(n)+A  
*           Q0
*  B0=-------------- +1
*       sigma_phi^2
*
*  sigma_phi_sqr=pi/4*1/number of bits transmitted
*  
 * @pre:
 *    - rx_data: pointer to complex array of downsampled data without guard bits; the first bit is the 
 *              first bit from the training sequence
 *    - data_size: size of received data
 *    - phi_hat: estimated angle from sync_catch
 *    - train: pointer to complex array with train sequence
 *    - train_size: size of train sequence
 *    - Q0: depend on the length of the training sequence: pi^2/8;
 *    - nElemIN: the size of x[] and y[]
 *    -sigma_phi_sqr : is defined as a constant term (here,as we have already 
 *               removed the free offset: should be low, for example: 10^-3
 *               sigma_phi_sqrt = pi/4*1/number of bits transmitted.
 *     -sigma_m_sqr   - may vary in time. 
 *     -out: data filtered
 *
 * @post:
 *    - out is now filtered
 *    - returns total number of data samples
 *
 */



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



