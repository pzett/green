//Header of the Receiver

#include <iostream>
#include <cstdio>
#include <complex>
#include <stdlib.h>
#include <cmath>
#include <fstream>

using namespace std;

template <class T, class U>
void filter(U b[], int nElemB, U a[], int nElemA, T x[], T y[], int nElemIN);

template <class T>
void splitComp(T in[], T real[], T imag[], int nElem);

template <class T>
void compMerge(T real[], T imag[], T out[], int nElem);

template <class T, class U>
void filterComp(U b[], int nElemB, U a[], int nElemA, T x[], T y[], int nElemIN);

template<class T>
void meanCplx(T data[], int no_elements, float mean[]);

template<class T>
void removeMean(T data[], int no_elements, float mean[], T out[]);


template<class T>
double powerTotArray(T data[], int no_elements);

template <class T, class W>
void lowPassFilter(T x[],T y[],int nElemIN,W shift);

void powerOfFreqBands(double data[],int nElem, double shiftindex,double power[],int numFilt);
