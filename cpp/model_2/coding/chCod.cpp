
#include <algorithm>
#include <iostream>
#include <fstream>
#include <cmath>
#include <itpp/base/vec.h>
#include <itpp/itbase.h>
#include <itpp/itsignal.h>
#include <complex>
#include <itpp/comm/modulator.h>
#include <itpp/comm/turbo.h>
#include <itpp/comm/interleave.h>
#include <itpp/itcomm.h>
#include <itpp/comm/rec_syst_conv_code.h>
#include <itpp/comm/llr.h>
#include <itpp/itcomm.h>
#include <itpp/signal/transforms.h>


using namespace itpp;

#define DispVal(X) std::cout << #X << " = " << X<< std::endl

#define M_PI 3.14159265358979323846

int main(){
  

  //load interleaver:
 int nBlock=71188;
  
  double * interleavingAC;
  interleavingAC= new double[nBlock];
  std::ifstream ifs( "interleaver.dat" , std::ifstream::in );
  if(ifs.is_open()){
    ifs.read((char * )interleavingAC,nBlock*sizeof(double));
    ifs.close();
  }else{
    std::cerr<<"Error loading interleaver!\n";
    exit(1);
  }

  //Convert to ivec:
  ivec interleavingA(nBlock);
  for(int i=0; i<nBlock;i++){
    interleavingA.set(i,(int)interleavingAC[i]);
  }

  DispVal(interleavingA);

  //interleavingA= "20 19 18 17 16 15 14 13 12 11 10 9 8 7 6 5 4 3 2 1";  
  //  ivec interleavingB(nBlock2);
  
  bvec dataBin(nBlock);
  dataBin = randb(nBlock);
    std::cout << "- reading - check!" << std::endl;

  //Channel Coding
  bvec dataPreCod;
  Punctured_Turbo_Codec turbo;
  ivec gen(2);
  int const_length=4;
  gen(0)=023;
  gen(1)=013;
  bmat Pmat("1 1 1 1 1 1 1 1;1 0 0 0 0 0 0;0 0 0 0 1 0 0 0");


  
  itpp::LLR_calc_unit llc=  itpp::LLR_calc_unit();
  turbo.set_parameters(gen,gen,const_length,interleavingA,Pmat,8,"LOGMAX",1.0,false,llc);
  dataPreCod=turbo.encode(dataBin);
  Sequence_Interleaver<bin> global_interleaver(dataPreCod.size());
 
  int rate=turbo.get_rate();
  int nBlockG=88992;


  DispVal(nBlockG);
  
  double * interleavingGC;
  interleavingGC= new double[nBlockG];
  std::ifstream ifs1( "interleaverGlobal.dat" , std::ifstream::in );
  if(ifs1.is_open()){
    ifs1.read((char * )interleavingGC,nBlockG*sizeof(double));
    ifs1.close();
  }else{
    std::cerr<<"Error loading interleaver!\n";
    exit(1);
  }
  std::cout << " - check!" << std::endl;
  //Convert to ivec:
  ivec interleavingG(nBlockG);
  for(int i=0; i<nBlockG;i++){
    interleavingG.set(i,(int)interleavingGC[i]);
  }

  //DispVal(interleavingG);

  global_interleaver.set_interleaver_sequence(interleavingG);
 
  bvec dataCoded=global_interleaver.interleave(dataPreCod);
  DispVal(dataPreCod.length());
  std::cout << " - check intG!" << std::endl;

  //DispVal(dataCoded);

  QAM md(16);
  cvec tx_signal=md.modulate_bits(dataCoded);
  DispVal(tx_signal.length());
  std::cout << " - check mod!" << std::endl;
  

  double sigma_sqr=0.01;

  

  //Channel
  cvec rx_signal= tx_signal +sqrt(sigma_sqr)*randn_c(dataCoded.size());  

  std::cout << "- channel - check!" << std::endl;
 //Receiver
  turbo.set_awgn_channel_parameters(1,1);
  
  Sequence_Interleaver<double> rxInter(dataPreCod.size());
  rxInter.set_interleaver_sequence(interleavingG);
  
  
  vec rx_LLR=md.demodulate_soft_bits(rx_signal,sigma_sqr,LOGMAP);

  
  DispVal(rx_LLR);
  vec rx2= rxInter.deinterleave(rx_LLR);
  //DispVal(rx2.length());
  bvec dataDet=turbo.decode(rx2);
  //DispVal(dataDet);
  bvec error=(dataDet-dataBin);
  std::cout << error << std::endl;

}
