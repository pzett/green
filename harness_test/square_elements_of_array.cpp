#include <iostream>
extern void square_elements_of_array(float data_to_harness[],int no_elements) ;


void square_elements_of_array(float data_to_harness[],int no_elements) {
  for (int i1=0;i1<no_elements;i1++) {
    data_to_harness[i1]=data_to_harness[i1]*data_to_harness[i1];
  };
};
