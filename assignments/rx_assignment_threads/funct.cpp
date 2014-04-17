//Functions of the Receiver 

#include <iostream>

using namespace std;




template<class T>
void removeMean(T data[], int no_elements, float mean[], T out[])
{
  for (int i=0;i<no_elements;i++){
    if (i%2==0){	
	out[i]=data[i]-mean[0];
      }
    else {
      out[i]=data[i]-mean[1];
    }
  };
};

template<class T>
void meanCplx(T data[], int no_elements, float mean[])
{
  float valueReal=0;
  float valueImag=0;
  for (int i=0;i<no_elements;i++){
    if (i%2==0){	
	valueReal=(valueReal+data[i])/2;
      }
    else {
	valueImag=(valueImag+data[i])/2;
    }
      mean[0]=valueReal;
      mean[1]=valueImag;
  };
};




/** Filter the real array x by the filter h(q)=b(q)/a(q)
 *
 *                             b[0]+b[1]*q^(-1)+...+b[nElemB-1]*q^(nElemB-1)
 *                       h(q)=----------------------------------------------
 *                              1+a[1]*q^(-1)+...+a[nElemA-1]*q^(nElemA-1)
 * @pre:
 *    - b: pointer to array of the numerator of h(q) b[n]
 *    - nElemB: size of b[]
 *    - a: pointer to array of the numerator of h(q) a[n]
 *             !!!  a[0]=1 !!!
 *    - nElemA: size of a[]
 *    - x: the array to filter
 *    - y: the array of filtered elements, size nElemIN
 *    - nElemIN: the size of x[] and y[]
 *
 * @post:
 *    - y is now filled
 *
 */
template <class T, class U>
void filter(U b[], int nElemB, U a[], int nElemA, T x[], T y[], int nElemIN){
  if (a[0]!=1){cerr << "a[0] not equal to 1";}
  for (int i=0;i<nElemIN;i++){
    T xn[nElemB];
    // Create an array with x[n], x[n-1], ..., x[n-(nElemB-1)] 
    for (int ii=0;ii<nElemB;ii++){    
      if (i-ii>=0){      
	xn[ii]=x[i-ii];
      } // If out of range, pad with zeros
      else{
	xn[ii]=0;
      }
      //DispVal(xn[ii]);
    }
    T yn[nElemA];
    yn[0]=0;
    // Create an array with 0, y[n-1], ..., y[n-(nElemA-1)]
    for (int ii=1; ii<nElemA; ii++){
      if (i-ii>=0){
	yn[ii]=y[i-ii];
      } // If out of range, pad with zeros
      else{
	yn[ii]=0;
      }
      // DispVal(yn[ii]);
    }

    // Compute y[i]
    //cout<<"here! "<< i <<"\n";
    y[i]=0; // Initialisation
    for (int ii=0;ii<nElemB;ii++){
      y[i]=y[i]+b[ii]*xn[ii];
    }
    for (int ii=1;ii<nElemA;ii++){
      y[i]=y[i]-a[ii]*yn[ii];
    }
    //DispVal(y[i]);
    //cout << "\n";
  }
};

/** Split a complex array into an array of real and an array of imag
 *
 * @pre:
 *    - in: pointer to complex array to split
 *    - real: pointer to array that will contain the real part
 *                            size of nElem/2
 *    - imag: pointer to array that will contain the real part
 *                            size of nElem/2
 *    - nElem: the size of in[]
 *
 * @post:
 *    - real is now filled
 *    - imag is now filled
 */
template <class T>
void splitComp(T in[], T real[], T imag[], int nElem){
  for (int i=0;i<nElem;i=i+2){
    real[i]=in[i];
    imag[i]=in[i+1];
  }
};

/** Merge an array of real and an array of imag into an array of complex
 *
 * @pre:
 *    - in: pointer to complex array to split
 *    - real: pointer to array that contains the real part
 *                            size of nElem
 *    - imag: pointer to array that contains the real part
 *                            size of nElem
 *    - out: pointer that will contain the complex array
 *                            size of 2*nElem
 *    - nElem: the size of real[]
 *
 * @post:
 *    - out is now filled
 */
template <class T>
void compMerge(T real[], T imag[], T out[], int nElem){
  int c=0;
  for (int i=0;i<nElem;i++){
    out[c]=real[i];
    out[c+1]=imag[i];
    c=c+2;
  }
};

/** Filter the complex array x by the filter h(q)=b(q)/a(q)
 *
 *                             b[0]+b[1]*q^(-1)+...+b[nElemB-1]*q^(nElemB-1)
 *                       h(q)=----------------------------------------------
 *                              1+a[1]*q^(-1)+...+a[nElemA-1]*q^(nElemA-1)
 * @pre:
 *    - b: pointer to array of the numerator of h(q) b[n]
 *    - nElemB: size of b[]
 *    - a: pointer to array of the numerator of h(q) a[n]
 *             !!!  a[0]=1 !!!
 *    - nElemA: size of a[]
 *    - x: the array to filter
 *    - y: the array of filtered elements, size nElemIN
 *    - nElemIN: the size of x[] and y[]
 *
 * @post:
 *    - y is now filled
 */
template <class T, class U>
void filterComp(U b[], int nElemB, U a[], int nElemA, T x[], T y[], int nElemIN){
  T real[nElemIN];
  T imag[nElemIN];
  splitComp(x,real,imag, nElemIN);
  T realFilt[nElemIN/2];
  T imagFilt[nElemIN/2];
  filter(b, nElemB, a, nElemA, real, realFilt, nElemIN/2);
  filter(b, nElemB, a, nElemA, imag, imagFilt, nElemIN/2);
  compMerge(realFilt,imagFilt, y, nElemIN/2);
};




/** This function lowpass filters an input signal
 *
 * @pre:
 *    - x        : input signal (array)
 *    - nElemIN  : array size of the signal
 *    - shift    : shift of the input signal. represents the lowpass cut off freq
 *
 * @post:
 *    - y        : output signal
 */
//template <class T,class W>

void lowPassFilter(double x[],double y[],int nElemIN,double shift)
{
  /* Low pass filter coefficients */
  double a[] ={1.0000,-3.7374,5.2460,-3.2774,0.7689};
  double b[] ={1.0e-04 *0.0562,1.0e-04*0.2247,1.0e-04*0.3370,1.0e-04*0.2247,1.0e-04*0.0562};
  int nElemB = 5;
  int nElemA = 5;

  // FILE * xFile;
  // FILE * xShift;
  // FILE * xFilt;

  //double count = 1.00;
  //const double pi = std::acos(-1);
  //const std::complex<double> i(0, 1);
  double xShifter[nElemIN];
  double xShifted[nElemIN];
  /* Filling the shifter */
 int counter = 0;
 //std::cout << "size: " << nElemIN << std::endl;
  for(int f=0;f<=nElemIN;f=f+2)
    {
      xShifter[f] = cos(2.00*3.14159*shift*counter);
      xShifter[f+1] = sin(2.00*3.14159*shift*counter);
      /* Multiplying the complex arrays elementwise */
      xShifted[f]=xShifter[f]*x[f] - xShifter[f+1]*x[f+1]; // real part
      xShifted[f+1]=xShifter[f]*x[f+1] + xShifter[f+1]*x[f]; //imag part
      counter++;
    };
  //std::cout << "counter: " << counter << std::endl;

  /* filtering */
  filter(b,nElemB,a,nElemA,xShifted,y,nElemIN);
  // fwrite(y,sizeof(std::complex<double>), nElemIN, xFilt);
  // fclose(xFilt);
}

/** Computes the power of the given array
 * @pre:
 *       - data: a pointer to an array of short
 *       - no_elements: the length of the array
 * @post:
 *       - power: a float with the power
 */
template<class T>
float powerTotArray( T data[], int no_elements){
  float power=0;
  float tmp;
  for (int i=0;i<no_elements;i++){
    tmp= (float) data[i];
    power= power+(tmp*tmp)/(no_elements/2);
    if(power<0){
      std::cout<<power;
    };

  };
  //float result=0;
  // std::cout<<power<<"\n";
  // std::cout<<(short)power<<"\n";
  return power;
};


void powerOfFreqBands(short data[],int nElem, double shiftindex,float power[],int numFilt)
{
  
  double dataD[nElem];

  for(int i=0;i<nElem;i++){
    dataD[i]=(double)data[i];
  }

  double temp[nElem];
  //double power[numFilt];
  for(int i=0;i<=(numFilt);i++)
    {
      float shift = shiftindex*i;
      //std::cout << "Shift (normalized w0): " << shift  << std::endl;
      lowPassFilter(dataD,temp,nElem,shift);
      
      power[i] = powerTotArray(temp,nElem);
      //std::cout << "power: " << power[i] << " at w0: " << shift << std::endl;
      //std::cout << "numFilt: " << i << std::endl;
    };
}
