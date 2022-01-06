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
#include <random>

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
    
    // the price of a single stock in the current time segment
    double currentPrice;

    // the price of a signle stock in the previous time segment 
    double previousPrice;

    double currentValue;
    double previousValue;

    // current number of share owned in portfolio
    double shares;

    // the current total value of stock portfolio 
    double value; 

    // flag to specify if transaction is purchase or sell -> used for std::vector<transaction_t> 
    bool purchase;

    std::string ticker;
    
};

class Portfolio {

public:
    Portfolio(double balance): m_balance(balance){};
    ~Portfolio(){};

    void purchase(Ochl *ref, double shares){

        double purchaseAmount = ref->open * shares;
        double purchasePrice = ref->open;
        std::string purchaseTicker = ref->ticker;

        m_mut.lock();

        if (purchaseAmount > m_balance){
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

/////////////////////////////////////////////////////////////////////////////////////////////////////////
        // TODO: WRITE UNIT TESTS
/////////////////////////////////////////////////////////////////////////////////////////////////////////
      
        // first check if we already own any stocks of this type 
        auto it = m_map_trans.find(purchaseTicker);

        if (it == m_map_trans.end()){
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
      
        m_mut.unlock();

    };

    void sell(Ochl *ref, double shares){

        double sellAmount = ref->open * shares;
        double sellPrice = ref->open;
        std::string sellTicker = ref->ticker;

        // TODO: update to smart lock that doesn't require .unlock();
        m_mut.lock();

        // first, check if actually own any shares 
        auto it = m_map_trans.find(sellTicker);
        if (it == m_map_trans.end()){
            // could not find it in the portfolio
            // - ignore request and return 
            m_mut.unlock();
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

        // increase transfer the value to the overall balance
        m_balance += sellAmount;

        m_mut.unlock();

    };

    void update(Ochl *ref){

        // if ticker in portfolio, update internal values 
        double currentPrice = ref->open;
        std::string ticker = ref->ticker;

        m_mut.lock();
        auto it = m_map_trans.find(ticker);
        if (it != m_map_trans.end()){

            auto t = m_map_trans[ticker];

            // it exists, so we update 
            double currentValue = t.shares * currentPrice;
            double previousValue = t.previousValue;
            
            t.value += currentValue - t.previousValue;

            // rotate the values for the next time tick
            m_map_trans[ticker].previousValue = currentValue;
            m_map_trans[ticker].previousPrice = currentPrice;

        }

        m_mut.unlock();


        // run simulation 
        simulateRandom(ref);

    }

    void simulateRandom(Ochl *ref){

        // randomly decide to take an action: buy or sell 
            // generate random number between 1 -> 100
            // if number is less than 10, take action 
        
        uint32_t randVal = randDist(eng);
        uint32_t numShares = randDist(eng);

        if (randVal < 10){
            // this will be a buy signal 
            purchase(ref, static_cast<double>(numShares));
            
        } else if (randVal > 10 && randVal < 20){
            //this will be a sell signal 
            sell(ref, static_cast<double>(numShares));
        }

    }

    double getNetValue(){
        // return m_balance + m_value;
        double tmp = m_balance;

        for (auto &m: m_map_trans){

            auto t = m.second;
            tmp += t.value;

            std::cout << " | " << t.ticker << "\t" << t.value << std::endl;

        }


        // for (auto &t : m_vec_trans){
        //     auto d = t.purchase ? " [BUY] " : "[SELL] ";

        //     std::cout << d;
        //     std::cout << t.ref.ticker << " (" << t.shares << ") " << t.price << std::endl;
        // }

        std::cout << "Num Transactions: " << m_vec_trans.size() << std::endl;
        std::cout << "Portfolio Balance: " << tmp << std::endl;

        return tmp;
    }

private:
    double m_balance;

    std::unordered_map<std::string, transaction_t> m_map_trans;
    std::vector<transaction_t> m_vec_trans;

    std::mutex m_mut;

    // Setup the random distribution
    static std::mt19937 eng;
    const static uint32_t randMin = 0;
    const static uint32_t randMax = 100;
    static std::uniform_int_distribution<uint32_t> randDist;
};

std::random_device rd;
std::mt19937 Portfolio::eng(rd());
std::uniform_int_distribution<uint32_t> Portfolio::randDist(Portfolio::randMin, Portfolio::randMax);


double balance = 1000;
std::shared_ptr<Portfolio> portfolio = std::make_shared<Portfolio>(balance);



void worker(std::shared_ptr<MessageQueue<Ochl>> q, std::mutex *m, std::mutex *queueMutex, std::string t, int c, std::unordered_map<std::string, Ochl> *ma){

    // int day = 0;
    // double smaStart = 0;

    // double open, close, high, low;

    // m->lock();

    // // int thisThread = c;

    // // std::cout << std::to_string(thisThread) << " " << " " 
    // // << std::this_thread::get_id()
    // // << " Worker for: " << t << std::endl;

    // m->unlock();
    
    while(true){
        // wait for a new value
        auto v = q->receive();
        // queueMutex->lock();
        // auto elem = q->at(t);
        auto maElem = ma->at(v.ticker);
        // queueMutex->unlock();

        // auto v = elem->receive();

        if ( maElem.counter < kEMA_DAYS ){
            maElem.open += v.open;
            maElem.close += v.close;
            maElem.high += v.high;
            maElem.low += v.low;
            maElem.counter++;
        } else if (maElem.counter = kEMA_DAYS) {
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

        // m->lock();

        // std::cout << " << Ticker update received: " << v.ticker << ": " << v.open << std::endl;
        // std::string s = std::string(" << Ticker update received: " ) + std::to_string(thisThread) + std::string(" ") + v.ticker + std::string(" ") + std::to_string(v.open) + " " ;

        // std::cout << s <<std::endl;

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
    // std::shared_ptr<std::unordered_map<std::string, std::shared_ptr<MessageQueue<Ochl>>>> q = std::make_shared<std::unordered_map<std::string, std::shared_ptr<MessageQueue<Ochl>>>>();
    std::shared_ptr<MessageQueue<Ochl>> q = std::make_shared<MessageQueue<Ochl>>();

    std::vector<std::thread> threads;

    std::mutex m;
    std::mutex queueMutex;

    for( auto t: tickers) {

        sma.insert({t, Ochl(0,t, 0, 0, 0, 0, 0)});

        // queueMutex.lock();
        // q->insert(std::make_pair(t, std::make_shared<MessageQueue<Ochl>>()));
        // queueMutex.unlock();

        // threads.push_back(std::thread(worker, q.get(), &m, &queueMutex, t, counter, &sma));

        // counter++;
    }

    for (int i = 0; i < 100; i++ ){
        threads.push_back(std::thread(worker, q, &m, &queueMutex, std::string(""), counter, &sma));
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

        // queueMutex.lock();
        // auto elem = q->at(tmp.ticker);
        // queueMutex.unlock();

        q->send(std::move(tmp));

        // std::this_thread::sleep_for(std::chrono::milliseconds(100));
        
        if (i % 10000 == 0 ){
            std::cout << "Processing element: " << i << std::endl;
        }

    }

    std::cout << "FINISHED" << std::endl;

    portfolio->getNetValue();

 
    // for (auto& t: threads){
    //     t.join();
    // }


    return 0;
}