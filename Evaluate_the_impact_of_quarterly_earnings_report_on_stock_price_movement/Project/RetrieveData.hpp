//
//  RetrieveData.hpp
//  RetrieveData
//
//  Created by Yi Xu on 2019/11/30.
//  Copyright © 2019 Yi Xu. All rights reserved.
//
/*
 Get 30 days before announcement day and 30 days after announcement day for each stock
 Retrieve stock data using multi-thread
 */
#ifndef RetrieveData_hpp
#define RetrieveData_hpp

#include <stdio.h>
#include <string>
#include <iostream>
#include <sstream>
#include <vector>
#include <locale>
#include <iomanip>
#include "curl/curl.h"
#include <map>
#include "Stock.hpp"
#include <thread>

using namespace std;

struct MemoryStruct
{
    char * memory;
    size_t size;
};

string getTimeinSeconds(string Time);

void *myrealloc(void *ptr, size_t size);

size_t write_data2(void *ptr, size_t size, size_t nmemb, void *data);

void retrievedata(map<string,vector<string>> &mapeps,map<string,Stock> &stockpool,vector<string> &trading_days,int num);

vector<string> get_true_startandend_date(vector<string> &trading_days,string announcement_day,int num);
map<string,Stock> fast_retrievedata(map<string,vector<string>> &mapeps,map<string,Stock> &stockpool,vector<string> &trading_days);

#endif /* RetrieveData_hpp */
