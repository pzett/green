
 #include <algorithm>
 #include <iostream>
 #include <fstream>
 #include <cmath>
 #include "arrayToCvec.cpp"
 #include <itpp/itbase.h>
 #include <itpp/itsignal.h>


 /*
 This function allocates data from pilot and source, which are stored in small buffers of doubles ordered like: (real[1],imag[1],real[2],...)
 */
using namespace std;
using namespace itpp;

cvec dataAlloc (double *data_qam, double data_pilot[], int pilot_pattern[], int data_pattern[], int N){

  int n=0;
  int m=0;
  std::complex<double> zeroC(0,0);
  cvec data_alloc(N);
  std::complex<double> *arrayData=(std::complex<double> *) data_qam;
  std::complex<double> *arrayPilot=(std::complex<double> *) data_pilot;
  std::complex<double> tmp[8];


  for(int i=0;i<N;i++){;
   if (pilot_pattern[m]==i){
     data_alloc.set(i,arrayPilot[m]);   
      m++;
   }
   else if (data_pattern[n]==i){
     data_alloc.set(i,arrayData[n]);   
      n++;
   }
   else{
     data_alloc.set(i,zeroC);
   }
  
 }
  return(data_alloc);

}
