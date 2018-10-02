# ITR (Individualized Treatment Recommendation) 

ITR is a library implementing the framework described in the paper _Estimating
  Optimal Treatment Regimes via Subgroup Identification in Randomized Control
  Trials and Observational Studies_ by Haoda Fu, Jin Zhou, Douglas E. Faries. 

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

## Authors
Bo Zhang (zhang_bo3 at lilly.com)

Jie Xue  (xue_jie at lilly.com)

Haoda Fu (fu_haoda at lilly.com)


