#include <iostream>
#include <complex>
#include <vector>

#include <itpp/itbase.h>
#include <itpp/itcomm.h> 
#include <itpp/stat/misc_stat.h>
#include <itpp/itsignal.h>

#include <queue>         

#include "arrayToCvec.cpp"

using namespace std;
using namespace itpp;
#define DispVal(X) std::cout << #X << " = " << X<< std::endl


void print(std::queue<cvec> s)
{
  int sizeQueue=s.size();
  DispVal(sizeQueue);
 
    while(sizeQueue!=0){
      cvec x= s.front();
      s.pop();
      std::cout << x << "\n";
      sizeQueue--;
    }
    return;
}
/** Removes the cyclic prefix, sufix. Corrects also the frequency shift and puts 
 *    the OFMD data in a queue   
 *
 * @pre:
 *    - data: vector with received data
 *    - tIni: first point of the the ODFM symbols
 *    - nCarries: number of carriers used in OFDM system
 *    - nOFDM_symbols: number of OFDM symbols
 *    - Pre and Post: size of used cyclic prefix and sufix
 *    - OFDM_data: queue with OFDM data processed
 *
 * @post:
 *    - CP is now removed. Data in queue!
 */


void CPremove(cvec dataC, int tIni, int nCarriers, int nOFDM_symbols, int Pre, int Post, std::queue<cvec> &OFDM_data){


  int pt=tIni;
  cvec aux(nCarriers), Pre1(Pre), Pre2(Pre);
  cvec Pre1conj(Pre), mult(Pre);
  cvec data(nCarriers);
  //cycle to Remove CP:
  
  for(int i1=0;i1<nOFDM_symbols;i1++){

    //cycle to get Prefix, OFDM sysmbol and Post fix:
    for(int i2=0; i2<Pre; i2++){
      Pre1.set(i2,dataC[pt+i2]);
    }
    //std::cout<<"Pre1="<<Pre1<<"\n";

    pt=pt+Pre;

    for(int i2=0;i2<nCarriers;i2++){
      aux.set(i2,dataC[i2+pt]);
    }
    //std::cout<<"aux="<<aux<<"\n";

    pt=pt+nCarriers-Pre;
    
    
    for(int i2=0; i2<Pre ;i2++){
      Pre2.set(i2,dataC[i2+pt]);
    }
    
    //std::cout<<"Pre2="<<Pre2<<"\n";

    pt=pt+Post+Pre;

    //Process data and store it in the queue:
   
    Pre1conj =itpp::conj(Pre1);
       
    mult= itpp::elem_mult(Pre1conj,Pre2); 	
  
    complex<double> sum=itpp::sum(mult);
    double nuOff=arg(sum)/nCarriers;
    DispVal(nuOff);
    
    complex<double> arg1;
   
    for(int i3=0; i3<nCarriers; i3++){
      arg1=complex<double>(0,-1*nuOff);
      aux[i3]=aux[i3]*exp(arg1);
    }
    //DispVal(aux);
    data=itpp::fft(aux, nCarriers);
    //DispVal(data);
      //Inserte data in Processing queue:
   
    OFDM_data.push(data);

   DispVal(i1);
   DispVal(pt);
   
  }
  
  return;
}

/*
int main(){
  
  int data_size=34610;
  
   // Create storage for the input
    complex<double> * data_r;
    data_r= new complex<double>[data_size];

    // Read data from file
    std::ifstream ifs_data( "data_to_harness.dat" , std::ifstream::in );
    ifs_data.read((char * ) data_r,2*data_size*sizeof(double));
    ifs_data.close();
    
    // for(int ii=0;ii<100;ii++){
    //   DispVal(data_r[ii]);
    //   DispVal(ii);
    // };
    

    
  int tIni=59; //Sampling time + length of training sequence upsampled
  
  int Post=1, Pre=18;
  int nCarriers=128;
  int nOFDM_symbols=235;
  
  std::queue<cvec> OFDM_data;
  
 
  cvec dataC=arrayToCvec(data_r, data_size);  
  //std::cout<<"data="<<dataC<<"\n";
  
  CPremove(dataC, tIni, nCarriers, nOFDM_symbols, Pre, Post, OFDM_data);

  //print(OFDM_data);

  return 0;
  }*/

