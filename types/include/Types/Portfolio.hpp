
#ifndef TYPES_INCLUDE_TYPES_PORTFOLIO_HPP_
#define TYPES_INCLUDE_TYPES_PORTFOLIO_HPP_

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

  // the price of a signle stock in the previous time segment
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

  void purchase(Ochl *ref, double shares) {
    double purchaseAmount = ref->open * shares;
    double purchasePrice = ref->open;
    std::string purchaseTicker = ref->ticker;

    m_mut.lock();

    if (purchaseAmount > m_balance) {
      // can not purchase -- not enough money left

      m_mut.unlock();
      return;
    }

    // m_mut.lock();

    // Store a reference for later debugging if needed
    transaction_t newTransaction;

    newTransaction.purchase = true;
    newTransaction.ref = *ref;
    newTransaction.currentPrice = purchasePrice;
    newTransaction.shares = shares;
    newTransaction.ticker = purchaseTicker;

    m_vec_trans.push_back(newTransaction);

    /////////////////////////////////////////////////////////////////////////////////////////////////////////
    // TODO: WRITE UNIT TESTS
    /////////////////////////////////////////////////////////////////////////////////////////////////////////

    // first check if we already own any stocks of this type
    auto it = m_map_trans.find(purchaseTicker);

    if (it == m_map_trans.end()) {
      // this is the first time purchasing this stock
      transaction_t newTrans;
      newTrans.currentPrice = purchasePrice;
      newTrans.previousPrice = purchasePrice;
      newTrans.currentValue = purchaseAmount;
      newTrans.previousValue = purchaseAmount;
      newTrans.shares = shares;
      newTrans.ref = *ref;
      newTrans.purchase = true;
      newTrans.value = 0;
      newTrans.ticker = purchaseTicker;

      m_map_trans[purchaseTicker] = newTrans;
    }

    // update values

    // withdrawl from the balance
    m_balance -= purchaseAmount;

    // update the shares value
    m_map_trans[purchaseTicker].value += purchaseAmount;

    // std::cout << "Action: [PURCHASE] " << shares << " " << purchaseTicker <<
    // std::endl; debugPortfolio();

    m_mut.unlock();
  };

  void sell(Ochl *ref, double shares) {
    double sellAmount = ref->open * shares;
    double sellPrice = ref->open;
    std::string sellTicker = ref->ticker;

    // TODO: update to smart lock that doesn't require .unlock();
    m_mut.lock();

    // first, check if actually own any shares
    auto it = m_map_trans.find(sellTicker);
    if (it == m_map_trans.end()) {
      // could not find it in the portfolio
      // - ignore request and return
      m_mut.unlock();
      return;
    }

    // next, check if we have enough shares to sell
    if (shares > m_map_trans[sellTicker].shares) {
      return;
    }

    // store transaction for debugging
    transaction_t newTransaction;
    newTransaction.purchase = false;
    newTransaction.ref = *ref;
    newTransaction.currentPrice = sellPrice;
    newTransaction.shares = shares;

    m_vec_trans.push_back(newTransaction);

    // update values

    // decrease the value of the portfolio
    m_map_trans[sellTicker].value -= sellAmount;

    // check if the number of shares sold is same as were available
    if (compare_float(shares, m_map_trans[sellTicker].shares)) {
      m_map_trans.erase(sellTicker);
    } else {
      m_map_trans[sellTicker].shares -= shares;
    }

    // increase transfer the value to the overall balance
    m_balance += sellAmount;

    // std::cout << "Action: [SELL] " << shares << " " << sellTicker <<
    // std::endl; debugPortfolio();

    m_mut.unlock();
  };

  void update(Ochl *ref) {
    // if ticker in portfolio, update internal values
    double currentPrice = ref->open;
    std::string ticker = ref->ticker;

    m_mut.lock();
    auto it = m_map_trans.find(ticker);
    if (it != m_map_trans.end()) {
      auto t = m_map_trans.at(ticker);

      // it exists, so we update
      double currentValue = t.shares * currentPrice;
      double previousValue = t.previousValue;

      t.value += currentValue - previousValue;

      // rotate the values for the next time tick
      t.previousValue = currentValue;
      t.previousPrice = currentPrice;

      m_map_trans.at(ticker) = t;
    }

    m_mut.unlock();

    // run simulation
    // simulateRandom(ref);
  }

  void simulateRandom(Ochl *ref) {
    auto it = m_map_trans.find(ref->ticker);

    if (it != m_map_trans.end()) {
      auto t = m_map_trans.at(ref->ticker);
      if (t.value < 0) {
        sell(ref, t.shares);
        return;
      } else if (t.value > 10) {
        purchase(ref, 1);
        return;
      }
    }

    // randomly decide to take an action: buy or sell
    // generate random number between 1 -> 100
    // if number is less than 10, take action

    uint32_t randVal = randDist(eng);
    uint32_t numShares = randDist(eng);

    if (randVal < 10) {
      // this will be a buy signal
      purchase(ref, static_cast<double>(numShares));

    } else if (randVal > 10 && randVal < 20) {
      // this will be a sell signal

      m_mut.lock();
      auto it = m_map_trans.find(ref->ticker);
      if (it == m_map_trans.end()) {
        // could not find it in the portfolio
        // - ignore request and return
        m_mut.unlock();
        return;
      }

      int shares =
          std::floor(m_map_trans[ref->ticker].shares * numShares / 100);

      m_mut.unlock();

      if (shares > 0 && m_map_trans[ref->ticker].value > 0) {
        sell(ref, shares);
      }
    }
  }

  double getNetValue() {
    double tmp = 0;

    for (auto &m : m_map_trans) {
      auto t = m.second;

      tmp += t.value;
    }

    return tmp;
  }

  double getNetWorth() {
    double tmp = m_balance;

    for (auto &m : m_map_trans) {
      auto t = m.second;

      tmp += t.value;
    }

    return tmp;
  }

  int getNumCompanies() { return m_map_trans.size(); }

  void debugPortfolio() {
    std::cout << "----PORTFOLIO---------------------------" << std::endl;

    std::cout << "Num Companies: " << m_map_trans.size() << std::endl;
    double tmp = 0;

    std::cout << " ---------------" << std::endl;

    for (auto &m : m_map_trans) {
      auto t = m.second;

      std::cout << " | [" << t.shares << "] " << t.ticker << " " << t.value
                << std::endl;

      tmp += t.value;
    }
    std::cout << " ---------------" << std::endl;

    std::cout << std::endl;

    std::cout << "Balance: $ " << m_balance << std::endl;
    std::cout << "  Value: $ " << tmp << std::endl;
    std::cout << "  Total: $ " << (m_balance + tmp) << std::endl << std::endl;

    std::cout << "----------------------------------------" << std::endl;
    std::cout << std::endl << std::endl;
  }

 private:
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

std::random_device rd;
std::mt19937 Portfolio::eng(rd());
std::uniform_int_distribution<uint32_t> Portfolio::randDist(Portfolio::randMin,
                                                            Portfolio::randMax);

#endif  // TYPES_INCLUDE_TYPES_PORTFOLIO_HPP_
