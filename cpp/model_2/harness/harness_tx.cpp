//FILTER_PHASE HARNESS TEST
//
//
//
// Copyright 2010 
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


#include <fstream>
#include <uhd/utils/thread_priority.hpp>
#include <uhd/utils/safe_main.hpp>
#include <uhd/usrp/multi_usrp.hpp>
#include <uhd/device.hpp>
#include <boost/program_options.hpp>
#include <boost/format.hpp>
#include <iostream>
#include <complex>
#include <uhd/types/clock_config.hpp>
#include <complex>

namespace po = boost::program_options;

using namespace std;

#define DispVal(X) std::cout << #X << " = " << X<< std::endl

extern double dummy_float;
 
// The filter_phase is defined in another file (normally in *.hpp file):

extern void tx_funct(  int N,int nUsedC,int nPilotC,int pre,int post,int nDataBin, int nPilotData,int nGuard,double data_pattern[],double data_bin[],double data_pil[],double pilot_pattern[],short output[]);



int UHD_SAFE_MAIN(int argc, char *argv[]){
    


    std::cout << "This program is goint to test C++ filter_phase using matlab as a reference. \n";
    
    /***************************************************************************************/
 

    //variables to be set by from command line 
    int nCar;
    int nUsedC;
    int nPilotC;
    int pre;
    int post;
    int nDataBin;
    int nPilotData;
    int nBits;


    //Program options start

    po::options_description desc("Allowed options");
    desc.add_options()
        ("help", "help message")
         /* command-line perfix, type, help string  */
      ("nCar", po::value<int>(&nCar)->default_value(128), "number of carriers")
      ("nUsedC", po::value<int>(&nUsedC)->default_value(89), "number of used carriers")
      ("nPilotC", po::value<int>(&nPilotC)->default_value(10), "number of pilots")
      ("pre", po::value<int>(&pre)->default_value(18), "number")
      ("post", po::value<int>(&post)->default_value(1), "number of pilots")
      ("nDataBin", po::value<int>(&nDataBin)->default_value(100000), "number of pilots")
      ("nPilotData", po::value<int>(&nPilotData)->default_value(40000), "number of pilots")
      ("nBits", po::value<int>(&nBits)->default_value(1), "number per axis in the modulation");

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    //print the help message
    if (vm.count("help")){
        std::cout << boost::format("harness: %s") % desc << std::endl;
        return ~0;
    }

    // Program options end


    //Load data and initialization parameters
    //Read data from a file
    double data_bin[nDataBin];
    std::ifstream ifs( "dataBinNPaq.dat" , std::ifstream::in );
    ifs.read((char * )data_bin,nDataBin*sizeof(double));
    ifs.close();

    //Read pilots from a file;
    double data_pil[nPilotData];
    std::ifstream ifs2( "dataPilotN.dat" , std::ifstream::in );
    ifs2.read((char * )data_pil,nPilotData*sizeof(double));
    ifs2.close();

    //Read data pattern from a file
    double data_pattern[nUsedC];
    std::ifstream ifs3( "dataPattern.dat" , std::ifstream::in );
    ifs3.read((char * )data_pattern,nUsedC*sizeof(double));
    ifs3.close();

    //Read pilot pattern from a file
    double pilot_pattern[nPilotC];
    std::ifstream ifs4( "pilotPattern.dat" , std::ifstream::in );
    ifs4.read((char * )pilot_pattern,nPilotC*sizeof(double));
    ifs4.close();


    
    // Call the function

    int nGuard = 10;
    int nTrain = 20;
    //short output[ceil(nDataBin/(2*nBits)/nUsedC)*nCar+nGuard+4*nTrain];
   
    //short output[ceil(nDataBin/(2*nBits)/nUsedC)*nCar+nGuard+4*nTrain];
    int outputSize =(1+ceil((nDataBin/(2*nBits))/nUsedC))*(nCar+pre+post)+2*nGuard+4*nTrain;
    std::cout << "Output buffer" << outputSize << std::endl;
    short output[2*outputSize];
    
    tx_funct(nCar,nUsedC,nPilotC,pre,post,nDataBin,nPilotData,nBits,data_pattern,data_bin,data_pil,pilot_pattern,output);

    //std::cout << "number_of_elements=" << res_size << std::endl;

    // for(int ii=0;ii<res_size;ii++){
    //    DispVal(res[ii]);
    //    DispVal(ii);
    //  };

     // Save data to file
     std::ofstream ofs( "data_from_harness.dat" , std::ifstream::out );
     ofs.write((char * )output,2*outputSize*sizeof(short));
     ofs.close();
     std::cout << "Data written" << std::endl;
    
    return 0;
}
