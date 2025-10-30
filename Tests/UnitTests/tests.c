#include "network_tests.h"
#include "compresion_tests.h"


int main(){
  unittests_network_run_all();
  unittests_compression_run_all();


  return 0;
}