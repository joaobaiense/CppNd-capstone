#include <iostream>
#include <string>
#include <stdlib.h>
#include <Utils/Csv.hpp>
#include <Utils/Queue.hpp>
#include <Types/Ochl.hpp>
#include <unordered_map>
#include <vector>
#include <thread>
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
    std::unordered_map<std::string, std::shared_ptr<MessageQueue<Ochl>>> q;
    std::vector<std::thread> threads;

    for( auto t: tickers) {

        sma.insert({t, Ochl(0,t, 0, 0, 0, 0, 0)});
        q.insert({t, std::make_shared<MessageQueue<Ochl>>()});

        // MessageQueue<Ochl> *tickerQueue = q.at(t).get();

        threads.emplace_back(std::thread([&q, t](){
        
        // threads.push_back(std::thread([]{
            
            while(true){
                // wait for a new value
                auto v = q.at(t)->receive();

                std::cout << "Ticker update received: " << v.ticker << ": " << v.open << std::endl;

            }
            
        }));
    }

    // Ochl tmp;

    // for (int i = 0; i < 10; i++){
    //     auto a = vec->at(i);
    //     Ochl tmp = Ochl(0, a.ticker, a.open, 0,0,0,0);

    //     auto tickerQueue = q.at(tmp.ticker);
    //     q.at(tmp.ticker)->send(std::move(tmp));

    // }

    // for (auto& t: threads){
    //     t.join();
    // }


    return 0;
}