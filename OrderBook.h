#pragma once
#include "OrderBookEntry.h"
#include "CSVReader.h"
#include <string>
#include <vector>

using namespace std;

class OrderBook {

    public:
        /** Construct, reading a csv file */
        OrderBook(std::string filename);

        /** Return vector of all known products in the dataset */
        vector<string> getKnownProducts(); //

        /** Return vector of Orders according to the sent filters */
        vector<OrderBookEntry> getOrders(
            OrderBookType type,
            string product,
            string timestamp
        ); //

        /** Returns the earlist time in the orderBook */
        string getEarlistTime(); //

        /** Returns the next time after the sent time in the orderBook
         * If there is no next timstamps, wraps around to the start
        */
        string getNextTime(string timestamp);   //

        void insertOrder(OrderBookEntry& order);  //

        vector<OrderBookEntry> matchAsksToBids(string product, string timestamp);   //

        static double getHighPrice(vector<OrderBookEntry>& orders);
        static double getLowPrice(vector<OrderBookEntry>& orders);
        static vector<double> get1HPrices(vector<OrderBookEntry>& orders, string& currentTime);
        static double getPriceMean(vector<OrderBookEntry>& orders);
        static double getPriceStd(vector<OrderBookEntry>& orders);

    private:
        vector<OrderBookEntry> orders;
};