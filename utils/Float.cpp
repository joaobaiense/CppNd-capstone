
#include "include/Utils/Float.hpp"

bool compare_float(double x, double y, double epsilon) {
  if (std::fabs(x - y) < epsilon) return true;  // they are same
  return false;                                 // they are not same
}