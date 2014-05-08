
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


int main(){
  
  int nBlock=20;

  ivec interleavingA(nBlock);
  interleavingA= "20 19 18 17 16 15 14 13 12 11 10 9 8 7 6 5 4 3 2 1";  
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
  bmat Pmat("1 1 1;1 0 0;0 1 0");
  itpp::LLR_calc_unit llc=  itpp::LLR_calc_unit();
  turbo.set_parameters(gen,gen,const_length,interleavingA,Pmat,8,"LOGMAX",1.0,false,llc);
  dataPreCod=turbo.encode(dataBin);
  Sequence_Interleaver<bin> global_interleaver(dataPreCod.size());
  global_interleaver.randomize_interleaver_sequence();
  bvec dataCoded=global_interleaver.interleave(dataPreCod);


  BPSK md;
  vec tx_signal=md.modulate_bits(dataCoded);

  //Channel
  vec rx_signal= tx_signal + sqrt(1)*randn(dataCoded.size());  

 //Receiver
  turbo.set_awgn_channel_parameters(1,1);
  
  Sequence_Interleaver<double> rxInter(dataPreCod.size());
  rxInter.set_interleaver_sequence(global_interleaver.get_interleaver_sequence());
  vec rx2= rxInter.deinterleave(rx_signal);
  bvec dataDet=turbo.decode(rx2);
  std::cout << dataDet-dataBin << std::endl;

}
