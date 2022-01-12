
#ifndef TYPES_INCLUDE_TYPES_PORTFOLIO_HPP_
#define TYPES_INCLUDE_TYPES_PORTFOLIO_HPP_

#include <Log/Log.hpp>
#include <Utils/Float.hpp>
#include <iostream>
#include <mutex>
#include <random>
#include <string>
#include <unordered_map>

#include "Ochl.hpp"

struct transaction_t {
  Ochl ref;

  // the price of a single stock in the current time segment
  double currentPrice;

  // the price of a single stock in the previous time segment
  double previousPrice;

  double currentValue;
  double previousValue;

  // current number of share owned in portfolio
  double shares;

  // the current total value of stock portfolio
  double value;

  // flag to specify if transaction is purchase or sell -> used for
  // std::vector<transaction_t>
  bool purchase;

  std::string ticker;
};

class Portfolio {
 public:
  Portfolio(double balance) : m_balance(balance){};
  ~Portfolio(){};

  void purchase(Ochl *ref, double shares);

  void sell(Ochl *ref, double shares);

  void update(Ochl *ref);

  virtual void runAlgorithm(Ochl *ref){};

  double getNetValue();

  double getNetWorth();

  int getNumCompanies() { return m_map_trans.size(); };

  void debugPortfolio();

 protected:
  double m_balance;

  std::unordered_map<std::string, transaction_t> m_map_trans;
  std::vector<transaction_t> m_vec_trans;

  std::mutex m_mut;

  // Setup the random distribution
  static std::mt19937 eng;
  const static uint32_t randMin = 1;
  const static uint32_t randMax = 100;
  static std::uniform_int_distribution<uint32_t> randDist;
};

#endif  // TYPES_INCLUDE_TYPES_PORTFOLIO_HPP_
