

using namespace std;


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

void filter(double b[], int nElemB, double a[], int nElemA, double x[], double y[], int nElemIN){
  if (a[0]!=1){cerr << "a[0] not equal to 1";}
  for (int i=0;i<nElemIN;i++){
    double xn[nElemB];
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
    double yn[nElemA];
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
}


double powerTotArray(double data[], int no_elements)
{
  double power = 0.0;
  double tmp;
  for (int i=0;i<no_elements;i++){
    tmp= data[i];
    power= power+(tmp*tmp)/(no_elements/2);
    if(power<0){
      std::cout<<power;
    };
  }
  return power;
}

bool transmissionDetection(double buff_double[],int nSamp){
  bool transmissionDetected = false;
  double threshold = 1000;
  double power = 0.0;

  // lowpassfilter
  double a[] ={1.0000,-3.7374,5.2460,-3.2774,0.7689};
  double b[] ={1.0e-04 *0.0562,1.0e-04*0.2247,1.0e-04*0.3370,1.0e-04*0.2247,1.0e-04*0.0562};
  int nElemB = 5;
  int nElemA = 5;
  double buffFilt[nSamp];
  filter(b,nElemB,a,nElemA,buff_double,buffFilt,nSamp);

  // power calculation
  power = powerTotArray(buffFilt, nSamp);

  // comparing with threshold
  if (power >= threshold)
    {
      transmissionDetected = true;
      std::cout << "IS transmitting!" << std::endl;
      std::cout << "received power: " << power << std::endl;
    };
  if(power < threshold)
    {
      std::cout << "not transmitting!" << std::endl;
      std::cout << "received power: " << power << std::endl;
    }
  return transmissionDetected;
}
