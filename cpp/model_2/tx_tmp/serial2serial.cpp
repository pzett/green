
 #include <algorithm>
 #include <iostream>
 #include <fstream>
 #include <cmath>
 #include "arrayToCvec.cpp"
 #include "dataAlloc.cpp"
 #include "prePost.cpp"
 #include <itpp/itbase.h>
 #include <itpp/itsignal.h>


 /*
 This function prepares a vector of NCarriers to be transmitted with OFDM.
 */
using namespace std;
using namespace itpp;

cvec dataAlloc (double *data_qam, double data_pilot[], double pilot_pattern[], double data_pattern[], int N){

  int n=0;
  int m=0;
  std::complex<double> zeroC(0,0);
  std::complex<double> *arrayData=(std::complex<double> *) data_qam;
  std::complex<double> *arrayPilot=(std::complex<double> *) data_pilot;
  cvec data_alloc(N);

  for(int i=0;i<N;i++){;
   if (pilot_pattern[m]-1==i){
     data_alloc.set(i,arrayPilot[m]);   
      m++;
   }
   else if (data_pattern[n]-1==i){
     data_alloc.set(i,arrayData[n]);   
      n++;
   }
   else{
     data_alloc.set(i,zeroC);
   }
  
 }
  return(data_alloc);
}


cvec prePost (cvec data_time, int pre, int post){

   data_time.ins(0, data_time.right(pre));
   data_time.ins(data_time.length(), data_time.left(post));
   return(data_time);

}



cvec main(double data_qam[], double data_pil[],double data_pattern[], double pilot_pattern[],int N, int nUsedC, int nPilotC, int pre, int post){
   

    //Read data from a file
    double data_qam[2*nUsedC];
    std::ifstream ifs( "dataMapped.dat" , std::ifstream::in );
    ifs.read((char * )data_qam,2*nUsedC*sizeof(double));
    ifs.close();

    //Read pilots from a file
    double data_pil[2*nPilotC];
    std::ifstream ifs2( "data_pilot.dat" , std::ifstream::in );
    ifs2.read((char * )data_pil,2*nPilotC*sizeof(double));
    ifs2.close();

    //Read data pattern from a file
    double data_pattern[nUsedC];
    std::ifstream ifs3( "dataPattern.dat" , std::ifstream::in );
    ifs3.read((char * )data_pattern,nUsedC*sizeof(double));
    ifs3.close();

    //Read pilot pattern from a file
    double pilot_pattern[nPilotC];
    std::ifstream ifs4( "pilotPattern.dat" , std::ifstream::in );
    ifs4.read((char * )pilot_pattern,nPilotC*sizeof(double));
    ifs4.close();


    cvec data_alloc=dataAlloc(data_qam,data_pil,pilot_pattern,data_pattern,N);

     std::cout << "datAlloc done, the final allocation per carrier is;" << std::endl;
     /*for(int i=0;i<data_alloc.length();i++){
      std::cout << data_alloc.get(i) << std::endl;
      }*/

      cvec data_time=itpp::ifft(data_alloc,N);//IFFT
      cvec out=prePost(data_time,pre,post);

      std::cout << "prePost is done. Check the results" << std::endl;
      for(int i=0;i<out.length();i++){
      std::cout << out.get(i) << std::endl;
      }

      return(out);
  }
