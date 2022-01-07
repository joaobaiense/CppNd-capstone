
#ifndef UTILS_INCLUDE_UTILS_FLOAT_HPP_
#define UTILS_INCLUDE_UTILS_FLOAT_HPP_

#include <cmath>

bool compare_float(double x, double y, double epsilon = 0.0000001f){
   if(std::fabs(x - y) < epsilon)
      return true; //they are same
    return false; //they are not same
}

#endif // UTILS_INCLUDE_UTILS_FLOAT_HPP_