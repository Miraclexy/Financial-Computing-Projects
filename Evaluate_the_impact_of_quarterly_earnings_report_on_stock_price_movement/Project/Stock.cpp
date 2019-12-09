//
//  Stock.cpp
//  RetrieveData
//
//  Created by Yi Xu on 2019/11/30.
//  Copyright © 2019 Yi Xu. All rights reserved.
//

#include "Stock.hpp"
#include <iostream>
#include <sstream>
#include <vector>
#include <locale>
#include <iomanip>
#include "curl/curl.h"
#include "operator_overloading.hpp"
#include "RetrieveData.hpp"

using namespace std;

string Stock::sCookies = "";
string Stock::sCrumb = "";
map<string,double> Stock::benchmark_price = map<string,double>();
map<string,double> Stock::benchmark_returns = map<string,double>();

// Retrieve data process
void Stock::Retrieve(string whichkind)
{
    struct MemoryStruct data;

    //file pointer to create file that store the data
    FILE *fp;
    /* declaration of an object CURL */
    CURL *handle;

    /* result of the whole process */
    CURLcode result;

    /* the first functions */
    /* set up the program environment that libcurl needs */
    curl_global_init(CURL_GLOBAL_ALL);
    
    /* curl_easy_init() returns a CURL easy handle that you're gonna reuse in other easy functions*/
    handle = curl_easy_init();

    /* if everything's all right with the easy handle... */
    if (handle)
    {
        while (true)
        {
            data.memory = NULL;
            data.size = 0;
            if (sCookies.length() == 0 || sCrumb.length() == 0)
            {
                //curl_easy_setopt(handle, CURLOPT_URL, "https://finance.yahoo.com/quote/AMZN/history?p=AMZN");
                curl_easy_setopt(handle, CURLOPT_URL, "https://finance.yahoo.com/quote/AMZN/history");
                
                curl_easy_setopt(handle, CURLOPT_SSL_VERIFYPEER, 0);
                curl_easy_setopt(handle, CURLOPT_SSL_VERIFYHOST, 0);
                curl_easy_setopt(handle, CURLOPT_COOKIEJAR, "cookies.txt");
                curl_easy_setopt(handle, CURLOPT_COOKIEFILE, "cookies.txt");

                curl_easy_setopt(handle, CURLOPT_ENCODING, "");


                curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, write_data2);
                curl_easy_setopt(handle, CURLOPT_WRITEDATA, (void *)&data);

                /* perform, then store the expected code in 'result'*/
                result = curl_easy_perform(handle);

                /* Check for errors */
                if (result != CURLE_OK)
                {
                    /* if errors have occured, tell us what is wrong with 'result'*/
                    fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(result));
                    break;
                }

                char cKey[] = "CrumbStore\":{\"crumb\":\"";
                char *ptr1 = strstr(data.memory, cKey);
                char *ptr2 = ptr1 + strlen(cKey);
                char *ptr3 = strstr(ptr2, "\"}");
                if ( ptr3 != NULL )
                    *ptr3 = NULL;

                sCrumb = ptr2;

                fp = fopen("cookies.txt", "r");
                char cCookies[100];
                if (fp) {
                    while (fscanf(fp, "%s", cCookies) != EOF);
                    fclose(fp);
                }
                else
                    cerr << "cookies.txt does not exists" << endl;

                sCookies = cCookies;
                free(data.memory);
                data.memory = NULL;
                data.size= 0;
            }
            
            

            string urlA = "https://query1.finance.yahoo.com/v7/finance/download/";
            string urlB = "?period1=";
            string urlC = "&period2=";
            string urlD = "&interval=1d&events=history&crumb=";
            string url = urlA + symbol + urlB + start_date + urlC + end_date + urlD + sCrumb;
            const char * cURL = url.c_str();
            const char * cookies = sCookies.c_str();

            curl_easy_setopt(handle, CURLOPT_SSL_VERIFYPEER, 0);
            curl_easy_setopt(handle, CURLOPT_SSL_VERIFYHOST, 0);
            curl_easy_setopt(handle, CURLOPT_COOKIEFILE, "cookies.txt");

            curl_easy_setopt(handle, CURLOPT_COOKIE, cookies);   // Only needed for 1st stock
            curl_easy_setopt(handle, CURLOPT_URL, cURL);


            curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, write_data2);
            curl_easy_setopt(handle, CURLOPT_WRITEDATA, (void *)&data);
            result = curl_easy_perform(handle);

            /* Check for errors */
            if (result != CURLE_OK)
            {
                /* if errors have occurred, tell us what is wrong with 'result'*/
                fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(result));
                break;
            }
            
            stringstream sData;
            sData.str(data.memory);
            string sValue, sDate;
            double dValue = 0;
            string line;
            getline(sData, line);
            if(whichkind=="benchmark")
            {
                vector<string> temp1;
                vector<double> temp2;
                while ( getline(sData, line) )
                {
                    sDate = line.substr(0, line.find_first_of(','));
                    line.erase(line.find_last_of(','));
                    sValue = line.substr(line.find_last_of(',')+1);
                    dValue = strtod(sValue.c_str(), NULL);

                    benchmark_price[sDate] = dValue;
                    sDate.append("T16:00:00");
                    temp1.push_back(getTimeinSeconds(sDate));
                    temp2.push_back(dValue);
                }
                for(int i=0;i<(temp1.size())-1;i++)
                {
                    benchmark_returns[temp1[i]] = (temp2[i+1] - temp2[i])/temp2[i];
                }
            }
            else
            {
                while ( getline(sData, line) )
                {
                    line.erase(line.find_last_of(','));
                    sValue = line.substr(line.find_last_of(',')+1);
                    dValue = strtod(sValue.c_str(), NULL);
                    
                    price.push_back(dValue);
                }
            }
            break;
        }
        free(data.memory);
        data.size= 0;
    }
    else
    {
            fprintf(stderr, "Curl init failed!\n");
    }

    /* cleanup since you've used curl_easy_init */
    curl_easy_cleanup(handle);

    /* this function releases resources acquired by curl_global_init() */
    curl_global_cleanup();
    
}


// constructor with parameters
Stock::Stock(string symbol_,string benchmark_symbol_,string start_date_,string end_date_)
{
    symbol=symbol_;
    benchmark_symbol=benchmark_symbol_;
    start_date=start_date_;
    end_date=end_date_;
    
    start_date.append("T16:00:00");
    start_date = getTimeinSeconds(start_date);
    end_date.append("T16:00:00");
    end_date = getTimeinSeconds(end_date);
    
    if(symbol==benchmark_symbol) Retrieve("benchmark");
    else
    {
        Retrieve("stock");
        set_benchmark_returns();
        set_AR();
    }
}


// copy constructor
Stock::Stock(const Stock &r):symbol(r.symbol),benchmark_symbol(r.benchmark_symbol),start_date(r.start_date),end_date(r.end_date)
{
    price = r.price;
    benchmark_ret = r.benchmark_ret;
    AR = r.AR;
}


// calculate daily returns
vector<double> Stock::daily_returns(const vector<double> &s)
{
    vector<double> returns(s.size()-1);
    for(int i =0;i<(s.size())-1;i++) returns[i] = (s[i+1] - s[i]) / s[i];
    return returns;
}



void Stock::set_benchmark_returns()
{
    if(benchmark_returns.empty()) cout<<"benchmark returns doesn't exist"<<endl;
    else
    {
        for(auto itr=benchmark_returns.find(start_date);itr!=benchmark_returns.find(end_date);itr++)
        {
            benchmark_ret.push_back(itr->second);
        }
    }
}



void Stock::set_AR()
{
    vector<double> stock_returns = daily_returns(price);
    AR = stock_returns - benchmark_ret;
}

