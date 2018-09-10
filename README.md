# ITR (Individual Treatment Recommendation) 

ITR is a library implementing the framework described in the paper _Estimating Optimal Treatment Regimes via Subgroup Identification in Randomized Control Trials and Observational Studies_ by Haoda Fu, Jin Zhou, Douglas E. Faries. 

The library is implemented in C++ and can be built with `cmake` version 3.12 or later. The library has been tested with `gcc` version 8.1.0 and `clang` version 9.1.0. 

## Installation 
Assume the source code has been unpacked into directory `/path/to/ITR`, the following steps will build the library in `/path/to/ITR-build` and install the library in `/path/to/ITR-install`. 

```
> cd /path/to/ITR-build
> cmake /path/to/ITR -DCMAKE_INSTALL_PREFIX=/path/to/ITR-install
> make 
```

One could also specify a particular compiler by prefixing `CC=/path/to/c_compiler CXX=/path/to/cpp_compiler` to the `cmake` configuration line. 

The library uses `googletest` for unit testing, which is downloaded automatically during the configuration phase. One is recommended to verify the build before the final installation, and this can be done as follows: 

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

This will install header file `ITR.h` to `/path/to/ITR-install/include/itr` and `libitr.a` to `path/to/ITR-install/lib`. 

## Usage 
The `demo` directory contains a simple example `demo.cpp` showing how to use the library. A simple digest of this example is given below. 

To use the library, one needs to include the header file `ITR.h`.  

Inside `main`, an `ITR::ITR` instance is created by passing two arguments to its constructor. The first one is a `std::string` object specifying the path to read the input data file (in csv format), and the second one is an unsigned integer specifying the search depth. 

For the csv input file, the library requires the first line be a header. The first column is the the subject identifier, followed by continuous variables (labeled as cont*), ordinal variable (labeled as ord*), nominal vairables (nom*), actions (A*), and responses (Y*). The library is case insensitive to the labels. 

For the search depth, the valid values are 1, 2, and 3. 

The constructor will throw an exception if the input data file does not exist or the search depth is invalid. 

After creating the instance, one performs the search by invoking the `run` method and can display the top `n` search results using the `report` method. 

Generally, one needs to provide `-I/path/to/ITR-install/include/itr` and `-L/path/to/ITR-install/lib -litr` to the compile command. For the `demo` example, it can be built and executed as follows 

```
> cd /path/to/ITR-build/demo
> make demo
> ./demo 
Loading input data ...
Creating search engine with depth 3
Searching ...
Completed in 1.728010e-02 seconds using 8 logical cores
Best 10 results: ...
Value is 6.884978e+01, obtained from
  X1 >= 4
  X2 < 1
  X7 in {1, 3, 4} 
Value is 6.787278e+01, obtained from
  X1 >= 5
  X6 in {4} 
  X7 in {4} 
...
```

The `demo` directory also contains three additional source files `SimulationData.h`, `SimluationData.cpp`, and `genData.cpp`. They can be built into a utility routine to generate simulation data as follows 
```
> cd /path/to/ITR-build/demo
> make genData
```
The available options can be found 
```
> ./genData --help
Usage: ./genData [OPTIONS]
--seed=NUM  Seed of the random number generator, default is 0
--nCont=NUM Number of continuous variables, default is 1
--nOrd=NUM  Number of ordinal variables, default is 1
--nNom=NUM  Number of noninal variables, default is 1
--nAct=NUM  Number of actions, default is 1
--nResp=NUM Number of responses, default is 1
```
By default, this generate covariates that are uniformly distributed in [0, 1]. One is free to change functions `genDouble` and `genInt` to anything that suit his/her need. 

## Authors
Jie Xue  (xue_jie at lilly.com)

Haoda Fu (fu_haoda at lilly.com)

Bo Zhang (zhang_bo3 at lilly.com)
