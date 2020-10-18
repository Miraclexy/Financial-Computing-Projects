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
    

