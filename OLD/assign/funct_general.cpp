//  General functions for the first assignement


#include <iostream>
#include <cstdio>
#include <complex>

#include <itpp/itbase.h>
#include <itpp/itsignal.h>


using namespace itpp;
using namespace std;

/** Takes the square of the array
 * @pre:
 *       - data_to_harness: a pointer to an array of float
 *       - no_elements: the length of the array
 * @post:
 *       - data_to_harness is now composed with the square of elements
 */
void square_elements_of_array(float data_to_harness[],int no_elements) {
  for (int i1=0;i1<no_elements;i1++) {
    data_to_harness[i1]=data_to_harness[i1]*data_to_harness[i1];
  };
};

/** Generate a continuous waveform
 * @pre:
 *    - buffer: pointer to the array of short that will contains the datas
 *    - total_num_samps: length of the buffer
 *    - nfreq: the normalized frequency of the continuous wave
 *    - Amp: the ampllitude (RMS lower the 5000)
 * @post:
 *    - buffer now contains the continuous wave
 */
void create_data_CW (double *buffer, int total_num_samps, double nfreq, double Amp )
{
  double r,i,count;
  count=0;

  for (int i1=0;i1<total_num_samps;i1=i1+2) {
   
      r=Amp*cos(2*3.1415926*nfreq*count);
      buffer[i1] = r;

      i=-1*Amp*sin(2*3.1415926*nfreq*count);
      buffer[i1+1] = i;

      count++;      
  };
};

/** Computes the power of the given array
 * @pre:
 *       - data: a pointer to an array of short
 *       - no_elements: the length of the array
 * @post:
 *       - power: a float with the power
 */
float powerTotArray( short data[], int no_elements){
  float power=0;
  float tmp;
  for (int i=0;i<no_elements;i++){
    tmp= (float) data[i];
    power= power+(tmp*tmp)/(no_elements/2);
    if(power<0){
      std::cout<<power;
    };

  };
  //float result=0;
  // std::cout<<power<<"\n";
  // std::cout<<(short)power<<"\n";
  return power;
};



//Functions to convert types of data:
//->Process data as double
//->Transmit data as short

/** Convert an array of complex short into an array of complex double
 * @pre:
 *    - array: the pointer to the array of complex short
 *    - nElem: the number of elem in array
 *    - out: a pointer to an complex double array of size nElem
 *
 * @post:
 *    - out is now filled with complex double
 */
void compShortToCompDouble(complex<short> *array, int nElem, complex<double>* out){
  double arrayS[nElem*2];
  int c=0;
  // Copy the datas to an array of double
  for (int i=0; i<nElem;i++){
    arrayS[c]=(double) array[i].real();
    arrayS[c+1]=(double) array[i].imag();
    
    c=c+2;
  }
 
  c=0;
  // Fill out with objects
  for (int i =0; i<nElem;i++){   
    out[i]= complex<double>( arrayS[c], arrayS[c+1]);
    c=c+2;
  
  }
 
}

/** Convert an array of complex double into an array of complex short
 * @pre:
 *    - array: the pointer to the array of complex double
 *    - nElem: the number of elem in array
 *    - out: a pointer to an complex short array of size nElem
 *                                               --------------
 * @post:
 *    - out is now filled with complex short
 */
void compDoubleToCompShort(complex<double> *array, int nElem, complex<short>* out){
  short arrayS[nElem*2];
  int c=0;
  // Copy the data to an array of short
  for (int i=0; i<nElem;i++){
    arrayS[c]=(short) array[i].real();
    arrayS[c+1]=(short) array[i].imag();
    
    c=c+2;
  }
  
  c=0;
  //Fill out
  for (int i =0; i<nElem;i++){   
    out[i]= complex<short>( arrayS[c], arrayS[c+1]);
    c=c+2;
  };
 
}
