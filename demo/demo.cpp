#include <iostream>
#include <string>
#include <getopt.h>
#include "ITR.h"

void printUsage(char *program) {
  std::cout << "Usage: " << program << " [OPTIONS]\n"
            << "--data=STRING Path to input file, default is sample100.csv\n"
            << "--thread=NUM  Number of threads to use, default is 1\n"
            << "--best=NUM    Number of top results to display, default is 5\n";
}

int main(int argc, char **argv) {
  std::string ifile{"sample100.csv"};
  unsigned nthreads = 1;
  unsigned ntop = 5;
  
  // Parse command line
  static struct option long_options[] =
    {
     {"data",   required_argument, 0, 'd'},
     {"thread", required_argument, 0, 't'},
     {"best",   required_argument, 0, 'b'},
     {"help",   no_argument,       0, 'h'},
     {0, 0, 0, 0},
    };
  
  int opt = 0;
  int long_index = 0;
  while ((opt = getopt_long(argc, argv, "d:t:b:h",
                            long_options, &long_index)) != -1) {
    switch (opt) {
    case 'd':
      ifile = optarg;
      break;
    case 't':
      nthreads = atoi(optarg);
      break;
    case 'b':
      ntop = atoi(optarg);
      break;
    case 'h':
    case '?':
      printUsage(argv[0]);
      return -1;
    }
  }
  
  try {
    Data input(ifile); 
    
    CompSearch cs(&input, 3, nthreads); 

    // Run the search
    cs.run();

    // Get the top 5 treatment recommendations.
    auto scores = cs.score(5);
    auto var = cs.var(5);

    for (size_t i = 0; i < ntop; ++i) {
      auto cut = cs.cut(i);
      auto dir = cs.dir(i);
      std::cout << "Score = " << scores[i]
                << ", X" << var(i, 0) << dir[0] << cut[0]
                << ", X" << var(i, 1) << dir[1] << cut[1]
                << ", X" << var(i, 2) << dir[2] << cut[2] << "\n";
    }
  } catch (const char *msg) {
    std::cout << msg << "\n";
  }
  
  return 0;
} 
