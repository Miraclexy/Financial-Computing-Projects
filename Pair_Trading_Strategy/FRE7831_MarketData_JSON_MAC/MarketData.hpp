//
//  MarketData.hpp
//  FRE7831_MarketData_JSON_MAC
//
//  Created by Yi Xu on 2020/5/17.
//  Copyright © 2020 NYU. All rights reserved.
//

#ifndef MarketData_hpp
#define MarketData_hpp

#include <stdio.h>
#pragma once
#include <string>
#include <map>
#include <sqlite3.h>
#include "Database.hpp"
using namespace std;

class Stock;
int PullMarketData(const string & url_request, string & read_buffer);
void GetStockPairs(map<int, pair<string,string>> &stockpairs);
int PopulateStockPairs(const map<int, pair<string,string>> &stockpairs, sqlite3 *db);
int PopulatePairPrices(sqlite3 *db, const string & read_buffer, Stock & stock, string tablename);
int PopulatePairPrices(sqlite3 *&db);



#endif /* MarketData_hpp */
