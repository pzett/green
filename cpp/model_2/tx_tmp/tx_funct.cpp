 
 #include <algorithm>
 #include <iostream>
 #include <fstream>
 #include <cmath>
 #include <itpp/itbase.h>
 #include <itpp/itsignal.h>


 /*
 This function prepares a vector of NCarriers to be transmitted with OFDM.
 */
using namespace std;
using namespace itpp;



cvec dataAlloc (std::complex<double> data_qam[], std::complex<double> data_pilot[], double pilot_pattern[], double data_pattern[], int N){

  int n=0;
  int m=0;
  std::complex<double> zeroC(0,0);
  cvec data_alloc(N);

  for(int i=0;i<N;i++){;
   if (pilot_pattern[m]-1==i){
     data_alloc.set(i,data_pilot[m]);   
      m++;
   }
   else if (data_pattern[n]-1==i){
     data_alloc.set(i,data_qam[n]);   
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
   data_time.ins(data_time.length(), data_time.mid(pre,post));
   return(data_time);

}







cvec serial2serial (std::complex<double> data_qam[],std::complex<double> data_pil[],double data_pattern[], double pilot_pattern[],int N, int nUsedC, int nPilotC, int pre, int post){

    cvec data_alloc=dataAlloc(data_qam,data_pil,pilot_pattern,data_pattern,N);
      std::cout << "Carrier allocation done" << std::endl;
      /*for(int i=0;i<data_alloc.length();i++){
      std::cout << data_alloc.get(i) << std::endl;
      }*/

      // cvec data_time=itpp::ifft(data_alloc,N);//IFFT
    cvec out=prePost(data_alloc,pre,post);

      std::cout << "Prefix and postfix added" << std::endl;
      /*for(int i=0;i<out.length();i++){
      std::cout << out.get(i) << std::endl;
      }*/

      return(out);
 }



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
}





int main(){

  /*Parameters:
    OFDM parameters
    nElements in the data
   */
  int N=128;
  int nUsedC=112;
  int nPilotC=16;
  int pre=18;
  int post=2;
  int nDataBin=100000;
  int nPilotData=100000;
  double shorBuffer[nUsedC];
  cvec outBuffer(0);
 

  //Load data and initialization parameters
    //Read data from a file
    double data_bin[nDataBin];
    std::ifstream ifs( "dataBinNPaq.dat" , std::ifstream::in );
    ifs.read((char * )data_bin,nDataBin*sizeof(double));
    ifs.close();

    //Read pilots from a file;
    double data_pil[nPilotData];
    std::ifstream ifs2( "dataPilotNPaq.dat" , std::ifstream::in );
    ifs2.read((char * )data_pil,nPilotData*sizeof(double));
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
    
    //Check results reading
      /*std::cout << "Check results of the read data" << std::endl;
      for(int i=0;i<nDataBin;i++){
      std::cout << data_bin[i] << std::endl;
      }*/

  //M-QAM mapping
     int nBitsAxis=2;
     int nQAM=nDataBin/nBitsAxis; 
     std::cout << "Number of symbols" <<nQAM <<std::endl;
     std::complex<double> dataMapped[nQAM];
     mqam(data_bin,dataMapped, nDataBin, nBitsAxis);
     std::complex<double> *pilotComp=(std::complex<double> *) data_pil;
     //Check modulation
     /*std::cout << "Check results of the modulation" << std::endl;
     for(int i=0;i<nQAM;i++){
       std::cout << dataMapped[i] << std::endl;
     }*/

  //Serial2Serial
     std::complex<double> shortBuffer[nUsedC];
     std::complex<double> shortPilot[nPilotC];
     for(int i=0;i<nQAM;){
       for(int j=0;j<nUsedC;j++){
	 if(i>=nQAM){
	   shortBuffer[j]=0;//Zero padding --> need of random sequence instead of zeros;
	   i++;  
	 }         
	 else {shortBuffer[j]=dataMapped[i++];}
       }
       for(int j=0;j<nPilotC;j++){
	 shortPilot[j]=pilotComp[(i/nUsedC-1)*nPilotC+j];
       }

    //Check results
       /*std::cout << "Check results of the first shortBuffer" << std::endl;
       for(int i=0;i<nUsedC;i++){
	 std::cout << shortBuffer[i] << std::endl;
       }*/
       
       cvec shortFrame=serial2serial(shortBuffer, shortPilot,data_pattern,pilot_pattern,N, nUsedC, nPilotC,pre,post);
       outBuffer.ins(outBuffer.length(),shortFrame);

    //Check results
       std::cout << "Length of the Output buffer = " << outBuffer.length()  <<std::endl;
       /*std::cout << "Check results of the first outBuffer" << std::endl;
       for(int i=0;i<outBuffer.length();i++){
	 std::cout << outBuffer.get(i) << std::endl;
       }*/
    }

  //Training sequence + upsample(and store it in a vec)
    int amp=1;
    int nTrain=20;
    int Q=4;
    double train[2*nTrain];
    cvec cvecTrain(0);
    
    std::ifstream ifs5( "dataTrain.dat" , std::ifstream::in );
    ifs5.read((char * )train,2*nTrain*sizeof(double));
    ifs5.close();
    std::complex<double> *trainTmp=(std::complex<double> *) &train[0];

    //Check results
    /* std::cout << "Check results of the training sequence" << std::endl;
     for(int i=0;i<nTrain;i++){
     std::cout << trainTmp[i] << std::endl;
     }*/

    for(int m=0;m<nTrain;m++){
      for(int n=0;n<Q;n++){
      cvecTrain.ins(cvecTrain.length(),trainTmp[m]);
      }
    }
    outBuffer.ins(0,cvecTrain);
    //Check results
    std::cout << "Length of the Output buffer with TS = " << outBuffer.length()  <<std::endl;
    /*std::cout <<"Check results of adding TS at the whole buffer" << std::endl;
    for(int i=0;i<outBuffer.length();i++){
      std::cout << outBuffer.get(i) << std::endl;
    } */

  //Insert guard bits & convert to short
    int nGuard=10;
    short output[outBuffer.length()+nGuard];
    fill_n(&output[0],nGuard,0);
    for(int j=0;j<2*outBuffer.length();j++){
      std::complex<double> tmp= outBuffer.get(floor(j/2));
      output[j+nGuard]=(short) std::real(tmp);
      output[++j+nGuard]=(short) std::imag(tmp);
    }

    std::ofstream ofs( "receivedN.dat" , std::ifstream::out );
    ofs.write((char * ) output, (2*outBuffer.length()+nGuard)*sizeof(short));
    ofs.close();

    //Check results
    std::cout << "Length of the last frame = " << 2*outBuffer.length()+nGuard  <<std::endl;
    /*std::cout << "Here comes the complete frame" << std::endl;
    for(int i=0;i<2*outBuffer.length()+nGuard;i++){
     std::cout << output[i] << std::endl;
     }*/
    return(0);
}
