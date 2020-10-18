//
//  main.cpp
//  FRE7831_MarketData_JSON_MAC
//
//  Created by Yi Xu on 2020/5/17.
//  Copyright © 2020 NYU. All rights reserved.
//

#include <string>
#include <iostream>
#include <vector>
#include <stdio.h>
#include "json/json.h"
#include "curl/curl.h"
#include <sqlite3.h>
#include "Stock.hpp"
#include "Database.hpp"
#include "MarketData.hpp"
#include "PairTrading.hpp"
#include <map>

using namespace std;

string GetUrl(string type, string symbol)
{
    string url_request;
    string api_token = "5ba84ea974ab42.45160048";
    if(type=="daily")
    {
        string url_common = "https://eodhistoricaldata.com/api/eod/";
        string start_date = "2010-01-01";
        string end_date = "2019-12-31";
        url_request = url_common + symbol + ".US?" + "from=" + start_date + "&to=" + end_date + "&api_token=" + api_token + "&period=d&fmt=json";
    }
    else if(type=="intraday")
    {
        // intraday trade
        string url_common = "https://eodhistoricaldata.com/api/intraday/";
        url_request = url_common + symbol + ".US?" + "&api_token=" + api_token + "&interval=5m&fmt=json";
    }
    return url_request;
}

    
int main(void)
{
    sqlite3 *db = NULL;
    if(OpenDatabase(db)!=0) return -1;
    map<int, pair<string,string>> stockpairs;
    map<string,Stock> stockMap;
    GetStockPairs(stockpairs);

    vector<string> symbols1;
    vector<string> symbols2;
    for(map<int,pair<string,string>>::const_iterator itr = stockpairs.begin();itr!=stockpairs.end();itr++)
    {
        symbols1.push_back(itr->second.first.c_str());
        symbols2.push_back(itr->second.second.c_str());
    }
    
    // This part only need to be executed once, then the data will be stored in database
    CreateTables(db);
    PopulateStockPairs(stockpairs, db);
     
    for(int i=0;i<symbols1.size();i++)
    {
        string readBuffer;

        string type1 = "daily";
        string symbol = symbols1[i];
        string url_request = GetUrl(type1, symbol);

        Stock myStock(symbol);
        stockMap[symbol] = myStock;

        PullMarketData(url_request, readBuffer);
        PopulatePairPrices(db, readBuffer, stockMap[symbol],"PairOnePrices");
    }

    for(int i=0;i<symbols2.size();i++)
    {
        string readBuffer;

        string type1 = "daily";
        string symbol = symbols2[i];
        string url_request = GetUrl(type1, symbol);

        Stock myStock(symbol);
        stockMap[symbol] = myStock;

        PullMarketData(url_request, readBuffer);
        PopulatePairPrices(db, readBuffer, stockMap[symbol],"PairTwoPrices");
    }
    
    PopulatePairPrices(db);
    CalculateVar(db);
    //
    
    // Pair Trading Strategy
    double K;
    cout<<"Enter K:"<<endl; cin>>K;
    map<int, StockPairPrices> stockpairprices;
    GetStockPairPrices(db, stockpairs, stockpairprices, K);
    strategy(stockpairprices);
    PopulateDailyProfit(db, stockpairprices); // only need to do once
    PopulateProfit(db);
    
    return 0;
}
    










//    bool bCompleted = false;
//
//    // menu
//    cout<<endl;
//    cout<<"~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"<<endl;
//    cout<<"A - Create DailyTrades and IntradayTrades Tables"<<endl;
//    cout<<"B - Retrieve DailyTrade and IntradayTrade Data"<<endl;
//    cout<<"C - Populate DailyTrades and IntradayTrades Tables"<<endl;
//    cout<<"D - Retrieve Data from DailyTrades and IntradayTrades Tables"<<endl;
//    cout<<"E - Drop All the tables"<<endl;
//    cout<<"X - Exit"<<endl;
//    cout<<"~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"<<endl<<endl;
//
//    while(!bCompleted)
//    {
//        string num;
//        cout<<"Enter the number : "; cin>>num;
//        if(num == "A")
//        {
//            CreateTables(db);
//            cout<<"DailyTrades and IntradayTrades Tables created."<<endl;
//        }
//        else if(num == "B")
//        {
//            vector<string> symbols;
//            symbols.push_back("MSFT");
//            symbols.push_back("TWTR");
//            symbols.push_back("IBM");
//            map<string,Stock> stockMap;
//
//            for(int i=0;i<symbols.size();i++)
//            {
//                string readBuffer;
//                string readBuffer2;
//
//                string type1 = "daily";
//                string symbol = symbols[i];
//                string url_request = GetUrl(type1, symbol);
//
//                string type2 = "intraday";
//                string url_request2 = GetUrl(type2, symbol);
//
//                Stock myStock(symbol);
//                stockMap[symbol] = myStock;
//
//                PullMarketData(url_request, readBuffer);
//                PullMarketData(url_request2, readBuffer2);
//                cout<<"DailyTrade and IntradayTrade Data retrieved."<<endl;
//            }
//        }
//        else if(num == "C")
//        {
//            vector<string> symbols;
//            symbols.push_back("MSFT");
//            symbols.push_back("TWTR");
//            symbols.push_back("IBM");
//            map<string,Stock> stockMap;
//
//            for(int i=0;i<symbols.size();i++)
//            {
//                string readBuffer;
//                string readBuffer2;
//
//                string type1 = "daily";
//                string symbol = symbols[i];
//                string url_request = GetUrl(type1, symbol);
//
//                string type2 = "intraday";
//                string url_request2 = GetUrl(type2, symbol);
//
//                Stock myStock(symbol);
//                stockMap[symbol] = myStock;
//
//                PullMarketData(url_request, readBuffer);
//                PullMarketData(url_request2, readBuffer2);
//
//                PopulateDailyTrades(readBuffer, stockMap[symbol]);
//                PopulateIntradayTrades(readBuffer2, stockMap[symbol]);
//
//                InsertTable(db, stockMap[symbol]);
//                cout<<"DailyTrades and IntradayTrades Tables populated."<<endl;
//            }
//        }
//        else if(num == "D")
//        {
//            DisplayTables(db);
//            cout<<"Data from DailyTrades and IntradayTrades Tables retrieved."<<endl;
//        }
//        else if(num == "E")
//        {
//            //drop tables
//            continue;
//        }
//        else if(num == "X")
//        {
//            break;
//        }
//        else
//        {
//            cout<<"Re-enter the number."<<endl;
//        }
//    }
//    return 0;
//}


                

