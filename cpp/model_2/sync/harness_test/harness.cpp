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
#include <itpp/itbase.h>
#include <itpp/itcomm.h> 
#include "arrayToCvec.cpp"


namespace po = boost::program_options;

using namespace std;
using namespace itpp;
 
// The sync_catch_channel is defined in another file (normally in *.hpp file):

extern cvec synchCatchChannel(cvec dataC, cvec trainCUp, int Post, int Pre , double *phiHat, double *AHat, int *delay, double *sigmaSqrNoise );

#define DispVal(X) std::cout << #X << " = " << X<< std::endl

template<class T>
void hold(T data[], T out[], int Q, int nElem){
  int c=0;
  for (int i=0; i<nElem;i++){
    for (int d=0;d<Q;d++){
    
      out[c+d]=data[i];
      
    }
    c=c+Q;
  }
}



int UHD_SAFE_MAIN(int argc, char *argv[]){
    


    std::cout << "This program is goint to test C++ filter_phasesync_catch_phase using matlab as a reference. \n";
    
    /***************************************************************************************/
 

    //variables to be set by from command line 
    int data_size;
    //std::string dummy_string;
    int train_size;
    int delay, Post, Pre;
    double phiHat, AHat, sigmaSqrNoise;
    itpp::cvec thetaEst;


    //Program options start

    po::options_description desc("Allowed options");
    desc.add_options()
        ("help", "help message")
         /* command-line perfix, type, help string  */
        ("data_size", po::value<int>(&data_size)->default_value(10), "number of elements in data")
      ("train_size", po::value<int>(&train_size)->default_value(10), "number of elements in train")
      ("length_pre", po::value<int>(&Pre)->default_value(0), "length_pre")
      ("length_post", po::value<int>(&Post)->default_value(0), "length_post");
     
    
    
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

    //Upsample train seq-> Done outside function:
   int Q=4;
   std::complex<double> trainSeqUp[Q*train_size];
   hold(train_r, trainSeqUp, Q, train_size);
    
    cvec dataC=arrayToCvec(data_r, data_size); 
    cvec trainC=arrayToCvec(trainSeqUp, Q*train_size); 
    
    
    // Call the function
    
    thetaEst = synchCatchChannel(dataC, trainC, Pre, Post , &phiHat,  &AHat, &delay, &sigmaSqrNoise );

    std::cout<<"Channel="<<thetaEst<<"\n";
    std::cout <<" tsamp="<<delay<<"\n";
    std::cout<<" phase_hat="<<phiHat<<"\n";
    std::cout<<" A_hat="<<AHat<<"\n";
    std::cout<<" sigma_square_noise="<<sigmaSqrNoise<<"\n";

    // for(int ii=0;ii<res_size;ii++){
    //    DispVal(res[ii]);
    //    DispVal(ii);
    //  };

    
    return 0;
}
