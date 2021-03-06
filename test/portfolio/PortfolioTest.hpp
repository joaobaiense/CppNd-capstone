#ifndef TEST_PORTFOLIO_PORTFOLIOTEST_HPP_
#define TEST_PORTFOLIO_PORTFOLIOTEST_HPP_

#include <gtest/gtest.h>

#include <Types/Portfolio.hpp>

class PortfolioTest : public ::testing::Test {
 public:
  PortfolioTest() {
    // Initialization code here
    balance = 1000;
  }

  void SetUp() override {
    // code here will execute just before the test ensues

    // initialize a portfolio
    p = std::make_shared<Portfolio>(balance);
    t = Ochl(0, std::string("TEST_TICKER"), 0, 0, 0, 0, 0);
  }

  void TearDown() override {
    // code weill be called after the test completes
    // exceptions allowed
  }

  ~PortfolioTest() {
    // Clean up any pending stuff, exceptions not allowed
  }

  // custom data members
  std::shared_ptr<Portfolio> p;
  double balance;
  Ochl t;
};

#endif  // TEST_PORTFOLIO_PORTFOLIOTEST_HPP_