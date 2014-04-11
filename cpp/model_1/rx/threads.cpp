//#include "threads.hpp"
#include <thread>         // std::thread, std::this_thread::yield
#include <queue>         // std::vector
#include <iostream>
#include <mutex>


using namespace std;

extern queue<bool> isReady;
extern queue<short*> bufferQ;
extern mutex mtx;
long countDetect=0;

void storeData(uhd::rx_streamer::sptr rx_stream, size_t buffer_size, uint nDetect){

  // Create storage for a single buffer from USRP
  short *buff_short;
  buff_short=new short[2*buffer_size]; 
 
  short *storage_short;
  storage_short=new short [2*nDetect];
  //short *ptr;

  size_t n_rx_samps=0;
  size_t n_rx_last;
  uhd::rx_metadata_t md;

  while (1){
    isReady.push(false);

    // Fill the storage buffer loop
    while (n_rx_samps<2*nDetect){
      n_rx_last=0;
      // Fill buff_short
      while (n_rx_last==0) {
	n_rx_last=rx_stream->recv(buff_short, buffer_size, md, 3.0);
      };
      // Check if no overflow
      if (n_rx_last!=buffer_size) {
	std::cerr << "I expect the buffer size to be always the same!\n";
	std::cout<<"Read only:"<<n_rx_last<<"\n";
	std::cout<<"Buffer:"<<buffer_size<<"\n";
	exit(1); 
      };

      // Fill storage
      int i1=2*n_rx_samps;
      int i2=0;   
      while ((i1<(int) (2*nDetect)) && (i2<2*((int) buffer_size))){	  
	storage_short[i1]=buff_short[i2];
	i1++; i2++;
      };
      n_rx_samps=n_rx_samps+n_rx_last;
      //std::cout << "n_rx_samps=" << n_rx_samps  << std::endl;	 
    }//storage_short now full

    double power=powerTotArray(storage_short, (int)2*nDetect);
    //mtx.lock();
    //cout << "power" << power << endl; 
    //mtx.unlock();

    //ptr=storage_short;
    bufferQ.push(storage_short);
    storage_short=new short [2*nDetect]; // Change memory cell used
 
    //usleep(1000000/4);
    isReady.front()=true; // Gives the start to detection part
    //mtx.unlock();
    countDetect++;
  }//end while 1
}
/*
void display(double power){
  //mtx.lock();
    cout << "power detect" << power << endl; 
    //mtx.unlock();
}
*/
void detection(uint nDetect){
  int count2=0;
  while(1){
    // wait for the full buffer
    while (! isReady.front()) { std::this_thread::yield();}
    isReady.pop();

    //Do something here with the just received buffer
    /*
    usleep(1000000/4);
    mtx.lock();
    cout << "Value queue ... " << " [" << (bufferQ.front())[0] << "," << (bufferQ.front())[1] << "]" << endl;
    mtx.unlock();
    */
    mtx.lock();
    short *p=bufferQ.front();
    mtx.unlock();
    double power=powerTotArray(p, (int)2*nDetect);
    if (power>100){
      mtx.lock();
      cout << "power " << power << endl; 
      cout << "countDetect " << countDetect << endl;
      cout << "count2 " << count2 << endl;
      mtx.unlock();
    }
    //std::thread displayT(display,power);

    // Relase the element
    delete[] (bufferQ.front());
    bufferQ.pop();
    count2++;
  }
}
