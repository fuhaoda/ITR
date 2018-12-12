#include <algorithm>
#include <numeric>
#include <functional>
#include <cmath>
#include "linesearch.h"

int MoreThuente::search(std::vector<double> &x, double &f,
                        std::vector<double> &g, const double *p, double &alpha)
{
  // Reset the counter
  nFuncEval_ = 0;

  // Check the input parameters for errors.
  if (alpha <= 0.0)
    return -1; 

  // Compute the initial gradient in the search direction and check that p is a
  // descent direction.
  double dginit = std::inner_product(g.begin(), g.end(), p, 0.0); 

  if (dginit > 0)
    return -2;
  
  // Initialize local variables
  bool bracket = false;
  bool stage1 = true; 
  double finit = f;
  double dgtest = mu_ * dginit;
  double width = alphaMax_ - alphaMin_;
  double width1 = width * 2.0;

  // Variables stx, fx, dgx contains the values of the step, function, and
  // directional derivative at the best step. Variables sty, fx, dgy contain the
  // values of the step, function, and directinoal derivative at the other
  // endpoint of the interval of uncertainty. Variables alpha, f, dg contain the
  // values of the step, function, and derivative at the current step.
  double stx{0.0}, sty{0.0}, fx{finit}, fy{finit}, dgx{dginit}, dgy{dginit};
  double stmin, stmax; 

  // Return value from updateTrail
  int uinfo = 0;

  // Save the existing minimizer location
  work_.assign(x.begin(), x.end()); 

  for (;;) {
    // Set the minimum and maximum steps to correspond to the represent interval
    // of uncertainty.
    if (bracket) {
      stmin = std::min(stx, sty);
      stmax = std::max(stx, sty);
    } else {
      stmin = stx;
      stmax = alpha + 4.0 * (alpha - stx);
    }

    // Force the step to be within the bounds of alphaMin_ and alphaMax_
    alpha = std::max(alpha, alphaMin_);
    alpha = std::min(alpha, alphaMax_);

    // If an unusal termination is to occur then let alpha be the lowest point
    // obtained so far. 
    if ((bracket && (alpha <= stmin || alpha >= stmax)) ||
        (nFuncEval_ >= maxFuncEval_ - 1) ||
        (uinfo != 0) ||
        (bracket && (stmax - stmin) <= xtol_ * stmax))
      alpha = stx; 

    // Evaluate the function and gradient at x + alpha * p, and compute the
    // directional derivative.
    std::transform(work_.begin(), work_.end(), p, x.begin(),
                   [alpha](double x, double y) { return x + alpha * y; }); 

    nFuncEval_++; 
    func_->eval(x, f, g);
    double dg = std::inner_product(g.begin(), g.end(), p, 0.0); 

    double ftest1 = finit + alpha * dgtest;

    // Test for convergences
    if ((bracket && (alpha <= stmin || alpha >= stmax)) || uinfo != 0)
      return -3; // Rounding error

    if (alpha == alphaMax_ && f <= ftest1 && dg <= dgtest)
      return -4; // Step is too large

    if (alpha == alphaMin_ && (f > ftest1 || dg >= dgtest))
      return -5; // Step is too small

    if (nFuncEval_ >= maxFuncEval_)
      return -6; // Too many function evaluations 

    if (bracket && (stmax - stmin) <= xtol_ * stmax)
      return -7; // Relative width is at most xtol_

    if (f <= ftest1 && fabs(dg) <= eta_ * (-dginit))
      return 0; // Sufficient decrease condition and directional derivative
                // condition hold.  
    
    // In the first stage we seek a step for which the modified function has a
    // nonpositive value and nonnegative derivative.
    if (stage1 && f <= ftest1 && dg >= std::min(mu_, eta_) * dginit)
      stage1 = false; 

    // A modified function is used to predict the step only if we have not
    // obtained a step for which the modified function has a nonpositive
    // function value and nonnegative derivative, and if a lower function value
    // has been obtained but the descrease is not sufficient. 

    if (stage1 && f <= fx && f > ftest1) {
      // Define the modified function and derivative values
      double fm = f - alpha * dgtest;
      double fxm = fx - stx * dgtest;
      double fym = fy - sty * dgtest;
      double dgm = dg - dgtest;
      double dgxm = dgx - dgtest;
      double dgym = dgy - dgtest;

      // Call updateTrial to update the interval of uncertainty and to compute
      // the new step.
      uinfo = updateTrial(stx, fxm, dgxm, sty, fym, dgym, alpha, fm, dgm,
                          bracket, stmin, stmax); 

      // Reset the function and gradient values for f
      fx = fxm + stx * dgtest;
      fy = fym + sty * dgtest;
      dgx = dgxm + dgtest;
      dgy = dgym + dgtest;
    } else {
      // Call updateTrial to update the interval of uncertainty and to compute
      // the new step.
      uinfo = updateTrial(stx, fx, dgx, sty, fy, dgy, alpha, f, dg,
                          bracket, stmin, stmax); 
    }

    // Force a sufficient decrease in the size of the interval of uncertainty
    if (bracket) {
      if (fabs(sty - stx) >= 0.66 * width1) {
        alpha = stx + 0.5 * (sty - stx);
      }
      width1 = width;
      width = fabs(sty - stx);
    }
  }
}


int
MoreThuente::updateTrial(double &x, double &fx, double &dx,
                         double &y, double &fy, double &dy,
                         double &t, double &ft, double &dt,
                         bool &bracket, double tmin, double tmax) const {

  // Check the input parameters for errors.
  // If the trial value t is out of the interval, or the function does not
  // decrease from x, or incorrect tmin/tmax is specified, the function will
  // return with status -1.
  if ((bracket && (t <= std::min(x, y) || t >= std::max(x,y))) ||
      dx * (t - x) >= 0.0 || tmax < tmin)
    return -1; 

  // Determine if the derivatives have opposite sign
  double sgnd = dt * (dx / fabs(dx));

  // Minimizers mc, mq, ms
  double mc, mq, ms;

  // New trial value
  double tnew;
  
  bool bound; 

  if (ft > fx) {
    // Case 1: A higher function value. The minimum is bracketed. If the cubic
    // step (mc) is closer to x than the quadratic step (mq), the cubic is
    // taken, else the average of the cubic and quadratic steps is taken.
    bracket = true;
    bound = true;

    mc = cubicMinimizer1(x, fx, dx, t, ft, dt);
    mq = quadMinimizer1(x, fx, dx, t, ft);

    if (fabs(mc - x) < fabs(mq - x)) {
      tnew = mc;
    } else {
      tnew = mc + 0.5 * (mq - mc);
    }
  } else if (sgnd < 0.0) {
    // Case 2: A lower function value and derivatives of opposite sign. The
    // minimum is bracketed. If the cubic step (mc) is closer to x than the
    // quadratic step (ms), the cubic step is taken, else the quadratic step is
    // taken.
    bracket = true;
    bound = false;

    mc = cubicMinimizer1(x, fx, dx, t, ft, dt);
    ms = quadMinimizer2(x, dx, t, dt);

    if (fabs(mc - t) > fabs(ms - t)) {
      tnew = mc;
    } else {
      tnew = ms;
    }
  } else if (fabs(dt) < fabs(dx)) {
    // Case 3: A lower function value, derivatives of the same sign, and the
    // magnitude of the derivative decreases. The cubic step (mc) is only used
    // if the cubic tends to infinity in the direction of the step or if the
    // minimum of the cubic is beyond t. Otherwise the cubic step is defined to
    // be either tmin or tmax. The quadratic step (ms) is also computed and if
    // the minimum is bracketed then the step closest to x is taken, else the
    // step farthest away is taken. 
    bound = true;
    mc = cubicMinimizer2(x, fx, dx, t, ft, dt, tmin, tmax);
    ms = quadMinimizer2(x, dx, t, dt); 

    if (bracket) {
      if (fabs(t - mc) < fabs(t - ms)) {
        tnew = mc;
      } else {
        tnew = ms;
      }
    } else {
      if (fabs(t - mc) > fabs(t - ms)) {
        tnew = mc;
      } else {
        tnew = ms;
      }
    }
  } else {
    // Case 4: A lower function value, derivative of the same sign, and the
    // magnitude of the derivative does not decrease. If the minimum is not
    // bracketed, the step is either tmin or tmax, else the cubic step is
    // taken. 
    bound = false;

    if (bracket) {
      tnew = cubicMinimizer1(t, ft, dt, y, fy, dy);
    } else if (t > x) {
      tnew = tmax;
    } else {
      tnew = tmin;
    }
  }

  // Update the interval of uncertainty. This update does not depend on the new
  // step or the case analysis above. See the bottom of p. 297 of the reference
  // for details. 
  if (ft > fx) {
    y = t;
    fy = ft;
    dy = dt;
  } else {
    if (sgnd < 0.0) {
      y = x;
      fy = fx;
      dy = dx;
    }

    x = t;
    fx = ft;
    dx = dt;
  }

  // Compute the new step and safeguard it.
  tnew = std::min(tmax, tnew);
  tnew = std::max(tmin, tnew); 

  if (bracket && bound) {
    if (y > x) {
      tnew = std::min(x + 0.66 * (y - x), tnew);
    } else {
      tnew = std::max(x + 0.66 * (y - x), tnew);
    }
  }

  t = tnew;

  return 0;
}

double
MoreThuente::cubicMinimizer1(double u, double fu, double du,
                             double v, double fv, double dv) const {
  double d = v - u;
  double theta = 3 * (fu - fv) / d + du + dv;
  double p = fabs(theta);
  double q = fabs(du);
  double r = fabs(dv);
  double s = std::max(std::max(p, q), r);
  double a = theta / s;
  double gamma = s * sqrt(a * a - (du / s) * (dv / s));
  if (v < u)
    gamma = -gamma;
  p = gamma - du + theta;
  q = gamma - du + gamma + dv;
  r = p / q;
  return u + r * d;       
}

double
MoreThuente::cubicMinimizer2(double u, double fu, double du,
                             double v, double fv, double dv,
                             double xmin, double xmax) const {
  double theta = (fu - fv) * 3 / (v - u) + du + dv;
  double p = fabs(theta);
  double q = fabs(du);
  double r = fabs(dv);
  double s = std::max(std::max(p, q), r);
  double a = theta / s;
  double gamma = s * sqrt(std::max(0.0,
                                   a * a - (du / s) * (dv / s)));
  if (u < v)
    gamma = -gamma;
  
  p = gamma - dv + theta;
  q = gamma + (du - dv) + gamma;
  r = p / q;
  
  if (r < 0.0 && gamma != 0) {
    return v + r * (u - v);
  } else if (v > u) {
    return xmax;
  } else {
    return xmin;
  }
} 

double
MoreThuente::quadMinimizer1(double u, double fu, double du,
                            double v, double fv) const {
  double a = v - u;
  return u + du / ((fu - fv) / a + du) * a / 2; 
}

double
MoreThuente::quadMinimizer2(double u, double du,
                            double v, double dv) const {
  return v + dv * (u - v) / (dv - du);   
}

