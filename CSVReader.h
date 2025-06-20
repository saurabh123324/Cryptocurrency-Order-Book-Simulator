#pragma once

#include <vector>
#include <string>
#include "OrderBookEntry.h"

using namespace std;

class CSVReader {
    public:
        CSVReader();
        static vector<OrderBookEntry> readCSV(string csvFilename);
        static vector<string> tokenize(string csvLine, char separator);
        static OrderBookEntry strToOBE(
            string timestamp,
            string product,
            OrderBookType orderType,
            string priceString,
            string amountString
        );
    private:
        static OrderBookEntry strToOBE(vector<string> strings);
};