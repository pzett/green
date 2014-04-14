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

extern void receiverSignalProcessing(short buff_short[], int buffersize,short data_bin[], int nDataC);


namespace po = boost::program_options;

using namespace std;

#define DispVal(X) std::cout << #X << " = " << X<< std::endl

extern double dummy_float;
 

int UHD_SAFE_MAIN(int argc, char *argv[]){
    


    std::cout << "This program is goint to test C++ filter_phase using matlab as a reference. \n";
    
    /***************************************************************************************/
 

    //variables to be set by from command line 
    int buffer_size;


    //Program options start

    po::options_description desc("Allowed options");
    desc.add_options()
        ("help", "help message")
         /* command-line perfix, type, help string  */
      ("buffer_size", po::value<int>(&buffer_size)->default_value(55680), "number of elements in data");

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    //print the help message
    if (vm.count("help")){
        std::cout << boost::format("harness imp_1: %s") % desc << std::endl;
        return ~0;
    }

    // Program options end


    
    // Create storage for the input
    short * buff_short;
    buff_short= new short[buffer_size];


    // Read data from file
    std::ifstream ifs_data( "data_to_harness.dat" , std::ifstream::in );
    ifs_data.read((char * ) buff_short, buffer_size*sizeof(short));
    ifs_data.close();

   


    for(int ii=buffer_size-20;ii<buffer_size;ii++){
      DispVal(buff_short[ii]);
      DispVal(ii);
    };
    
    // Call the function
    short * data_bin;
    int nDataB=6250;
    data_bin = new short[nDataB];

   
    receiverSignalProcessing( buff_short, buffer_size, data_bin, nDataB);

    std::cout << "number_of_elements=" << nDataB << std::endl;

    // for(int ii=0;ii<res_size;ii++){
    //    DispVal(res[ii]);
    //    DispVal(ii);
    //  };

    
     // Save data to file
     std::ofstream ofs( "data_from_harness.dat" , std::ifstream::out );
     ofs.write((char * ) data_bin, nDataB*sizeof(short));
     ofs.close();

    
    return 0;
}
