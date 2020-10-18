//
//  MarketData.cpp
//  FRE7831_MarketData_JSON_MAC
//
//  Created by Yi Xu on 2020/5/17.
//  Copyright © 2020 NYU. All rights reserved.
//

#include "MarketData.hpp"
#include <string>
#include <iostream>
#include <vector>
#include <stdio.h>
#include "json/json.h"
#include "curl/curl.h"
#include <sqlite3.h>
#include <map>
#include "Stock.hpp"
#include "Database.hpp"
#include <sstream>
#include <fstream>

using namespace std;


//writing call back function for storing fetched values in memory
static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
    ((string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}


int PullMarketData(const string & url_request, string & read_buffer)
{
    //global initiliation of curl before calling a function
    curl_global_init(CURL_GLOBAL_ALL);
    
    CURL * handle = NULL;
    handle = curl_easy_init();
    CURLcode result;
    
    curl_easy_setopt(handle, CURLOPT_URL, url_request.c_str());
    //curl_easy_setopt(myHandle, CURLOPT_URL, "https://eodhistoricaldata.com/api/eod/AAPL.US?from=2020-03-01&to=2020-03-31&api_token=5ba84ea974ab42.45160048&period=d&fmt=json");

    //adding a user agent
    curl_easy_setopt(handle, CURLOPT_USERAGENT, "Mozilla/5.0 (Windows NT 10.0; Win64; x64; rv:74.0) Gecko/20100101 Firefox/74.0");
    curl_easy_setopt(handle, CURLOPT_SSL_VERIFYPEER, 0);
    curl_easy_setopt(handle, CURLOPT_SSL_VERIFYHOST, 0);

    // send all data to this function
    curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, WriteCallback);

    // we pass our 'chunk' struct to the callback function
    curl_easy_setopt(handle, CURLOPT_WRITEDATA, &read_buffer);
    result = curl_easy_perform(handle);

    // check for errors
    if (result != CURLE_OK) {
        fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(result));
    }
    curl_easy_cleanup(handle);
    
    return 0;
}


void GetStockPairs(map<int, pair<string,string>> &stockpairs)
{
    ifstream file;
    file.open("PairTrading.txt");
    string data;
    string line;
    
    int i = 1;
    while(getline(file,line))
    {
        istringstream is(line);
        while(is>>data)
        {
            stringstream sstr(data);
            string token;
            getline(sstr, token, ',');
            string stock1 = token.c_str();
            getline(sstr, token, ',');
            string stock2 = token.c_str();
            stockpairs[i] = pair<string, string>(stock1,stock2);
            i++;
        }
    }
}


int PopulateStockPairs(const map<int, pair<string,string>> &stockpairs, sqlite3 *db)
{
    for(map<int,pair<string,string>>::const_iterator itr = stockpairs.begin();itr!=stockpairs.end();itr++)
    {
        char stockpairs_insert_table[512];
        sprintf(stockpairs_insert_table, "INSERT INTO StockPairs (id, symbol1, symbol2, volatility, profit_loss) VALUES(%d, \"%s\", \"%s\", %f, %f)", itr->first, itr->second.first.c_str(), itr->second.second.c_str(), 0.0,0.0);
        if (InsertTable(db, stockpairs_insert_table) == -1)
            return -1;
    }
    return 0;
}


int PopulatePairPrices(sqlite3 *db, const string & read_buffer, Stock & stock, string tablename)
{
    //json parsing
    Json::CharReaderBuilder builder;
    Json::CharReader* reader = builder.newCharReader();
    Json::Value root;   // will contains the root value after parsing.
    string errors;

    bool parsingSuccessful = reader->parse(read_buffer.c_str(), read_buffer.c_str() + read_buffer.size(), &root, &errors);
    if (not parsingSuccessful)
    {
        // Report failures and their locations in the document.
        cout << "Failed to parse JSON" << endl << read_buffer << errors << endl;
        return -1;
    }
    else
    {
        cout << "\nSuccess parsing json\n" << root << endl;
        string date;
        float open = 0, high = 0, low = 0, close = 0, adjusted_close = 0;
        int volume = 0;

        for (Json::Value::const_iterator itr = root.begin(); itr != root.end(); itr++) {
            {
                //cout << *itr << endl;
                for (Json::Value::const_iterator inner = (*itr).begin(); inner != (*itr).end(); inner++)
                {
                    //cout << inner.key() << ": " << *inner << endl;

                    if (inner.key().asString() == "adjusted_close")
                    {
                        if (inner->isNull() || inner->isNumeric())
                            adjusted_close = inner->asFloat();
                        else
                            adjusted_close = (float)atof(inner->asCString());
                    }
                    else if (inner.key().asString() == "close")
                    {
                        if (inner->isNull() || inner->isNumeric())
                            close = inner->asFloat();
                        else
                            close = (float)atof(inner->asCString());
                    }
                    else if (inner.key() == "date")
                        date = inner->asString();
                    else if (inner.key().asString() == "high")
                    {
                        if (inner->isNull() || inner->isNumeric())
                            high = inner->asFloat();
                        else
                            high = (float)atof(inner->asCString());
                    }
                    else if (inner.key().asString() == "low")
                    {
                        if (inner->isNull() || inner->isNumeric())
                            low = inner->asFloat();
                        else
                            low = (float)atof(inner->asCString());
                    }
                    else if (inner.key() == "open")
                    {
                        if (inner->isNull() || inner->isNumeric())
                            open = inner->asFloat();
                        else
                            open = (float)atof(inner->asCString());
                    }
                    else if (inner.key().asString() == "volume")
                    {
                        if (inner->isNull() || inner->isNumeric())
                            volume = inner->asInt();
                        else
                            volume = atoi(inner->asCString());
                    }
                    else
                    {
                        cout << "Invalid json field" << endl;
                        return -1;
                    }
                }
            TradeData aTrade(date, open, high, low, close, adjusted_close, volume);
            stock.addTrade(aTrade);
            }
        }
    }
    
    char *error = NULL;
    for(int i = 0; i < stock.getTrades().size();i++)
    {
        if(tablename=="PairOnePrices")
        {
            char sqlInsert[512];
            sprintf(sqlInsert, "INSERT INTO PairOnePrices(symbol, date, open, high, low, close, adjusted_close, volume) VALUES(\"%s\", \"%s\", %f, %f, %f, %f, %f, %ld)", stock.getSymbol().c_str(), stock.getTrades()[i].GetDate().c_str(), stock.getTrades()[i].GetOpen(), stock.getTrades()[i].GetHigh(), stock.getTrades()[i].GetLow(), stock.getTrades()[i].GetClose(), stock.getTrades()[i].GetAdjustedClose(), stock.getTrades()[i].GetVolume());
                int rc = sqlite3_exec(db, sqlInsert, NULL, NULL, &error);
                if (rc)
                {
                    cerr << "Error executing SQLite3 statement: " << sqlite3_errmsg(db) << endl << endl;
                    sqlite3_free(error);
                }
        }
        else if(tablename=="PairTwoPrices")
        {
            char sqlInsert[512];
            sprintf(sqlInsert, "INSERT INTO PairTwoPrices(symbol, date, open, high, low, close, adjusted_close, volume) VALUES(\"%s\", \"%s\", %f, %f, %f, %f, %f, %ld)", stock.getSymbol().c_str(), stock.getTrades()[i].GetDate().c_str(), stock.getTrades()[i].GetOpen(), stock.getTrades()[i].GetHigh(), stock.getTrades()[i].GetLow(), stock.getTrades()[i].GetClose(), stock.getTrades()[i].GetAdjustedClose(), stock.getTrades()[i].GetVolume());
                int rc = sqlite3_exec(db, sqlInsert, NULL, NULL, &error);
                if (rc)
                {
                    cerr << "Error executing SQLite3 statement: " << sqlite3_errmsg(db) << endl << endl;
                    sqlite3_free(error);
                }
        }
    }
    
    return 0;
}


int PopulatePairPrices(sqlite3 *&db)
{
    char *error = NULL;
    int rc = 0;
    // Drop the table if exists
    string sql_dropatable = "DROP TABLE IF EXISTS PairPrices";
    if (sqlite3_exec(db, sql_dropatable.c_str(), 0, 0, 0) != SQLITE_OK) {
        cout << "SQLite can't drop PairPrices table" << std::endl;
        sqlite3_close(db);
        return -1;
    }

    // Populate PairPrices table
    const char *sqlCreateTable = "CREATE TABLE PairPrices AS SELECT StockPairs.symbol1 as symbol1, StockPairs.symbol2 as symbol2, PairOnePrices.date as date, PairOnePrices.open as open1, PairOnePrices.close as close1, PairTwoPrices.open as open2, PairTwoPrices.close as close2, 0 as profit_loss From StockPairs, PairOnePrices, PairTwoPrices Where(((StockPairs.symbol1 = PairOnePrices.symbol) and (StockPairs.symbol2 = PairTwoPrices.symbol)) and (PairOnePrices.date = PairTwoPrices.date)) ORDER BY symbol1, symbol2";
    
    rc = sqlite3_exec(db, sqlCreateTable, NULL, NULL, &error);
    if (rc)
    {
        cerr << "Error executing SQLite3 statement: " << sqlite3_errmsg(db) << endl << endl;
        sqlite3_free(error);
    }
    else
    {
        cout << "Populate PairPrices table." << endl << endl;
    }
    
    return 0;
}











            
