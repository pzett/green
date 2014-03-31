// GREEN CW Transmission

#include <iostream>
#include <cstdio>
#include <complex>



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
  short power;
  for (int i=0;i<no_elements;i++){
    short tmp=(short) data[i];
    power=(short) power+tmp*tmp/(no_elements/2);
  };
return power;
};


