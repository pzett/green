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

extern int filter_phase(complex<double> * data, int data_size, double phi_hat, complex<double> * train, int train_size, double Q0, double sigma_phi_sqr, double sigma_m_sqr, complex<double> * out );



int UHD_SAFE_MAIN(int argc, char *argv[]){
    


    std::cout << "This program is goint to test C++ filter_phase using matlab as a reference. \n";
    
    /***************************************************************************************/
 

    //variables to be set by from command line 
    int data_size;
    //std::string dummy_string;
    int train_size;
    double phi_hat;


    //Program options start

    po::options_description desc("Allowed options");
    desc.add_options()
        ("help", "help message")
         /* command-line perfix, type, help string  */
        ("data_size", po::value<int>(&data_size)->default_value(10), "number of elements in data")
      ("train_size", po::value<int>(&train_size)->default_value(10), "number of elements in train")
      ("phi_hat", po::value<double>(&phi_hat)->default_value(0), "phi_hat");

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
    complex<double> * data_r;
    data_r= new complex<double>[data_size];

    complex<double> * train_r;
    train_r= new complex<double>[train_size];


    // Read data from file
    std::ifstream ifs_data( "data_to_harness.dat" , std::ifstream::in );
    ifs_data.read((char * ) data_r,2*data_size*sizeof(double));
    ifs_data.close();

    // Read data from file
    std::ifstream ifs_train( "train_to_harness.dat" , std::ifstream::in );
    ifs_train.read((char * )train_r,2*train_size*sizeof(double));
    ifs_train.close();

    // for(int ii=0;ii<data_size;ii++){
    //   DispVal(data_r[ii]);
    //   DispVal(ii);
    // };
    

    // for(int ii=0;ii<train_size;ii++){
    //   DispVal(train_r[ii]);
    //   DispVal(ii);
    // };
    
    // Call the function
    
    complex<double> * res;
    
    int res_size=data_size-train_size;

    res= new complex<double>[res_size];
    

    res_size= filter_phase( data_r, data_size,  phi_hat, train_r, train_size, 0.01,0.01, 0.01, res);

    std::cout << "number_of_elements=" << res_size << std::endl;

    // for(int ii=0;ii<res_size;ii++){
    //    DispVal(res[ii]);
    //    DispVal(ii);
    //  };

    DispVal(res_size);
     // Save data to file
     std::ofstream ofs1( "data_from_harness.dat" , std::ifstream::out );
     ofs1.write((char * )res, 2*res_size*sizeof(double));
     ofs1.close();

    
    return 0;
}
