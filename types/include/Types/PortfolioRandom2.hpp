#ifndef TYPES_INCLUDE_TYPES_PORTFOLIORANDOM2_HPP_
#define TYPES_INCLUDE_TYPES_PORTFOLIORANDOM2_HPP_

#include "Portfolio.hpp"

class PortfolioRandom2 : public Portfolio {
 public:
  PortfolioRandom2(double balance) : Portfolio(balance){};
  void runAlgorithm(Ochl *ref) override;
};

#endif  // TYPES_INCLUDE_TYPES_PORTFOLIORANDOM2_HPP_