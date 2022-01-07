#ifndef TYPES_INCLUDE_TYPES_OCHL_HPP_
#define TYPES_INCLUDE_TYPES_OCHL_HPP_

#include <string>

class Ochl {
 public:
  Ochl(){};
  Ochl(uint64_t date, std::string ticker, double open, double close,
       double high, double low, uint64_t volume)
      : date(std::move(date)),
        ticker(std::move(ticker)),
        open(std::move(open)),
        close(std::move(close)),
        high(std::move(high)),
        low(std::move(low)),
        volume(std::move(volume)){};

  ~Ochl(){};

  // copy
  // move

  double open = 0.0;
  double close = 0.0;
  double high = 0.0;
  double low = 0.0;
  uint64_t volume = 0;
  uint64_t counter = 0;

  bool kill = false;

  // private:
  std::string ticker = "";
  uint64_t date = 0;
};

#endif  // TYPES_INCLUDE_TYPES_OCHL_HPP_
