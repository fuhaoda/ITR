# ITR (Individualized Treatment Recommendation) 

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
> make install
```

This will install header file `ITR.h`, `Data.h`, and `SearchEngine.h`
  to `/path/to/ITR-install/include/itr` and
  `libitr.a` to `path/to/ITR-install/lib`.  One could also specify a particular compiler by
  prefixing
  `CC=/path/to/c_compiler CXX=/path/to/cpp_compiler` to the `cmake`
  configuration line.  

## Usage
The `demo` directory contains a simple example `demo.cpp` showing how to use the
  library. A simple digest of this example is given below.

To use the library, one needs to include the header file `ITR.h`. 

  Inside `main`, one first creates an `ITR::ITR` instance. The non-type parameter
  specifies the depth of the search, where the valid values are 1, 2, or 3.
  Additionally, the constructor takes two arguments: The first one 
  is a `std::string` object specifying the path
  to the input csv file, and the second one is the number of threads used in the search. 
Here, one has the choice of running the search sequentially or in
  parallel by passing an `unsigned` integer `nThreads` to the method. If
  `nThreads` is 1, the search is done sequentially. If `nThreads` is greater
  than 1, the search is done in parallel.
  
For the csv input file, the first line must be a header. The first column is the
  subject identifier, followed by continuous variables (labeled as cont*),
  ordinal variables (labeled as ord*), nominal variables (nom*), actions (A*),
  responses (Y*), and condition probablity P(A = 1 | X). The ITR library is case
  insensitive to these labels. The constructor of `ITR::ITR` will throw an exception if the input file does not
  exist or the search depth is invalid. 

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
Searching 689048 choices ...
Completed in 5.079716e-03 seconds using 8 threads
Score = 6.884978e+01, rule =  X1 < 49.8351,  X2 >= 49.6823,  X7 not in {0, 2} 
Score = 6.787278e+01, rule =  X1 < 58.8109,  X6 not in {2, 3},  X7 not in {2, 3} 
Score = 6.744488e+01, rule =  X1 < 49.8351,  X7 not in {0, 2},  X8 not in {2} 
Score = 6.742991e+01, rule =  X1 < 42.1108,  X5 < 3,  X6 not in {2, 3} 
Score = 6.740870e+01, rule =  X1 < 49.8351,  X6 not in {0, 2},  X8 not in {2} 
```

## Authors
Bo Zhang (zhang_bo3 at lilly.com)

Jie Xue  (xue_jie at lilly.com)

Haoda Fu (fu_haoda at lilly.com)


