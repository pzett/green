cutSec (){

  float Buffer;
  int i;
  float buffer_1s;

  const int Fs=25e6;
  nSample=1/Fs;

  buffer_1s= new float [nSample];
  for (i=0; i<nSample; i++){
    buffer_1s[i]=buffer[i];
  } 


}
