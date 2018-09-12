#include <getopt.h>
#include <cstdio>
#include <random> 
#include "SimulationData.h"

// Default values
unsigned seed = 0;
size_t nSample = 10; 
size_t nCont = 1;
size_t nOrd = 1;
size_t nNom = 1;
size_t nResp = 1;
double lDble = 0;
double uDble = 1;
int lInt = 0;
int uInt = 5; 

// By default, double values are drawn uniformly from [0, 1], and integer values
// are also drawn uniformly from [0, 1]. 
std::default_random_engine engine;

void genDouble(double *arr, size_t n, double a, double b) {
  std::uniform_real_distribution<> dist{a, b};
  for (size_t i = 0; i < n; ++i)
    arr[i] = dist(engine);
}

void genInt(int *arr, size_t n, int a, int b) {
  std::uniform_int_distribution<> dist{a, b};
  for (size_t i = 0; i < n; ++i)
    arr[i] = dist(engine);
}

void printUsage(char *program) {
  fprintf(stdout, "Usage: %s [OPTIONS]\n"
          "--seed=NUM    Seed of the random number generator, default is 0\n"
          "--nSample=NUM Number of samples, default is 10\n"
          "--nCont=NUM   Number of continuous variables, default is 1\n"
          "--nOrd=NUM    Number of ordinal variables, default is 1\n"
          "--nNom=NUM    Number of noninal variables, default is 1\n"
          "--nResp=NUM   Number of responses, default is 1\n"
          "--lDble=NUM   Lower bound of the continuous variable, default is 0\n"
          "--uDble=Num   Upper bound of the continuous variable, default is 1\n"
          "--lInt=NUM    Lower bound of the ord/nom variable, default is 0\n"
          "--uInt=NUM    Upper bound of the ord/nom variable, default is 5\n",
          program);
}

int main(int argc, char **argv) {
  // Parse command line arguments
  static struct option long_options[] =
    {
     {"seed", required_argument, 0, 's'},
     {"nSample", required_argument, 0, 'p'}, 
     {"nCont", required_argument, 0, 'c'},
     {"nOrd", required_argument, 0, 'o'},
     {"nNom", required_argument, 0, 'n'},
     {"nResp", required_argument, 0, 'r'},
     {"lDble", required_argument, 0, 'a'},
     {"uDble", required_argument, 0, 'b'},
     {"lInt", required_argument, 0, 'A'},
     {"uInt", required_argument, 0, 'B'}, 
     {"help", no_argument, 0, 'h'}, 
     {0, 0, 0, 0}
    }; 

  int opt = 0;
  int long_index = 0;
  while ((opt = getopt_long(argc, argv, "s:p:c:o:n:r:a:b:A:B:h",
                            long_options, &long_index)) != -1) {
    switch(opt) {
    case 's':
      seed = atoi(optarg);
      break;
    case 'p':
      nSample = atoi(optarg);
      break; 
    case 'c':
      nCont = atoi(optarg);
      break;
    case 'o':
      nOrd = atoi(optarg);
      break;
    case 'n':
      nNom = atoi(optarg);
      break;
    case 'r':
      nResp = atoi(optarg);
      break;
    case 'a':
      lDble = atof(optarg);
      break;
    case 'b':
      uDble = atof(optarg);
      break;
    case 'A':
      lInt = atoi(optarg);
      break;
    case 'B':
      uInt = atoi(optarg);
      break;      
    case 'h':
    case '?': 
      printUsage(argv[0]);
      break;
    }
  }

  // Update the seed of the engine
  engine = std::default_random_engine{seed}; 

  // Create an instance of the SimulationData class
  SimulationData data{nSample, nCont, nOrd, nNom, nResp};

  // Generate actual data
  data.generateContVariable(genDouble, lDble, uDble);
  data.generateOrdVariable(genInt, lInt, uInt);
  data.generateNomVariable(genInt, lInt, uInt); 
  data.generateAct(genInt, 0, 1);
  data.generateResp(genDouble, lDble, uDble); 

  // Save the data into a csv file
  data.save(); 

  return 0;
}

