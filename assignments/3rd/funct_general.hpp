#include <iostream>
#include <complex>
#include <itpp/itbase.h>
#include <itpp/itsignal.h>

#include "rx_funct.cpp"

using namespace itpp;
using namespace std;

void square_elements_of_array(double data_to_harness[],int no_elements) ;

void create_data_CW (double buffer[], int total_num_samps, double nfreq, double Amp );


void compShortToCompDouble(complex<short> *array, int nElem, complex<double>* out);

void compDoubleToCompShort(complex<double> *array, int nElem, complex<short>* out);

void powerOfFreqBands(double data[],int nElem, double shiftindex,double power[],int numFilt);

void lowPassFilter(double x[],double y[],int nElemIN,double shift);
