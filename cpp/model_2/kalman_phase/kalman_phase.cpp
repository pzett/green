#include <iostream>

#include <cmath>
//#include "arrayToCvec.cpp"
#include <itpp/itbase.h>
#include <itpp/itsignal.h>
#include <string>
//#include <math.h>


#define Print(X) std::cout << #X << " = " << X<< std::endl
#define M_PI 3.14159265358979323846
using namespace itpp;
using namespace std;


double modP(double a, double b){
  if (a<0){
  double rem=std::ceil(a/b);
  rem=a-rem*b;
  //std::cout<<"rem "<<rem<<std::endl;
  rem=b+rem;
  return rem;
  }else{
  double rem=std::floor(a/b);
  rem=a-rem*b;
 return rem;
  }
}

void kalmanPhase (double y_1,itpp::mat R1,itpp::mat R2,itpp::mat x0, itpp::mat Q0,itpp::mat *x_predO, itpp::mat *y_predO, itpp::mat *P_1O, itpp::mat *x_hat_1O, itpp::mat *Q_1O ){

  // Initialisation of the model
  itpp::mat F("1 6.28318 ; 0 1");
  itpp::mat G("1 0 ; 0 0");
 itpp::mat H("1 0");

  itpp::mat x_pred= F*x0;

  itpp::mat y_pred= H*x_pred;

  itpp::mat P_1=F*Q0*transpose(F)+G*R1*transpose(G);


  //measurement update
  itpp::mat L_1=P_1*transpose(H)*inv(H*P_1*transpose(H)+R2);

 itpp::mat x_hat_1=x_pred+L_1*(modP((y_1-y_pred.get(0,0))+M_PI,2*M_PI)-M_PI);

  itpp::mat Q_1=P_1-P_1*transpose(H)*inv(H*P_1*transpose(H)+R2)*H*P_1;

  // Save the output
  x_predO->set_submatrix(0,0,x_pred);
y_predO->set_submatrix(0,0,y_pred);
P_1O->set_submatrix(0,0,P_1);
x_hat_1O->set_submatrix(0,0,x_hat_1);
Q_1O->set_submatrix(0,0,Q_1);


}
int main (){
  double y_1=1;
  itpp::mat R1("0.001 0; 0 0");
  itpp::mat R2("0.1");
  itpp::mat x0("1;0.1");
  itpp::mat Q0("3 0 ; 0 9.8696");
  itpp::mat x_pred(2,1);
  itpp::mat y_pred(1,1);
  itpp::mat P_1(2,2);
  itpp::mat x_hat_1(2,1);
  itpp::mat Q_1(2,2); 
  int nElem=100000;
  // Import data
  // Loading the training sequence
  double data[nElem];
  std::ifstream ifs( "data_to_harness.dat" , std::ifstream::in );
  if(ifs.is_open()){
    ifs.read((char * )data,nElem*sizeof(double));
    ifs.close();
  }else{
    std::cerr<<"Error downloading train!\n";
    exit(1);
  }

  // Step over time instants
  double resultC[nElem];
  for (int i=0; i<nElem; i++){
    Print(i);
    y_1=data[i];
    Print(y_1);
    kalmanPhase (y_1,R1, R2, x0,  Q0, &x_pred, &y_pred, &P_1, &x_hat_1, &Q_1 );
    x0=x_hat_1;
    Print(x0);
    Q0=Q_1;
    Print(Q0);
    resultC[i]=y_pred.get(0,0);
  }


  std::cout<< M_PI;
  // // Save data to file
  std::ofstream ofs( "data_from_harness.dat" , std::ifstream::out );
  if (!ofs.is_open()){std::cerr << "File not open";}
  ofs.write((char * )resultC , nElem*sizeof(double));
  ofs.close();

  return 0;
}
