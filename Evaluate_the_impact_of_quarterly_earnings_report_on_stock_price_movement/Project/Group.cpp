//
//  Group.cpp
//  Retrievedata
//
//  Created by Yi Xu on 2019/12/1.
//  Copyright © 2019 Song Tang. All rights reserved.
//

#include "Group.hpp"
#include <string>   // include getline()
#include <vector>
#include <map>
#include <iostream>
#include <fstream>
#include <sstream>  //include istringstream()
#include <ctime>
#include <cstdlib>
#include <random> // std::default_random_engine
#include <chrono> // std::chrono::system_clock
#include "operator_overloading.hpp"


using namespace std;

// convert 01/02/2019 type data into 2019-01-02 type
string convert_time(string time)
{
    string year;
    string month;
    string date;
    string time2;
    time2 = time.substr(6,4)+"-"+time.substr(0,2)+"-"+time.substr(3,2);
    return time2;
}


// read data from txt
vector<vector<string>> readfile(string filename)
{
    ifstream file;
    file.open(filename);
    
    vector<vector<string>> res;
    
    string line;
    string data;

    while(getline(file,line))
    {
        vector<string> temp;
        vector<string> usefultemp;
        istringstream is(line);
        double act;
        double est;
        while(is>>data)
        {
            temp.push_back(data);
        }
        usefultemp.push_back(temp[0]);  // stock name
        usefultemp.push_back(convert_time(temp[3]));  // announcement date
        usefultemp.push_back(temp[6]);  // actual EPS
        usefultemp.push_back(temp[7]);  // estimated EPS
        
        stringstream actual(temp[6]);
        stringstream estimate(temp[7]);
        actual>>act;
        estimate>>est;
        usefultemp.push_back(to_string((act-est)/est));  // surprise
        usefultemp.push_back("");   // start date
        usefultemp.push_back("");   // end date

        res.push_back(usefultemp);      // res has a headline which we do not need
    }
    return res;
}


// put data into map for later benefit
map<string,vector<string>> match(vector<vector<string>> &res)
{
    map<string,vector<string>> mapres;
    for(int i=1;i<res.size();i++)
    {
        vector<string> temp2;
        temp2.push_back(res[i][1]);  // announcement date
        temp2.push_back(res[i][2]);  // actual EPS
        temp2.push_back(res[i][3]);  // estimated EPS
        temp2.push_back(res[i][4]);  // surprise
        temp2.push_back(res[i][5]);  // start date
        temp2.push_back(res[i][6]);  // end date
        
        mapres[res[i][0]] = temp2;
     }
    return mapres;
}


// sorting
void swap(vector<string> *a,vector<string> *b)
{
    vector<string> temp = *a;
    *a = *b;
    *b = temp;
}

// bubble sort, from small number to large number
void bubblesort(vector<vector<string>> &v)
{
    for(int i=0;i<v.size()-1;i++)
    {
        for(int j=0;j<v.size()-i-1;j++)
        {
            if(v[j][4]>v[j+1][4]) swap(&v[j], &v[j+1]);
        }
    }
}


// divide into three groups, beat with the highest surprise, and miss with the lowest
void get_three_groups(vector<vector<string>> allstockinfo, vector<string> &group1, vector<string> &group2, vector<string> &group3)
{
    // delete the headline we do not need
    vector<vector<string>> allstockinfo2;
    for(int i=1;i<allstockinfo.size();i++)
    {
        allstockinfo2.push_back(allstockinfo[i]);
    }
    
    bubblesort(allstockinfo2);
    for(int i=0;i<int(allstockinfo2.size()/3);i++)
    {
        // group3 is the miss group
        group3.push_back(allstockinfo2[i][0]);  // save stock name
    }
    
    for(int i=int(allstockinfo2.size()/3);i<int(2*(allstockinfo2.size()/3));i++)
    {
        // group2 is the meet group
        group2.push_back(allstockinfo2[i][0]);  // save stock name
    }
    
    for(int i=int(2*(allstockinfo2.size()/3));i<allstockinfo2.size();i++)
    {
        // group1 is the beat group
        group1.push_back(allstockinfo2[i][0]);  // save stock name
    }
    // each group contains 165 or so stocks
}


// get 30 samples from group
void get_sample(vector<string> &group,vector<string> &samplegroup)
{
    // randomly sorted a vector
    unsigned seed = (unsigned int)(chrono::system_clock::now ()).time_since_epoch ().count ();
    shuffle (group.begin (), group.end (), default_random_engine (seed));
    // get first 30 stocks
    for(int i=0;i<30;i++)
    {
        samplegroup.push_back(group[i]);
    }
}


// calculate average AAR for one day at one time
vector<double> daily_AAR(vector<string> &samplegroup,map<string,Stock>&stockpool_)
{
    vector<double> aar;
    for(int i=0;i<61;i++)   // in total 61 days
    {
        double x = 0.0;
        for(vector<string>::iterator itr=samplegroup.begin();itr!=samplegroup.end();itr++)
        {
            x += (stockpool_[*itr].get_AR()[i]);
        }
        x /= samplegroup.size();
        aar.push_back(x);
    }
    return aar;
}


// calculate daily CAAR
vector<double> daily_CAAR(vector<double> &aar)
{
    vector<double> caar;
    caar.push_back(aar[0]);
    for(int i=1;i<61;i++)    // in total 61 days
    {
        caar.push_back(caar[i-1]+aar[i]);
    }
    return caar;
}

// sqrt function for vector
vector<double> sqrt2(const vector<double>&x)
{
    vector<double> res(x.size());
    for(int i=0;i<res.size();i++)
    {
        res[i] = sqrt(x[i]);
    }
    return res;
}


vector<vector<vector<double>>> bootstrap(map<string,Stock>&stockpool_)
{
    
    vector<vector<string>> stockdata = readfile("EPS3.txt");
    vector<string> Beat;
    vector<string> Meet;
    vector<string> Miss;
    get_three_groups(stockdata, Beat, Meet, Miss);  // contains stock names

    //        avg_AAR   std_AAR   avg_CAAR   std_CAAR
    // Beat  vector(61)
    // Meet      finalres is something like this
    // Miss
    // initialize finalres
    vector<vector<vector<double>>> finalres;
    vector<double> t(61);
    vector<vector<double>> mid;
    mid.push_back(t);
    mid.push_back(t);
    mid.push_back(t);
    mid.push_back(t);
    finalres.push_back(mid);
    finalres.push_back(mid);
    finalres.push_back(mid);

    vector<double> beat_avg_aar(61);
    vector<double> meet_avg_aar(61);
    vector<double> miss_avg_aar(61);
    vector<double> beat_std_aar(61);
    vector<double> meet_std_aar(61);
    vector<double> miss_std_aar(61);

    vector<double> beat_avg_caar(61);
    vector<double> meet_avg_caar(61);
    vector<double> miss_avg_caar(61);
    vector<double> beat_std_caar(61);
    vector<double> meet_std_caar(61);
    vector<double> miss_std_caar(61);

    // do bootstrap 30 times
    for(int i=0;i<30;i++)
    {
        vector<string> Beat_sample;
        vector<string> Meet_sample;
        vector<string> Miss_sample;
        get_sample(Beat, Beat_sample);
        get_sample(Meet, Meet_sample);
        get_sample(Miss, Miss_sample);

        vector<double> beataar = daily_AAR(Beat_sample,stockpool_);
        vector<double> meetaar = daily_AAR(Meet_sample,stockpool_);
        vector<double> missaar = daily_AAR(Miss_sample,stockpool_);

        vector<double> beatcaar = daily_CAAR(beataar);
        vector<double> meetcaar = daily_CAAR(meetaar);
        vector<double> misscaar = daily_CAAR(missaar);

        beat_avg_aar = beat_avg_aar + beataar;
        meet_avg_aar = meet_avg_aar + meetaar;
        miss_avg_aar = miss_avg_aar + missaar;
        
        // std = sqrt(E(x^2)-E(x)^2)
        beat_std_aar = beat_std_aar + beataar*beataar;
        meet_std_aar = meet_std_aar + meetaar*meetaar;
        miss_std_aar = miss_std_aar + missaar*missaar;
        
        beat_avg_caar = beat_avg_caar + beatcaar;
        meet_avg_caar = meet_avg_caar + meetcaar;
        miss_avg_caar = miss_avg_caar + misscaar;
        
        // std = sqrt(E(x^2)-E(x)^2)
        beat_std_caar = beat_std_caar + beatcaar*beatcaar;
        meet_std_caar = meet_std_caar + meetcaar*meetcaar;
        miss_std_caar = miss_std_caar + misscaar*misscaar;
        
        
        Beat_sample.clear();Meet_sample.clear();Miss_sample.clear();
        beataar.clear();meetaar.clear();missaar.clear();
        beatcaar.clear();meetcaar.clear();misscaar.clear();
        
        
//      cout<<i+1<<"th bootstrap finished.";
    }

    finalres[0][0] = beat_avg_aar/30.0;
    finalres[1][0] = meet_avg_aar/30.0;
    finalres[2][0] = miss_avg_aar/30.0;
    finalres[0][1] = sqrt2(beat_std_aar/30.0 - finalres[0][0]*finalres[0][0]);
    finalres[1][1] = sqrt2(meet_std_aar/30.0 - finalres[1][0]*finalres[1][0]);
    finalres[2][1] = sqrt2(miss_std_aar/30.0 - finalres[2][0]*finalres[2][0]);
    
    finalres[0][2] = beat_avg_caar/30.0;
    finalres[1][2] = meet_avg_caar/30.0;
    finalres[2][2] = miss_avg_caar/30.0;
    finalres[0][3] = sqrt2(beat_std_caar/30.0 - finalres[0][2]*finalres[0][2]);
    finalres[1][3] = sqrt2(meet_std_caar/30.0 - finalres[1][2]*finalres[1][2]);
    finalres[2][3] = sqrt2(miss_std_caar/30.0 - finalres[2][2]*finalres[2][2]);
    
    cout<<"Bootstrap finished!"<<endl;
    return finalres;
}



// get trading days since the date shown on Yahoo Finance is just trading days
vector<string>get_trading_days(Stock &s)
{
    vector<string> trading_days;
    for(map<string,double>::iterator itr=s.benchmark_price.begin();itr!=s.benchmark_price.end();itr++)
        trading_days.push_back(itr->first);
    return trading_days;
}




