#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include "CSVReader.h"

using namespace std;

CSVReader::CSVReader() {

}
vector<OrderBookEntry> CSVReader::readCSV(string csvFilename) {
    vector<OrderBookEntry> entries;
    ifstream csvFile(csvFilename);
    string line;

    if (csvFile.is_open()) {
        while (getline(csvFile, line)) {
            try {
                OrderBookEntry obe = strToOBE(tokenize(line, ','));
                entries.push_back(obe);
            } catch (const exception& e) {
                cout << "CSVReader::readCSV has read bad data " << endl;
            }
        } // end while
        csvFile.close();
        cout << "CSVReader::readCSV has read " << entries.size() << " entries" << endl;
    }
    else {
        cout << "File can not be opened!" << endl;
    }

    return entries;
}
vector<string> CSVReader::tokenize(string csvLine, char separator) {

    // string vector tokens ## stores the tokens
    vector<string> tokens;

    // int start end ## used to delineate token positions
    signed int start, end;
    string token;

    start = csvLine.find_first_not_of(separator, 0);
    do {
        end = csvLine.find_first_of(separator, start);
        if (start == csvLine.length() || start == end) {break;}
        if (end >= 0) { // a token is found
            token = csvLine.substr(start, end - start);
        }
        else {
            token = csvLine.substr(start, csvLine.length() - start);
        }
        tokens.push_back(token);
        start = end + 1;
    } while (end > 0);

    return tokens;
}
OrderBookEntry CSVReader::strToOBE(vector<string> tokens) {
    if (tokens.size() != 5) {
        cout << "\nError: a bad line!\n" << endl;
        throw exception{};
    }
    double price, amount;
    try {
        price = stod(tokens[3]);
        amount = stod(tokens[4]);
    } catch (exception& e) {
        cout << "Bad float(s)! " << tokens[3] << " | " << tokens[4] << endl;
        throw e;
    }
    OrderBookEntry obe{
        tokens[0],
        tokens[1],
        OrderBookEntry::strToOBT(tokens[2]),
        price,
        amount
    };
    return obe;
}
OrderBookEntry CSVReader::strToOBE(
    string timestamp,
    string product,
    OrderBookType orderType,
    string priceString,
    string amountString
) {
    double price, amount;
    try {
        price = stod(priceString);
    } catch (exception& e) {
        cout << "CSVReader::strToOBE() Bad float(s)! " << priceString << endl;
    }
    try {
        amount = stod(amountString);
    } catch (exception& e) {
        cout << "CSVReader::strToOBE() Bad float(s)! " << amountString << endl;
    }

    OrderBookEntry obe{
        timestamp,
        product,
        orderType,
        price,
        amount
    };
    return obe;
}