#ifndef __FDF_H__
#define __FDF_H__

#include <vector>

// Class of real-valued differentiable function.
class FDF {
public:
  virtual ~FDF() = default; 

  // Evaluate the function at location x.
  virtual void eval(const std::vector<double> &x, double &f) const = 0;

  // Evaluate the gradient of the function at location x. 
  virtual void eval(const std::vector<double> &x,
                    std::vector<double> &g) const = 0;

  // Evaluate the value of the function and its gradient at location x.
  virtual void eval(const std::vector<double> &x, double &f,
                    std::vector<double> &g) const = 0;

  // Return the dimension of the function
  virtual size_t dim() const = 0;
};

#endif 
