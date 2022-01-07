
#include "PortfolioTest.hpp"
#include <Utils/Float.hpp>


TEST_F(PortfolioTest, willNotUpdateWorthIfUpdateTickerIfNotInPortfolio){
    t.open = 10;
    t.ticker = std::string("TEST_TICKER");

    p->update(&t);

    EXPECT_TRUE(compare_float(balance, p->getNetWorth()));
}

TEST_F(PortfolioTest, purchaseWillUpdateNetWorth){
    t.open = 10; 
    t.ticker = std::string("TEST_TICKER");

    p->purchase(&t, 1);

    EXPECT_TRUE(compare_float(t.open, p->getNetValue()));
}

