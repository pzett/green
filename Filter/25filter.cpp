#include "filter.cpp"
#include <cstdio>
#include <complex>
#include <stdlib.h>
#include <cmath>
#include <fstream>


using namespace std;


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
      xShifter[f] = cos(-2.00*3.14159*shift*counter);
      xShifter[f+1] = sin(-2.00*3.14159*shift*counter);
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

/** This function lowpass filters an input signalThis function calculates the power of an array
 *
 * @pre:
 *    - data         : input signal (array)
 *    - no_elements  : array size of the signal
 *
 * @post:
 *    - power        : power of the input signal
 */

double powerTotArray(double data[], int no_elements)
{
  double power = 0.0;
  double tmp;
  for (int i=0;i<no_elements;i++){
    tmp= data[i];
    power= power+(tmp*tmp)/(no_elements/2);
    if(power<0){
      std::cout<<power;
    };
  }
  return power;
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
  //complex<double>* dataC = (complex<double>*)data;
  //std::complex<double> tempC[nElem];
  double temp[nElem];
  //double power[numFilt];
  for(int i=0;i<=(numFilt);i++)
    {
      double shift = shiftindex*i;
      //std::cout << "Shift (normalized w0): " << shift  << std::endl;
      lowPassFilter(data,temp,nElem,shift);
      //double* temp = (double*) tempC;
      power[i] = powerTotArray(temp,nElem);
      //std::cout << "power: " << power[i] << " at w0: " << shift << std::endl;
      //std::cout << "numFilt: " << i << std::endl;
    };
}


int main(){

  //int rows = 1;//rows of the matrix = number of filters
  int numFilt = 25;
  int nElem = 1000;//input data size
  double data[nElem];//input data
  double shiftindex = 0.04;//how big the shift between the filters is (equidistantial)
  // creating data
  for(int i=0;i<nElem-1; i=i+2){
    data[i]=(double)3*cos(3.14159*0.3*i);//+cos(2*3.14159*0.2*i);
    data[i+1]=(double) 10*sin(3.14159*0.1*i);
  };

  double power[numFilt];
  
  powerOfFreqBands(data,nElem,shiftindex,power,numFilt);
 
  for(int k=0;k<numFilt;k++)
    {
      std::cout << "power: " << power[k] << " " << std::endl;
    };  
  return 0;
}


  /* Done in the function powerOfFreqBands */
  // complex<double>* dataC = (complex<double>*)data; //casting
  // std::complex<double>  y[nElem][numFilt];//y[rows][nElem];//storage for output data
  // std::complex<double> tempC[nElem];
  // double power[numFilt];
  // for(int i=0;i<(numFilt);i++)
  //   {
  //     double shift = shiftindex*i;
  //     //std::cout << "Shift (normalized w0): " << shift  << std::endl;
  //     lowPassFilter(dataC,tempC,nElem/2,shift);
  //     double* temp = (double*) tempC;
  //     power[i] = powerTotArray(temp,nElem/2);
  //     //std::cout << "power: " << power[i] << " at w0: " << shift << std::endl;
  //   };
  /* -------------------------------------- */
