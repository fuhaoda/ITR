# ITR (Individual Treatment Recommendation) 

ITR is a library implementing the framework described in the paper _Estimating
  Optimal Treatment Regimes via Subgroup Identification in Randomized Control
  Trials and Observational Studies_ by Haoda Fu, Jin Zhou, Douglas E. Faries. 
  
The library is implemented in C++ and can be built with `cmake` version 3.12 or
  later. The library has been tested with `gcc` version 8.1.0 and `clang`
  version 9.1.0.  

## Installation 
Assume the source code has been unpacked into directory `/path/to/ITR`, the
  following steps will build the library in `/path/to/ITR-build` and install the
  library in `/path/to/ITR-install`.  

```
> cd /path/to/ITR-build
> cmake /path/to/ITR -DCMAKE_INSTALL_PREFIX=/path/to/ITR-install
> make 
```

One could also specify a particular compiler by prefixing
  `CC=/path/to/c_compiler CXX=/path/to/cpp_compiler` to the `cmake`
  configuration line.  

The library uses `googletest` for unit testing, which is downloaded
  automatically during the configuration phase. One is recommended to verify the
  build before the final installation, and this can be done as follows:  

```
> cd /path/to/ITR-build/test
> make test
> ./test
```

Once the build is verified, one can complete the installation 

```
> cd /path/to/ITR-build
> make install
```

This will install header file `ITR.h` to `/path/to/ITR-install/include/itr` and
  `libitr.a` to `path/to/ITR-install/lib`.  

## Usage
The `demo` directory contains a simple example `demo.cpp` showing how to use the
  library. A simple digest of this example is given below.

To use the library, one needs to include the header file `ITR.h`. 

Inside `main`, an `ITR::ITR` instance is created by passing two arguments to its
  constructor. The first argument is a `std::string` object specifying the path
  to the input csv file. The second argument is an unsigned integer specifying
  the search depth. 

For the csv input file, the first line must be a header. The first column is the
  subject identifier, followed by continuous variables (labeled as cont*),
  ordinal variables (labeled as ord*), nominal variables (nom*), actions (A*),
  and responses (Y*). The ITR library is case insensitive to these labels.

For the search depth, the valid values are 1, 2, and 3.   

The constructor of `ITR::ITR` will throw an exception if the input file does not
  exist or the search depth is invalid. 

After the instance is created, one performs the search by invoking the `run`
  method. Here, one has the choice of running the search sequentially or in
  parallel by passing an `unsigned` integer `nThreads` to the method. If
  `nThreads` is 1, the search is done sequentially. If `nThreads` is greater
  than 1, the search is done in parallel. 

Once the search completes, one can query the top `n` search results by calling
  the `report` method. 

Generally, one needs to provide `-I/path/to/ITR-install/include/itr` and
  `-L/path/to/ITR-install/lib -litr` to the compile command. For the `demo`
  example, it can be built as follows

```
> cd /path/to/ITR-build/demo
> make demo
> ./demo
```

We add a few options to the `demo` program, which can be found by
```
> .demo
Usage: ./demo [OPTIONS]
--data=STRING  Path to input file, default is sample100.csv
--thread=NUM   Number of threads to use, default is 1
--best=NUM     Number of top results to display, default is 5
```

An sample output of the `demo` program looks like
```
> ./demo --thread=8
Loading input data ...
Creating search engine with depth 3
Searching 86131 choices ...
Completed in 1.102359e-02 seconds using 8 threads
Best 5 results: ...
Value is 6.884978e+01, obtained from
  X1 >= 4
  X2 < 1
  X7 in {0, 2} 
Value is 6.787278e+01, obtained from
  X1 >= 5
  X6 in {2, 3} 
  X7 in {2, 3} 
Value is 6.744488e+01, obtained from
  X1 >= 4
  X7 in {0, 2} 
  X8 in {2} 
Value is 6.742991e+01, obtained from
  X1 >= 3
  X5 >= 3
  X6 in {2, 3} 
Value is 6.740870e+01, obtained from
  X1 >= 4
  X6 in {0, 2} 
  X8 in {2} 
```

The `demo` directory also contains three additional source files
  `SimulationData.h`, `SimluationData.cpp`, and `genData.cpp`. They can be built
  into a utility routine to generate simulation data as follows  
```
> cd /path/to/ITR-build/demo
> make genData
```
The available options can be found 
```
> ./genData --help
Usage: ./genData [OPTIONS]
--seed=NUM    Seed of the random number generator, default is 0
--nSample=NUM Number of samples, default is 10
--nCont=NUM   Number of continuous variables, default is 1
--nOrd=NUM    Number of ordinal variables, default is 1
--nNom=NUM    Number of noninal variables, default is 1
--nResp=NUM   Number of responses, default is 1
--lDble=NUM   Lower bound of the continuous variable, default is 0
--uDble=Num   Upper bound of the continuous variable, default is 1
--lInt=NUM    Lower bound of the ord/nom variable, default is 0
--uInt=NUM    Upper bound of the ord/nom variable, default is 5
```
By default, this generates continuous covariates that are uniformly distributed
  in [0, 1] and ordinal and nominal covariates that are uniformly distributed in
  [0, 5]. One is free to change functions `genDouble` and `genInt` in
  `genData.cpp` to anything that suits his/her need. 

## Authors
Jie Xue  (xue_jie at lilly.com)

Haoda Fu (fu_haoda at lilly.com)

Bo Zhang (zhang_bo3 at lilly.com)
