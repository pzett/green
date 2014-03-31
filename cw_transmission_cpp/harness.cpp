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
extern double dummy_float;
 
// The square_elements_of_array is defined in another file (normally in *.hpp file):
extern void square_elements_of_array(float data_to_harness[],int no_elements) ;

namespace po = boost::program_options;

int UHD_SAFE_MAIN(int argc, char *argv[]){
    
  #if 0
    if (!(uhd::set_thread_priority_safe(1,true))) {
      std::cout << "Problem setting thread priority" << std::endl;
      return 1;
    };
   #endif

    std::cout << "This program is an example of how to test C++ implementations using matlab as a reference. \n";
    std::cout << "For more information open the source code file and search for the section of comment lines \n";
    std::cout << "begining with README \n \n";
    /***************************************************************************************/
    /* README: */
    /*
       The principles of comparing matlab and C++ implementations are the following.
       First isolate (i.e. clearly define) the function you are going to test.
       In this case the function is square_elements_of_array. The matlab implementation
       is found in the file square_elements_of_array.m. This function is trivial but serves
       as an example of the methodology. In the file harness.m we test the C++ implementation
       void square_elements_of_array(float input_vector[],number_of_elements), which
       is contained in the file square_elements_of_array.cpp. The file harness.c is used
       to provide an interface between the matlab script harness.m and the function to
       be tested which is located in square_elements_of_array.cpp. 
       The harness works as follows.  The matlab script harness.m randomizes a vector of floats.
       It writes these floats in the data file data_to_harness.dat. It then calls the program
       harness with the number of elements as a command line parameter. This is done
       using the matlab function "system" which exekvates commands in a bash shell (i.e. like
       a terminal window). The harness loads the floats from the file, calls square_elements_of_array,
       writes the results to the file data_from_harness.dat and exits. The control is now
       returned to the matlab script harness.m which loads the values from file into the
       vector result_from_harness. Finally, the matlab script harness.m calls the matlab
       reference implementation square_elements_of_array and compares the result
       with that of C++ implementation by plotting the results. In practice, more complex
       comparison methods may be neccessary e.g. if the outputs are matrices.
       Note harness is a real program which can be called from command line (e.g. bash),
       although it takes most of it's inputs from file. The code between comments "program options
       start" and "program options end" performance the loading of parameters from the command
       line. In our case there is only one real parameter namely "number_of_elements"
       this parameter is passed to the program as e.g.:

       > sudo ./harness --no_elements=15   

       To illustrate the use of command line parameters we have added dummy_string and
       and dummy_float as well. The arguments can be provided in any order from the command line.
       Note that default values can also be set (study code below). 
       In the matlab script harness.m we call the harness program by the following code:

       cmd_str=['sudo ./harness '];
       cmd_str=[cmd_str,' --no_elements=',num2str(length(data_to_harness))];
       system(cmd_str);


       This first two lines creates a string with the exact same content as the command line
       above. The third line opens a bash shell to execute it and waits for it to complete.

       Why do we use "sudo" on everything? Answer: because we run everything with high priority
       which gives the highest speed. 

       To compile the harness write "make harness". When you implement important functions
       in your project, make one harness for each e.g. harness_decoder,        
       
    */

    //variables to be set by from command line 
    int number_of_elements;
    std::string dummy_string;
    float dummy_float;


    //Program options start

    po::options_description desc("Allowed options");
    desc.add_options()
        ("help", "help message")
         /* command-line perfix, type, help string  */
        ("no_elements", po::value<int>(&number_of_elements)->default_value(10), "number of elements in vector")
        ("dummy_str",po::value<std::string>(&dummy_string)->default_value("Default string"), "dummy string") 
        ("dummy_fl",po::value<float>(&dummy_float)->default_value(3.14), "dummy float") 
    ;
    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    //print the help message
    if (vm.count("help")){
        std::cout << boost::format("harness: %s") % desc << std::endl;
        return ~0;
    }

    // Program options end


    std::cout<< "dummy_string=" << dummy_string << std::endl;
    std::cout<< "dummy_float=" << dummy_float << std::endl;


    // Create storage for the input
    float data_to_harness[number_of_elements];

    // Read data from file
    std::ifstream ifs( "data_to_harness.dat" , std::ifstream::in );
    ifs.read((char * )data_to_harness,number_of_elements*sizeof(float));
    ifs.close();


    // Call the function
    square_elements_of_array(data_to_harness,number_of_elements) ;

    std::cout << "number_of_elements=" << number_of_elements << std::endl;

    // Save data to file
    std::ofstream ofs( "data_from_harness.dat" , std::ifstream::out );
    ofs.write((char * )data_to_harness,number_of_elements*sizeof(float));
    ofs.close();


    
    
    return 0;
}
