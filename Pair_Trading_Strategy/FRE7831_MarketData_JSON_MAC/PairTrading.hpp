//
//  PairTrading.hpp
//  FRE7831_MarketData_JSON_MAC
//
//  Created by Yi Xu on 2020/10/15.
//  Copyright © 2020 NYU. All rights reserved.
//

#ifndef PairTrading_hpp
#define PairTrading_hpp

#include <stdio.h>
#include <iostream>
#include "Stock.hpp"
#include <sqlite3.h>

using namespace std;

void GetStockPairPrices(sqlite3* db, map<int, pair<string,string>> stockpairs, map<int, StockPairPrices> &stockpairprices, double K);
void strategy(map<int, StockPairPrices> &stockpairprices);
int PopulateDailyProfit(sqlite3 *&db, map<int, StockPairPrices> &stockpairprices);
int PopulateProfit(sqlite3 *db);

#endif /* PairTrading_hpp */
