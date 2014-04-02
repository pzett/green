#include <iostream>
#include <complex>
#include <itpp/itbase.h>
#include <itpp/itsignal.h>


using namespace itpp;
using namespace std;

void square_elements_of_array(float data_to_harness[],int no_elements) ;

void create_data_CW (double buffer[], int total_num_samps, double nfreq, double Amp );

float powerTotArray( short  data[], int no_elements);

void compShortToCompDouble(complex<short> *array, int nElem, complex<double>* out);

void compDoubleToCompShort(complex<double> *array, int nElem, complex<short>* out);
