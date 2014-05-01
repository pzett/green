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

// The Kalman Gain is defined in another file (normally in *.hpp file)

extern std::queue<itpp::vec> kalmanGain (std::queue<itpp::vec> Pilot, int nPilot, double F, double G, double H, double R1, double R2, vec x0, double Q0);

void print(std::queue<itpp::vec> s)
{
  int sizeQueue=s.size();
  DispVal(sizeQueue);
 
    while(sizeQueue!=0){
      itpp::vec x= s.front();
      s.pop();
      std::cout << x << "\n";
      sizeQueue--;
    }
    return;
}


int UHD_SAFE_MAIN(int argc, char *argv[]){
    

    std::cout << "This program is goint to test C++ Kalman_gain using matlab as a reference. \n";
    
    /***************************************************************************************/
 

    //variables to be set by from command line 
   
    int nPilot;
    double F;
    double G;
    double H;
    double R1;
    double R2; 
    double Q0;
    int data_size;

    //Program options start

    po::options_description desc("Allowed options");
    desc.add_options()
        ("help", "help message")
         /* command-line perfix, type, help string  */
      ("nPilot", po::value<int>(&nPilot)->default_value(0), "")
      ("F", po::value<double>(&F)->default_value(0), "")
      ("G", po::value<double>(&G)->default_value(0), "")
      ("H", po::value<double>(&H)->default_value(0), "")
      ("R1", po::value<double>(&R1)->default_value(0), "")
      ("R2", po::value<double>(&R2)->default_value(0), "")
      ("Q0", po::value<double>(&Q0)->default_value(0), "")
      ("data_size", po::value<int>(&data_size)->default_value(0), "");
     
    
    
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
    double * data_r;
    data_r= new double[data_size];

    double * data_init;
    data_init= new double[nPilot];


    // Read data from file
    std::ifstream ifs_data( "data_to_harness.dat" , std::ifstream::in );
    ifs_data.read((char * ) data_r,data_size*sizeof(double));
    ifs_data.close();

    // Read data from file
    std::ifstream ifs_train( "init_to_harness.dat" , std::ifstream::in );
    ifs_train.read((char * )data_init, nPilot*sizeof(double));
    ifs_train.close();

    // for(int ii=0;ii<data_size;ii++){
    //   DispVal(data_r[ii]);
    //   DispVal(ii);
    // };
    

    // for(int ii=0;ii<data_init_size;ii++){
    //   DispVal(data_init[ii]);
    //   DispVal(ii);
    // };

    //save data in queue:
    std::queue<vec> Pilot;
    
    itpp::vec aux(nPilot);
    for(int i=0; i<data_size;i=nPilot+i){
      for(int k=0;k<nPilot;k++){
	aux.set(k,data_r[i+k]);
      }
      Pilot.push(aux);
    }
   
    vec x0(nPilot);
    for(int i=0;i<nPilot;i++){
      x0.set(i,data_init[i]);
    }

    //print(Pilot);
    

    //Call the function
    std::queue<itpp::vec> filtPilot=  kalmanGain ( Pilot, nPilot, F, G, H, R1, R2,  x0,  Q0);
    
    //print(filtPilot);
    
    
     int sizeQueue=filtPilot.size();
     double result[data_size];
     int count=0;
    while(sizeQueue!=0){
      itpp::vec x= filtPilot.front();
      filtPilot.pop();
      for(int i=0;i<nPilot;i++){
	result[count]=x[i];
	  count++;
      }
      sizeQueue--;
    }

     // Save data to file
     std::ofstream ofs1( "data_from_harness.dat" , std::ifstream::out );
     ofs1.write((char * )result, data_size*sizeof(double));
     ofs1.close();
 

    return 0;
}
