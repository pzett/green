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


#include <fstream> // PZ
#include <uhd/utils/thread_priority.hpp>
#include <uhd/utils/safe_main.hpp>
#include <uhd/usrp/multi_usrp.hpp>
#include <uhd/device.hpp>
#include <boost/program_options.hpp>
#include <boost/format.hpp>
#include <iostream>
#include <complex>
#include <thread>
#include <uhd/types/clock_config.hpp>
//#include <gruel/realtime.h>

#include <ctime>

#include <sstream>
// #include <itpp/itbase.h>
// #include <itpp/itsignal.h>

#include "funct_general.hpp"

#include "rx_funct.hpp"



namespace po = boost::program_options;

void test(int x){
  std::cout<<"thread called:"<<x<<"\n";
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
        ("freq", po::value<double>(&freq)->default_value(0), "rf center frequency in Hz")
        ("LOoffset", po::value<double>(&LOoffset)->default_value(0), "Offset between main LO and center frequency")
        ("10MHz",po::value<bool>(&use_external_10MHz)->default_value(false), "external 10MHz on 'REF CLOCK' connector (true=1=yes)")
      //  ("PPS",po::value<bool>(&trigger_with_pps)->default_value(false), "trigger reception with 'PPS IN' connector (true=1=yes)")
        ("filename",po::value<std::string>(&filename)->default_value("data_from_usrp.dat"), "output filename") 
        ("gain",po::value<float>(&gain)->default_value(0), "set the receiver gain") 
        ("8bits_scaling",po::value<double>(&scaling_8bits)->default_value(0.0), 
    "input scaling (invers) when 8bits is used, set to zero to get 16bits")
      
      /////////////////////////////
      ("realTime",po::value<bool>(&realTime)->default_value(false), "receives in loop and compares with synch sequence")
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

    size_t buffer_size=1000; // Select buffer size
    short *buff_short, *storage_short; //Always read short buffers
   
    storage_short=new short[2*total_num_samps]; // Create storage for the entire received signal to be saved on disk (2* for handling complex).
    
    buff_short=new short[2*buffer_size]; // Create storage for a single buffer


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

    if (realTime == false){

      
    //Reads just one time the USRP rx dev and process data

    stream_cmd.num_samps = buffer_size;
    stream_cmd.stream_now = true;

    
    stream_cmd.stream_mode=uhd::stream_cmd_t::STREAM_MODE_START_CONTINUOUS;

    //stream_cmd.time_spec = uhd::time_spec_t(seconds_in_future);
    

    dev->issue_stream_cmd(stream_cmd);

    size_t num_rx_samps=0;
    size_t num_rx_samps_latest_call;

    uhd::rx_metadata_t md;
    
    //keep reading until everything is read


    while (num_rx_samps<total_num_samps) {
      
     
      num_rx_samps_latest_call=0;   

       
      while (num_rx_samps_latest_call==0) {
	 num_rx_samps_latest_call= 
	   rx_stream->recv(&buff_short[0],buffer_size, md, 3.0);
       };
      
       if (num_rx_samps_latest_call!=buffer_size) {
	 std::cerr << "I expect the buffer size to be always the same!\n";
	 std::cout<<"Read only:"<<num_rx_samps_latest_call<<"\n";
	 std::cout<<"Buffer:"<<buffer_size<<"\n";
         exit(1); 
	 //May be changed->don't want program to crash when data is not available
       };

       /* Process the just received buffer -> Put all toghether in one buffer*/
       int i1=2*num_rx_samps;
       int i2=0;
      
       while ((i1<(int) (2*total_num_samps)) && (i2<2*((int) buffer_size))) 
       {	  
	  storage_short[i1]=buff_short[i2];
	  i1++; i2++;
       };
	 
       num_rx_samps=num_rx_samps+num_rx_samps_latest_call;
       std::cout << "num_rx_samps=" << num_rx_samps  << std::endl;
    };


    // Save output to disc

    //Computes total power:
    double P=powerTotArray( storage_short, 2*total_num_samps);

    std::cout << "\nTotal power=" << P <<"\n";

    //Computes power in the output of 25 filters:
    int num_filt=25;
       
    double shiftindex = 0.04;//how big the shift between the filters is (equidistantial)

    double *power_band;
    power_band=new double[num_filt];

     double *buff_double;
     buff_double=new double[2*buffer_size];
    

    for(int ii=0;ii<(int)(2*buffer_size);ii++){
      	 buff_double[ii]=(double)buff_short[ii];

    };

         
    powerOfFreqBands(buff_double, 2*buffer_size, shiftindex, power_band,num_filt);
	     

     for(int ii=0; ii< num_filt; ii++){
       DispVal(power_band[ii]);
     };

    std::ofstream s1(filename.c_str(), std::ios::binary);   


    s1.write((char *) storage_short,4*total_num_samps); 
    s1.flush(); //PZ
    s1.close(); //PZ
    

    //Process the received data with MATLAB from written file

    //finished reading 
    std::cout << std::endl << "Done reading->Data to MATLAB!" << std::endl << std::endl;

    }else{

      //REAL TIME IMPLEMENTATION
      //Reads in loop untill it finds someone is trying to transmit -> Power detection and training sequence match//
    
    stream_cmd.num_samps = buffer_size;
    stream_cmd.stream_now = true;
    
    //stream_cmd.stream_mode=uhd::stream_cmd_t::STREAM_MODE_START_CONTINUOUS;

    //stream_cmd.time_spec = uhd::time_spec_t(seconds_in_future);

    dev->issue_stream_cmd(stream_cmd);

	  
    std::cout << "Stop the transmitter by pressing ctrl-c\n";
	  
      stream_cmd.num_samps = buffer_size;
      stream_cmd.stream_now = true;
    
       stream_cmd.stream_mode=uhd::stream_cmd_t::STREAM_MODE_START_CONTINUOUS;

       //stream_cmd.time_spec = uhd::time_spec_t(seconds_in_future);
       dev->issue_stream_cmd(stream_cmd);
       
       
       size_t num_rx_samps_latest_call;
       size_t num_rx_samps;
       
       double tp;
       double Power=0;
      
       uhd::rx_metadata_t md;
       int i1,i2;
       
       int sec_count=0;
       int one_sec=rx_rate/((int)buffer_size);
       
       int num_filt=25;
       
       double shiftindex = 0.04;//how big the shift between the filters is (equidistantial)
       
       float mean[2];
       short * buff_short_nomean;
       buff_short_nomean=new short[2*buffer_size];

       double *power_band;
       power_band=new double[num_filt];
     
       double *total_bandPower;
       total_bandPower=new double[num_filt];

       double *buff_double;
       buff_double=new double[2*(int)(rx_rate)];
       
       //Cycle to read continuously data

	 num_rx_samps=0;
	    
	 std::cout<<"buffer:"<<buffer_size<<"\n";

	 for(int ii=0;ii<num_filt;ii++){
	   total_bandPower[ii]=0.0;
	 }
	  
	 int loop_count=0;

	 int size_all=100;

	 double * all[size_all];
	 double * all_thread [size_all];

	 for(int ii=0;ii<size_all;ii++){
	   all[ii]= new double[2*(int)(rx_rate)];
	   all_thread[ii]=new double [2*(int)(rx_rate)];
	 }

	
	 total_num_samps= 1000;
	   
	 std::vector<std::thread> threads;

	 while(loop_count<100){
	  
	    DispVal(loop_count);
	    
	     

	   while (num_rx_samps<total_num_samps) {
	     
	      
	     
	     num_rx_samps_latest_call=0;   
	     	
       
	     //while (num_rx_samps_latest_call==0) {
	       num_rx_samps_latest_call= 
		 rx_stream->recv(&buff_short[0],buffer_size, md, 3.0);
	       //};

	     
	     if (num_rx_samps_latest_call!=buffer_size) {
	       std::cerr << "I expect the buffer size to be always the same!\n";
	       std::cout<<"Read only:"<<num_rx_samps_latest_call<<"\n";
	       std::cout<<"Buffer:"<<buffer_size<<"\n";
	       exit(1); 
	       //May be changed->don't want program to crash when data is not available
	     };

	     /* Process the just received buffer -> Put all toghether in one buffer*/
	     int i1=2*num_rx_samps;
	     int i2=0;
      
	     while ((i1<(int) (2*total_num_samps)) && (i2<2*((int) buffer_size))) 
	       {	  
		all[loop_count][i1]=(double)buff_short[i2];
		 i1++; i2++;
	       };
	 
	     num_rx_samps=num_rx_samps+num_rx_samps_latest_call;
	     //std::cout << "num_rx_samps=" << num_rx_samps  << std::endl;
  
	   };


	   //
	 std::clock_t c_start = std::clock();
	 auto t_start = std::chrono::high_resolution_clock::now();
	 
	 // powerOfFreqBands( &all[0][0], 100, shiftindex, power_band, num_filt);
	 
	 //tp =  powerTotArray( all[loop_count], 1000);

	 //copy(&all[0][0],&all[0][0] ,a1);
	 int loop_count2=loop_count;
	 
	 threads.push_back(std::thread(powerOfFreqBands, &all_thread[loop_count2][0], 100, shiftindex, power_band, num_filt));
	
	 //sleep(000);
	//DispVal(power_band[0]);

	 // for(long ii=0; ii<1000000;ii++) {
	 //   all[0][ii]=0;
	 // }


	 std::clock_t c_end = std::clock();
    auto t_end = std::chrono::high_resolution_clock::now();
 
    std::cout << "CPU time used: "
              << 1000000.0 * (c_end-c_start) / CLOCKS_PER_SEC
              << " us\n";
    std::cout << "Wall clock time passed: "
              << std::chrono::duration_cast<std::chrono::microseconds>(t_end - t_start).count()
              << " us\n";
	   

	   // for(int ii=0;ii<10; ii++){
	   //   DispVal(all[loop_count][ii]);
	   // }

	   //std::thread one{test,1};

	   // std::thread one;
	   
	   //  if(loop_count==0){
	   //    one(powerOfFreqBands, all[loop_count], 1000, shiftindex, power_band, num_filt );
	   //    //one.join();
	   //  }


	   // if(loop_count==99){
	   //   one.join();
	   //   loop_count=-1;
	   // }
	   
	   loop_count++;
	   num_rx_samps=0;

	  };

	 for (auto& th : threads) th.join();
	 
	 // while (loop_count<100000) {
	 //   loop_count++;
	 //   //DispVal(loop_count);
	   
	 //      num_rx_samps_latest_call=0;             
	 //      while (num_rx_samps_latest_call==0) {
	 // 	num_rx_samps_latest_call= 
	 // 	  rx_stream->recv(&buff_short[0],buffer_size, md, 0.1);
	 //      };
	      
	 //       if (num_rx_samps_latest_call!=buffer_size)  {
	 //       	 // std::cerr << "I expect the buffer size to be always the same!\n";
	 //       	 // std::cout<<"buffer:"<<buffer_size<<"\n";
	 // 	  std::cout<<"last:"<<num_rx_samps_latest_call<<"\n";
	 // 	 //exit(1); 
	 //       };
	       

	 //       //Computes the total power of the buffer:
	       
	 //       meanCplx(buff_short, 2*num_rx_samps_latest_call, mean);
	      
	 //       removeMean(buff_short, 2*num_rx_samps_latest_call, mean, buff_short_nomean);
	       
	 //       tp =  powerTotArray( buff_short_nomean, 2*num_rx_samps_latest_call);
	       
	 //        for(int ii=0;ii<(int)(2*num_rx_samps_latest_call);ii++){
	 //              buff_double[ii]=(double)buff_short[ii];
	 //        };
	       
	       
	 //       //powerOfFreqBands(buff_double, num_rx_samps_latest_call, shiftindex, power_band, num_filt);
	       

	 //       Power=Power+tp;

	 //       for(int iii=0;iii<num_filt;iii++){
	 // 	 total_bandPower[iii]=total_bandPower[iii]+power_band[iii];
	 //       };  


	 //       sec_count++;
	 //       //DispVal(sec_count);
	 //      if(sec_count>=one_sec){

	 //       for(int k=0;k<num_filt;k++){
	 //       	std::cout << "power: " << total_bandPower[k] << " " << std::endl;
	 //       }; 
	      
	     
	 //      std::cout << "Total power received last second =" << Power/one_sec <<"\n"; 
	 //      sec_count=0;
	 //      Power=0;
	    
	 //      }

	     
	 //    };
	 
    };

    //finished
    std::cout << std::endl << "Done receiving!" << std::endl << std::endl; 

    return 0;
}
