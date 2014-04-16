//Project Group: Wireless Transmission over 60GHz
//
//
// Copyright Per Zetterberg, KTH Royal Institute of Technology
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

#include <uhd/utils/thread_priority.hpp>
#include <uhd/utils/safe_main.hpp>
#include <uhd/usrp/multi_usrp.hpp>
#include <uhd/types/dict.hpp>
#include <boost/program_options.hpp>
#include <boost/format.hpp>
#include <iostream>
#include <complex>
#include <cstdio>
#include <stdio.h>      
#include <stdlib.h>
#include <fstream>

#include "tx_funct_2.hpp"



namespace po = boost::program_options;

int UHD_SAFE_MAIN(int argc, char *argv[]){


  
  //Seting priority in the processor to run faster -> run with sudo
  if (uhd::set_thread_priority_safe(1,true)) {
    std::cout << "set priority went well " << std::endl;
  };


  //variables to be set by po -> Set when initializing the tx program
  std::string args;
  double seconds_in_future;
  size_t total_num_samps;
  double tx_rate, freq, LOoffset;
  float gain;
  bool forever, use_8bits;
  bool use_external_10MHz, trigger_with_pps;
  bool readFile;
  std::string filename;
  uhd::tx_streamer::sptr tx_stream;
  uhd::device_addr_t dev_addr;
  uhd::usrp::multi_usrp::sptr dev;
  uhd::stream_args_t stream_args;

  //setup the program options -> Passing it from terminal with boost library 
  po::options_description desc("Allowed options");
  desc.add_options()
    ("help", "help message")
    ("args", po::value<std::string>(&args)->default_value(""), "simple uhd device address args")
    ("secs", po::value<double>(&seconds_in_future)->default_value(3), "number of seconds in the future to transmit")
    ("nsamps", po::value<size_t>(&total_num_samps)->default_value(27840), "total number of samples to transmit")
    ("txrate", po::value<double>(&tx_rate)->default_value(100e6/16), "rate of outgoing samples")
    ("freq", po::value<double>(&freq)->default_value(5.5e9), "rf center frequency in Hz")
    ("LOoffset", po::value<double>(&LOoffset)->default_value(10e6), "Offset between main LO and center frequency")
    ("forever",po::value<bool>(&forever)->default_value(true), "run indefinetly")
    ("10MHz",po::value<bool>(&use_external_10MHz)->default_value(false), 
     "external 10MHz on 'REF CLOCK' connector (true=1=yes)")
    ("PPS",po::value<bool>(&trigger_with_pps)->default_value(false), 
     "trigger reception with 'PPS IN' connector (true=1=yes)")
    ("filename",po::value<std::string>(&filename)->default_value("data_to_usrp.dat"), "input filename")
    ("gain",po::value<float>(&gain)->default_value(0), "gain of transmitter")
    ("8bits",po::value<bool>(&use_8bits)->default_value(false), "Use eight bits/sample to increase bandwidth")
  
    ////////////////////////////////
    ("readFile",po::value<bool>(&readFile)->default_value(false), "defines if program reads data from file or generate data itself")
    ;

    
  //Variables stored in boost objects
  po::variables_map vm;
  po::store(po::parse_command_line(argc, argv, desc), vm);
  po::notify(vm);

  //print the help message
  if (vm.count("help")){
    std::cout << boost::format("tx %s") % desc << std::endl;
    return ~0;
  }

  //////// Create buffer storage to pass to USRP -> complex short always
 
  std::complex<short> *buffer;
  buffer = new std::complex<short>[total_num_samps];
  int nAll=13920*4;
  short *all;
  all = new short[nAll];
  ////////////////// Create data to be transmitted ///////////////////////
   
  if(readFile==true){

    /* Read input from disc -> data previous generated by MATLAB*/ 
    FILE *fp = 0;
    fp = fopen(filename.c_str(), "rb");    
    if (fp == 0){
      perror(filename.c_str());
      return 1;
    }
    int r=fread(buffer, sizeof(uint32_t),total_num_samps, fp);
    printf("r=%d \n",r);
    fclose(fp);
      
    printf("USING MATLAB DATA!\n");

  }else{

 
    imp_1(all);
    /*
      for (int i=(0)*4;i<(10)*4;i++){
      cout << "all["<<i<<"] = " << all[i]<<"\n";
      }
    */
    buffer= (std::complex<short> * ) all;
    /*
      for (int i=(0)*4;i<(10)*4;i++){
      cout << "buffer["<<i<<"] = " << buffer[i]<<"\n";
      }
     
    int nData=6250;
    int nTrain=100*2; // To store complex part *2
    int nPref=500*2;
    int nGuard=10*2;

    //cout<<nAll;
  
    // for (int i=(nPref+nGuard+nTrain-1)*4;i<(nPref+nGuard+nTrain+10)*4;i++){
    for(int i=nAll-100;i<nAll;i++){
      cout << "allUp["<<i<<"] = " << all[i]<<"\n";
    }

    //for (int i=0; i<500;i++){cout << buffer[i] << "\n";}
    */

     

    // FILE * xFile;
    // xFile = fopen("sent.dat","wb");
    // fwrite(buffer, 2*sizeof(short),nAll/2,xFile);
    // fclose(xFile);

    //ATENTION:Always convert to complex<short> to send to the USRP transmitter

    printf("USING CPP IMPLEMENTATION!\n");
    
  };
 /////////////////////////////////////////////////////////////////////////

  // Save data to file to check what was sent
     std::ofstream ofs( "sent.dat" , std::ifstream::out );
     ofs.write((char * ) buffer, nAll*sizeof(short));
     ofs.close();


    //create a USRP device and streamer
    dev_addr["addr0"]="192.168.10.2";
    

    dev = uhd::usrp::multi_usrp::make(dev_addr);    


    ///////////// Internal variables of USRP (Not important now, set as default)
    
    uhd::clock_config_t my_clock_config; 

    if (!forever) {
      //my_clock_config.ref_source=uhd::clock_config_t::REF_SMA; 
      dev->set_time_source("external");
    };

    if (use_external_10MHz) { 
      //my_clock_config.pps_source=uhd::clock_config_t::PPS_SMA; 
      dev->set_clock_source("external");
    }
    else {
      //my_clock_config.pps_source=uhd::clock_config_t::PPS_SMA; 
      dev->set_clock_source("internal");
    };


    //dev->set_tx_bandwidth(36e6);


    std::cout << "freq=" << freq << "\n";
    std::cout << "LOoffset=" << LOoffset << "\n";
    

    uhd::tune_result_t tr;
    uhd::tune_request_t trq(freq,LOoffset); //std::min(tx_rate,10e6));
    tr=dev->set_tx_freq(trq,0);
    
    bool is_xcvr2450=false;
    uhd::dict<std::string, std::string> tx_info;    
    tx_info=dev->get_usrp_tx_info(0);

    if (tx_info.has_key("tx_subdev_name")) {
      std::string str=tx_info.get("tx_subdev_name");
      uint temp=str.find("XCVR2450");
      if (temp<str.length()) {
	is_xcvr2450=true;
      };
    };

    if (is_xcvr2450) {
      dev->set_tx_antenna("J2");
      dev->set_rx_antenna("J1");
      
      //uhd::meta_range_t range=dev->get_tx_bandwidth_range();

      if (abs(LOoffset)>=6e6) {
	//dev->set_tx_bandwidth(28e6);
	dev->set_tx_bandwidth(3.96e+07);
      };
      
    };

    dev->set_tx_gain(gain);
    std::cout << tr.to_pp_string() << "\n";
 
      //dev->set_clock_config(my_clock_config); // PZ

    stream_args.cpu_format="sc16";
    if (use_8bits)
      stream_args.otw_format="sc8";
    else
      stream_args.otw_format="sc16";

   


    tx_stream=dev->get_tx_stream(stream_args);

    


    //set properties on the device
    std::cout << boost::format("Setting TX Rate: %f Msps...") % (tx_rate/1e6) << std::endl;
    dev->set_tx_rate(tx_rate);
    std::cout << boost::format("Actual TX Rate: %f Msps...") % (dev->get_tx_rate()/1e6) << std::endl;
    std::cout << boost::format("Setting device timestamp to 0...") << std::endl;


        //Check what is being sent

    // for(int ii=0;ii<11;ii++){
    //   std::cout << real(buffer[ii])<< "->value \n";
    //   std::cout << imag(buffer[ii])<< "->value\n \n"; 
    // };


    ///////////////Transmission of data///////////////////////////////////

    /*----> send data in loop
      ----> send data once and let receiver handle it */
    
    uhd::tx_metadata_t md;

    if (forever) {

      std::cout << "Stop the transmitter by pressing ctrl-c \n";

      md.start_of_burst = true;
      md.end_of_burst = false;
      md.has_time_spec = false;
      
     
      //send the entire buffer, let the driver handle fragmentation

      /*
      num_tx_samps = dev->send(
      buffer, total_num_samps, md,
      uhd::io_type_t::COMPLEX_INT16,
      uhd::device::SEND_MODE_FULL_BUFF);
      */
      tx_stream->send(buffer,total_num_samps,md,60);

      
      md.start_of_burst = false;

      while (1) {

	/*
	num_tx_samps = dev->send(
        buffer, total_num_samps, md,
        uhd::io_type_t::COMPLEX_INT16,
        uhd::device::SEND_MODE_FULL_BUFF
         );
	*/

        tx_stream->send(buffer,total_num_samps,md,3);

	md.start_of_burst = false;
	md.end_of_burst = false;
	md.has_time_spec = false;
	
	std::cout << "Sending data in loop... \n";

      }
    }
    else
    {

      //Transmit data once and let receiver handle it
    
    dev->set_time_now(uhd::time_spec_t(0.0));
    md.start_of_burst = true;
    md.end_of_burst = true;
    md.has_time_spec = true;
    md.time_spec = uhd::time_spec_t(seconds_in_future);

    //send the entire buffer, let the driver handle fragmentation

    /*
      num_tx_samps = dev->send(
	buffer, total_num_samps, md,
        //&buff.front(), buff.size(), md,
        uhd::io_type_t::COMPLEX_FLOAT32,
        uhd::device::SEND_MODE_FULL_BUFF);
    */
    

    tx_stream->send(buffer,total_num_samps,md,60);
    
    std::cout << "\nData only sent once \n";

    };
 
    //finished


    std::cout << std::endl << "Done! Transmission completed" << std::endl << std::endl;

    return 0;
}

