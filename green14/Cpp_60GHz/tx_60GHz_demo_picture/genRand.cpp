
#include <algorithm>
#include <iostream>
#include <fstream>
#include <cmath>

#include <itpp/base/vec.h>
#include <itpp/itsignal.h>
#include <itpp/itbase.h>
#include <itpp/itcomm.h> 
#include <itpp/stat/misc_stat.h>
#include <itpp/signal/transforms.h>
#include <itpp/comm/interleave.h>

#include <complex>
#include <itpp/comm/modulator.h>
#include <stdio.h> 
#include <math.h>  


 /*
 This function write a sequence of random bits into a file pseudoNoise.it
 */
using namespace std;
using namespace itpp;

int main(){
  bvec pseudoNoise = randb(131072);
  bvec pseudoNoise2;
  it_file my_file("pseudoNoise.it");
my_file << Name("pseudoNoise") << pseudoNoise;
 my_file.flush();
 my_file.close();
 /*
  it_file my_file2("pseudoNoise.it");
  my_file2 >> pseudoNoise2;
 my_file2.flush();
 my_file2.close();

 cout << pseudoNoise << "\n";
 cout << pseudoNoise2 << "\n";
 */
return 0;

}
