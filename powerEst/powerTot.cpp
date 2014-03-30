#include <iostream>
#include <fstream>
#include <itpp/itbase.h>
using namespace itpp;

template <class T>
float powerTot(Vec<T> datas){
  /*
  float power;
  for (int i=0;i<no_elements;i++){
    power=power+datas[i]*datas[i]/no_elements;
  };*/
  T meanDatas=sum(datas)/datas.length();
  Vec<T> datas2=datas-meanDatas;
  std::cout << "mean: " << meanDatas <<'\n';
  float power=(float) sum_sqr(abs(datas2))/datas2.length();
  return power;
};

int main () {
/*
  ifstream myfile;
  myfile.open("datas.txt");
*/
  itpp::vec n="0 , 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14,15,16,17,18,19";
  itpp::vec datas=3+cos(2*3.14159*0.1*n)+cos(2*3.14159*0.2*n);
  std::cout << "datas to be sent:" << datas << '\n';
  float power =powerTot(datas);
  std::cout << "power= " << power << '\n';
}
