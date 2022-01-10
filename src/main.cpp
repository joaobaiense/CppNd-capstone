#include <stdlib.h>

#include <Log/Log.hpp>
#include <Types/Ochl.hpp>
#include <Types/Portfolio.hpp>
#include <Types/PortfolioRandom.hpp>
#include <Types/PortfolioRandom2.hpp>
#include <Utils/Csv.hpp>
#include <Utils/Queue.hpp>
#include <algorithm>
#include <iostream>
#include <mutex>
#include <random>
#include <set>
#include <string>
#include <thread>
#include <unordered_map>
#include <vector>

// CSV INDICES
const int kIDX_DATE = 0;
const int kIDX_SYMBOL = 1;
const int kIDX_OPEN = 2;
const int kIDX_CLOSE = 3;
const int kIDX_HIGH = 4;
const int kIDX_LOW = 4;
const int kIDX_VOLUME = 5;

const int kSMOTHING = 2;
const int kEMA_DAYS = 12;

const double kEMA_FACTOR = (kSMOTHING / (1 + kEMA_DAYS));

auto calculate_ema = [](double yesterday, double today) -> double {
  return today * kEMA_FACTOR + yesterday * (1 - kEMA_FACTOR);
};

void update_ema(Ochl *elem, Ochl *newElem) {
  if (elem->counter < kEMA_DAYS) {
    elem->open += newElem->open;
    elem->close += newElem->close;
    elem->high += newElem->high;
    elem->low += newElem->low;
    elem->counter++;
  } else if (elem->counter == kEMA_DAYS) {
    elem->open /= kEMA_DAYS;
    elem->close /= kEMA_DAYS;
    elem->high /= kEMA_DAYS;
    elem->low /= kEMA_DAYS;
  } else {
    elem->open = calculate_ema(elem->open, newElem->open);
    elem->close = calculate_ema(elem->close, newElem->close);
    elem->high = calculate_ema(elem->high, newElem->high);
    elem->low = calculate_ema(elem->low, newElem->low);
  }
}

void worker(std::shared_ptr<MessageQueue<Ochl>> q,
            std::shared_ptr<Portfolio> p) {
  while (true) {
    // wait for a new value
    auto v = q->receive();

    if (v.kill) {
      // stop the worker thread
      return;
    }

    p->update(&v);
    p->runAlgorithm(&v);
  }
}

void summary(std::shared_ptr<Portfolio> p, const double &balance) {
  p->debugPortfolio();

  double worth = p->getNetValue();
  double percentageChange = (((worth - balance) / balance * 100) * 100) / 100;

  LDEBUG("Portfolio Value: $ {0:.2f} Delta: {1:.2f} %", worth,
         percentageChange);
}

int main(int argc, char *argv[]) {
  Log::init("SIM");

  double balance = 10000;

  if (argc == 2) {
    LDEBUG("User provided balance: {}", argv[1]);

    try {
      balance = std::stod(argv[1]);
    } catch (const std::exception &e) {
      LERROR("Could not parse user provided input for balance.");
      LDEBUG("Defauting balance to {}", balance);
    }
  }

  std::set<std::string> tickers;
  int counter = 0;

  ///////////////////////////////////////////////////////////////////////
  //
  //  LOAD DATA
  //
  ///////////////////////////////////////////////////////////////////////

  std::ifstream file("data/capstone_data_test.csv");

  std::shared_ptr<std::vector<Ochl>> vec =
      std::make_shared<std::vector<Ochl>>();

  Ochl d;
  double open, close, high, low;
  uint64_t date, volume;
  std::string symbol;

  CSVRow row;
  while (file >> row) {
    if (row.size() == 7) {
      date = std::stoull(std::string(row[kIDX_DATE]));
      symbol = std::string(row[kIDX_SYMBOL]);

      // remove white space from symbol
      symbol.erase(std::remove(symbol.begin(), symbol.end(), ' '),
                   symbol.end());

      open = std::stod(std::string(row[kIDX_OPEN]));
      close = std::stod(std::string(row[kIDX_CLOSE]));
      high = std::stod(std::string(row[kIDX_HIGH]));
      low = std::stod(std::string(row[kIDX_LOW]));
      volume = static_cast<uint32_t>(std::stod(std::string(row[kIDX_VOLUME])));

      d = Ochl(date, symbol, open, close, high, low, volume);

      vec->push_back(d);

      // add to list of tickers
      tickers.insert(symbol);
    }
  }

  ///////////////////////////////////////////////////////////////////////
  //
  //  INIT PORTFOLIO & WORKERS
  //
  ///////////////////////////////////////////////////////////////////////

  std::shared_ptr<Portfolio> p1 = std::make_shared<PortfolioRandom>(balance);
  std::shared_ptr<Portfolio> p2 = std::make_shared<PortfolioRandom2>(balance);

  // std::unordered_map<std::string, Ochl> sma;
  std::shared_ptr<MessageQueue<Ochl>> q1 =
      std::make_shared<MessageQueue<Ochl>>();

  std::shared_ptr<MessageQueue<Ochl>> q2 =
      std::make_shared<MessageQueue<Ochl>>();

  std::vector<std::thread> threads;

  // for (auto t : tickers) {
  //   sma.insert({t, Ochl(0, t, 0, 0, 0, 0, 0)});
  // }

  for (int i = 0; i < 4; i++) {
    threads.push_back(std::thread(worker, q1, p1));
  }

  for (int i = 0; i < 4; i++) {
    threads.push_back(std::thread(worker, q2, p2));
  }

  ///////////////////////////////////////////////////////////////////////
  //
  //  PROCESS DATA POINTS
  //
  ///////////////////////////////////////////////////////////////////////

  for (int i = 0; i < vec->size(); i++) {
    auto tt = vec->at(i);
    Ochl tmp(tt.date, tt.ticker, tt.open, tt.close, tt.high, tt.low, tt.volume);
    Ochl tmp2 = tmp;

    q1->send(std::move(tmp));
    q2->send(std::move(tmp2));
  }

  summary(p1, balance);
  summary(p2, balance);

  LDEBUG("");
  LDEBUG("Num Data Points: {}", vec->size());
  LDEBUG("Num Tickers: {}", tickers.size());

  ///////////////////////////////////////////////////////////////////////
  //
  //  CLEANUP
  //
  ///////////////////////////////////////////////////////////////////////

  for (int i = 0; i < 4; i++) {
    Ochl tmp(0, "", 0, 0, 0, 0, 0);
    tmp.kill = true;

    Ochl tmp2 = tmp;

    q1->send(std::move(tmp));
    q2->send(std::move(tmp2));
  }

  for (auto &t : threads) {
    t.join();
  }

  return 0;
}