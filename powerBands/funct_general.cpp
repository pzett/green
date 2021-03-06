//  General functions for the first assignement


#include <iostream>
#include <cstdio>
#include <complex>

#include <itpp/itbase.h>
#include <itpp/itsignal.h>

#include "funct_general.hpp"


using namespace itpp;
using namespace std;

/** Takes the square of the array
 * @pre:
 *       - data_to_harness: a pointer to an array of float
 *       - no_elements: the length of the array
 * @post:
 *       - data_to_harness is now composed with the square of elements
 */
void square_elements_of_array(double data_to_harness[],int no_elements) {
  for (int i1=0;i1<no_elements;i1++) {
    data_to_harness[i1]=data_to_harness[i1]*data_to_harness[i1];
    cout<<data_to_harness[i1]<<"\n";
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




/** This function lowpass filters an input signalThis function calculates the power of an array
 *
 * @pre:
 *    - data         : input signal (array)
 *    - nElem        : array size of the signal
 *    - shiftindex   : shifts the data (normalized frequency); in assignment 0.04;
 *    - numFilt      : number of filters; in assignment: 25;
 *
 * @post:
 *    - power        : power of freq bands of input signal
 */

void powerOfFreqBands(double data[],int nElem, double shiftindex,double power[],int numFilt)
{
  
  double temp[nElem];
  //double power[numFilt];
  for(int i=0;i<=(numFilt);i++)
    {
      double shift = shiftindex*i;
      //std::cout << "Shift (normalized w0): " << shift  << std::endl;
      lowPassFilter(data,temp,nElem,shift);
      
      power[i] = powerTotArray(temp,nElem);
      //std::cout << "power: " << power[i] << " at w0: " << shift << std::endl;
      //std::cout << "numFilt: " << i << std::endl;
    };
}


/** This function lowpass filters an input signal
 *
 * @pre:
 *    - x        : input signal (array)
 *    - nElemIN  : array size of the signal
 *    - shift    : shift of the input signal. represents the lowpass cut off freq
 *
 * @post:
 *    - y        : output signal
 */
//template <class T,class W>
void lowPassFilter(double x[],double y[],int nElemIN,double shift)
{
  /* Low pass filter coefficients */
  double a[] ={1.0000,-3.7374,5.2460,-3.2774,0.7689};
  double b[] ={1.0e-04 *0.0562,1.0e-04*0.2247,1.0e-04*0.3370,1.0e-04*0.2247,1.0e-04*0.0562};
  int nElemB = 5;
  int nElemA = 5;

  // FILE * xFile;
  // FILE * xShift;
  // FILE * xFilt;

  //double count = 1.00;
  //const double pi = std::acos(-1);
  //const std::complex<double> i(0, 1);
  double xShifter[nElemIN];
  double xShifted[nElemIN];
  /* Filling the shifter */
 int counter = 0;
 //std::cout << "size: " << nElemIN << std::endl;
  for(int f=0;f<=nElemIN;f=f+2)
    {
      xShifter[f] = cos(2.00*3.14159*shift*counter);
      xShifter[f+1] = sin(2.00*3.14159*shift*counter);
      /* Multiplying the complex arrays elementwise */
      xShifted[f]=xShifter[f]*x[f] - xShifter[f+1]*x[f+1]; // real part
      xShifted[f+1]=xShifter[f]*x[f+1] + xShifter[f+1]*x[f]; //imag part
      counter++;
    };
  //std::cout << "counter: " << counter << std::endl;

  /* filtering */
  filter(b,nElemB,a,nElemA,xShifted,y,nElemIN);
  // fwrite(y,sizeof(std::complex<double>), nElemIN, xFilt);
  // fclose(xFilt);
}
