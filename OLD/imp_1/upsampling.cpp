#include <iostream>

using namespace std;

/** hold the data during Q samples
 * @pre:
 *     - data: an array with the data
 *     - out:array same type as data. output. Length nElem*Q 
 *     - Q. the time of the hold
 *     - nElem: length of data
 *
 * @post:
 *     - out is filled
 */
template<class T>
void hold(T data[], T out[], int Q, int nElem){
  int c=0;
  for (int i=0; i<nElem;i++){
    for (int d=0;d<Q;d++){
    out[c+d]=data[i];
    }
    c=c+Q;
  }
}
/*
int main(){
  int nElem=8;
  int Q=2;
  short data[]={0, 1, 2, 3, 4, 5, 6, 7};
  short out[nElem*Q];
  hold(data, out, Q, nElem);
    for (int i=0; i<nElem*Q;i++){
      cout<< i << " = " << out[i]<<"\n";
    }
  return 0;
}
*/
