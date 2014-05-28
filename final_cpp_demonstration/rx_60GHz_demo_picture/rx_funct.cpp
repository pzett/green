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

#define M_PI 3.141592653589793238461



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
    double nuOff=arg(sum)/(2*M_PI*nCarriers);
    //DispVal(nuOff);
    
    complex<double> arg1;
   
    for(int i3=0; i3<nCarriers; i3++){
      arg1=complex<double>(0,-1*2*M_PI*nuOff*i3);
      aux[i3]=aux[i3]*exp(arg1);
    }
    //DispVal(aux);
    data=itpp::fft(aux);
    //DispVal(data);
      //Inserte data in Processing queue:
    

    OFDM_data.push(data);

    //DispVal(i1);
    //DispVal(pt);
   
  }

 
  return;
}


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
  for (; lPilot<nPilot && pilot_pos[lPilot]< (nElem/2); lPilot++){  //std::cout << pilot_pos[lPilot]<< std::endl;
}
  lPilot--;


  // Create cyclic extension of pilot position
  vec pilotP( pilot_pos, nPilot);
  pilotP.ins(0, pilotP.right(1)-nElem);
  //std::cout << pilotP << std::endl;
  //Create a cyclic extension of pilotFFT
  vec pilotFFTp=pilotFFT; 
  pilotFFTp.ins(0, pilotFFTp.right(1));

  // Evaluate the delta amplitude
  pilotFFT.ins(pilotFFT.length(),pilotFFT.left(1));
  //std::cout << pilotFFT << std::endl;
  //std::cout << pilotFFTp << std::endl;
  vec pilotFFTd=pilotFFT-pilotFFTp;
  //std::cout << pilotFFTd << std::endl;

  // Create the ouput vector
  vec out(nElem);
  // Fill the higher frequencies
  for (int i=0; i<lPilot+1; i++){//+1 because of the periodisation
    int deltaI=pilotP(i+1)-pilotP(i);
    //std::cout <<i<<  " delta " <<deltaI << std::endl;
    double step=pilotFFTd(i)/deltaI;
    //std::cout << step << std::endl;
    for (int ii=0;ii<deltaI; ii++ ){
      out.set(modP((pilotP(i)+ii),(double) nElem), pilotFFTp(i)+ii*step);
      //std::cout << out << std::endl;
    }
  }
  //std::cout << out<<std::endl;
  // Fill the frequencies around DC
  for(int i=pilotP(lPilot+1);i<nElem/2;i++ ){
    out.set(i, pilotFFTp(lPilot+1));
    //std::cout << out<<std::endl;
  }
  for(int i=pilotP(lPilot+2);i>=nElem/2;i-- ){
    out.set(i, pilotFFTp(lPilot+2));
    //std::cout << out<<std::endl;
  }
  // Fill the low freq
  for (int i=lPilot+2;i<nPilot; i++){
    int deltaI=pilotP(i+1)-pilotP(i);
    //std::cout <<i<<  " delta " <<deltaI << std::endl;
    double step=pilotFFTd(i)/deltaI;
    //std::cout << step << std::endl;
    for (int ii=0;ii<deltaI; ii++ ){
      out.set(pilotP(i)+ii, pilotFFTp(i)+ii*step);
      //std::cout << out<<std::endl;
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
  //std::cout << pilotP << std::endl;
  //Create a cyclic extension of pilotFFT
  vec pilotFFTp=pilotFFT; 
  pilotFFTp.ins(0, pilotFFTp.right(1));

  // Evaluate the delta amplitude
  pilotFFT.ins(pilotFFT.length(),pilotFFT.left(1));
  //std::cout << pilotFFT << std::endl;
  //std::cout << pilotFFTp << std::endl;
  vec pilotFFTd=pilotFFT-pilotFFTp;
  //std::cout << pilotFFTd << std::endl;

  // Create the ouput vector
  vec out(nElem);
  // Fill the higher frequencies
  for (int i=0; i<lPilot+1; i++){//+1 because of the periodisation
    int deltaI=pilotP(i+1)-pilotP(i);// Difference in index
    //std::cout <<i<<  " delta " <<deltaI << std::endl;
 
    double step=(modP(pilotFFTd(i)+M_PI,2*M_PI)-M_PI)/deltaI;// Difference in phase
  
    //std::cout << step << std::endl;
    for (int ii=0;ii<deltaI; ii++ ){
      out.set(modP((pilotP(i)+ii),(double) nElem), modP(pilotFFTp(i)+ii*step+M_PI,2*M_PI)-M_PI);
      //std::cout << out << std::endl;
    }
  }
  //std::cout << out<<std::endl;
  // Fill the frequencies around DC
  for(int i=pilotP(lPilot+1);i<nElem/2;i++ ){
    out.set(i, modP(pilotFFTp(lPilot+1)+M_PI,2*M_PI)-M_PI);
    //std::cout << out<<std::endl;
  }
  for(int i=pilotP(lPilot+2);i>=nElem/2;i-- ){
    out.set(i, modP(pilotFFTp(lPilot+2)+M_PI,2*M_PI)-M_PI);
    //std::cout << out<<std::endl;
  }
  // Fill the low freq
  for (int i=lPilot+2;i<nPilot; i++){
    int deltaI=pilotP(i+1)-pilotP(i);
    //std::cout <<i<<  " delta " <<deltaI << std::endl;
    double step=(modP(pilotFFTd(i)+M_PI,2*M_PI)-M_PI)/deltaI;// Difference in phase
    //std::cout << step << std::endl;
    for (int ii=0;ii<deltaI; ii++ ){
      out.set(pilotP(i)+ii,modP(pilotFFTp(i)+ii*step+M_PI,2*M_PI)-M_PI );
      //std::cout << out<<std::endl;
    }  
  }
  return out;
}


/** Function that takes each packet, processes it and gives the binary data in it 
 *
 * @pre:
 *    - dataCvec: vector with received packet data
 *    - tIni: First point of the the OFDM symbols
 *    - nDataB
 *
 * @post:
 *    - Data bin recovered
 */


itpp::bvec ProcessData(cvec dataCvec, int tIni, int nDataB){  
 //CP removal of OFDM symbols and FFT to get OFDM data:
    
    //DispVal(tIni);
    int PostFix=4, PreFix=10;
    int nCarriers=32;
    int nSymbolsOFDM=800;
    
    std::queue<cvec> dataOFDM;

    CPremove(dataCvec, tIni, nCarriers, nSymbolsOFDM, PreFix, PostFix, dataOFDM);
    

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


 
 
  //Take OFDM data and Pilots, equalize and do hard detection:
  std::queue<itpp::cvec> constOFDM;

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


  // int tmpSize=2*nUsedCarrier*nSymbolsOFDM;
  // double tmp[2*nUsedCarrier*nSymbolsOFDM];
  // int count=0;
  // for(int i=0;i<2*nUsedCarrier*nSymbolsOFDM;i++){
  // 	//DispVal(count);
  // 	tmp[count]=DataFilt[i].real();
  // 	tmp[count+1]=DataFilt[i].imag();
	
  // 	  count=count+2;
  //  }
  
  // std::ofstream ofs( "constOFDM.dat" , std::ifstream::out );
  //    ofs.write((char * ) tmp, tmpSize*sizeof(double));
  //    ofs.close();


    cvec toDemo=arrayToCvec(DataFilt, nUsedCarrier*nSymbolsOFDM);

    QAM qam(4);
    itpp::bvec bits=qam.demodulate_bits(toDemo);
    it_file dataFile("Data/bits.it");
  dataFile << Name("bits") << toDemo;
  dataFile.flush();
  dataFile.close();


   return bits;

}

void displayPic(bvec decode){
  bvec pseudoNoise;
  it_file my_file("pseudoNoise.it");
  my_file >> pseudoNoise;
  my_file.flush();
  my_file.close();

  bvec picBin= pseudoNoise-decode;

  ivec picDec;
  for(int i=0;i<picBin.length(); i=i+8 ){
    //cout<< picBin.get(i,i+8)<< endl;
    picDec.ins(picDec.length(),bin2dec(picBin.get(i,i+7)));
  }
  it_file picFile("picture.it");
  picFile << Name("myPicture") << picDec;
  picFile.flush();
  picFile.close();

  
  
}

///////////////////////////////////////MAIN FUNCTION////////////////////////////////////////////////////////////

void receiverSignalProcessing(short buff_short[], int buffersize, short data_bin[], int total_num_bits){

  std::cout<<"Running Receiver!\n";

  // std::string s1= "received.dat" ;
  // std::ofstream ofs(s1, std::ifstream::out );
  // ofs.write((char * ) buff_short, buffersize*sizeof(short));
  // ofs.close();


  //Conversion to double -> More precision in processing
  double buff_double[buffersize];
  for(int k=0;k<buffersize;k++){
    buff_double[k] =(double) buff_short[k];
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

  //for(int i=0;i<nTrainSeq;i++){
  //	DispVal(trainSeq[i]);
  //}
 

  //Upsampling training sequence:
  int Q=2;
  std::complex<double> trainSeqUp[Q*nTrainSeq];
  hold(trainSeq, trainSeqUp, Q, nTrainSeq);


  // Conversion to cvec
  cvec trainC=arrayToCvec(trainSeqUp, Q*nTrainSeq); 
  cvec buff_cvec=arrayToCvec(buff_double, buffersize);


  //Residual Frequency Correction
  double freq_residual=0.01;
  for(int ii=0; ii<buff_cvec.length();ii++){
    buff_cvec[ii]=buff_cvec[ii]*std::exp(complex<double>(0,2*M_PI*(ii+1)*freq_residual));
    //DispVal(exp(complex<double>(0,-2*M_PI*(ii+1)*freq_residual)));
    //DispVal(ii);
  }

  //Loop to process the different packets

  int delay;

  //Length of each packet:
  int length_paquet=37028, security=100;

  int nDataB=27200;

  itpp::bvec decode(total_num_bits);

  int number_it=0;

  for(int current_num_bits=0; (current_num_bits < total_num_bits) && (number_it<5); ){

    synchCatchChannel(buff_cvec, trainC, &delay);  //Catch the start of transmission

    int tIni=delay+Q*nTrainSeq+4*46; //Beginnig of OFDM data - 4 OFDM symbols to protect

    DispVal(delay);

    cvec dataCvec=buff_cvec.get(0,delay+length_paquet+security); //Get packet

    buff_cvec.del(0,delay+length_paquet-security); //Take packet out 

    //if(number==0)exit(1);
    DispVal(number_it);

    number_it++;
    
    itpp::bvec pack = ProcessData(dataCvec, tIni, nDataB); //Call function to process the packet
    
  // it_file dataFile("Data/pack.it");
  // dataFile << Name("pack") << pack;
  // dataFile.flush();
  // dataFile.close();


    //Put received data in a vector by packet:
    int i=0; 
    while (i<nDataB && current_num_bits < total_num_bits){ 
      data_bin[current_num_bits]=(short)pack[i];
      decode[current_num_bits]=pack[i];
      current_num_bits++;
      i++;
    }
    DispVal(current_num_bits);
    //DispVal(decode);
  }

  DispVal(number_it);


  //Compute the BER of the whole image:
  bvec picBinInter;
  it_file my_fileB("binPicture.it");
  my_fileB >> picBinInter;
  my_fileB.flush();
  my_fileB.close();

  float countError=0;
  bvec errorVec(picBinInter.length());
  DispVal(picBinInter.length());
  DispVal(decode.length());
  for(int i =0; i<picBinInter.length(); i++){
    errorVec[i]=(picBinInter[i]+decode[i]);
    if(errorVec[i]){
      countError++;
    }
    //cout << picBinInter[i] <<" - "<< decode[i] << endl;
  }


  // vec errorVec=to_vec(picBinInter.operator==(decode));
  cout<< "BER = " << countError/(total_num_bits)<< endl;

  // it_file errorFile("Data/errorVec.it");
 //  errorFile << Name("errorVec") << errorVec;
 //  errorFile.flush();
 //  errorFile.close();

 // it_file dataFile("Data/receivedBin.it");
 //  dataFile << Name("decode") << decode;
 //  dataFile << Name("picBinInter") << picBinInter;
 //  dataFile.flush();
 //  dataFile.close();

    
   std::cout << " Processing Done! " << std::endl;
  
   displayPic(decode);

  return;

}





