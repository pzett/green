// GREEN CW Transmission

#include <iostream>
#include <cstdio>
#include <complex>

#include <itpp/itbase.h>
#include <itpp/itsignal.h>


using namespace itpp;


using namespace std;


void square_elements_of_array(float data_to_harness[],int no_elements) {
  for (int i1=0;i1<no_elements;i1++) {
    data_to_harness[i1]=data_to_harness[i1]*data_to_harness[i1];
  };
};


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

short powerTotArray( short data[], int no_elements){
  short power=0;
  short tmp;
  for (int i=0;i<no_elements;i++){
    tmp= data[i];
    power= power+(tmp*tmp/((no_elements/2)));
  };
return power;
};


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
  band[i]=sum(P(i*nbr, (i+1)*nbr))*fs/n;
 };
}
