#include <cstdio>
#include <getopt.h>
#include <string>
#include <sys/stat.h> 
#include "ITR.h"

void print_usage(char *program) {
  fprintf(stdout, "Usage: %s --input=FILE --depth=NUM\n", program);
}

bool parse_cmd_args(int argc, char **argv,
                    std::string &input, unsigned &depth) {
  static struct option long_options[] =
    {
     {"input", required_argument, 0, 'i'},
     {"depth", required_argument, 0, 'd'},
     {"help", no_argument, 0, 'h'},
     {0, 0, 0, 0}
    };

  int opt = 0;
  int long_index = 0;
  while ((opt = getopt_long(argc, argv, "i:d:h",
                            long_options, &long_index)) != -1) {
    switch (opt) {
    case 'i':
      input = optarg;
      break;
    case 'd':
      depth = atoi(optarg);
      break;
    case 'h':
      print_usage(argv[0]);
      return false;
    }
  }

  struct stat buffer;
  if (stat(input.c_str(), &buffer) != 0) {
    fprintf(stderr, "Usage ERROR: input file does not exist\n");
    return false;
  }

  
  if (depth != 1 && depth != 2 && depth != 3) {
    fprintf(stderr, "Usage ERROR: depth of the search can only be 1, 2, or 3\n");
    return false;
  }
  
  return true; 
}

int main(int argc, char **argv) {
  // Default values 
  std::string input{"TestSample_100.csv"};
  unsigned depth = 3; 

  // Parse command line arguments if exist 
  if (!parse_cmd_args(argc, argv, input, depth))
    return -1;

  // Create the ITR instance
  ITR::ITR instance(input, depth); 

  // Run the comprehensive search
  instance.run();

  // Print out the report
  instance.report(); 

  return 0; 
}

  
