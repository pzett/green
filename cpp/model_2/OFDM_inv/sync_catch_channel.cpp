#include <iostream>
#include <complex>
#include <vector>

#include <itpp/itbase.h>
#include <itpp/itcomm.h> 
#include <itpp/stat/misc_stat.h>

#include <queue>         

#include "arrayToCvec.cpp"

using namespace std;
using namespace itpp;
#define DispVal(X) std::cout << #X << " = " << X<< std::endl


/** 

Receives data ->synchronizes and take OFDM symbols to a queue;

Also uses the CP to estimate frequency offset

 */
void CPremove(cvec dataC, int tIni, int nCarriers, int nOFDM_symbols, int Pre, int Post, std::queue<cvec> &OFDM_data){


  int pt=tIni;
  cvec aux(nCarriers), Pre1(Pre), Pre2(Post);
  //cycle to Remove CP:
  for(int i1=0;i1<nOFDM_symbols;i1++){

    //cycle to get Prefix, OFDM sysmbol and Post fix:
    for(int i2=0; i2<Pre; i2++){
      Pre1.set(i2,dataC[pt+i2]);
    }
    std::cout<<"Pre1="<<Pre1<<"\n";

    pt=pt+Pre;

    for(int i2=0;i2<nCarriers;i2++){
      aux.set(i2,dataC[i2+pt]);
    }
    std::cout<<"aux="<<aux<<"\n";

    pt=pt+nCarriers-Pre;
    
    
    for(int i2=0; i2<Pre ;i2++){
      Pre2.set(i2,dataC[i2+pt]);
    }
    std::cout<<"Pre2="<<Pre2<<"\n";

    pt=pt+Post+Pre;

    //Process data and store it in the queue:
    double nuOff;
    Pre1=itpp::conj(Pre1);
    //DispVal(Pre1);
    cvec mult= itpp::elem_mult(Pre1,Pre2); 	
  
    complex<double> sum=itpp::sum(mult);
    nuOff=arg(sum);
    DispVal(nuOff);
    
    complex<double> arg1;
    for(int i3=0; i3<nCarriers; i3++){
      arg1=complex<double>(-1*nuOff);
      aux[i3]=aux[i3]*exp(arg1);
    }
    
    cvec data=itpp::fft(aux, nCarriers);
    DispVal(data);
      //Inserte data in Processing queue:
   
    OFDM_data.push(data);

  }
  
 
    return ;
}


int main(){
  
  int nElem=13;
  double data[]={0,0,0,0,0,0,1,1,0,0,0,0,1,1,2,2,1,1,0,0,3,3,1,1,2,2};
  int nIni=3;
  
  int Post=1, Pre=1;
  int nCarriers=3;
  int nOFDM_symbols=2;
  int tIni=3;
  
  std::queue<cvec>  OFDM_data;
  
 
  cvec dataC=arrayToCvec(data, 2*nElem);  
  std::cout<<"data="<<dataC<<"\n";
  
  CPremove(dataC, tIni, nCarriers, nOFDM_symbols, Pre, Post, OFDM_data);

  cvec rec=OFDM_data.front();
  DispVal(rec);
  OFDM_data.pop();

  rec=OFDM_data.front();
  DispVal(rec);
  OFDM_data.pop();


  return 0;
}

