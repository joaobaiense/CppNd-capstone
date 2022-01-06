#include <iostream>
#include <string>
#include <stdlib.h>
#include <Utils/Csv.hpp>
#include <Utils/Queue.hpp>
#include <Types/Ochl.hpp>
#include <unordered_map>
#include <vector>
#include <thread>
#include <mutex>
#include <set>

const int kIDX_DATE = 0;
const int kIDX_SYMBOL = 1;
const int kIDX_OPEN = 2;
const int kIDX_CLOSE = 3;
const int kIDX_HIGH = 4;
const int kIDX_LOW = 4;
const int kIDX_VOLUME = 5;

int main()
{

    std::set<std::string> tickers;

    ///////////////////////////////////////////////////////////////////////
    //
    //  LOAD DATA 
    //
    ///////////////////////////////////////////////////////////////////////


    // TODO: load both files
    std::ifstream file("data/capstone_data_test.csv");

    std::shared_ptr<std::vector<Ochl>> vec = std::make_shared<std::vector<Ochl>>();

    Ochl d;
    double open, close, high, low;
    uint64_t date, volume;
    std::string symbol;

    CSVRow row;
    while (file >> row) {

        if (row.size() == 7){

            date = std::stoull(std::string(row[kIDX_DATE]));
            symbol = std::string(row[kIDX_SYMBOL]);
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

    // d = vec->back();
    // std::cout << "Ticker: " << tmp.ticker << " Volume: " << std::to_string(tmp.volume) << std::endl;

    // std::cout << d.date << "\t"
    //     << d.open << "\t"
    //     << d.close << "\t"
    //     << d.high << "\t"
    //     << d.low << "\t"
    //     << d.volume << "\t"
    //     << std::endl;

    std::cout << "Num data points: " << vec->size() << std::endl;
    std::cout << "Num tickers: " << tickers.size() << std::endl;

    ///////////////////////////////////////////////////////////////////////
    //
    //  INIT SMA DATA 
    //
    ///////////////////////////////////////////////////////////////////////

    std::unordered_map<std::string, Ochl> sma;
    std::shared_ptr<std::unordered_map<std::string, std::shared_ptr<MessageQueue<Ochl>>>> q = std::make_shared<std::unordered_map<std::string, std::shared_ptr<MessageQueue<Ochl>>>>();

    std::vector<std::thread> threads;

    std::mutex m;

    for( auto t: tickers) {

        sma.insert({t, Ochl(0,t, 0, 0, 0, 0, 0)});

        auto mq = MessageQueue<Ochl>();
        q->insert(std::make_pair(t, std::make_shared<MessageQueue<Ochl>>()));

        threads.emplace_back(std::thread([=, &m, &q](){

            // std::cout << "Starting worker for: " << t << std::endl;

            // while(true){
            //     // wait for a new value
            //     auto v = q->at(t)->receive();

            //     m.lock();
            //     // std::cout << "Ticker update received: " << v.ticker << ": " << v.open << std::endl;
            //     std::string s = std::string(" << Ticker update received: " ) + v.ticker + std::string(" ") + std::to_string(v.open);

            //     std::cout << s <<std::endl;

            //     m.unlock();

            // }
            
        }));
    }

    // Ochl tmp;

    // for (int i = 0; i < 10; i++){
    //     // auto a = vec->at(i);
    //     tmp = vec->at(i);

    //     auto tickerQueue = q->at(tmp.ticker);

    //     std::cout << " >> Sending " << tmp.ticker << std::endl; 
    //     q->at(tmp.ticker)->send(std::move(tmp));

    //     std::this_thread::sleep_for(std::chrono::milliseconds(100));

    // }

    for (auto& t: threads){
        t.join();
    }


    return 0;
}