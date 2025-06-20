#include "OrderBook.h"
#include <map>
#include <ctime>
#include <sstream>
#include <iomanip>
#include <cmath>
#include <iostream>

using namespace std;

/* Construct, reading a csv file */
OrderBook::OrderBook(std::string filename) {
    orders = CSVReader::readCSV(filename);
}

/* Return vector of all known products in the dataset */
vector<string> OrderBook::getKnownProducts() {
    vector<string> products;
    map<string, bool> prodMap;

    for (OrderBookEntry& order : orders) {
        prodMap[order.product] = true;
    }

    // Now flatten the map to a vector of strings
    for (auto const& product : prodMap) {
        products.push_back(product.first);
    }

    return products;
}

/* Return vector of Orders according to the sent filters */
vector<OrderBookEntry> OrderBook::getOrders(
    OrderBookType type,
    string product,
    string timestamp
) {
    vector<OrderBookEntry> orders_sub;
    for (OrderBookEntry& entry : orders) {
        if (
            entry.orderType == type &&
            entry.product == product &&
            entry.timestamp == timestamp
        ) {
            orders_sub.push_back(entry);
        }
    }

    return orders_sub;
}


double OrderBook::getHighPrice(vector<OrderBookEntry>& orders) {
    double max = orders[0].price;
    for (OrderBookEntry& entry : orders) {
        if (entry.price > max) {max = entry.price;}

    }
    return max;
}

double OrderBook::getLowPrice(vector<OrderBookEntry>& orders) {
    double min = orders[0].price;
    for (OrderBookEntry& entry : orders) {
        if (entry.price < min) {min = entry.price;}

    }
    return min;
}

vector<double> OrderBook::get1HPrices(vector<OrderBookEntry>& orders, string& currentTime) {
    // Parse the currentTime into a time_t object
    tm tm = {};
    istringstream ss(currentTime.substr(0, 19));
    ss >> get_time(&tm, "%Y/%m/%d %H:%M:%S");
    time_t currentTimestamp = mktime(&tm);

    double firstPrice = 0.0;
    double lastPrice = 0.0;
    bool foundFirst = false;

    // Iterate over the orders to find the prices within the 1H window
    for (const auto& order : orders)
    {
        // Parse the order timestamp into a time_t object
        std::tm orderTime = {};
        std::istringstream orderSS(order.timestamp.substr(0, 19));
        orderSS >> std::get_time(&orderTime, "%Y/%m/%d %H:%M:%S");
        time_t orderTimestamp = mktime(&orderTime);

        // Check if the order is within the 1-hour window
        if (difftime(orderTimestamp, currentTimestamp) <= 60) {
            if (!foundFirst) {
                firstPrice = order.price;
                foundFirst = true;
            }
            lastPrice = order.price;
        }

        if (!foundFirst) {
            throw runtime_error("No orders found within the 24-hour window");
        }
    }

    vector<double> prices1H;
    prices1H.push_back(firstPrice);
    prices1H.push_back(lastPrice);

    return prices1H;
}

double OrderBook::getPriceMean(vector<OrderBookEntry>& orders) {
    double sum = 0;
    for (const OrderBookEntry& order : orders) {
        sum += order.price;
    }
    return orders.empty() ? 0 : sum / orders.size();
}

double OrderBook::getPriceStd(vector<OrderBookEntry>& orders) {
    double sum_sq = 0;
    double priceMean = getPriceMean(orders);
    for (const OrderBookEntry& order : orders) {
        sum_sq += pow((order.price - priceMean), 2);
    }
    return sqrt(sum_sq);
}

string OrderBook::getEarlistTime() {
   return orders[0].timestamp;
}

string OrderBook::getNextTime(string timestamp) {
   string next_timestamp = "";
   for (OrderBookEntry& entry : orders) {
        if (entry.timestamp > timestamp) {
            next_timestamp = entry.timestamp;
            break;
        }
   }
   if (next_timestamp == "") {
        next_timestamp = orders[0].timestamp;
   }
   return next_timestamp;
}

void OrderBook::insertOrder(OrderBookEntry& order) {
    orders.push_back(order);
    sort(orders.begin(), orders.end(), OrderBookEntry::compateByTimestamp);
}
/*
vector<OrderBookEntry> OrderBook::matchAsksToBids(
    string product,
    string timestamp
) {
    // asks = orderbook.asks
    vector<OrderBookEntry> asks = getOrders(OrderBookType::ask, product, timestamp);
    // bids = orderbook.bids
    vector<OrderBookEntry> bids = getOrders(OrderBookType::bid, product, timestamp);
    cout << "Matching for: " << timestamp << endl;
    cout << "Asks count: " << asks.size() << ", Bids count: " << bids.size() << endl;
    // sales = []
    vector<OrderBookEntry> sales;

    // check to ensure we have bids and asks to process.
    if (asks.size() == 0 || bids.size() == 0) {
        std::cout << " OrderBook::matchAsksToBids no bids or asks" << std::endl;
        return sales;
    }

    // sort asks lowest first
    sort(asks.begin(), asks.end(), OrderBookEntry::compateByPriceAsc);
    // sort bids highest first
    sort(bids.begin(), bids.end(), OrderBookEntry::compateByPriceDesc);

    for (OrderBookEntry& ask : asks) {
        for (OrderBookEntry& bid : bids) {
            if (bid.price >= ask.price) {
                OrderBookEntry sale{
                    timestamp, product, OrderBookType::asksale, ask.price, 0
                };

                if (bid.username == "simuser") {
                    sale.username = "simuser";
                    sale.orderType = OrderBookType::bidsale;
                    cout << "yoyooyo" << endl;
                }
                if (ask.username == "simuser") {
                    sale.username = "simuser";
                    sale.orderType = OrderBookType::asksale;
                }

                if (bid.amount == ask.amount) {
                    sale.amount = ask.amount;
                    sales.push_back(sale);
                    bid.amount = 0; // make sure this bid won't be processed again
                    break;
                }
                if (bid.amount > ask.amount) {
                    sale.amount = ask.amount;
                    sales.push_back(sale);
                    bid.amount = bid.amount - ask.amount;
                    break;
                }
                if (bid.amount < ask.amount && bid.amount > 0) {
                    sale.amount = bid.amount;
                    sales.push_back(sale);
                    ask.amount = ask.amount - bid.amount;
                    bid.amount = 0;
                    continue; // move to the next bid
                }

            }
        }
    }
    return sales;
}
*/

vector<OrderBookEntry> OrderBook::matchAsksToBids(string product, string timestamp) {
    vector<OrderBookEntry> asks = getOrders(OrderBookType::ask, product, timestamp);
    vector<OrderBookEntry> bids = getOrders(OrderBookType::bid, product, timestamp);

    cout << "Matching for: " << timestamp << endl;
    cout << "Asks count: " << asks.size() << ", Bids count: " << bids.size() << endl;

    vector<OrderBookEntry> sales;
    if (asks.empty() || bids.empty()) {
        cout << "OrderBook::matchAsksToBids no bids or asks" << endl;
        return sales;
    }

    sort(asks.begin(), asks.end(), OrderBookEntry::compateByPriceAsc);
    sort(bids.begin(), bids.end(), OrderBookEntry::compateByPriceDesc);

    size_t askIdx = 0, bidIdx = 0;
    while (askIdx < asks.size() && bidIdx < bids.size()) {
        OrderBookEntry& ask = asks[askIdx];
        OrderBookEntry& bid = bids[bidIdx];

        if (bid.price >= ask.price) {
            double matchAmount = min(ask.amount, bid.amount);

            OrderBookEntry sale{
                timestamp,
                product,
                (bid.username == "simuser") ? OrderBookType::bidsale : OrderBookType::asksale,
                ask.price,
                matchAmount,
                (bid.username == "simuser") ? "simuser" : ask.username
            };
            sales.push_back(sale);

            ask.amount -= matchAmount;
            bid.amount -= matchAmount;

            if (ask.amount == 0) ++askIdx;
            if (bid.amount == 0) ++bidIdx;
        } else {
            // No more possible matches; bids are too low
            break;
        }
    }
    return sales;
}



