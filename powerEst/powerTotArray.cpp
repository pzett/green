#include <iostream>
//#include <cmath>

using namespace std;
/*
  template<class T>
void meanCplx(T data[], int no_elements, float mean[])
{
  float valueReal;
  float valueImag;
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
*/
float powerTotArray( float data[], int no_elements){
  float power;
  for (int i=0;i<no_elements;i++){
    float tmp=(float) data[i];
    power=(float) power+tmp*tmp/(no_elements/2);
  };
return power;
};

/*
int main () {

  //itpp::vec n="0 , 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14,15,16,17,18,19";
  //itpp::vec datas=3+cos(2*3.14159*0.1*n)+cos(2*3.14159*0.2*n);

  int no_elements = 20;
  float data[no_elements];

  for(int i=0;i<no_elements-1; i=i+2){
    data[i]=(double) cos(2*3.14159*0.1*i)+cos(2*3.14159*0.2*i);
    data[i+1]=(double) sin(2*3.14159*0.1*i);
  };
  std::cout << "datas to be sent:" << *data << '\n';
  std::cout << "first" << data[1] << '\n';
  float power =powerTotArray(data, no_elements);
  std::cout << "power= " << power << '\n';
}
*/
