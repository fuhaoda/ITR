# ITR (Individualized Treatment Recommendation) 

ITR is a library implementing the framework described in the paper [_Estimating
  Optimal Treatment Regimes via Subgroup Identification in Randomized Control
  Trials and Observational Studies_ by Haoda Fu, Jin Zhou, Douglas E. Faries](https://onlinelibrary.wiley.com/doi/full/10.1002/sim.6920). 

## Installation

The R version of the library is implemented using Rcpp. It can be installed
  simply by issuing
```
> R CMD INSTALL ITR_0.4.0.tar.gz 
```
To install it to a personal directory, say `/path/to/Rlibs`, 
 one needs to first create a file
  `~/.Renviron` containing the following line
```
  R_LIBS=/path/to/Rlibs
```
and then run
```
> R CMD INSTALL --library=/path/to/Rlibs
```

The package requires C++14, so you may still encounter an error.

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

On a shared cluster, if your `CXX14` variable does not have the full path to `g++`, you may need to load the `gcc` environment module (i.e. run `module load gcc`).

## Authors
Bo Zhang (zhang_bo3 at lilly.com)

Jie Xue  (xue_jie at lilly.com)

Haoda Fu (fu_haoda at lilly.com)
