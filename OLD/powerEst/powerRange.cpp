//void powerRange(float data[], int no_elements, float power[]){
#include <itpp/itbase.h>
#include <itpp/itsignal.h>
using namespace itpp;
using namespace std;


void powerRange(cvec *data, float *band){
  vec P;
  int data_size=data->size();
  vec window=hanning(data_size);
  cout << window << '\n';
  cvec dataW=to_cvec(zeros(data_size));
  /*
  for(int i = 0;i<data_size; i++){
    dataW.set(i,data[i] * window[i]);
    }*/
  dataW=elem_mult(*data,to_cvec(window));
  P=sqr(abs(fft(dataW,64)));
  // P=spectrum(*data);
  cout << "P" << P<<'\n' << '\n';
  cout << "dataW" << dataW <<'\n';
  int n=P.size();
  cout << "N" << n << '\n';
  int nbr=floor(n/25);
  cout<< "nbr" << nbr << '\n';
  float fs=25e6;

  // cvec band;

for (int i=0;i<25;i++){
  band[i]=sum(P(i*nbr, (i+1)*nbr))*fs/n;
 };
}

int main () {
/*
  ifstream myfile;
  myfile.open("datas.txt");
*/
  itpp::vec n="0 , 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14,15,16,17,18,19";
  /*
  cvec datas;
  for (int ii=0; ii< 20; ii++){
    string tmp1=sprintf(cos(2*3.14159*0.1*ii));
    string tmp2=ftos(sin(2*3.14159*0.1*ii));
    datas(ii)="("+tmp1 +","+tmp2 +")";
    };*/
  itpp::cvec datas=to_cvec(cos(2*3.14159*0.1*n),sin(2*3.14159*0.1*n));
  std::cout << "datas to be sent:" << datas << '\n';
  float power[25];
  powerRange(&datas,power );
  for (int i=0;i<25;i++){
  std::cout << "power= " << power[i] << '\n';
  };
}
