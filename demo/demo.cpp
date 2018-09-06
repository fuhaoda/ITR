#include <string>
#include <sys/stat.h> 
#include "ITR.h"


int main(int argc, char **argv) {
  // Create an ITR instance. The constructor takes two arguments: the first one
  // is the path of the input data file, and the second one is the depth of the
  // search. 
  ITR::ITR instance("demo_input.csv", 3); 

  // Run the search with the specified depth
  instance.run();

  // Display the result. By default, the function prints out the best value and
  // the associated cut information. Optionally, one can provide an integer
  // input, say n, and the function will print out information associated with
  // the best n search results. 
  instance.report(10); 

  return 0; 
}

  
