
#include "include/Types/Portfolio.hpp"

// Initialize random engine
std::random_device rd;
std::mt19937 Portfolio::eng(rd());
std::uniform_int_distribution<uint32_t> Portfolio::randDist(Portfolio::randMin,
                                                            Portfolio::randMax);

void Portfolio::purchase(Ochl *ref, double shares) {
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
}

void Portfolio::sell(Ochl *ref, double shares) {
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

void Portfolio::update(Ochl *ref) {
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
}

double Portfolio::getNetValue() {
  double tmp = 0;

  for (auto &m : m_map_trans) {
    auto t = m.second;

    tmp += t.value;
  }

  return tmp;
}

double Portfolio::getNetWorth() {
  double tmp = m_balance;

  for (auto &m : m_map_trans) {
    auto t = m.second;

    tmp += t.value;
  }

  return tmp;
}

void Portfolio::debugPortfolio() {
  LDEBUG("");

  LDEBUG("----PORTFOLIO---------------------------");

  LDEBUG("| Num Companies: {}", m_map_trans.size());

  double tmp = 0;

  LDEBUG("|  ---------------");

  for (auto &m : m_map_trans) {
    auto t = m.second;

    LDEBUG("|  | [{}] {} {:.2f}", t.shares, t.ticker, t.value);

    tmp += t.value;
  }

  LDEBUG("|  ---------------");

  LDEBUG("| Balance: $ {0:.2f}", m_balance);
  LDEBUG("|   Value: $ {0:.2f}", tmp);
  LDEBUG("|   Total: $ {0:.2f}", (m_balance + tmp));
  LDEBUG("----------------------------------------");
  LDEBUG("");
}