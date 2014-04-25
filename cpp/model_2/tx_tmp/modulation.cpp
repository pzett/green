#include <iostream>
#include <fstream>
#include <complex>
#include <cmath>



using namespace std;

/** Transfer the information bits to a QPSK constellation
 * @pre:
 *     - data_bin: an array of 0 and 1
 *     - data_qpsk: complex array of double. output. Length nElem 
 *     - nElem: length of data_bin
 *
 * @post:
 *     - data_qpsk is filled with -1/sqrt(2) and 1/sqrt(2) according to
 *
 * complex part (Q channel)
 *         ^
 *         |
 *  10 x   |   x 00   (odd bit, even bit)
 *         |
 *  -------+------->  real part (I channel)
 *         |
 *  11 x   |   x 01
 *         |
 */
void qpsk(double data_bin[], double data_qpsk[], int nElem){
  for (int i=0;i<nElem;i=i+2){
    if( round(data_bin[i])==0){ 
      // 00 mapped to 1+j
      if (round( data_bin[i+1])==0){data_qpsk[i]=1/1.4142 ; data_qpsk[i+1]=1/1.4142;}
      // 01 mapped to 1-j
      else if( round(data_bin[i+1])==1){data_qpsk[i]=1/1.4142 ; data_qpsk[i+1]=-1/1.4142;}
      else {cerr << "The input data_bin is not composed of 01"<<"\n";}
    } 
    else if( round(data_bin[i])==1){
      // 10 mapped to -1+j
      if ( round(data_bin[i+1])==0){data_qpsk[i]=-1/1.4142 ; data_qpsk[i+1]=1/1.4142;}
      // 11 mapped to -1-j
      else if(round(data_bin[i+1])==1){data_qpsk[i]=-1/1.4142 ; data_qpsk[i+1]=-1/1.4142;}
      else {cerr << "The input data_bin is not composed of 01"<<"\n";;}
    }
    else {cerr << "The input data_bin is not composed of 01"<<"\n";;}
  }
}
/** Transfer the information bits to a BPSK constellation
 * @pre:
 *     - data_bin: an array of 0 and 1
 *     - data_bpsk: complex array of double. output. Length nElem 
 *     - nElem: length of data_bin
 *
 * @post:
 *     - data_bpsk is filled with -1 and 1 according to
 *
 * complex part (Q channel)
 *         ^
 *         |
 *         |   
 *    1    |    -1
 *  --x----+-----x-->  real part (I channel)
 *         |
 *         |  
 *         |
 */

void bpsk(double data_bin[], double data_bpsk[], int nElem){
  for (int i=0;i<nElem;i++){
    if(round(data_bin[i])==0){
      // 0 mapped to 1
      data_bpsk[i]=1;
    }
    else if(round(data_bin[i])==1){
      // 1 mapped to -1
      data_bpsk[i]=-1;
    }
    else{cerr << "The input data data_bin is not binary" << std::endl;} 
  }
}

/** Transfer the information bits to a general M-QAM  constellation
 * @pre:
 *     - data_bin: an array of 0 and 1
 *     - data_16qam: complex array of double. output. Length nElem 
 *     - nElem: length of data_bin
 *
 * @post:
 *     - data_mqam is filled with -1/sqrt(2) and 1/sqrt(2) according toqam sympols
 *
 * complex part (Q channel)
 *
 *           ^
 *  x    x   |   x    x
 *           |   
 *  x    x   |   x    x
 *  ---------+---------->  real part (I channel)
 *  x    x   |   x    x
 *           |  
 *  x    x   |   x    x
 *
 */

void mqam(double data_bin[],std::complex<double> data_mqam[], int nElem, int nBitsPerAxis){
  int counter = 0;
  int nBits = nBitsPerAxis-1;
  for(int i=0;i<nElem;i=i+2*nBits){
    double summation1 = 0.0;
    double summation2 = 0.0;
    for(int k=1;k<=nBits;k++){
      double sumBits1 = 0.0;
      double sumBits2 = 0.0;
      for(int j=0;j<k;j++){sumBits1 = sumBits1 + data_bin[i+j];sumBits2 = sumBits2 + data_bin[i+j+nBits];};//std::cout << j << std::endl;};
      summation1 = summation1 + pow(2.0,nBits-k)*pow(-1.0,sumBits1);
      summation2 = summation2 + pow(2.0,nBits-k)*pow(-1.0,sumBits2);
      data_mqam[counter].real(summation1);
      data_mqam[counter].imag(summation2); 
    //std::cout << pow(-1.0,sumBits) << std::endl;
    //std::cout << sumBits2 << std::endl;
    }
    counter++;
  }
  //for(int m=0;m<50;m++){std::cout << data_mqam[m].real() << ", " << data_mqam[m].imag() << std::endl;};
  // std::cout << summation1 << std::endl;
  // std::cout << summation2 << std::endl;
  //std::cout << "( " << data_mqam[2].real() << ", " << data_mqam[1].imag() << " )" << std::endl;
  
}


/** Transfer the information bits to a general 16-QAM  constellation
 * @pre:
 *     - data_bin: an array of 0 and 1
 *     - data_16qam: complex array of double. output. Length nElem 
 *     - nElem: length of data_bin
 *
 * @post:
 *     - data_16qam is filled with -1/sqrt(2) and 1/sqrt(2) according to
 *
 * complex part (Q channel)
 *
 *  1000 1100^   0100 0000
 *  x    x   |   x    x
 *  1001 1101|   0101 0001
 *  x    x   |   x    x
 *  ---------+---------->  real part (I channel)
 *  x    x   |   x    x
 *  1011 1111|   0111 0011
 *  x    x   |   x    x
 *  1010 1110|   0110 0010
 */

void sixteenQam(double data_bin[],std::complex<double> data_16qam[], int nElem){
  int counter = 0;
  for(int i=0;i<nElem;i=i+4){
    if(data_bin[i]==0 && data_bin[i+1]==0){
      data_16qam[counter].real(3);
       }
    else if(data_bin[i]==0 && data_bin[i+1]==1){
      data_16qam[counter].real(1);
       }
    else if(data_bin[i]==1 && data_bin[i+1]==1){
      data_16qam[counter].real(-1);
       }
    else if(data_bin[i]==1 && data_bin[i+1]==0){
      data_16qam[counter].real(-3);
       }
    if(data_bin[i+2]==0 && data_bin[i+3]==0){
      data_16qam[counter].imag(3);
       }
    else if(data_bin[i+2]==0 && data_bin[i+3]==1){
      data_16qam[counter].imag(1);
       }
    else if(data_bin[i+2]==1 && data_bin[i+3]==1){
      data_16qam[counter].imag(-1);
       }
    else if(data_bin[i+2]==1 && data_bin[i+3]==0){
      data_16qam[counter].imag(-3);
       }
    counter++;
  }
}






// possible modulations are:
// 
// BPSK, QPSK, 16QAM, M-QAM
//

int main(){

  int modulation = 16; // refers to bpsk, 4 refers to qpsk ...

  int nElem = 6000;
  int nBitsPerAxis = 4;
  double data_bin[nElem];
  std::complex<double> data_mod[nElem/(2*nBitsPerAxis)];
  // for(int i=0;i<nElem;i++){data_bin[i]=1;};

 // Read data from file
    std::ifstream ifs_data( "data_to_harness.dat" , std::ifstream::in );
    ifs_data.read((char * ) data_bin, nElem*sizeof(double));
    ifs_data.close();

    //for(int i=0;i<nElem;i++){std::cout << data_bin[i] << std::endl;};

  if (modulation==2){
    std::cout << "BPSK" << std::endl;
    double data_mod[nElem];
    bpsk(data_bin,data_mod,nElem);
  }
  else if (modulation==4){
    std::cout << "QPSK" << std::endl;
    double data_mod[nElem/2];
    qpsk(data_bin,data_mod,nElem);
  }
  else if (nBitsPerAxis==4){
    std::cout << "M-QAM" << std::endl;
    mqam(data_bin,data_mod,nElem,nBitsPerAxis);
  }
  else{
    std::cout << "No known modulation type used" << std::endl;
  }


  // // Verifying the modulation
  // double data_modTest[2000];
  // // Read data from file
  // std::ifstream ifs_data2( "data_mod_to_harness.dat" , std::ifstream::in );
  // ifs_data2.read((char * ) data_modTest, (2000)*sizeof(double));
  // ifs_data2.close();
  // int faultCounter = 0;
  // int count = 0;
  // for(int i=0;i<2000;i=i+2){
  //   if(data_modTest[i]!=data_mod[count].real()){faultCounter++;};
  //   if(data_modTest[i+1]!=data_mod[count].imag()){faultCounter++;};
  //   count++;
  //   //std::cout << "(" << data_modTest[i] << ", " << data_modTest[i+1] << ")" << std::endl;
  // }
  // std::cout << "Faults: " << faultCounter << std::endl;
    
  return 0;
}
