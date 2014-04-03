#include "filter.cpp"
#include <iostream>

using namespace std;

template <class T>
void notch(T x[], T y[], int nElemIN){
// Defining the notch filter, design with matlab
  int nElemA = 2;
  int nElemB = 2;
  double a[]={1.0000, -0.9000};
  double b[]={1.0000, -1.0000};
  filterComp(b,nElemB,a,nElemA,x,y,nElemIN);
}

// This is for removing the DC offset on the fly

template <class T>
void removeDco(T x[], T y[], int nElemIN){
  double a1 = 0.99;
  double a2 = 0.99;
  double c1 = 1.00;
  double c2 = 1.00;  
  int i1 = 0;
  int i2 = 1;
  while(i1 <= nElemIN+1 &&  i2 <= nElemIN)
    {
      c1=c1+a1*x[i1];
      y[i1]=x[i1]-(1-a1)*c1;
      c2=c2+a2*x[i2];
      y[i2]=x[i2]-(1-a2)*c2;
      i1=i1+2;
      i2=i2+2;
	};
}

// Testing the removal
// int main(){
//   int nElement = 10;
//   int count = 0;
//   double in[]={20.0000,10.0000,19.9211,11.2533,19.6858,12.4869,19.2978,13.6812,18.7631,14.8175};// a complex wave with dc
//   double out[nElement];        // complex wave withOUT dc
//   double expected[]={4.1915,0.0828,4.3374,1.2331,4.3287,2.3655,4.1692,3.4617,3.8650,4.5045};
//   //testing the flters
//   removeDco(in,out,nElement);

// for (int i=0;i<nElement;i++){
//   // if(out[i] == expected[i])
//   //   {
//   //     count++;
//   //   };
//   // std::cout << count << std::endl;
//   // if(count == nElement)
//   //   {
//   //     std::cout << "Filter works!" << std::endl;
//   //   }
//   cout << "expected[" << i << "]= " << expected[i] << std::endl;
//  }
// for (int i=0;i<nElement;i++){
//   cout << "out[" << i << "]= " << out[i] << std::endl;
//  }

//   return 0;
// };
