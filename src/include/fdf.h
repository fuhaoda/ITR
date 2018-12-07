#ifndef __FDF_H__
#define __FDF_H__

// Class of real-valued differentiable function.
class FDF {
public:
  // Constructor
  //
  // f: A real-valued differentiable function
  // g: The gradient of f
  // dim: Dimension of f's domain
  FDF(double (*f)(const double *),
      void (*g)(const double *, double *),
      size_t dim): f_{f}, g_{g}, dim_{dim} { }

  // Evaluate the function at location x
  void eval(const double *x, double &f) const {
    f = f_(x);
  }
      
  // Evaluate the gradient of the function at location x
  void eval(const double *x, double *g) const {
    g_(x, g);
  }
    
  // Evaluate the value of the function and its gradient at location x
  void eval(const double *x, double &f, double *g) const {
    f = f_(x);
    g_(x, g);
  }

  // Return the dimension of the domain of f
  size_t dim() const { return dim_; }
  
private:
  double (*f_)(const double *);
  void (*g_)(const double *x, double *); 
  size_t dim_;
}; 


#endif 
