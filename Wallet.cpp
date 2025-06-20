#include "Wallet.h"
#include "CSVReader.h"
#include <iostream>
using namespace std;

Wallet::Wallet() {

}

void Wallet::insertCurrency(string type, double amount) {
    if (amount < 0) {
        throw exception();
    }

    double balance;
    if (currencies.count(type) == 0) {
        balance = 0;
    } else {
        balance = currencies[type];
    }
    balance += amount;
    currencies[type] = balance;
}

bool Wallet::removeCurrency(string type, double amount) {
    if (amount < 0) {
        throw exception();
    }

    double balance;
    if (currencies.count(type) == 0) {
        return false;
    } else {
        // do we have an enough amount of this currency?
        if (containsCurrency(type, amount)) { // yes
            cout << "Removing " << type << ": " << to_string(amount) << endl;
            currencies[type] -= amount;
            return true;
        } else {                              // not enough
            return false;
        }
    }
}

bool Wallet::containsCurrency(string type, double amount) {
    if (currencies.count(type) == 0 || currencies[type] < amount) {
        return false;
    } else {
        return true; // currencies[type];
    }
}

bool Wallet::canFullfillOrder(const OrderBookEntry order) {
    vector<string> currencyPair = CSVReader::tokenize(order.product, '/');

    // ask
    if (order.orderType == OrderBookType::ask) {
        string currency = currencyPair[0];
        double amount = order.amount;
        cout << "Wallet::canFullfillOrder " << currency << " / " << amount << endl;

        return containsCurrency(currency, amount);
    }

    // bid
    if (order.orderType == OrderBookType::bid) {
        string currency = currencyPair[1];
        double amount = order.amount * order.price;
        return containsCurrency(currency, amount);
    }

    return false;
}

string Wallet::toString() {
    string out;
    for (pair<string, double> pair : currencies) {
        string currency = pair.first;
        double amount = pair.second;
        out += currency + " : " + to_string(amount) + "\n";
    }
    return out;
}

void Wallet::processSale(OrderBookEntry& sale) {
    vector<string> currencyPair = CSVReader::tokenize(sale.product, '/');

    // ask
    if (sale.orderType == OrderBookType::asksale) {
        cout << "\n\n\nASSSSSSK!\n\n\n" << endl;
        string currencyOutgoing = currencyPair[0];
        string currencyIncoming = currencyPair[1];
        double amountOutgoing = sale.amount;
        double amountIncoming = sale.amount * sale.price;
        currencies[currencyOutgoing] -= amountOutgoing;
        currencies[currencyIncoming] += amountIncoming;
        cout << "\n\n\nASSSSSSK!\n\n\n" << endl;
    } else {cout << "\n\n\nQQ\n\n\n" << endl;}

    // bid
    if (sale.orderType == OrderBookType::bidsale) {
        cout << "\n\n\nBIIIIIIID\n\n\n!" << endl;
        string currencyOutgoing = currencyPair[1];
        string currencyIncoming = currencyPair[0];
        double amountOutgoing = sale.amount * sale.price;
        double amountIncoming = sale.amount;
        currencies[currencyOutgoing] -= amountOutgoing;
        currencies[currencyIncoming] += amountIncoming;
        cout << "\n\n\nBIIIIIIID!\n\n\n" << endl;
    } else {cout << "\n\n\nQAAAAAQ\n\n\n" << endl;}
}