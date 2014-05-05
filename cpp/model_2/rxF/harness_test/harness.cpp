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
#include <vector>
#include <queue>
#include <itpp/itbase.h>
#include <itpp/itcomm.h> 
#include <itpp/stat/misc_stat.h>
#include <itpp/itsignal.h>
#include <itpp/itcomm.h>


namespace po = boost::program_options;

using namespace std;
using namespace itpp;
 

#define DispVal(X) std::cout << #X << " = " << X<< std::endl

// The Receiver signal Processing is defined in another file (normally in *.hpp file)

extern void receiverSignalProcessing(short buff_short[], int buffersize,short data_bin[], int nDataC);


// void print(std::queue<itpp::vec> s)
// {
//   int sizeQueue=s.size();
//   DispVal(sizeQueue);
 
//     while(sizeQueue!=0){
//       itpp::vec x= s.front();
//       s.pop();
//       std::cout << x << "\n";
//       sizeQueue--;
//     }
//     return;
// }


int UHD_SAFE_MAIN(int argc, char *argv[]){
    

    std::cout << "This program is goint to test C++ Second model receiver using matlab as a reference. \n";
    
    /***************************************************************************************/
 

    //variables to be set by from command line 
   
    int buffersize;
    int nDataB;

    //Program options start

    po::options_description desc("Allowed options");
    desc.add_options()
        ("help", "help message")
         /* command-line perfix, type, help string  */
      ("buffersize", po::value<int>(&buffersize)->default_value(0), "")
      ("nDataB", po::value<int>(&nDataB)->default_value(0), "");
     
    
    
    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    //print the help message
    if (vm.count("help")){
        std::cout << boost::format("harness: %s") % desc << std::endl;
        return ~0;
    }

    // Program options end

  

    // Create storage for the input
    short * data_r;
    data_r= new short[buffersize];



    // Read data from file
    std::ifstream ifs_data( "data_to_harness.dat" , std::ifstream::in );
    ifs_data.read((char * ) data_r,buffersize*sizeof(short));
    ifs_data.close();


    // for(int ii=0;ii<buffersize;ii++){
    //   DispVal(data_r[ii]);
    //   DispVal(ii);
    // };
 
    

    //Call the function
    short data_bin[nDataB];
    
    receiverSignalProcessing(data_r,buffersize, data_bin, nDataB);


     // Save data to file
     // std::ofstream ofs1( "data_from_harness.dat" , std::ifstream::out );
     // ofs1.write((char * )result, data_size*sizeof(double));
     // ofs1.close();
 

    return 0;
}
