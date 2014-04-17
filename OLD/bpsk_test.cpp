//Test of bpsk

#include <itpp/itbase.h>
#include <itpp/itcomm.h> 
#include <itpp/comm/modulator.h>
#include <complex>


using namespace itpp;


int main( int argc, char *argv[])
{

BPSK bpsk;
Convolutional_Code code;
ivec generator(3);
 

int ber;
int i,end;


generator(0)=0133;
generator(1)=0165;
generator(2)=0171;

code.set_generator_polynomials(generator, 2);

 bvec bits=randb(100);



bvec encoded_bits, decoded_bits;

code.encode_tail(bits, encoded_bits);


vec tx_signal, rx_signal;




tx_signal = bpsk.modulate_bits(encoded_bits);



 int N0=4;

 vec noise=sqrt(N0 / 2) * randn(tx_signal.size());


rx_signal = tx_signal+noise; 
 

code.decode_tail(rx_signal, decoded_bits);


 
 
 end=bits.size();
 std::cout <<  bits << std::endl;
 std::cout <<  decoded_bits << std::endl;
 i=0;
 ber=0;
 //std::cout << "ber=" << ber << "\n";

 while(i != end-1){
  
   if(decoded_bits[i] != bits[i]){
     ber=ber+1;
     //std::cout << "ber=" << ber << "\n";
   };
   i++;
 };
 float error=(float)(ber)/end;

 std::cout << "BER=" << error << "\n";

 return 0;

}




