#include "filter.cpp"
#include <cmath>
#include <complex> 
#include <stdlib.h>

using namespace std;

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

bool transmissionDetection(double buff_double[],int nSamp){
  bool transmissionDetected = false;
  double threshold = 1.0;
  double power = 0.0;

  // lowpassfilter
  double a[] ={1.0000,-3.7374,5.2460,-3.2774,0.7689};
  double b[] ={1.0e-04 *0.0562,1.0e-04*0.2247,1.0e-04*0.3370,1.0e-04*0.2247,1.0e-04*0.0562};
  int nElemB = 5;
  int nElemA = 5;
  double buffFilt[nSamp];
  filter(b,nElemB,a,nElemA,buff_double,buffFilt,nSamp);

  // power calculation
  power = powerTotArray(buffFilt, nSamp);

  // comparing with threshold
  if (power >= threshold)
    {
      transmissionDetected = true;
      std::cout << "IS transmitting!" << std::endl;
      std::cout << "received power: " << power << std::endl;
    };
  if(power < threshold)
    {
      std::cout << "not transmitting!" << std::endl;
      std::cout << "received power: " << power << std::endl;
    }
  return transmissionDetected;
}


int main(){

  bool transMissDetect = false;
  //testing data
  int nSamp = 1000;
  short buff_short[nSamp];
  double buff_double[nSamp];
  std::fill_n(buff_short,nSamp,1.0); 

  // Casting from short to double
  for(int f=0;f<nSamp;f++){
    buff_double[f] = buff_short[f];
    //std::cout << buff_double[f] << std::endl;
  }
  transMissDetect = transmissionDetection(buff_double,nSamp);

  return 0;
}
