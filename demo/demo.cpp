#include <string>
#include <iostream>
#include <getopt.h> 
#include "ITR.h"

void printUsage(char *program) {
  fprintf(stdout, "Usage: %s [OPTIONS]\n"
          "--data=STRING  Path to input file, default is sample100.csv\n"
          "--thread=NUM   Number of threads to use, default is 1\n",
          program);
}

int main(int argc, char **argv) {
  std::string ifile{"sample100.csv"};
  unsigned nThreads = 1;

  // Parse command line
  static struct option long_options[] =
    {
     {"data", required_argument, 0, 'd'},
     {"thread", required_argument, 0, 't'},
     {"help", no_argument, 0, 'h'},
     {0, 0, 0, 0}, 
    };

  int opt = 0;
  int long_index = 0;
  while ((opt = getopt_long(argc, argv, "d:t:h",
                            long_options, &long_index)) != -1) {
    switch (opt) {
    case 'd':
      ifile = optarg;
      break;
    case 't':
      nThreads = atoi(optarg);
      break;
    case 'h':
    case '?':
      printUsage(argv[0]);
      return -1;
    }
  }
  
  try {
    // Create an ITR instance. The constructor takes two arguments: The first
    // one is the path of the input data file, and the second one is the depth
    // of the search.

    // The valid values for the search depth are 1, 2, or 3.

    // The constructor will throw an exception if the file does not exist or if
    // the depth of the search is invalid. 
    ITR::ITR instance{ifile, 3};

    // Run the search with specified depth
    instance.run(nThreads);

    // Display the result. By default, the function prints out the best value
    // and the associated cut information. Optionally, one can provide an
    // integer input, say n, and the function will print out information
    // associated with the best n search results. If n is larger than the number
    // of searches performed, it will be truncated internally. 
    instance.report(5);
  } catch (const char *msg) {
    std::cout << msg << "\n";
  }

  return 0; 
}

  
