#pragma once
#include <string>

using namespace std;

enum class OrderBookType{bid, ask, unknown, asksale, bidsale};

class OrderBookEntry
{
    public:
        OrderBookEntry(
            string _timestamp,
            string _product,
            OrderBookType _orderType,
            double _price,
            double _amount,
            string _username = "dataset"
        );
        static OrderBookType strToOBT(string s);
        static bool compateByTimestamp(OrderBookEntry& e1, OrderBookEntry& e2) {
            return e1.timestamp < e2.timestamp;
        }
        static bool compateByPriceAsc(OrderBookEntry& e1, OrderBookEntry& e2) {
            return e1.price < e2.price;
        }
        static bool compateByPriceDesc(OrderBookEntry& e1, OrderBookEntry& e2) {
            return e1.price > e2.price;
        }

        double price;
        double amount;
        string timestamp;
        string product;
        OrderBookType orderType;
        string username;
};