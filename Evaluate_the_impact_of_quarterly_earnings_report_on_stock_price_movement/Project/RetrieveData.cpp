//
//  RetrieveData.cpp
//  RetrieveData
//
//  Created by Yi Xu on 2019/11/30.
//  Copyright © 2019 Yi Xu. All rights reserved.
//

#include "RetrieveData.hpp"
#include <string>
#include <iostream>
#include <sstream>
#include <vector>
#include <locale>
#include <iomanip>
#include "curl/curl.h"
#include "Stock.hpp"

using namespace std;

string getTimeinSeconds(string Time)
{
    std::tm t = {0};
    std::istringstream ssTime(Time);
    char time[100];
    memset(time, 0, 100);
    if (ssTime >> std::get_time(&t, "%Y-%m-%dT%H:%M:%S"))
    {
//        cout << std::put_time(&t, "%c %Z") << "\n"
//                  << std::mktime(&t) << "\n";
        sprintf (time, "%ld", mktime(&t));
        return string(time);
    }
    else
    {
        cout << "Parse failed\n";
        return "";
    }
}



void *myrealloc(void *ptr, size_t size)
{
    /* There might be a realloc() out there that doesn't like reallocing
    NULL pointers, so we take care of it here */
    if (ptr)
        return realloc(ptr, size);
    else
        return malloc(size);
}



size_t write_data2(void *ptr, size_t size, size_t nmemb, void *data)
{
    size_t realsize = size * nmemb;
    struct MemoryStruct *mem = (struct MemoryStruct *)data;

    mem->memory = (char *)myrealloc(mem->memory, mem->size + realsize + 1);
    if (mem->memory) {
        memcpy(&(mem->memory[mem->size]), ptr, realsize);
        mem->size += realsize;
        mem->memory[mem->size] = 0;
    }
    return realsize;
}



vector<string> get_true_startandend_date(vector<string> &trading_days,string announcement_day)
{
    vector<string> true_date;
    vector<string>::iterator itr=find(trading_days.begin(),trading_days.end(),announcement_day);
    true_date.push_back(*(itr-30));
    true_date.push_back(*(itr+31));
    return true_date;
}


void retrievedata(map<string,vector<string>> &mapeps,map<string,Stock> &stockpool,vector<string> &trading_days)
{
    for(map<string,vector<string>>::iterator itr=mapeps.begin();itr!=mapeps.end();itr++)
    {
        Stock s(itr->first,"SPY",get_true_startandend_date(trading_days,itr->second[0])[0],get_true_startandend_date(trading_days,itr->second[0])[1]);
        stockpool[itr->first] = s;
    }
}
