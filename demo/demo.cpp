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
  unsigned nThreads = 1;
  unsigned nTop = 5;
  
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
      nThreads = atoi(optarg);
      break;
    case 'b':
      nTop = atoi(optarg);
      break;
    case 'h':
    case '?':
      printUsage(argv[0]);
      return -1;
    }
  }
  
  try {
    ITR instance{ifile, 3, nThreads};
    
    // Run the search
    instance.run();
    
    // Retrive the top n treatment recommendations.
    auto scores = instance.score(5);
    auto var = instance.var(5);

    for (size_t i = 0; i < nTop; ++i) {
      auto cut = instance.cut(i);
      auto dir = instance.dir(i);
      std::cout << "Score = " << scores[i] << ", "
                << "X" << var(i, 0)
                << dir[0] << cut[0] << ", "
                << "X" << var(i, 1)
                << dir[1] << cut[1] << ", "
                << "X" << var(i, 2)
                << dir[2] << cut[2] << "\n";
    }
  } catch (const char *msg) {
    std::cout << msg << "\n";
  }
  
  return 0;
} 
