//
//  PairTrading.cpp
//  FRE7831_MarketData_JSON_MAC
//
//  Created by Yi Xu on 2020/10/15.
//  Copyright © 2020 NYU. All rights reserved.
//

#include "PairTrading.hpp"
#include <iostream>
#include "curl/curl.h"
#include <sqlite3.h>
#include "Database.hpp"
#include "math.h"

using namespace std;

void GetStockPairPrices(sqlite3* db, map<int, pair<string,string>> stockpairs, map<int, StockPairPrices> &stockpairprices, double K)
{
    char *error = NULL;
    char **results = NULL;
    char **results2 = NULL;
    int rows, columns;
    
    for(map<int,pair<string,string>>::const_iterator itr = stockpairs.begin();itr!=stockpairs.end();itr++)
    {
        StockPairPrices spr(itr->second);
        spr.SetK(K);
        
        string sql = "SELECT volatility FROM StockPairs WHERE id = " + to_string(itr->first);
        const char *sqlSelect = sql.c_str();
        sqlite3_get_table(db, sqlSelect, &results, &rows, &columns, &error);
        spr.SetVolatility(stod(results[1]));
        
        map<string, PairPrice> dailypairprices;
        string sql2 = "SELECT * FROM PairPrices WHERE ((symbol1 = \"" + itr->second.first + "\") and (symbol2 = \"" + itr->second.second + "\"))";
        const char *sqlSelect2 = sql2.c_str();
        sqlite3_get_table(db, sqlSelect2, &results2, &rows, &columns, &error);
        string date;
        double open1 = 0, close1 = 0, open2 = 0, close2 = 0, profit_loss = 0;
        for (int rowCtr = 0; rowCtr <= rows; ++rowCtr)
        {
            if (rowCtr == 0) continue;
            for (int colCtr = 0; colCtr < columns; ++colCtr)
            {
                int cellPosition = (rowCtr * columns) + colCtr;
                //cout<<results2[cellPosition]<<endl;
                if(colCtr == 0) {continue;}
                else if(colCtr == 1) {continue;}
                else if(colCtr == 2) {date = results2[cellPosition];}
                else if(colCtr == 3) {open1 = stod(results2[cellPosition], NULL);}
                else if(colCtr == 4) {close1 = stod(results2[cellPosition], NULL);}
                else if(colCtr == 5) {open2 = stod(results2[cellPosition], NULL);}
                else if(colCtr == 6) {close2 = stod(results2[cellPosition], NULL);}
                else if(colCtr == 7) {profit_loss = stod(results2[cellPosition], NULL);}
            }
            PairPrice pairprice = {open1, close1, open2, close2};
            spr.SetDailyPairPrice(date, pairprice);
            spr.UpdateProfitLoss(date, profit_loss);
        }
        stockpairprices[itr->first] = spr;
    }
    
    sqlite3_free_table(results);
    sqlite3_free_table(results2);
    //sqlite3_close(db);
}


void strategy(map<int, StockPairPrices> &stockpairprices)
{
    double vol = 0, open1 = 0, open2 = 0, close1 = 0, close2 = 0, close1pre = 0, close2pre = 0, profit = 0;
    int N2 = 0, N1 = 10000;
    string date;
    for(map<int,StockPairPrices>::iterator itr = stockpairprices.begin();itr!=stockpairprices.end();itr++)
    {
        map<string,PairPrice> dp = itr->second.GetDailyPrices();
        vol = itr->second.GetVolatility();
        for(map<string,PairPrice>::iterator itr2 = dp.begin(); itr2!= dp.end();itr2++)
        {
            if(itr2 == dp.begin())
            {
                close1pre = itr2->second.dClose1;
                close2pre = itr2->second.dClose2;
            }
            else
            {
                date = itr2->first;
                open1 = itr2->second.dOpen1;
                open2 = itr2->second.dOpen2;
                close1 = itr2->second.dClose1;
                close2 = itr2->second.dClose2;
                
                N2 = N1 * open1 / open2;
                profit = N1 * (open1 - close1) - N2 * (open2 - close2);
                
                // short the first, long the second
                if(abs(close1pre / close2pre - open1 / open2) > itr->second.GetK() * sqrt(vol))
                {
                    itr->second.UpdateProfitLoss(date, profit);
                }
                else
                {
                    itr->second.UpdateProfitLoss(date, -profit);
                }
                close1pre = close1;
                close2pre = close2;
            }
        }
    }
}


int PopulateDailyProfit(sqlite3 *&db, map<int, StockPairPrices> &stockpairprices)
{
    char *error = NULL;
    for(map<int,StockPairPrices>::iterator itr = stockpairprices.begin();itr!=stockpairprices.end();itr++)
    {
        map<string, PairPrice> dp = itr->second.GetDailyPrices();
        for(map<string,PairPrice>::iterator itr2 = dp.begin(); itr2!= dp.end();itr2++)
        {
            string sql = string("Update PairPrices SET profit_loss = ") + to_string(itr2->second.dProfit_Loss) + " WHERE ((symbol1 = \"" + itr->second.GetStockPair().first + "\") and (symbol2 = \"" + itr->second.GetStockPair().second + "\") and (date = \"" + itr2->first + "\"));";
            const char * sqlUpdate = sql.c_str();
            int rc = sqlite3_exec(db, sqlUpdate, NULL, NULL, &error);
            if (rc)
            {
                cerr << "Error executing SQLite3 statement: " << sqlite3_errmsg(db) << endl << endl;
                sqlite3_free(error);
            }
        }
    }
    return 0;
}


int PopulateProfit(sqlite3 *db)
{
    char *error = NULL;
    string sql = string("Update StockPairs SET profit_loss = ")
    + "(SELECT SUM(profit_loss) FROM PairPrices "
    + "WHERE (StockPairs.symbol1 = PairPrices.symbol1) AND (StockPairs.symbol2 = PairPrices.symbol2) "
    + "GROUP BY symbol1, symbol2);";
    const char * sqlUpdate = sql.c_str();
    int rc = sqlite3_exec(db, sqlUpdate, NULL, NULL, &error);
    if (rc)
    {
        cerr << "Error executing SQLite3 statement: " << sqlite3_errmsg(db) << endl << endl;
        sqlite3_free(error);
    }
    sqlite3_close(db);
    return 0;
}

