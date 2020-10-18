//
//  Stock.hpp
//  FRE7831_MarketData_JSON_MAC
//
//  Created by Yi Xu on 2020/4/21.
//  Copyright © 2020 NYU. All rights reserved.
//

#ifndef Stock_hpp
#define Stock_hpp

#include <stdio.h>

#pragma once
#include<iostream>
#include<string>
#include<map>
#include<vector>

using namespace std;


class TradeData
{
private:
    string sDate;
    double dOpen;
    double dHigh;
    double dLow;
    double dClose;
    double dAdjClose;
    long lVolume;

public:
    TradeData() :sDate(""), dOpen(0), dHigh(0), dLow(0), dClose(0), dAdjClose(0), lVolume(0) {}

    TradeData(string sDate_, double dOpen_, double dHigh_, double dLow_, double dClose_, double dAdjClose_, long lVolume_) :
        sDate(sDate_), dOpen(dOpen_), dHigh(dHigh_), dLow(dLow_), dClose(dClose_), dAdjClose(dAdjClose_), lVolume(lVolume_) {}

    TradeData(const TradeData& TradeData_) :
        sDate(TradeData_.sDate), dOpen(TradeData_.dOpen), dHigh(TradeData_.dHigh), dLow(TradeData_.dLow),
        dClose(TradeData_.dClose), dAdjClose(TradeData_.dAdjClose), lVolume(TradeData_.lVolume) {}

    TradeData operator=(const TradeData& TradeData_)
    {
        sDate = TradeData_.sDate;
        dOpen = TradeData_.dOpen;
        dHigh = TradeData_.dHigh;
        dLow = TradeData_.dLow;
        dClose = TradeData_.dClose;
        dAdjClose = TradeData_.dAdjClose;
        lVolume = TradeData_.lVolume;

        return *this;
    }

    friend ostream& operator << (ostream& out, const TradeData& TradeData_)
    {
        out << "Date: " << TradeData_.sDate << " Open: " << TradeData_.dOpen << " High: " << TradeData_.dHigh << " Low: " << TradeData_.dLow
            << " Close: " << TradeData_.dClose << " Adjusted_Close: " << TradeData_.dAdjClose << " Volume: " << TradeData_.lVolume << endl;
        return out;
    }


    double GetOpen() const { return dOpen; }
    double GetHigh() const { return dHigh; }
    double GetLow() const { return dLow; }
    double GetClose() const { return dClose; }
    double GetAdjustedClose() const { return dAdjClose; }
    long GetVolume() const { return lVolume; }
    string GetDate() const { return sDate; }
};


class Stock
{
private:
    string sSymbol;
    vector<TradeData> trades;

public:
    Stock(string sSymbol_ = "") :sSymbol(sSymbol_) {}

    Stock(string sSymbol_, const vector<TradeData> trades_) :sSymbol(sSymbol_), trades(trades_) {}

    Stock(const Stock& Stock_) :sSymbol(Stock_.sSymbol), trades(Stock_.trades) {}

    Stock operator=(const Stock& Stock_)
    {
        sSymbol = Stock_.sSymbol;
        trades = Stock_.trades;
        return *this;
    }

    void addTrade(const TradeData& trade) { trades.push_back(trade); }

    string getSymbol() const { return sSymbol; }

    const vector<TradeData>& getTrades() const { return trades; }

    friend ostream & operator<<(ostream & ostr, const Stock & stock)
    {
        ostr << "Symbol: " << stock.sSymbol << endl;
        for (vector<TradeData>::const_iterator itr = stock.trades.begin(); itr != stock.trades.end(); itr++)
            ostr << *itr;
        return ostr;
    }
};


struct PairPrice
{
    double dOpen1;
    double dClose1;
    double dOpen2;
    double dClose2;
    double dProfit_Loss;

    PairPrice() : dOpen1(0), dClose1(0), dOpen2(0), dClose2(0), dProfit_Loss(0) {}

    PairPrice(double dOpen1_, double dClose1_, double dOpen2_, double dClose2_) :
        dOpen1(dOpen1_), dClose1(dClose1_), dOpen2(dOpen2_), dClose2(dClose2_), dProfit_Loss(0) {}
};


class StockPairPrices
{
private:
    pair<string, string> stockPair;
    double volatility;
    double k;
    map<string, PairPrice> dailyPairPrices;

public:
    StockPairPrices() { volatility = 0; k = 0; }

    StockPairPrices(pair<string, string> stockPair_) { stockPair = stockPair_; volatility = 0; k = 0; }

    void SetDailyPairPrice(string sDate_, PairPrice PairPrice_)
    {
        dailyPairPrices.insert(pair<string, PairPrice>(sDate_, PairPrice_));
    }

    void SetVolatility(double volatility_) { volatility = volatility_; }

    void SetK(double k_) { k = k_; }

    void UpdateProfitLoss(string sDate_, double dProfit_Loss_)
    {
        dailyPairPrices[sDate_].dProfit_Loss = dProfit_Loss_;
    }

    pair<string, string> GetStockPair() const { return stockPair; }

    map<string, PairPrice> GetDailyPrices() const { return dailyPairPrices; }

    double GetVolatility() const { return volatility; }

    double GetK() const { return k; }
};



#endif /* Stock_hpp */
