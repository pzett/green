
int main() {


  int buffersize = 55680*2;
  double buff_double[buffersize];

  // Read data from file
  std::ifstream ifs( "data_receive.dat" , std::ifstream::in );
  ifs.read((char * )buff_double,buffersize*sizeof(double));
  ifs.close();

  // Casting to short
  short buff_short[buffersize];
  for(int k=0;k<buffersize;k++){
    buff_short[k] = (short) buff_double[k];
    //cout << buff_short[k] << endl;
  };

  // // creating some testing data
  // int buffersize = 1000;
  // short buff_short[buffersize];
  // // creating data
  // for(int i=0;i<buffersize-1; i=i+2){
  //   buff_short[i]=(short) 3*cos(3.14159*0.62*i) +1* cos(3.14159*0.56*i);//+cos(2*3.14159*0.2*i);
  //   buff_short[i+1]=(short) 10*sin(3.14159*0.6*i);
  //   //std::cout << buff_short[i] << " " << buff_short[i+1] << std::endl;
  // };

  short data_bin[buffersize/8];
  receiverSignalProcessing(buff_short,buffersize,data_bin);


// for(int k=0;k<buffersize/8;k++){
//   std::cout << data_bin[k] << std::endl;
//   }
//  std::cout << "length data_bin    : " << sizeof(data_bin)/sizeof(data_bin[0]) << std::endl;  

  
  return 0;
}
