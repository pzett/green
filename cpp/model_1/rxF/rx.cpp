//Project Group: Wireless Transmission over 60GHz
//
//
// Copyright 2010 Ettus Research LLC
// Copyright 2010-2014 Modified by Per Zetterberg, KTH.
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
// Threads initialisation
#include <mutex>
#include <thread>         // std::thread, std::this_thread::yield
#include <queue>         // std::vector
#include <semaphore.h>
#include <pthread.h>
#include <sys/resource.h>
#include <sys/time.h>

#include <fstream> // PZ
#include <uhd/utils/thread_priority.hpp>
#include <uhd/utils/safe_main.hpp>
#include <uhd/usrp/multi_usrp.hpp>
#include <uhd/device.hpp>
#include <boost/program_options.hpp>
#include <boost/format.hpp>
#include <iostream>
#include <complex>
#include <cmath>
#include <uhd/types/clock_config.hpp>

#include "rx_funct.cpp"


//#include <gruel/realtime.h>

//#include <ctime>

//#include <sstream>
// #include <itpp/itbase.h>
// #include <itpp/itsignal.h>

#define DispVal(X) std::cout << #X << " = " << X<< std::endl



std::thread *detectionT, *usrpT, *processT;
sem_t usrpReady, detectionReady;
std::queue<short*> usrpQ;
std::queue<short*> detectionQ;
std::mutex mtx; //cout
std::mutex mtxUsrp;
std::mutex mtxDetection;

namespace po = boost::program_options;

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
  return (power);
};

// Processing thread nStorage: size of the array
void processing(int nStorage){
  short *data;
  short *data_bin;
  int nDataB=6250;
  data_bin=new short[nDataB];
  bool forever=true;
  while(forever){
    // Ask for data
    sem_wait(&detectionReady);
    data=detectionQ.front();

    std::this_thread::yield();
    //Do something heavy with data:
    
    //Process online data:

    receiverSignalProcessing(data, nStorage, data_bin, nDataB);

    // Save data to file
    std::ofstream ofs( "received.dat" , std::ifstream::out );
    ofs.write((char * ) data, nStorage*sizeof(short));
    ofs.close();
    //std::cout << "Finish Writing to file\n";




    std::cout << "Data Received and Processed!\n";

     // Save data to file
     std::ofstream ofs1( "data_from_harness.dat" , std::ifstream::out );
     ofs1.write((char * ) data_bin, nDataB*sizeof(short));
     ofs1.close();

    std::this_thread::yield();
    //Release the memory
    delete[] (detectionQ.front());
    mtxDetection.lock();
    detectionQ.pop();
    mtxDetection.unlock();
    forever=false;
  }
}

// Thread to import data from the USRP !Size of the arrays in complex -> 2*buffer_size !
void usrpGetData(uhd::rx_streamer::sptr rx_stream, uhd::usrp::multi_usrp::sptr dev, size_t buffer_size){

  // Set priority of the thread
  int which = PRIO_PROCESS;
  id_t pid;
  int priority = -20;
  int ret;

  pid = getpid();
  ret = setpriority(which, pid, priority);
  if(ret!=0){  std::cout << "Main priority went wrong in usrpT: " << ret << std::endl ;}


  // Create storage for a single buffer from USRP
  short *buff_short;
  buff_short=new short[2*buffer_size]; 

 
  size_t n_rx_last;
  uhd::rx_metadata_t md;
  //int time=buffer_size/(25)-100; // microsecondes

  while (1){
    n_rx_last=0;

    // Fill buff_short
    while (n_rx_last==0) {
      n_rx_last=rx_stream->recv(&buff_short[0], buffer_size, md, 3.0);
      std::this_thread::yield(); // Avoid active waiting
    };

    // Check if no overflow
    if (n_rx_last!=buffer_size) {
      std::cerr << "I expect the buffer size to be always the same!\n";
      std::cout<<"Read only:"<<n_rx_last<<"\n";
      std::cout<<"Buffer:"<<buffer_size<<"\n";
      //exit(1); 
    };

    // Add the just received buffer to the queue
    mtxUsrp.lock();
    usrpQ.push(buff_short);
    mtxUsrp.unlock();
    // Change memory cell used
    buff_short=new short [2*buffer_size];

    // Gives the start to detection part
    sem_post( &usrpReady); 

  }//end while 1
}

// Will analyse every buffer from 
void detection(uint nDetect, int nStorage){
  bool isDetected=false;
  int nCurrent=0;
  short *storage_short;
  storage_short=new short [nStorage];
  short *p=0;
  float power;
  int i2=0,count_trans=0;
  while(1){
    std::this_thread::yield();
    if (isDetected){
      // Store the elements in storage_short
      if (nCurrent<nStorage){
	// Fill storage_short
	i2=0;
	int conj_rx;
	while ((nCurrent<nStorage) && (i2<2*((int) nDetect))){	  
	  conj_rx=pow(-1,nCurrent);
	  //DispVal(conj_rx);
	  storage_short[nCurrent]=(p[i2])*conj_rx;
	  nCurrent++; i2++;
	};
	// Relase the just stored element
	std::this_thread::yield();
	delete[] (usrpQ.front());
	mtxUsrp.lock();
	usrpQ.pop();
	mtxUsrp.unlock();
	std::this_thread::yield();

	//Wait for a new element
	sem_wait(&usrpReady);
	p=usrpQ.front();
      }
      else{
	// End of transmission
	count_trans=0;
	isDetected=false;
	nCurrent=0;
	mtxDetection.lock();
	detectionQ.push(storage_short);
	mtxDetection.unlock();
	sem_post(&detectionReady);
	storage_short=new short[nStorage];
      }
    }
    else{
      // Wait for new element
      sem_wait(&usrpReady);
      p=usrpQ.front();
      // Test of detection
      power=powerTotArray(p, (int)2*nDetect);
      //DispVal(power);

      count_trans++;//Avoid transient power amplification

      //Power threshold=200 150USRP2 count trns 10000 

      if (power>150&& count_trans>10000){
	// If detection, keep the element
	std::cout<<"Detected Transmission\n";
	//exit(1);
	isDetected=true;
      }
      else{
	// Release the element
	std::this_thread::yield();
	delete[] (usrpQ.front());
	mtxUsrp.lock();
	usrpQ.pop();
	mtxUsrp.unlock();
	std::this_thread::yield();
      }
    }
  }
}





int UHD_SAFE_MAIN(int argc, char *argv[]){

  // Set priority of the main thread
  int which = PRIO_PROCESS;
  id_t pid;
  int priority = -20;
  int ret;

  pid = getpid();
  ret = setpriority(which, pid, priority);
  if(ret!=0){  std::cout << "Main priority went wrong: " << ret << std::endl ;}

    
  //Seting priority in the processor to run faster -> run with sudo
  if (!(uhd::set_thread_priority_safe(1,true))) {
    std::cout << "Problem setting thread priority" << std::endl;
    return 1;
  };
  //variables to be set by po -> Set when initializing the rx program
 
  size_t total_num_samps;
  double rx_rate, freq, LOoffset;
  bool use_external_10MHz;
  double scaling_8bits;
  std::string filename;
  float gain;
  bool realTime;
  uhd::device_addr_t dev_addr;
  uhd::usrp::multi_usrp::sptr dev;
  uhd::tune_result_t tr;
  uhd::stream_args_t stream_args;
  uhd::rx_streamer::sptr rx_stream;


  //setup the program options-> Passing it from terminal with boost library 
  po::options_description desc("Allowed options");
  desc.add_options()
    ("help", "help message")
    ("nsamps", po::value<size_t>(&total_num_samps)->default_value(27840), "total number of samples to receive")
    ("rxrate", po::value<double>(&rx_rate)->default_value(100e6/4), "rate of incoming samples")
    ("freq", po::value<double>(&freq)->default_value(5.5e9), "rf center frequency in Hz")
    ("LOoffset", po::value<double>(&LOoffset)->default_value(10e6), "Offset between main LO and center frequency")
    ("10MHz",po::value<bool>(&use_external_10MHz)->default_value(false), "external 10MHz on 'REF CLOCK' connector (true=1=yes)")
    ("filename",po::value<std::string>(&filename)->default_value("data_from_usrp.dat"), "output filename") 
    ("gain",po::value<float>(&gain)->default_value(15), "set the receiver gain") 
    ("8bits_scaling",po::value<double>(&scaling_8bits)->default_value(0.0), 
     "input scaling (invers) when 8bits is used, set to zero to get 16bits")
    ("realTime",po::value<bool>(&realTime)->default_value(true), "receives in loop and compares with synch sequence")
    ;
   
  //Variables stored in boost objects
  po::variables_map vm;
  po::store(po::parse_command_line(argc, argv, desc), vm);
  po::notify(vm);

  //print the help message
  if (vm.count("help")){
    std::cout << boost::format("rx %s") % desc << std::endl;
    return ~0;
  }

  ///////////Set device variables to read data////////////////
    
  dev_addr["addr0"]="192.168.10.2";
  dev = uhd::usrp::multi_usrp::make(dev_addr);//Create the device

  //receiving format
  stream_args.cpu_format="sc16";
  if (scaling_8bits==0.0) {
    stream_args.otw_format="sc16";     
  } else {
    stream_args.otw_format="sc8";
    std::stringstream temp_ss;
    temp_ss << scaling_8bits;
    stream_args.args["peak"]=temp_ss.str();
  };

  rx_stream=dev->get_rx_stream(stream_args); //set/get the streamer values to the device

  uhd::clock_config_t my_clock_config; 

  if (use_external_10MHz) { 
    my_clock_config.ref_source=uhd::clock_config_t::REF_SMA; 
  };  

  /////////////////Create an USRP device////////////////////////
  std::cout << std::endl;
 
  dev->set_rx_rate(rx_rate);

  bool is_xcvr2450=false;
  uhd::dict<std::string, std::string> rx_info;    
  rx_info=dev->get_usrp_rx_info(0);

  if (rx_info.has_key("rx_subdev_name")) {
    std::string str=rx_info.get("rx_subdev_name");
    uint temp=str.find("XCVR2450");
    if (temp<str.length()) {
      is_xcvr2450=true;
    };
  };

  if (is_xcvr2450) {
    dev->set_tx_antenna("J2");
    dev->set_rx_antenna("J1");      

    if (LOoffset>=9e6) {
      dev->set_rx_bandwidth(3.96e+07);
    };      
  };

  std::cout << "rx_rate=" << rx_rate << std::endl;
  std::cout << "freq=" << freq << std::endl;
  std::cout << "gain=" << gain << std::endl;
  std::cout << "LOoffset="<<LOoffset <<std::endl;

  uhd::tune_request_t trq(freq,LOoffset); 

  tr=dev->set_rx_freq(trq);
  dev->set_rx_gain(gain);

  std::cout << "tr=" << tr.actual_rf_freq << std::endl;


  if (use_external_10MHz) {
    dev->set_clock_config(my_clock_config); // PZ
    usleep(1e6); // Wait for the 10MHz to lock
  }; 

  size_t buffer_size=1000; // Select buffer USRP and detection size. <25.000

  int nStorage=2*total_num_samps; // Size of the buffer for the processing part

  dev->set_time_now(uhd::time_spec_t(0.0));

  std::cout << boost::format("Setting device timestamp to 0...") << std::endl;

   

  // Initialisation  

    
  uhd::stream_cmd_t stream_cmd(uhd::stream_cmd_t::STREAM_MODE_NUM_SAMPS_AND_DONE);

  //USRP parameters
  stream_cmd.num_samps = buffer_size;
  stream_cmd.stream_now = true;
  stream_cmd.stream_mode=uhd::stream_cmd_t::STREAM_MODE_START_CONTINUOUS;
  dev->issue_stream_cmd(stream_cmd);


  //////////////////////////////////Read data to buff_short and do processing////////////

  //Launch threads
  sem_init(&usrpReady, 0,0); 
  sem_init(&detectionReady, 0,0);
  std::thread usrpT(usrpGetData, rx_stream, dev, buffer_size);
  std::thread detectionT(detection, buffer_size, nStorage);
  std::thread processT(processing, nStorage);

  // Set highest priiority for usrpT
    sched_param sch;
    int policy; 
    pthread_getschedparam(usrpT.native_handle(), &policy, &sch);
    sch.sched_priority = 99;
    if(pthread_setschedparam(usrpT.native_handle(), SCHED_FIFO, &sch)) {
        std::cout << "Failed to setschedparam: " << std::strerror(errno) << '\n';
    }

    std::cout << "Priority set"<< std::endl;

  usrpT.join();
  detectionT.join();
  processT.join();

  //finished
  std::cout << std::endl << "Done receiving!" << std::endl << std::endl; 

  return 0;
}

