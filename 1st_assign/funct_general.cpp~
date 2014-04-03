// GREEN CW Transmission

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
void create_data_CW (short *buffer, int total_num_samps, float nfreq, float Amp )
{
  short r,i,count;
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
 *       - power: a short with the power
 */
short powerTotArray( short data[], int no_elements){
  short power=0;
  short tmp;
  for (int i=0;i<no_elements;i++){
    tmp= data[i];
    power= power+(tmp*tmp/((no_elements/2)));
  };
return power;
};

/** Computes the power in the bands centered in 0, 1, ..., 12, -12, -11, ..., -1 
 * @pre:
 *       - data_to_harness: a pointer to an array of float
 *       - no_elements: the length of the array
 * @post:
 *       - data_to_harness is now composed with the square of elements
 */
/* WRONG!!!
void powerRange(cvec *data, float *band){
  vec P;
  int data_size=data->size();
  vec window=hanning(data_size);
  cout << window << '\n';
  cvec dataW=to_cvec(zeros(data_size));
  
  dataW=elem_mult(*data,to_cvec(window));
  P=sqr(abs(fft(dataW,64)));
  
  cout << "P" << P<<'\n' << '\n';
  cout << "dataW" << dataW <<'\n';
  int n=P.size();
  cout << "N" << n << '\n';
  int nbr=floor(n/25);
  cout<< "nbr" << nbr << '\n';
  float fs=25e6;


for (int i=0;i<25;i++){
  if (i==0){
  }
  else {}
  band[i]=sum(P(i*nbr, (i+1)*nbr))*fs/n;
 };
}
*/
