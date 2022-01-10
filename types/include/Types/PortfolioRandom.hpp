#ifndef TYPES_INCLUDE_TYPES_PORTFOLIORANDOM_HPP_
#define TYPES_INCLUDE_TYPES_PORTFOLIORANDOM_HPP_

#include "Portfolio.hpp"

class PortfolioRandom : public Portfolio {
 public:
  PortfolioRandom(double balance) : Portfolio(balance){};
  void runAlgorithm(Ochl *ref) override;
};

#endif  // TYPES_INCLUDE_TYPES_PORTFOLIORANDOM_HPP_