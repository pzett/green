#include <iostream>

#include <cmath>
//#include "arrayToCvec.cpp"
#include <itpp/itbase.h>
#include <itpp/itsignal.h>
#include <string>
using namespace itpp;

void kalmanPhase (double y_1,itpp::mat R1,itpp::mat R2,itpp::mat x0, itpp::mat Q0,itpp::mat *x_predO, itpp::mat *y_predO, itpp::mat *P_1O, itpp::mat *x_hat_1O, itpp::mat *Q_1O ){
  itpp::mat F("1 6.28318 ; 1 0 ");
  std::cout << F << std::endl;
  itpp::mat G("1 0 ; 0 0");
 itpp::mat H("1 0");

  itpp::mat x_pred= F*x0;
std::cout << x_pred << std::endl;
  itpp::mat y_pred= H*x_pred;
std::cout << y_pred << std::endl;
  itpp::mat P_1=F*Q0*transpose(F)+G*R1*transpose(G);
std::cout << P_1 << std::endl;

  //measurement update
  itpp::mat L_1=P_1*transpose(H)*inv(H*P_1*transpose(H)+R2);
std::cout << L_1 << std::endl;
 itpp::mat x_hat_1=x_pred+L_1*itpp::mod((y_1-y_pred.get(0,0)),6.28318);
std::cout << x_hat_1 << std::endl;
  itpp::mat Q_1=P_1-P_1*transpose(H)*inv(H*P_1*transpose(H)+R2)*H*P_1;
std::cout << Q_1 << std::endl;
  // Save the output
  x_predO->set_submatrix(0,0,x_pred);
y_predO->set_submatrix(0,0,y_pred);
P_1O->set_submatrix(0,0,P_1);
x_hat_1O->set_submatrix(0,0,x_hat_1);
Q_1O->set_submatrix(0,0,Q_1);


}
int main (){
  double y_1=1;
  itpp::mat R1("0.1 0; 0 0");
  itpp::mat R2("0.1");
  itpp::mat x0("1;1");
  itpp::mat Q0("3 0 ; 0 9.8696");
  itpp::mat x_pred(2,1);
  itpp::mat y_pred(1,1);
  itpp::mat P_1(2,2);
  itpp::mat x_hat_1(2,1);
  itpp::mat Q_1(2,2); 
  kalmanPhase (y_1,R1, R2, x0,  Q0, &x_pred, &y_pred, &P_1, &x_hat_1, &Q_1 );
  return 0;
}
