// MODIFIED FOR RX!!! DO NOT USE AS MODEL!!!


//#include <itpp/itbase.h>
#include <iostream>

using namespace std;
//using namespace itpp;

#define DispVal(X) std::cout << #X << " = " << X<< std::endl

/** Filter the real array x by the filter h(q)=b(q)/a(q)
 *
 *                             b[0]+b[1]*q^(-1)+...+b[nElemB-1]*q^(nElemB-1)
 *                       h(q)=----------------------------------------------
 *                              1+a[1]*q^(-1)+...+a[nElemA-1]*q^(nElemA-1)
 * @pre:
 *    - b: pointer to array of the numerator of h(q) b[n]
 *    - nElemB: size of b[]
 *    - a: pointer to array of the numerator of h(q) a[n]
 *             !!!  a[0]=1 !!!
 *    - nElemA: size of a[]
 *    - x: the array to filter
 *    - y: the array of filtered elements, size nElemIN
 *    - nElemIN: the size of x[] and y[]
 *
 * @post:
 *    - y is now filled
 *
 */

void filter(double b[], int nElemB, double a[], int nElemA, double x[], double y[], int nElemIN){
  if (a[0]!=1){cerr << "a[0] not equal to 1";}
  for (int i=0;i<nElemIN;i++){
    double xn[nElemB];
    // Create an array with x[n], x[n-1], ..., x[n-(nElemB-1)] 
    for (int ii=0;ii<nElemB;ii++){    
      if (i-ii>=0){      
	xn[ii]=x[i-ii];
      } // If out of range, pad with zeros
      else{
	xn[ii]=0;
      }
      //DispVal(xn[ii]);
    }
    double yn[nElemA];
    yn[0]=0;
    // Create an array with 0, y[n-1], ..., y[n-(nElemA-1)]
    for (int ii=1; ii<nElemA; ii++){
      if (i-ii>=0){
	yn[ii]=y[i-ii];
      } // If out of range, pad with zeros
      else{
	yn[ii]=0;
      }
      // DispVal(yn[ii]);
    }

    // Compute y[i]
    //cout<<"here! "<< i <<"\n";
    y[i]=0; // Initialisation
    for (int ii=0;ii<nElemB;ii++){
      y[i]=y[i]+b[ii]*xn[ii];
    }
    for (int ii=1;ii<nElemA;ii++){
      y[i]=y[i]-a[ii]*yn[ii];
    }
    //DispVal(y[i]);
    //cout << "\n";
  }
}

/** Split a complex array into an array of real and an array of imag
 *
 * @pre:
 *    - in: pointer to complex array to split
 *    - real: pointer to array that will contain the real part
 *                            size of nElem/2
 *    - imag: pointer to array that will contain the real part
 *                            size of nElem/2
 *    - nElem: the size of in[]
 *
 * @post:
 *    - real is now filled
 *    - imag is now filled
 */
void splitComp(double in[], double real[], double imag[], int nElem){
  for (int i=0;i<nElem;i=i+2){
    real[i]=in[i];
    imag[i]=in[i+1];
  }
}

/** Merge an array of real and an array of imag into an array of complex
 *
 * @pre:
 *    - in: pointer to complex array to split
 *    - real: pointer to array that contains the real part
 *                            size of nElem
 *    - imag: pointer to array that contains the real part
 *                            size of nElem
 *    - out: pointer that will contain the complex array
 *                            size of 2*nElem
 *    - nElem: the size of real[]
 *
 * @post:
 *    - out is now filled
 */
void compMerge(double real[], double imag[], double out[], int nElem){
  int c=0;
  for (int i=0;i<nElem;i++){
    out[c]=real[i];
    out[c+1]=imag[i];
    c=c+2;
  }
}
