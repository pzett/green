#include "filter.cpp"
#include <cstdio>
#include <complex>
#include <stdlib.h>
#include <cmath>
#include <fstream>


using namespace std;

template <class T,class W>
void lowPassFilter(T x[],T y[],int nElemIN,W shift)
{
  /* Low pass filter coefficients */
  double a[] ={1.0000,-3.7374,5.2460,-3.2774,0.7689};
  double b[] ={1.0e-04 *0.0562,1.0e-04*0.2247,1.0e-04*0.3370,1.0e-04*0.2247,1.0e-04*0.0562};
  int nElemB = 5;
  int nElemA = 5;

  FILE * xFile;
  FILE * xShift;
  FILE * xFilt;

  double count = 1.00;
  const double pi = std::acos(-1);
  const std::complex<double> i(0, 1);
  T xShifter[nElemIN];
  T xShifted[nElemIN];
  /* Filling the shifter */
  for(int f=0;f<nElemIN;f++)
    {
      xShifter[f] = exp(i*2.00*pi*(-shift)*count);
      /* Multiplying the complex arrays elementwise */
      xShifted[f]=xShifter[f]*x[f];
      count++;
    };
  xFile = fopen("x.bin","wb");
  xShift = fopen("xShift.bin","wb");
  xFilt = fopen("xFilt.bin","wb");
  fwrite(x, sizeof(std::complex<double>), nElemIN, xFile);
  fwrite(xShifted,sizeof(std::complex<double>), nElemIN, xShift);
  fclose(xFile);
  fclose(xShift);
  /* filtering */
  filter(b,nElemB,a,nElemA,xShifted,y,nElemIN);
  fwrite(y,sizeof(std::complex<double>), nElemIN, xFilt);
  fclose(xFilt);
}
template <class T>
double powerTotArray(T data[], int no_elements)
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

int main(){

  //int rows = 1;//rows of the matrix = number of filters
  int numFilt = 25;
  int nElem = 1000;//input data size
  double data[nElem];//input data
  double shiftindex = 0.04;//how big the shift between the filters is (equidistantial)
  // creating data
  for(int i=0;i<nElem-1; i=i+2){
    data[i]=(double)3+3* cos(3.14159*0.05*i);//+cos(2*3.14159*0.2*i);
    data[i+1]=(double) 10*sin(3.14159*0.4*i);
  };
  complex<double>* dataC = (complex<double>*)data; //casting
  std::complex<double>  y[nElem][numFilt];//y[rows][nElem];//storage for output data
  std::complex<double> tempC[nElem];
  double power[numFilt];
  for(int i=0;i<(numFilt);i++)
    {
      double shift = shiftindex*i;
      //std::cout << "Shift (normalized w0): " << shift  << std::endl;
      lowPassFilter(dataC,tempC,nElem/2,shift);
      double* temp = (double*) tempC;
      power[i] = powerTotArray(temp,nElem/2);
      //std::cout << "power: " << power[i] << " at w0: " << shift << std::endl;
    };

  for(int k=0;k<numFilt;k++)
    {
      std::cout << "power: " << power[k] << " " << std::endl;
    };  
  return 0;
}
