#include <itpp/itcomm.h>
#include <itpp/itbase.h>
#include <iostream>
//#include <sys/time.h>

#include <ctime>
#include <stdlib.h> 
//#include <ratio>
#include <chrono>


using namespace itpp;

int main(){
  QAM qam; qam.set_M(64);
  Convolutional_Code code;
  ivec generator(2);
  generator(0)=0133;
  generator(1)=0165;
  //generator(2)=0171;
  code.set_generator_polynomials(generator, 7);
  bvec bits=randb(80000), encoded_bits, decoded_bits;
  cvec tx_signal, rx_signal;
  code.encode_tail(bits, encoded_bits);
  tx_signal = qam.modulate_bits(encoded_bits);
  rx_signal = tx_signal + sqrt(0.1)*randn(tx_signal.size());
  vec llr=qam.demodulate_soft_bits(rx_signal, sqrt(0.1));
std::chrono::high_resolution_clock::time_point t_start = std::chrono::high_resolution_clock::now();
  code.decode_tail(llr, decoded_bits);
  std::chrono::high_resolution_clock::time_point t_end = std::chrono::high_resolution_clock::now();
    std::cout << "Data Processed!\n";
    std::cout << "Wall clock time passed: "
              << std::chrono::duration_cast<std::chrono::microseconds>(t_end - t_start).count()
              << " us\n";

  std::cout << code.get_rate() << std::endl;
  //std::cout<< "in:  "<< bits << std::endl;
  //std::cout << "out: " << decoded_bits << std::endl;

  return 0;
}
