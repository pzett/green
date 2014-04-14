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
//#include <gruel/realtime.h>

//#include <ctime>

//#include <sstream>
// #include <itpp/itbase.h>
// #include <itpp/itsignal.h>

//#include "funct_general.hpp"
//#include "rx_funct.hpp"



std::thread *detectionT, *storeT, *displayT;
sem_t isReady;
std::queue<short*> bufferQ;
std::mutex mtx;
std::mutex mtxQ;

//#include "threads.cpp"
//#include "transmissionDetection.cpp"
namespace po = boost::program_options;

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

  
  size_t n_rx_last;
  uhd::rx_metadata_t md;
  size_t n_rx_samps=0;
  int time=buffer_size/(25); // microsecondes

  while (1){
    n_rx_samps=0;
    // Fill the storage buffer loop
    while (n_rx_samps<nDetect){
      n_rx_last=0;
      // Fill buff_short
      while (n_rx_last==0) {
	n_rx_last=rx_stream->recv(&buff_short[0], buffer_size, md, 3.0);
	//std::this_thread::yield();
      };
      // Check if no overflow
      if (n_rx_last!=buffer_size) {
	std::cerr << "I expect the buffer size to be always the same!\n";
	std::cout<<"Read only:"<<n_rx_last<<"\n";
	std::cout<<"Buffer:"<<buffer_size<<"\n";
	exit(1); 
      };
      /*
      // Fill storage
      int i1=2*n_rx_samps;
      int i2=0;   
      while ((i1<(int) (2*nDetect)) && (i2<2*((int) buffer_size))){	  
	storage_short[i1]=buff_short[i2];
	i1++; i2++;
      };
      */
      //storage_short=buff_short;
      n_rx_samps=n_rx_samps+n_rx_last;
      //std::cout << "n_rx_samps=" << n_rx_samps  << std::endl;	 
    }//storage_short now full

    /*
    power=powerTotArray(storage_short, (int)2*nDetect);
    mtx.lock();
    std::cout << countDetect  <<" power stored " << power << std::endl; 
    mtx.unlock();
    */
    /*
    mtxQ.lock();
    bufferQ.push(storage_short);
    mtxQ.unlock();
    storage_short=new short [2*nDetect]; // Change memory cell used
    */

    mtxQ.lock();
    bufferQ.push(buff_short);
    mtxQ.unlock();
    buff_short=new short [2*buffer_size]; // Change memory cell used

    //usleep(1000000/4);
    sem_post( &isReady); // Gives the start to detection part

    //std::this_thread::sleep_for(std::chrono::microseconds(time));

    //int a=0;
    //sem_getvalue(&isReady, &a);
    //if (a>1000){std::cout << "Computer overload" << std::endl; exit(1);}

    //countDetect++;
  }//end while 1
}

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
    //std::this_thread::yield();
    if (power2>100){
      //mtx.lock();
      std::cout<< count2 << " power detect " << power2 << std::endl; 
      //mtx.unlock();
    }
    //std::thread displayT(display,power);

    // Relase the element
    //std::this_thread::yield();
    delete[] (bufferQ.front());
    mtxQ.lock();
    bufferQ.pop();
    mtxQ.unlock();
    //std::this_thread::yield();
    count2++;
  }
}





int UHD_SAFE_MAIN(int argc, char *argv[]){
    
  //Seting priority in the processor to run faster -> run with sudo
  if (!(uhd::set_thread_priority_safe(1,true))) {
    std::cout << "Problem setting thread priority" << std::endl;
    return 1;
  };
  //variables to be set by po -> Set when initializing the rx program
  //double seconds_in_future=0.01;
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
    ("nsamps", po::value<size_t>(&total_num_samps)->default_value(1000), "total number of samples to receive")
    ("rxrate", po::value<double>(&rx_rate)->default_value(100e6/4), "rate of incoming samples")
    ("freq", po::value<double>(&freq)->default_value(5.5e6), "rf center frequency in Hz")
    ("LOoffset", po::value<double>(&LOoffset)->default_value(10e6), "Offset between main LO and center frequency")
    ("10MHz",po::value<bool>(&use_external_10MHz)->default_value(false), "external 10MHz on 'REF CLOCK' connector (true=1=yes)")
    //  ("PPS",po::value<bool>(&trigger_with_pps)->default_value(false), "trigger reception with 'PPS IN' connector (true=1=yes)")
    ("filename",po::value<std::string>(&filename)->default_value("data_from_usrp.dat"), "output filename") 
    ("gain",po::value<float>(&gain)->default_value(30), "set the receiver gain") 
    ("8bits_scaling",po::value<double>(&scaling_8bits)->default_value(0.0), 
     "input scaling (invers) when 8bits is used, set to zero to get 16bits")
      
    /////////////////////////////
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

  /*
    if (trigger_with_pps) {
    my_clock_config.pps_source=uhd::clock_config_t::PPS_SMA; 
    };
  */

  if (use_external_10MHz) { 
    my_clock_config.ref_source=uhd::clock_config_t::REF_SMA; 
  };  

  /////////////////Create an USRP device////////////////////////
  std::cout << std::endl;
  //uhd::device::sptr udev = dev->get_device();
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
    //uhd::meta_range_t range=dev->get_tx_bandwidth_range();

    if (LOoffset>=9e6) {
      dev->set_rx_bandwidth(3.96e+07);
    };      
  };

  std::cout << "rx_rate=" << rx_rate << std::endl;
  std::cout << "freq=" << freq << std::endl;
  std::cout << "gain=" << gain << std::endl;
  uhd::tune_request_t trq(freq,LOoffset); 
  //dev->set_rx_bandwidth(36e6);
  tr=dev->set_rx_freq(trq);
  dev->set_rx_gain(gain);

  std::cout << "tr=" << tr.actual_rf_freq << std::endl;

  /*
    double target_rf_freq;
    double actual_rf_freq;
    double target_dsp_freq;
    double actual_dsp_freq;
  */

  // 
  //dev->set_tx_antenna("J2");
  //dev->set_rx_antenna("J1");
    

  if (use_external_10MHz) {
    dev->set_clock_config(my_clock_config); // PZ
    usleep(1e6); // Wait for the 10MHz to lock
  }; 

  size_t buffer_size=25000; // Select buffer size

  uint nDetect=25000;
  short buff_detect[2*nDetect];

  short storage_short [2*total_num_samps]; // Create storage for the entire received signal to be saved on disk (2* for handling complex).


  /*if (trigger_with_pps) {
    dev->set_time_next_pps(uhd::time_spec_t(0.0));
    usleep(1e6); 
    } */
  //else {
  dev->set_time_now(uhd::time_spec_t(0.0));
  //};
  std::cout << boost::format("Setting device timestamp to 0...") << std::endl;

  //setup streaming
  //std::cout << std::endl;
  //std::cout << boost::format("Begin streaming %u samples, %d seconds in the future...")
  //    % total_num_samps % seconds_in_future << std::endl;
    

  //////////////////////////////////Read data to buff_short and do processing////////////

  uhd::stream_cmd_t stream_cmd(uhd::stream_cmd_t::STREAM_MODE_NUM_SAMPS_AND_DONE);


  //USRP parameters
  stream_cmd.num_samps = buffer_size;
  stream_cmd.stream_now = true;
  stream_cmd.stream_mode=uhd::stream_cmd_t::STREAM_MODE_START_CONTINUOUS;
  dev->issue_stream_cmd(stream_cmd);

  //Launch threads
  sem_init(&isReady, 0,0); 
  std::thread storeT(storeDataX, rx_stream, buffer_size, nDetect);
  std::thread detectionT(detection, nDetect);

  storeT.join();
  detectionT.join();

  //finished
  std::cout << std::endl << "Done receiving!" << std::endl << std::endl; 

  return 0;
}
