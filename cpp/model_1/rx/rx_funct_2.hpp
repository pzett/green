#include <itpp/itbase.h>
#include <itpp/itsignal.h>
#include <complex>
#include <iostream>
#include <cmath>
#include <cstdio>
#include <stdio.h>      
#include <stdlib.h>

#include "arrayToCvec.cpp"
#include "filter_phase.cpp"

#include "common.cpp"
#include "rx_funct_2.cpp"


/** Make hard decision based on QPSK constellation
 * @pre:
 *     - data_bin: an array to be filled with 0 1. Length nElem*2
 *     - data_qpsk: complex with the datas. Length nElem 
 *     - nElem: length of data_qpsk
 *
 * @post:
 *     - data_bin is filled with 0 and 1 according to
 *
 * complex part (Q channel)
 *         ^
 *         |
 *  10 x   |   x 00   (odd bit, even bit)
 *         |
 *  -------+------->  real part (I channel)
 *         |
 *  11 x   |   x 01
 *         |
 */
void hardDetect(complex<double> data_qpsk[], short data_bin[], int nElem)


/** Return the index of the start of the training sequence and fill phase with its phase
 *
 * @pre:
 *    - dataC: array of the received data
 *    - nElem: size of dataC[]
 *    - trainC: pointer to array of the training sequence
 *             !!!  length assume to be < than nElem!!!
 *    - nElemT: size of trainC[]
 *    - phase: pointer to a double
 *
 * @post:
 *    - phase is now filled with the phase of trainC[index]
 *    - return the index of the beginning of the training sequence
 */
int synch(complex<double> dataC[], int nElem, complex<double> trainC[], int nElemT, double *phase)


/** Return the frequency offset (1st max- 2nd max) in the range [0.55:0.65[ 
 * @pre:
 *   - data: The data to analyse
 *
 *@post:
 *   - nu: the normalised frequency offset
 */
double freqOff(cvec data)


/** apply a square window on data_compl, output
 * 
 */
void matchedFilter(std::complex<double> data_compl[],int dataLength,std::complex<double> output[], int outLength)

/** Translate the date from buff_short into binary sequence data_bin
 *
 */
void receiverSignalProcessing(short buff_short[],int buffersize,short data_bin[])

