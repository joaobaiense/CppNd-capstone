
#include "include/Types/PortfolioRandom2.hpp"

void PortfolioRandom2::runAlgorithm(Ochl *ref) {
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

    int shares = std::floor(m_map_trans[ref->ticker].shares * numShares / 100);

    m_mut.unlock();

    if (shares > 0 && m_map_trans[ref->ticker].value > 0) {
      sell(ref, shares);
    }
  }
}