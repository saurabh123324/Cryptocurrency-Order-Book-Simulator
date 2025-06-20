#include "OrderBookEntry.h"
#include <string>
using namespace std;

OrderBookEntry::OrderBookEntry(
    std::string _timestamp,
    std::string _product,
    OrderBookType _orderType,
    double _price,
    double _amount,
    string _username
)
: price(_price),
  amount(_amount),
  timestamp(_timestamp),
  product(_product),
  orderType(_orderType),
  username(_username)
{

}

OrderBookType OrderBookEntry::strToOBT(string s) {
  if (s == "ask") {return OrderBookType::ask;}
  else if (s == "bid") {return OrderBookType::bid;}
  else {return OrderBookType::unknown;}
}