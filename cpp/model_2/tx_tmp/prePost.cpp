
#include <algorithm>
#include <iostream>
#include <fstream>
#include <cmath>
#include <itpp/itbase.h>
#include <itpp/itsignal.h>
#include <vector>

/*
This function puts the prefix and postfix into the data
 */

using namespace std;
using namespace itpp;

cvec prePost (cvec data_time, int pre, int post){

   data_time.ins(0, data_time.right(pre));
   data_time.ins(data_time.length(), data_time.left(post));
   return(data_time);

}
