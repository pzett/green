#include <complex>
#include <iostream>
#include <vector>
#include <queue> 
#include <cmath>     
#include <string>

#include <ctime>
#include <chrono>
#include <stdlib.h> 

#include <sys/resource.h>
#include <sys/time.h>

#include <itpp/itbase.h>
#include <itpp/itcomm.h> 
//#include <itpp/stat/misc_stat.h>

#include "arrayToCvec.cpp"

using namespace std;
using namespace itpp;

#define DispVal(X) std::cout << #X << " = " << X<< std::endl

#define M_PI 3.14159265358979323846


/** Modulo function
 * @pre:
 *     - a: input (numerator)
 *     - b: input (denominator)
 *     -
 *
 * @post:
 *     - Done
 */


double modP(double a, double b){
  if (a<0){
    double rem=std::ceil(a/b);
    rem=a-rem*b;
    //std::cout<<"rem "<<rem<<std::endl;
    rem=b+rem;
    return rem;
  }else{
    double rem=std::floor(a/b);
    rem=a-rem*b;
    return rem;
  }
}


/** Return the channel FIR estimation based on trainSeq and fill phiHat, Ahat, delay, sigmaSqrNoise
 *
 * @pre:
 *    - dataC: cvec of the received data
 *    - trainC: pointer to array of the training sequence
 *             !!!  length assume to be < than dataC.length()!!!
 *    - delay: pointer to a int with synchornization point
 *
 * @post:
 *    - Estimators are now filled!
 *    - returned the channel FIR estimation
 */
void synchCatchChannel(cvec dataC, cvec trainCUp , int *delay ){

  if(dataC.length()<trainCUp.length()){
    cerr << "The training sequence is too big compare to datas!\n";
    exit(1);
  }

  //std::cout<<"Data="<<dataC<<"\n";
  //std::cout<<"Train="<<trainCUp<<"\n";
  
  //Computes crosscorrelation:

  int timeLag=1000;

  cvec dataCSmall=dataC(1,timeLag);

  cvec crossCorr=itpp::xcorr(dataCSmall, trainCUp, timeLag);

  
    //std::cout<<"Cross="<<crossCorr<<"\n";

  //Search for the maximum
  double max=0.0;
  int index=0, count=0;
  double tmp[crossCorr.length()-timeLag+1];
  for(int i=timeLag-1;i<crossCorr.length();i++){
    tmp[count]=abs(crossCorr[i]);
   
    //DispVal(tmp[count]);
    //DispVal(count);
    if (tmp[count]> max ){
      max=tmp[count];
      index=i;
     
    }
     count++;
  }
  
  index=index-timeLag+1;
  *delay=index;


  // Save data to file
     std::ofstream ofs( "xcorr.dat" , std::ifstream::out );
     ofs.write((char * ) tmp, (crossCorr.length()-timeLag+1)*sizeof(double));
     ofs.close();

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
    pt=pt+Pre;

    for(int i2=0;i2<nCarriers;i2++){
      aux.set(i2,dataC[i2+pt]);
    }
    pt=pt+nCarriers-Pre;
    
    
    for(int i2=0; i2<Pre ;i2++){
      Pre2.set(i2,dataC[i2+pt]);
    }

    pt=pt+Post+Pre;

    //Process data and store it in the queue:
   
    Pre1conj =itpp::conj(Pre1);
       
    mult= itpp::elem_mult(Pre1conj,Pre2); 	
  
    complex<double> sum=itpp::sum(mult);

    
    double nuOff=(arg(sum))/(2*M_PI*nCarriers);
    
    
    complex<double> arg1;
   
    for(int i3=0; i3<nCarriers; i3++){
      arg1=complex<double>(0,-1*2*M_PI*nuOff*i3);
      aux[i3]=aux[i3]*exp(arg1);
    }
    
    data=itpp::fft(aux);
    
    //Inserte data in Processing queue:
   
    OFDM_data.push(data);

    
  }

 
  return;
}



// Upsampling by a rectangular function of length Q
template<class T>
void hold(T data[], T out[], int Q, int nElem){
  int c=0;
  for (int i=0; i<nElem;i++){
    for (int d=0;d<Q;d++){
    
      out[c+d]=data[i];
      
    }
    c=c+Q;
  }
}



/** Interpolation on the amplitude
 *      - PilotFFT: the amplitude at the different pilots
 *      - pilot_pos[]: The index of the different pilot.
 *      - nPilot: The number of pilot 
 *      - nElem: The number of point in the FFT
 *  EXPL: if one pilot is at 0.25 in normalised freq and nElem=80, pilot_pos=20
 *                           0                                =80           =0
 * 
 */
vec interp(vec pilotFFT, double pilot_pos[], int nPilot, int nElem){
  // Search the highest frequency pilot
  int lPilot=0;
  for (; lPilot<nPilot && pilot_pos[lPilot]< (nElem/2); lPilot++){  
  }
  lPilot--;


  // Create cyclic extension of pilot position
  vec pilotP( pilot_pos, nPilot);
  pilotP.ins(0, pilotP.right(1)-nElem);
  //Create a cyclic extension of pilotFFT
  vec pilotFFTp=pilotFFT; 
  pilotFFTp.ins(0, pilotFFTp.right(1));

  // Evaluate the delta amplitude
  pilotFFT.ins(pilotFFT.length(),pilotFFT.left(1));
  vec pilotFFTd=pilotFFT-pilotFFTp;
  // Create the ouput vector
  vec out(nElem);
  // Fill the higher frequencies
  for (int i=0; i<lPilot+1; i++){//+1 because of the periodisation
    int deltaI=pilotP(i+1)-pilotP(i);
    double step=pilotFFTd(i)/deltaI;
    for (int ii=0;ii<deltaI; ii++ ){
      out.set(modP((pilotP(i)+ii),(double) nElem), pilotFFTp(i)+ii*step);
    }
  }
  // Fill the frequencies around DC
  for(int i=pilotP(lPilot+1);i<nElem/2;i++ ){
    out.set(i, pilotFFTp(lPilot+1));
  }
  for(int i=pilotP(lPilot+2);i>=nElem/2;i-- ){
    out.set(i, pilotFFTp(lPilot+2));
  }
  // Fill the low freq
  for (int i=lPilot+2;i<nPilot; i++){
    int deltaI=pilotP(i+1)-pilotP(i);
    double step=pilotFFTd(i)/deltaI;
    for (int ii=0;ii<deltaI; ii++ ){
      out.set(pilotP(i)+ii, pilotFFTp(i)+ii*step);
    }  
  }
  
  return out;
}

/** IDEM interp but with periodicity of the phase taking into account.
 *
 */
vec interpPhase(vec pilotFFT, double pilot_pos[], int nPilot, int nElem){
  // Search the highest frequency pilot
  int lPilot=0;
  for (; lPilot<nPilot && pilot_pos[lPilot]< (nElem/2); lPilot++){ }
  lPilot--;


  // Create cyclic extension of pilot position
  vec pilotP( pilot_pos, nPilot);
  pilotP.ins(0, pilotP.right(1)-nElem);
  //Create a cyclic extension of pilotFFT
  vec pilotFFTp=pilotFFT; 
  pilotFFTp.ins(0, pilotFFTp.right(1));

  // Evaluate the delta amplitude
  pilotFFT.ins(pilotFFT.length(),pilotFFT.left(1));
  vec pilotFFTd=pilotFFT-pilotFFTp;

  // Create the ouput vector
  vec out(nElem);
  // Fill the higher frequencies
  for (int i=0; i<lPilot+1; i++){//+1 because of the periodisation
    int deltaI=pilotP(i+1)-pilotP(i);// Difference in index
 
    double step=(modP(pilotFFTd(i)+M_PI,2*M_PI)-M_PI)/deltaI;// Difference in phase
  
    for (int ii=0;ii<deltaI; ii++ ){
      out.set(modP((pilotP(i)+ii),(double) nElem), modP(pilotFFTp(i)+ii*step+M_PI,2*M_PI)-M_PI);
    }
  }
  // Fill the frequencies around DC
  for(int i=pilotP(lPilot+1);i<nElem/2;i++ ){
    out.set(i, modP(pilotFFTp(lPilot+1)+M_PI,2*M_PI)-M_PI);
  }
  for(int i=pilotP(lPilot+2);i>=nElem/2;i-- ){
    out.set(i, modP(pilotFFTp(lPilot+2)+M_PI,2*M_PI)-M_PI);
  }
  // Fill the low freq
  for (int i=lPilot+2;i<nPilot; i++){
    int deltaI=pilotP(i+1)-pilotP(i);
    double step=(modP(pilotFFTd(i)+M_PI,2*M_PI)-M_PI)/deltaI;// Difference in phase
    for (int ii=0;ii<deltaI; ii++ ){
      out.set(pilotP(i)+ii,modP(pilotFFTp(i)+ii*step+M_PI,2*M_PI)-M_PI );
    }  
  }
  return out;
}



///////////////////////////////////////MAIN FUNCTION OF THE PROGRAM////////////////////////////////////////////////////////////

void receiverSignalProcessing(short buff_short[], int buffersize,short data_bin[], int nDataB){

  std::cout<<"Running Receiver!\n";

  //Conversion to double -> More precision in processing
  double buff_double[buffersize];
  for(int k=0;k<buffersize;k++){
    buff_double[k] =(double) buff_short[k];
    
  }

 
  //Correcting the residual frequency offset:
 
  cvec buff_cvec = arrayToCvec(buff_double,buffersize);
  double freq_residual=0.01;
  
 
  for(int ii=0; ii<buff_cvec.length();ii++){
    buff_cvec[ii]=buff_cvec[ii]*std::exp(complex<double>(0,2*M_PI*(ii+1)*freq_residual));
  }


  // Loading the training sequence
  int nTrainSeq=12;
  complex<double> * trainSeq;
  trainSeq= new complex<double>[nTrainSeq];
  std::ifstream ifs( "dataTrain.bin" , std::ifstream::in );
  if(ifs.is_open()){
    ifs.read((char * )trainSeq,2*nTrainSeq*sizeof(double));
    ifs.close();
  }else{
    std::cerr<<"Error loading data!\n";
    exit(1);
  } 

  //Upsampling training sequence:
  int Q=2;
  std::complex<double> trainSeqUp[Q*nTrainSeq];
  hold(trainSeq, trainSeqUp, Q, nTrainSeq);


  // synchronization and channel coarse estimation:
  cvec trainC=arrayToCvec(trainSeqUp, Q*nTrainSeq); 
  
  int delay=0;

  synchCatchChannel(buff_cvec, trainC, &delay);  //Catch the start of transmission


  //CP removal of OFDM symbols and FFT to get OFDM data:
  int tIni=delay+Q*nTrainSeq+4*46; //Beginnig of OFDM data -> Use 4 OFDM symbols for protection 
  int PostFix=4, PreFix=10;
  int nCarriers=32;
  int nSymbolsOFDM=800;
    
  std::queue<cvec> dataOFDM;


  CPremove(buff_cvec, tIni, nCarriers, nSymbolsOFDM, PreFix, PostFix, dataOFDM);
 

  //Pick data corresponding to the different pattern
     
  //Read Pilot Patern from file: 
  int nUsedPilot=6;
  double * pilotPattern;
  pilotPattern= new double[nUsedPilot];
  std::ifstream ifs1( "pilotPattern.dat" , std::ifstream::in );
  if(ifs1.is_open()){
    ifs1.read((char * )pilotPattern, nUsedPilot*sizeof(double));
    ifs1.close();
  }else{
    std::cerr<<"Error loading data!\n";
    exit(1);
  }

  //Read data Patern from file: 
  int nUsedCarrier=17;
  double * dataPattern;
  dataPattern= new double[nUsedCarrier];
  std::ifstream ifs2( "dataPattern.dat" , std::ifstream::in );
  if(ifs2.is_open()){
    ifs2.read((char * )dataPattern, nUsedCarrier*sizeof(double));
    ifs2.close();
  }else{
    std::cerr<<"Error loading data!\n";
    exit(1);
  }


  //Read Pilots from file: 
  int nPilotData=nSymbolsOFDM*nUsedPilot;
  complex<double> * dataPilot;
  dataPilot= new complex<double>[nPilotData];
  std::ifstream ifs3( "dataPilotN.dat" , std::ifstream::in );
  if(ifs3.is_open()){
    ifs3.read((char * )dataPilot, 2*nPilotData*sizeof(double));
    ifs3.close();
  }else{
    std::cerr<<"Error loading data!\n";
    exit(1);
  }


 
  //Take OFDM data and Pilots:
  
  itpp::cvec dataAux(nUsedCarrier);
  itpp::vec gainAux(nUsedPilot);
  itpp::vec phaseAux(nUsedPilot);
  int countPilot=0;

  itpp::vec gainInterp(nUsedCarrier), phaseInterp(nUsedCarrier);

  double pilotPat[nUsedPilot];

  for(int i=0;i<nUsedPilot;i++)pilotPat[i]=pilotPattern[i]-1;

  complex<double> correct, phaseCorrect;

  complex<double> DataFilt[nUsedCarrier*nSymbolsOFDM]; 

  int counter=0;

  for(int i1=0;i1<nSymbolsOFDM;i1++){
    itpp::cvec aux=dataOFDM.front();
    dataOFDM.pop();
    int m=0,n=0;
    double gain, phase;
    for(int i2=0;i2<nCarriers;i2++){
      if (pilotPattern[m]-1==i2){	
	gain=std::abs(aux[i2]/dataPilot[countPilot]);
	phase=std::arg(aux[i2]/dataPilot[countPilot]);
	gainAux.set(m, gain);
	phaseAux.set(m, phase);
	m++;
	countPilot++;
      }
      if (dataPattern[n]-1==i2){
	dataAux.set(n, aux[i2]);   
	n++;
      }
     
    }
    
     
    gainInterp=interp( gainAux, pilotPat,nUsedPilot, nCarriers);
    phaseInterp=interpPhase(phaseAux, pilotPat, nUsedPilot, nCarriers);
     
     //Correction of channel impulse responce:
     n=0;
     for(int i2=0; i2<nCarriers; i2++){
       
       if (dataPattern[n]-1==i2){
	
	 phaseCorrect=std::exp(complex<double>(0,phaseInterp[i2]));
	 correct=dataAux[n]/(gainInterp[i2]*phaseCorrect);
	 

	DataFilt[counter]=correct;
        counter++;
	 
	n++;
       }        
     }
  }
  


  //Detection

  cvec toDemo=arrayToCvec(DataFilt, nDataB);

  QAM qam(4);
  bvec bits=qam.demodulate_bits(toDemo);
   
  //for(int i=0; i<nDataB; i++)DispVal(data_bin[i]);
  for(int i=0; i<nDataB;i++){
    data_bin[i]=(short)bits[i];
  }



    //Read Data from file: 
 
  double * dataTx;
  dataTx= new double[nDataB];
  std::ifstream ifs5( "dataBintX.dat" , std::ifstream::in );
  if(ifs5.is_open()){
    ifs5.read((char * )dataTx, nDataB*sizeof(double));
    ifs5.close();
  }else{
    std::cerr<<"Error loading data!\n";
    exit(1);
  }

   double ERROR=0.0;
   for(int i=0;i<nDataB;i++){
     if(data_bin[i]!=dataTx[i]){ ERROR++;}
   }
   std::cout<<"BER="<<ERROR/(nDataB)<<std::endl;
  
   std::cout<<"Rate="<<(double)((1-2*ERROR/nDataB)*(nDataB)*25e3)/(buffersize/2)<<std::endl;
  

  std::cout << " " << std::endl;
  std::cout << " Processing Done! " << std::endl;

  return;

}




