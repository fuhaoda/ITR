# ITR (Individualized Treatment Recommendation) 

ITR is a library implementing the framework described in the paper
[_Estimating Optimal Treatment Regimes via Subgroup Identification in Randomized Control
  Trials and Observational Studies_ by Haoda Fu, Jin Zhou, Douglas E. Faries](https://onlinelibrary.wiley.com/doi/abs/10.1002/sim.6920). 

## Installation

The library is implemented in C++ and exported as a module to R using Rcpp.

### C++
The library can be built as a static library using `cmake` version 3.12 or
later. The library has been tested with `gcc` version 8.1.0 and `clang` version
9.1.0. 

Assume the source code has been unpacked to `/path/to/ITR`, the following steps
will build the library in `/path/to/ITR-build` and install the library in
`/path/to/ITR-install`.

```
> cd /path/to/ITR-build
> cmake /path/to/ITR -DCMAKE_INSTALL_PREFIX=/path/to/ITR-install
> make
> make install
```

This will install header file `ITR.h`, `Data.h`, `SearchEngine.h`, and `Types.h`
to `/path/to/ITR-install/include/itr` and `libitr.a` to
`/path/to/ITR-install/lib`. One could also specify a particular compiler by
prefixing `CC=/path/to/c_compiler CXX=/path/to/cpp_compiler` to the `cmake`
configuration line.

### R 
Assume the source code has been unpacked to `/path/to/ITR`, the package can be
installed by issuing
```
> R CMD INSTALL /path/to/ITR
```

The package requires C++14, so you may encounter an error: 

```
Error in .shlib_internal(args) :
  C++14 standard requested but CXX14 is not defined
```

If that is the case, create a personal `~/.R/Makevars` file with the following lines.

```
CXX14 = g++ # or the full path to your C++14 compliant compiler
CXX14FLAGS = -O2 -g $(LTO)
CXX14PICFLAGS = -fPIC
CXX14STD = -std=c++14
```

On a shared cluster, if your `CXX14` variable does not have the full path to
`g++`, you may need to load the `gcc` environment module (i.e., run `module load
gcc`). 

## Usage

### C++

The `demo` directory contains a simple example `demo.cpp` demonstrating how to
use the library. A simple digest of this example is given below.

To use the library, one needs to include the header file `ITR.h`.

Inside `main`, one creates an `ITR` instance. The constructor takes three
arguments: The first one is a `std::string` object specifying the path to the
input csv file, the second one specifies the depth of the search, and the third
one specifies the number of threads used in the search, allowing the user to run
the search sequentially or in parallel.

For the csv input file, the first line must be a header. The first column is the
subject identifier, followed by continuous variables (labeled as cont*), ordinal
variables (ord*), nominal variables (nom*), actions (A*), responses (Y*), and
conditional probability P(A=1|X). The ITR library is case insensitive to these
labels. The constructor will throw an exception if the input file does not exist
or the search depth is invalid.

Once the search completes, one can query the top `n` search results using the
following methods:

1. `score`: Get the score of the top `n` searches.
2. `var`: Get the index of the variables associated with the top scores.
3. `cut`: Get the cut information of a given top score.
4. `dir`: Get the direction of the cut associated with a given top score. 

Generally, one needs to provide `-I/path/to/ITR-install/include/itr` and
`-L/path/to/ITR-install/lib -litr` to the compile command. For the `demo`
example, it can be built and executed as follows

```
> cd /path/to/ITR-build/demo
> make demo
> ./demo
```

There are a few options to the `demo` program, which can be found by
```
> ./demo --help
Usage: ./demo [OPTIONS]
--data=STRING Path to input file, default is sample100.csv
--thread=NUM  Number of threads to use, default is 1
--best=NUM    Number of top results to display, default is 5
```

An sample output of the `demo` program looks like
```
> ./demo --thread=8
Loading input data ...
Creating search engine with depth 3
Searching 689048 choices ...
Completed in 6.264286e-03 seconds using 8 threads
Score = 6.884978e+01, X1 < 49.8351 (percentile 50), X2 >= 49.6823 (percentile 20), X7 not in 0 2 
Score = 6.787278e+01, X1 < 58.8109 (percentile 60), X6 not in 2 3 , X7 not in 2 3 
Score = 6.744488e+01, X1 < 49.8351 (percentile 50), X7 not in 0 2 , X8 not in 2 
Score = 6.742991e+01, X1 < 42.1108 (percentile 40), X5 < 3 (4 out of 5), X6 not in 2 3 
Score = 6.740870e+01, X1 < 49.8351 (percentile 50), X6 not in 0 2 , X8 not in 2 
```

### R
Load the library
```
> library('ITR')
```

Create an ITR instance by pasing the path to the input data file, the depth of
the search, and the number of threads to run the search to the constructor
```
> itr <- new(ITR,'sample100.csv', 3, 8)
Loading input data ...
Creating search engine with depth 3
```

Run the search
```
> itr$run()
Searching 689048 choices ...
Completed in 1.370409e-02 seconds using 8 threads
```

Get the scores of the best 5 searches
```
> itr$score(5)
[1] 68.84978 67.87278 67.44488 67.42991 67.40870
```

Get the variables associated with the best 5 searches
```
> itr$var(5)
     [,1] [,2] [,3]
[1,]    1    2    7
[2,]    1    6    7
[3,]    1    7    8
[4,]    1    5    6
[5,]    1    6    8
```

Get the cut information of a particular search, say, the second best one, 
```
> itr$cut(2)
[[1]]
[1] "58.8109 (percentile 60)"

[[2]]
[1] "2 3 "

[[3]]
[1] "2 3 "

```

Get the direction of the cut associated with a particular search, say, the
second best one
```
> itr$dir(2)
[[1]]
[1] " < "

[[2]]
[1] " not in "

[[3]]
[1] " not in "
```

## Authors
Bo Zhang (zhang_bo3 at lilly.com)

Jie Xue  (xue_jie at lilly.com)

Haoda Fu (fu_haoda at lilly.com)
