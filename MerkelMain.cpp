#include <iostream>
#include <vector>
#include "MerkelMain.h"
#include "OrderBookEntry.h"
#include "CSVReader.h"

using namespace std;

MerkelMain::MerkelMain() {

}

void MerkelMain::init() {
    int input;
    currentTime = orderBook.getEarlistTime();

    wallet.insertCurrency("BTC", 10);
    wallet.insertCurrency("ETH", 100);

    while (true) {
        printMenu();
        input = getUserOption();
        processUserOption(input);
    }

}

void MerkelMain::printMenu() {
    cout << endl;
    cout << "----- MerkleRex Menu -----" << endl;
    cout << "1. Print help" << endl;
    cout << "2. Print exchange stats" << endl;
    cout << "3. Make an ask" << endl;
    cout << "4. Make a bid" << endl;
    cout << "5. Print wallet" << endl;
    cout << "6. Continue" << endl;
    cout << "===========================" << endl;
    cout << "Current time: " << currentTime << endl;
}

void MerkelMain::printHelp() {
    cout << "Help: Your aim is to make money, analyze the market, and make bids and offers.\n" << endl;
}

void MerkelMain::printMarketStats() {
    for (string const& p : orderBook.getKnownProducts()) {
        cout << "Product: " << p << endl;
        vector<OrderBookEntry> entries = orderBook.getOrders(
            OrderBookType::ask, p, currentTime
        );
        cout << "Asks seen: " << entries.size() << endl;
        cout << "Maximum ask: " << OrderBook::getHighPrice(entries) << endl;
        cout << "Minimum ask: " << OrderBook::getLowPrice(entries) << endl;

        vector<double> prices1H = OrderBook::get1HPrices(entries, currentTime);
        double change = prices1H[1] - prices1H[0];
        double change_percent = change / prices1H[0] * 100;
        cout << "Ask price change in the last 1H: " << prices1H[1] << " - " <<  prices1H[0] << " = " << change << " (" << change_percent << "%)" << endl;

        cout << "Average ask price: " << OrderBook::getPriceMean(entries) << endl;
        cout << "Std of ask price: " << OrderBook::getPriceStd(entries) << endl;
        cout << endl;

    }
}

void MerkelMain::enterAsk() {
    cout << "Make an ask - enter product, price, and amount. E.g., ETH/BTC,0.021,1." << endl;
    string input;
    char ch;
    // Read characters one by one until newline is encountered
    while (cin.get(ch) && ch != '\n') {
        input += ch;
    }
    cout << "You entered: " << input << endl;

    vector<string> tokens = CSVReader::tokenize(input, ',');
    if (tokens.size() != 3) {
        cout << "Bad input: " << input << endl;
        cout << "There should be 3 tokens!" << endl;
    }
    else {
        try {
            OrderBookEntry obe = CSVReader::strToOBE(
                currentTime,
                tokens[0],
                OrderBookType::ask,
                tokens[1],
                tokens[2]
            );
            obe.username = "simuser";

            if (wallet.canFullfillOrder(obe)) {
                cout << "Wallet looks good." << endl;
                orderBook.insertOrder(obe);
            } else {
                cout << "Wallet has insufficient funds." << endl;
            }

        } catch (const exception& e) {
            cout << "MerkelMain::enterAsk() bad input!" << endl;
        }
    }
}

void MerkelMain::enterBid() {
    cout << "Make a bid - enter product, price, and amount. E.g., ETH/BTC, 0.023, 0.5." << endl;
    string input;
    char ch;
    // Read characters one by one until newline is encountered
    while (cin.get(ch) && ch != '\n') {
        input += ch;
    }
    cout << "You entered: " << input << endl;

    vector<string> tokens = CSVReader::tokenize(input, ',');
    if (tokens.size() != 3) {
        cout << "MerkelMain::enterBid Bad input: " << input << endl;
        cout << "There should be 3 tokens!" << endl;
    }
    else {
        try {
            OrderBookEntry obe = CSVReader::strToOBE(
                currentTime,
                tokens[0],
                OrderBookType::bid,
                tokens[1],
                tokens[2]
            );
            obe.username = "simuser";

            if (wallet.canFullfillOrder(obe)) {
                cout << "Wallet looks good." << endl;
                orderBook.insertOrder(obe);
            } else {
                cout << "Wallet has insufficient funds." << endl;
            }

        } catch (const exception& e) {
            cout << "MerkelMain::enterBid() bad input!" << endl;
        }
    }
}

void MerkelMain::printWallet() {
    cout << "Print wallet:\n" << endl;
    cout << wallet.toString() << endl;
}

void MerkelMain::gotoNextTimeFrame() {
    cout << "Continue: Going to next time frame.\n" << endl;
    vector<OrderBookEntry> sales = orderBook.matchAsksToBids("ETH/BTC", currentTime);
    cout << "Sales: " << sales.size() << endl;
    for (OrderBookEntry& sale : sales) {
        cout << "Sale price=" << sale.price << ", amount=" << sale.amount << endl;
        if (sale.username == "simuser") {
            // update the wallet
            cout << "Processing ..." << endl;
            wallet.processSale(sale);
        }
    }
    currentTime = orderBook.getNextTime(currentTime);
}

void MerkelMain::warnInvalid() {
    cout << "Invalid choice. Please select an integer between 1 and 6.\n" << endl;
}

int MerkelMain::getUserOption() {
    string line;
    int userOption = 0;
    cout << "Select an option from 1 to 6: ";
    getline(cin, line);
    try {
        userOption = stoi(line);
        cout << "Your choice: " << userOption << endl;
    } catch(const std::exception& e) {
        cout << "Bad input." << endl;
    }

    return userOption;
}

void MerkelMain::processUserOption(int userOption) {
    if (userOption == 1) {
        printHelp();
    } else if (userOption == 2) {
        printMarketStats();
    } else if (userOption == 3) {
        enterAsk();
    } else if (userOption == 4) {
        enterBid();
    } else if (userOption == 5) {
        printWallet();
    } else if (userOption == 6) {
        gotoNextTimeFrame();
    } else {
        warnInvalid();
    }
}