#include <stdlib.h>

#include <Log/Log.hpp>
#include <Types/Ochl.hpp>
#include <Types/Portfolio.hpp>
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

void worker(std::shared_ptr<MessageQueue<Ochl>> q, std::mutex *m,
            std::mutex *queueMutex, std::string t, int c,
            std::unordered_map<std::string, Ochl> *ma,
            std::shared_ptr<Portfolio> portfolio) {
  while (true) {
    // wait for a new value
    auto v = q->receive();

    if (v.kill) {
      // stop the worker thread
      return;
    }

    auto maElem = ma->at(v.ticker);

    if (maElem.counter < kEMA_DAYS) {
      maElem.open += v.open;
      maElem.close += v.close;
      maElem.high += v.high;
      maElem.low += v.low;
      maElem.counter++;
    } else if (maElem.counter == kEMA_DAYS) {
      maElem.open /= kEMA_DAYS;
      maElem.close /= kEMA_DAYS;
      maElem.high /= kEMA_DAYS;
      maElem.low /= kEMA_DAYS;
    } else {
      maElem.open = calculate_ema(maElem.open, v.open);
      maElem.close = calculate_ema(maElem.close, v.close);
      maElem.high = calculate_ema(maElem.high, v.high);
      maElem.low = calculate_ema(maElem.low, v.low);
    }

    portfolio->update(&v);
    portfolio->simulateRandom(&v);
  }
}

int main() {
  Log::init("SIM");

  std::set<std::string> tickers;
  int counter = 0;

  ///////////////////////////////////////////////////////////////////////
  //
  //  LOAD DATA
  //
  ///////////////////////////////////////////////////////////////////////

  std::ifstream file("data/capstone_data.csv");

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

  double balance = 10000;
  std::shared_ptr<Portfolio> portfolio = std::make_shared<Portfolio>(balance);
  std::unordered_map<std::string, Ochl> sma;
  std::shared_ptr<MessageQueue<Ochl>> q =
      std::make_shared<MessageQueue<Ochl>>();
  std::vector<std::thread> threads;

  std::mutex m;
  std::mutex queueMutex;

  for (auto t : tickers) {
    sma.insert({t, Ochl(0, t, 0, 0, 0, 0, 0)});
  }

  for (int i = 0; i < 100; i++) {
    threads.push_back(std::thread(worker, q, &m, &queueMutex, std::string(""),
                                  counter, &sma, portfolio));
  }

  ///////////////////////////////////////////////////////////////////////
  //
  //  PROCESS DATA POINTS
  //
  ///////////////////////////////////////////////////////////////////////

  for (int i = 0; i < vec->size(); i++) {
    auto tt = vec->at(i);
    Ochl tmp(tt.date, tt.ticker, tt.open, tt.close, tt.high, tt.low, tt.volume);

    q->send(std::move(tmp));
  }

  portfolio->debugPortfolio();

  double worth = portfolio->getNetValue();
  double percentageChange = (((worth - balance) / balance * 100) * 100) / 100;

  LDEBUG("Portfolio Value: $ {0:.2f} Delta: {1:.2f} %", worth,
         percentageChange);
  LDEBUG("Num Data Points: {}", vec->size());
  LDEBUG("Num Tickers: {}", tickers.size());

  ///////////////////////////////////////////////////////////////////////
  //
  //  CLEANUP
  //
  ///////////////////////////////////////////////////////////////////////

  for (int i = 0; i < threads.size(); i++) {
    Ochl tmp(0, "", 0, 0, 0, 0, 0);
    tmp.kill = true;

    q->send(std::move(tmp));
  }

  for (auto &t : threads) {
    t.join();
  }

  return 0;
}