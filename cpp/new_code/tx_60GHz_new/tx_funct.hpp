
#include <algorithm>
#include <iostream>
#include <fstream>
#include <cmath>

#include <itpp/base/vec.h>
#include <itpp/itsignal.h>
#include <itpp/itbase.h>
#include <itpp/itcomm.h> 
#include <itpp/stat/misc_stat.h>
#include <itpp/signal/transforms.h>

#include <complex>
#include <itpp/comm/modulator.h>
#include <stdio.h> 
#include <math.h>  


 /*
 This function prepares a vector of NCarriers to be transmitted with OFDM.
 */
using namespace std;
using namespace itpp;

void tx_funct(int16_t output[]);
