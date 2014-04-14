#include <fstream> // PZ
#include <uhd/utils/thread_priority.hpp>
#include <uhd/utils/safe_main.hpp>
#include <uhd/usrp/multi_usrp.hpp>
#include <uhd/device.hpp>
#include <boost/program_options.hpp>
#include <boost/format.hpp>
#include <iostream>
#include <complex>

#include <uhd/types/clock_config.hpp>


#include <thread>         // std::thread, std::this_thread::yield
#include <queue>         // std::vector
#include <iostream>
#include <mutex>
#include <semaphore.h>

#include <chrono>

//#include "threads.hpp"

using namespace std;
extern sem_t isReady;
extern std::queue<short*> bufferQ;
extern std::mutex mtx;
extern std::mutex mtxQ;


long countDetect=0;

/** Computes the power of the given array
 * @pre:
 *       - data: a pointer to an array of short
 *       - no_elements: the length of the array
 * @post:
 *       - power: a float with the power
 */
float powerTotArray( short data[], int no_elements){
  float power=0;
  float tmp;
  for (int i=0;i<no_elements;i++){
    tmp= (float) data[i];
    power= power+(tmp*tmp)/(no_elements/2);
    if(power<0){
      std::cout<<power;
    };

  };
  //float result=0;
  // std::cout<<power<<"\n";
  // std::cout<<(short)power<<"\n";
  return power;
};


void storeDataX(uhd::rx_streamer::sptr rx_stream, size_t buffer_size, uint nDetect){

  float power;

  // Create storage for a single buffer from USRP
  short *buff_short;
  buff_short=new short[2*buffer_size]; 
 
  short *storage_short;
  storage_short=new short [2*nDetect];

  size_t n_rx_samps=0;
  size_t n_rx_last;
  uhd::rx_metadata_t md;

  int time=nDetect/(25); // microsecondes

  while (1){

    // Fill the storage buffer loop
    while (n_rx_samps<2*nDetect){
      n_rx_last=0;
      // Fill buff_short
      while (n_rx_last==0) {
	n_rx_last=rx_stream->recv(buff_short, buffer_size, md, 3.0);
	std::this_thread::yield();
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


    power=powerTotArray(storage_short, (int)2*nDetect);
    mtx.lock();
    cout << countDetect  <<" power stored " << power << endl; 
    mtx.unlock();

    mtxQ.lock();
    bufferQ.push(storage_short);
    mtxQ.unlock();
    storage_short=new short [2*nDetect]; // Change memory cell used
 
    //usleep(1000000/4);
    sem_post( &isReady); // Gives the start to detection part

    std::this_thread::sleep_for(std::chrono::microseconds(time));

    int a=0;
    sem_getvalue(&isReady, &a);
    if (a>1000){cout << "Computer overload" << endl; exit(1);}

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
  float power2;
  while(1){
    // wait for the full buffer
    sem_wait(&isReady);

    //Do something here with the just received buffer
    /*
    usleep(1000000/4);
    mtx.lock();
    cout << "Value queue ... " << " [" << (bufferQ.front())[0] << "," << (bufferQ.front())[1] << "]" << endl;
    mtx.unlock();
    */
    short *p=bufferQ.front();

    power2=powerTotArray(p, (int)2*nDetect);
    if (power2>100){
      mtx.lock();
      cout<< count2 << " power detect " << power2 << endl; 
      mtx.unlock();
    }
    //std::thread displayT(display,power);

    // Relase the element
    mtxQ.lock();
    delete[] (bufferQ.front());
    bufferQ.pop();
    mtxQ.unlock();
    count2++;
  }
}
