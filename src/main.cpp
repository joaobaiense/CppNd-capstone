#include <iostream>
#include <string>
#include <stdlib.h>
#include <Utils/Csv.hpp>
#include <Types/Ochl.hpp>

int main()
{

    // TODO: load both files
    std::ifstream file("data/cap_data_a.csv");

    std::shared_ptr<std::vector<Ochl>> vec = std::make_shared<std::vector<Ochl>>();


    Ochl d;
    double open, close, high, low;
    uint64_t date, volume;
    std::string symbol;

    CSVRow row;
    while (file >> row)
    {

        if (row.size() == 7){
            date = std::stoull(std::string(row[0]));
            symbol = std::string(row[1]);
            open = std::stod(std::string(row[2]));
            close = std::stod(std::string(row[3]));
            high = std::stod(std::string(row[4]));
            low = std::stod(std::string(row[5]));
            volume = static_cast<uint32_t>(std::stod(std::string(row[6])));

            d = Ochl(date, symbol, open, close, high, low, volume);

            vec->push_back(d);
        }
        

    }

    d = vec->back();
    // std::cout << "Ticker: " << tmp.ticker << " Volume: " << std::to_string(tmp.volume) << std::endl;

    // std::cout << d.date << "\t"
    //     << d.open << "\t"
    //     << d.close << "\t"
    //     << d.high << "\t"
    //     << d.low << "\t"
    //     << d.volume << "\t"
    //     << std::endl;

    std::cout << "Vec size: " << vec->size() << std::endl;

    return 0;
}