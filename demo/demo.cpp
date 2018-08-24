#include "ITR.h"

void print_usage() {
}

int main(int argc, char **argv) {
  // Parse command line arguments

  // Create the ITR instance
  ITR::ITR instance(std::string(argv[0]), 3); 

  // Run the comprehensive search
  instance.run();

  // Print out the report
  instance.report(); 
  
  return 0; 
}

  
