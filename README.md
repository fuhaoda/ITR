# ITR (Individualized Treatment Recommendation) 

ITR is a library implementing the framework described in the paper [_Estimating
  Optimal Treatment Regimes via Subgroup Identification in Randomized Control
  Trials and Observational Studies_ by Haoda Fu, Jin Zhou, Douglas E. Faries](https://onlinelibrary.wiley.com/doi/full/10.1002/sim.6920). 

## Installation

The R version of the library is implemented using Rcpp and can be installed by 
issuing 
```
> R CMD INSTALL ITR_0.4.0.tar.gz 
```

# Usage
- Create an ITR instance. The constructor takes three arguments: the path to 
the input data file, the depth of the search, and the number of threads to 
run the search. 
```
> itr <- new(ITR, 'sample100.csv', 3, 8)
Loading input data ...
Creating search engine with depth 3
```
- Run the search 
```
> itr$run()
Searching 689048 choices ...
Completed in 1.093183e-02 seconds using 8 threads
```
- Get the scores of the best 5 searches. 
```
> itr$score(5) 
[1] 68.84978 67.87278 67.44488 67.42991 67.40870
```
- Get the variables associated with the best 5 searches
```
> itr$var(5)
     [,1] [,2] [,3]
[1,]    1    2    7
[2,]    1    6    7
[3,]    1    7    8
[4,]    1    5    6
[5,]    1    6    8
```
- Get the cut information of the ith best result
```
> itr$cut(1)
[[1]]
[1] "49.8351 (percentile 50)"

[[2]]
[1] "49.6823 (percentile 20)"

[[3]]
[1] "0 2 "
```
- Get the directions of the cuts. For a continuous or ordinal variable, direction 
1 means < and direction 0 means >=. For a nominal variable, direction 1 means
included and direction 0 means excluded. 
```
> itr$dir(5)
     [,1] [,2] [,3]
[1,]    1    0    0
[2,]    1    0    0
[3,]    1    0    0
[4,]    1    1    0
[5,]    1    0    0
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
