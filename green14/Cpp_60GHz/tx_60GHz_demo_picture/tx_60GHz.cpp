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

// This program has the same functionality as tx.cpp but for the // 60GHz tranceiver.

#include <fstream>
#include <uhd/utils/thread_priority.hpp>
#include <uhd/utils/safe_main.hpp>
#include <uhd/usrp/multi_usrp.hpp>
#include <uhd/usrp/subdev_spec.hpp>
#include <uhd/device.hpp>
#include <boost/program_options.hpp>
#include <boost/format.hpp>

#include <itpp/base/vec.h>
#include <itpp/itsignal.h>
#include <itpp/itbase.h>
#include <itpp/itcomm.h> 
#include <itpp/stat/misc_stat.h>
#include <itpp/signal/transforms.h>


#include <iostream>
#include <complex>
#include <uhd/types/clock_config.hpp>
//#include <gruel/realtime.h>
#include "board_60GHz.hpp"
#include "tx_funct.cpp"



namespace po = boost::program_options;

extern void tx_funct(int16_t output[],bvec data_bin,int nDataBin);
extern bvec prepairPic(int16_t picRaw[],int nPicRaw);

int UHD_SAFE_MAIN(int argc, char *argv[]){



    if (uhd::set_thread_priority_safe(1,true)) {
       std::cout << "set priority went well " << std::endl;
    };


    //variables to be set by po
    std::string args;
    double seconds_in_future;
    size_t total_num_samps;
    double tx_rate, freq, LOoffset;
    float gain;
    bool demoMode, use_8bits;
    bool use_external_10MHz; 
    std::string filename;
    uhd::tx_streamer::sptr tx_stream;
    uhd::device_addr_t dev_addr;
    uhd::usrp::multi_usrp::sptr dev;
    uhd::stream_args_t stream_args;

    //setup the program options
    po::options_description desc("Allowed options");
    desc.add_options()
      ("help", "help message")
      ("args", po::value<std::string>(&args)->default_value(""), "simple uhd device address args")
      ("secs", po::value<double>(&seconds_in_future)->default_value(3), "number of seconds in the future to transmit")
      ("nsamps", po::value<size_t>(&total_num_samps)->default_value(37028), "total number of samples to transmit")//9428
      ("txrate", po::value<double>(&tx_rate)->default_value(100e6/4), "rate of outgoing samples")
      ("freq", po::value<double>(&freq)->default_value(70e6), "rf center frequency in Hz")
      ("LOoffset", po::value<double>(&LOoffset)->default_value(0), "Offset between main LO and center frequency")
      ("demoMode",po::value<bool>(&demoMode)->default_value(true), "demo mode")
      ("10MHz",po::value<bool>(&use_external_10MHz)->default_value(false), 
       "external 10MHz on 'REF CLOCK' connector (true=1=yes)")
      ("filename",po::value<std::string>(&filename)->default_value("codedData.dat"), "input filename")
      ("gain",po::value<float>(&gain)->default_value(0), "gain of transmitter(0-13) ")
      ("8bits",po::value<bool>(&use_8bits)->default_value(false), "Use eight bits/sample to increase bandwidth")
    ;

    
    
    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    
    //print the help message
    if (vm.count("help")){
      std::cout << boost::format("tx %s") % desc << std::endl;
      return ~0;
    }
    
 ///////////////////////////////////////////////////////////////// START PROCESSING /////////////////////////////////////////////////////////////////////
    
    std::complex<int16_t> *buffer0;
    buffer0 = new std::complex<int16_t>[total_num_samps];
    std::complex<int16_t> *buffer1;
    buffer1 = new std::complex<int16_t>[total_num_samps];
    std::complex<int16_t> *buffer2;
    buffer2 = new std::complex<int16_t>[total_num_samps];
    std::complex<int16_t> *buffer3;
    buffer3 = new std::complex<int16_t>[total_num_samps];
    std::complex<int16_t> *buffer4;
    buffer4 = new std::complex<int16_t>[total_num_samps];

    
    int16_t *aux0;
    aux0 = new int16_t[2*total_num_samps];
    int16_t *aux1;
    aux1 = new int16_t[2*total_num_samps];
    int16_t *aux2;
    aux2 = new int16_t[2*total_num_samps];
    int16_t *aux3;
    aux3 = new int16_t[2*total_num_samps];
    int16_t *aux4;
    aux4 = new int16_t[2*total_num_samps];
    
    //generate the picture as grayscale
    int r=system("octave image_transmition.m &");
    if(r){
      std::cout<<" loading picture - check!\n";
    }
    
    int nPicRaw = 16384;//size of the image in grayscale 128*128
    double nBinPac = 27200;//size of binary data in one packet
 

    //loading picture as grayscale
    int16_t pictureRaw[nPicRaw];
    std::ifstream ifs( "data_toSend.dat", std::ifstream::in );
    ifs.read((char * )pictureRaw,nPicRaw*sizeof(int16_t));
    ifs.close();  
    
    //converting grayscale to binary and XOR with pseudonoise
    itpp::bvec picBinInter = prepairPic(pictureRaw,nPicRaw);//transforms grayscale in binary data

    //cutting the large binary data into 5 packets
    bvec dataBinTmp0;
    dataBinTmp0.ins(dataBinTmp0.length(),picBinInter.get(0,(nBinPac-1)));
    bvec dataBinTmp1;
    dataBinTmp1.ins(dataBinTmp1.length(),picBinInter.get(nBinPac,(2*nBinPac-1)));
    bvec dataBinTmp2;
    dataBinTmp2.ins(dataBinTmp2.length(),picBinInter.get(2*nBinPac,(3*nBinPac-1)));
    bvec dataBinTmp3;
    dataBinTmp3.ins(dataBinTmp3.length(),picBinInter.get(3*nBinPac,(4*nBinPac-1)));
    bvec dataBinTmp4;
    dataBinTmp4.ins(dataBinTmp4.length(),picBinInter.get(4*nBinPac,picBinInter.length()));
    dataBinTmp4.ins(dataBinTmp4.length(),randb(nBinPac-dataBinTmp4.length())); //filling the last packet with random data

    //saving the binary picture
    it_file my_file("binPicture.it");
    my_file << Name("picBinInter") << picBinInter;
    my_file.flush();
    my_file.close();
    
    
    //processing each packet
    tx_funct(aux0,dataBinTmp0,dataBinTmp0.length());
    tx_funct(aux1,dataBinTmp1,dataBinTmp1.length());
    tx_funct(aux2,dataBinTmp2,dataBinTmp2.length());
    tx_funct(aux3,dataBinTmp3,dataBinTmp3.length());
    tx_funct(aux4,dataBinTmp4,dataBinTmp4.length());
    
    //filling the output buffer
    for(int i=0,count1=0;i<(int)(2*total_num_samps);i=i+2){
      buffer0[count1]=std::complex<short>(aux0[i],aux0[i+1]);
      buffer1[count1]=std::complex<short>(aux1[i],aux1[i+1]);
      buffer2[count1]=std::complex<short>(aux2[i],aux2[i+1]);
      buffer3[count1]=std::complex<short>(aux3[i],aux3[i+1]);
      buffer4[count1]=std::complex<short>(aux4[i],aux4[i+1]);
      count1++;
    }
 
    
   
    // Save data to file to check what was sent
    std::ofstream ofs( "sent0.dat" , std::ifstream::out );
    ofs.write((char * ) buffer0, 2*total_num_samps*sizeof(int16_t));
    ofs.flush();
    ofs.close();
    // Save data to file to check what was sent
    std::ofstream ofs1( "sent1.dat" , std::ifstream::out );
    ofs1.write((char * ) buffer1, 2*total_num_samps*sizeof(int16_t));
    ofs1.flush();
    ofs1.close();
    // Save data to file to check what was sent
    std::ofstream ofs2( "sent2.dat" , std::ifstream::out );
    ofs2.write((char * ) buffer2, 2*total_num_samps*sizeof(int16_t));
    ofs2.flush();
    ofs2.close();
    // Save data to file to check what was sent
    std::ofstream ofs3( "sent3.dat" , std::ifstream::out );
    ofs3.write((char * ) buffer3, 2*total_num_samps*sizeof(int16_t));
    ofs3.flush();
    ofs3.close();
    // Save data to file to check what was sent
    std::ofstream ofs4( "sent4.dat" , std::ifstream::out );
    ofs4.write((char * ) buffer4, 2*total_num_samps*sizeof(int16_t));
    ofs4.flush();
    ofs4.close();

    //Conjugate!!!
    for(int i=0; i<(int)(total_num_samps);i++){
      buffer0[i]=std::conj(buffer0[i]);
      buffer1[i]=std::conj(buffer1[i]);
      buffer2[i]=std::conj(buffer2[i]);
      buffer3[i]=std::conj(buffer3[i]);
      buffer4[i]=std::conj(buffer4[i]);
    } 
    
    

    std::cout << " ----------- " << std::endl;
    std::cout<<" Conjugated! \n";
    std::cout << " ----------- " << std::endl;
    
    ///////////////////////////////////////////////////////////////// END  PROCESSING /////////////////////////////////////////////////////////////////////
    
    //create a usrp device and streamer
    dev_addr["addr0"]="192.168.10.2";
    dev = uhd::usrp::multi_usrp::make(dev_addr);    


    // Internal variables 
    uhd::clock_config_t my_clock_config; 

    if (!demoMode) {
      dev->set_time_source("external");
    };

    if (use_external_10MHz) { 
      dev->set_clock_source("external");
    }
    else {
      dev->set_clock_source("internal");
    };


    uhd::usrp::dboard_iface::sptr db_iface;
    db_iface=dev->get_tx_dboard_iface(0);

    board_60GHz_TX  my_60GHz_TX(db_iface);  //60GHz 
    my_60GHz_TX.set_gain(gain); // 60GHz 

    uhd::tune_result_t tr;
    uhd::tune_request_t trq(freq,LOoffset); //std::min(tx_rate,10e6));
    tr=dev->set_tx_freq(trq,0);
    

    //dev->set_tx_gain(gain);
    std::cout << tr.to_pp_string() << "\n";
 

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


    

    uhd::tx_metadata_t md;


    if(demoMode){

    dev->set_time_now(uhd::time_spec_t(0.0));
    md.start_of_burst = true;
    md.end_of_burst = false;
    md.has_time_spec = false;
    md.time_spec = uhd::time_spec_t(seconds_in_future);

   
    tx_stream->send(buffer0,total_num_samps,md,60);
    
    tx_stream->send(buffer1,total_num_samps,md,3);
    
    tx_stream->send(buffer2,total_num_samps,md,3);

    tx_stream->send(buffer3,total_num_samps,md,3);

    tx_stream->send(buffer4,total_num_samps,md,3);

    tx_stream->send(buffer4,total_num_samps,md,3);

    md.start_of_burst = false;

    std::cout << " " << std::endl;
    std::cout<< "picture transmitted once!" << std::endl;
    std::cout << " " << std::endl;
    
    int f=system("octave toMatlab.m");
    if(f){
      std::cout << " Programm Paused - Press Any Key To leave! " << std::endl;
    }


    }
    else
    {
    
    dev->set_time_now(uhd::time_spec_t(0.0));
    md.start_of_burst = true;
    md.end_of_burst = false;
    md.has_time_spec = false;
    md.time_spec = uhd::time_spec_t(seconds_in_future);

      
    tx_stream->send(buffer0,total_num_samps,md,60);
    
    tx_stream->send(buffer1,total_num_samps,md,3);
    
    tx_stream->send(buffer2,total_num_samps,md,3);

    tx_stream->send(buffer3,total_num_samps,md,3);

    tx_stream->send(buffer4,total_num_samps,md,3);

    tx_stream->send(buffer4,total_num_samps,md,3);

    md.start_of_burst = false;

    std::cout << " " << std::endl;
    std::cout<< "picture transmitted once!" << std::endl;
    std::cout << " " << std::endl;

    };

    //finished
    std::cout << std::endl << "Done!" << std::endl << std::endl;


    return 0;
}





