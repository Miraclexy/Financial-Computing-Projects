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
    true_date.push_back(*(itr+31));   // when calculate returns, need one more day, so here is 31
    return true_date;
}


/* original retrievedata with no multi-thread
void retrievedata(map<string,vector<string>> &mapeps,map<string,Stock> &stockpool,vector<string> &trading_days)
{
    for(map<string,vector<string>>::iterator itr=mapeps.begin();itr!=mapeps.end();itr++)
    {
        Stock s(itr->first,"SPY",get_true_startandend_date(trading_days,itr->second[0])[0],get_true_startandend_date(trading_days,itr->second[0])[1]);
        stockpool[itr->first] = s;
    }
}
*/


mutex stockpool_m;  // mutual exclusive lock to protect write in stockpool
void retrievedata(map<string,vector<string>> &mapeps,map<string,Stock> &stockpool,vector<string> &trading_days,int num)  // num is to divide the mission of retrieve data into 8 missions
{
    int l,r;  // define the left and right point for each multi thread
    if(num == 8)
    {
        r = int(mapeps.size());
        l = 7*int(int(mapeps.size())/8);
    }
    else
    {
        l = (num-1)*int(int(mapeps.size())/8);
        r = num*int(int(mapeps.size())/8);
    }
    
    map<string,vector<string>>::iterator itr=mapeps.begin();
    int temp = l;
    for(int i=0;i<(num-1)*int(int(mapeps.size())/8);i++) itr++;  // move itr to the right start
    for(itr=itr;itr!=mapeps.end();itr++)
    {
       if(temp == r) break;
        Stock s(itr->first,"SPY",get_true_startandend_date(trading_days,itr->second[0])[0],get_true_startandend_date(trading_days,itr->second[0])[1]);
        lock_guard<mutex> guard(stockpool_m); stockpool[itr->first] = s; // protect this part
        temp += 1;
    }
}


map<string,Stock> fast_retrievedata(map<string,vector<string>> &mapeps,map<string,Stock> &stockpool,vector<string> &trading_days)
{
    // use ref to make it copyable
    thread t1(retrievedata,ref(mapeps),ref(stockpool),ref(trading_days),1);
    thread t2(retrievedata,ref(mapeps),ref(stockpool),ref(trading_days),2);
    thread t3(retrievedata,ref(mapeps),ref(stockpool),ref(trading_days),3);
    thread t4(retrievedata,ref(mapeps),ref(stockpool),ref(trading_days),4);
    thread t5(retrievedata,ref(mapeps),ref(stockpool),ref(trading_days),5);
    thread t6(retrievedata,ref(mapeps),ref(stockpool),ref(trading_days),6);
    thread t7(retrievedata,ref(mapeps),ref(stockpool),ref(trading_days),7);
    thread t8(retrievedata,ref(mapeps),ref(stockpool),ref(trading_days),8);
    
    // make it in order
    t1.join();
    t2.join();
    t3.join();
    t4.join();
    t5.join();
    t6.join();
    t7.join();
    t8.join();
    
    return stockpool;
}


