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
#include <algorithm>

const int kIDX_DATE = 0;
const int kIDX_SYMBOL = 1;
const int kIDX_OPEN = 2;
const int kIDX_CLOSE = 3;
const int kIDX_HIGH = 4;
const int kIDX_LOW = 4;
const int kIDX_VOLUME = 5;

const int kSMOTHING = 2;
const int kEMA_DAYS = 12;

const double kEMA_FACTOR = (kSMOTHING/(1+kEMA_DAYS)); 

auto calculate_ema = [](double yesterday, double today) -> double {
    return today * kEMA_FACTOR + yesterday * (1 - kEMA_FACTOR);
};

struct transaction_t {
    Ochl ref;
    double refPrice;
    double shares;
    bool purchase;
};

class Portfolio {

public:
    Portfolio(double balance): m_balance(balance){};
    ~Portfolio(){};

    void purchase(Ochl *ref, double shares){

        double amount = ref->open * shares;
        double newPrice = ref->open;

        if (amount > m_balance){
            // can not purchase -- not enough money left 
            return;
        }

        m_mut.lock();

        transaction_t newTransaction;
        newTransaction.purchase = true;
        newTransaction.ref = *ref;
        newTransaction.refPrice = newPrice;
        newTransaction.shares = shares;

        m_vec_trans.push_back(newTransaction);

        // subtract from balance 
        m_balance -= amount;

/////////////////////////////////////////////////////////////////////////////////////////////////////////
        // TODO: WRITE UNIT TESTS
/////////////////////////////////////////////////////////////////////////////////////////////////////////
        auto it = m_map_trans.find(ref->ticker);
        if (it != m_map_trans.end()){
            // we already own shares, so update values 
            transaction_t t = it->second;

            double worth = newPrice * t.shares;
            double originalWorth = t.refPrice * t.shares;

            double delta = worth - originalWorth;

            shares += delta / newPrice;

            // double equivalentShares = 

        }

        // add to value 
        m_value += amount;

        transaction_t newTransaction;
        newTransaction.purchase = true;
        newTransaction.ref = *ref;
        newTransaction.refPrice = newPrice;
        newTransaction.shares = shares;

        m_map_trans[ref->ticker] = newTransaction;


        m_mut.unlock();

    };

    void sell(Ochl *ref, double shares){

        double amount = ref->open * shares;
        double newPrice = ref->open;

        m_mut.lock();

        auto it = m_map_trans.find(ref->ticker);
        if (it != m_map_trans.end()){

            transaction_t t = it->second;

            if (shares > t.shares){
                m_mut.unlock();
                return;
            }

            t.shares -= shares;
        }

        // subtract from value 
        m_value -= amount;

        // add to balance 
        m_balance += amount;

        transaction_t newTransaction;
        newTransaction.purchase = false;
        newTransaction.ref = *ref;
        newTransaction.refPrice = newPrice;
        newTransaction.shares = shares;

        m_vec_trans.push_back(newTransaction);

        m_mut.unlock();

    };

    void update(Ochl *ref){

        // if ticker in portfolio, update m_value 
        auto it = m_map_trans.find(ref->ticker);
        if (it != m_map_trans.end()){

            transaction_t t = it->second;

            //update the portfolio value 
        }

        // run simulation 
    }

    void simulateRandom(Ochl *ref){

        // randomly decide to take an action: buy or sell 
            // generate random number between 1 -> 100
            // if number is less than 10, take action 

        // randomly decide number of shares to perform action 



    }

    double getNetValue(){
        return m_balance + m_value;
    }

private:
    double m_balance = 0;
    double m_value = 0; 

    std::unordered_map<std::string, transaction_t> m_map_trans;
    std::vector<transaction_t> m_vec_trans;

    std::mutex m_mut;
};

void worker(std::unordered_map<std::string, std::shared_ptr<MessageQueue<Ochl>>> *q, std::mutex *m, std::mutex *queueMutex, std::string t, int c, std::unordered_map<std::string, Ochl> *ma){

    int day = 0;
    double smaStart = 0;

    double open, close, high, low;

    m->lock();

    int thisThread = c;

    // std::cout << std::to_string(thisThread) << " " << " " 
    // << std::this_thread::get_id()
    // << " Worker for: " << t << std::endl;

    m->unlock();
    
    while(true){
        // wait for a new value
        queueMutex->lock();
        auto elem = q->at(t);
        auto maElem = ma->at(t);
        queueMutex->unlock();

        auto v = elem->receive();

        if ( day < kEMA_DAYS ){
            open += v.open;
            close += v.close;
            high += v.high;
            low += v.low;
            day++;

        } else if (day = kEMA_DAYS) {
            maElem.open = open/kEMA_DAYS;
            maElem.close = close/kEMA_DAYS;
            maElem.high = high/kEMA_DAYS;
            maElem.low = low/kEMA_DAYS;
        } else {
            maElem.open = calculate_ema(maElem.open, v.open);
            maElem.close = calculate_ema(maElem.close, v.close);
            maElem.high = calculate_ema(maElem.high, v.high);
            maElem.low = calculate_ema(maElem.low, v.low);
        }

        // m->lock();

        // std::cout << "[" << thisThread << "] Ticker update received: " << v.ticker << ": " << v.open << std::endl;
        // // std::string s = std::string(" << Ticker update received: " ) + std::to_string(thisThread) + std::string(" ") + v.ticker + std::string(" ") + std::to_string(v.open) + " " ;

        // // std::cout << s <<std::endl;

        // m->unlock();

    }
    
}

int main()
{

    std::set<std::string> tickers;
    int counter = 0;

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

            // remove white space from symbol
            symbol.erase(std::remove (symbol.begin(), symbol.end(), ' '), symbol.end());

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
    std::mutex queueMutex;

    for( auto t: tickers) {

        sma.insert({t, Ochl(0,t, 0, 0, 0, 0, 0)});

        queueMutex.lock();
        q->insert(std::make_pair(t, std::make_shared<MessageQueue<Ochl>>()));
        queueMutex.unlock();

        threads.push_back(std::thread(worker, q.get(), &m, &queueMutex, t, counter, &sma));

        counter++;
    }

    std::cout << "Num Worker Threads: " << threads.size() << std::endl;

    std::this_thread::sleep_for(std::chrono::seconds(1));


    for (int i = 0; i < vec->size(); i++){
        // auto a = vec->at(i);
        auto tt = vec->at(i);
        Ochl tmp(tt.date, tt.ticker, tt.open, tt.close, tt.high, tt.low, tt.volume);

        // m.lock();
        // std::cout << " >> Sending " << tmp.ticker << std::endl; 
        // m.unlock();

        queueMutex.lock();
        auto elem = q->at(tmp.ticker);
        queueMutex.unlock();

        elem->send(std::move(tmp));

        // std::this_thread::sleep_for(std::chrono::milliseconds(100));
        
        if (i % 10000 == 0 ){
            std::cout << "Processing element: " << i << std::endl;
        }

    }

    std::cout << "FINISHED" << std::endl;

 
    for (auto& t: threads){
        t.join();
    }


    return 0;
}