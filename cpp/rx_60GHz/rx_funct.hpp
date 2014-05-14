#include <complex>
#include <iostream>
#include <vector>
#include <queue> 
#include <cmath>     
#include <string>

#include <itpp/itbase.h>
#include <itpp/itcomm.h> 
#include <itpp/stat/misc_stat.h>


using namespace std;
using namespace itpp;

#define DispVal(X) std::cout << #X << " = " << X<< std::endl

#define M_PI 3.14159265358979323846

void receiverSignalProcessing(short buff_short[], int buffersize,short data_bin[], int nDataC);
