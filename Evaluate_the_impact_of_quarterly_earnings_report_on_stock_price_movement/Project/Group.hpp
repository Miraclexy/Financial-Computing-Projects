//
//  Group.hpp
//  Retrievedata
//
//  Created by Yi Xu on 2019/12/1.
//  Copyright © 2019 Song Tang. All rights reserved.
//

/*
 Read EPS data from txt;
 Put dato into map;
 Rort stocks and divide stocks into three groups based on Surprises;
 Do bootstrap for 30 times
    each time:
        pick 30 samples for each group;
        calculated AAR and CAAR;
    calculated average AAR, std AAR, average CAAR and std CAAR for each group
 */
#ifndef Group_hpp
#define Group_hpp

#include <stdio.h>
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include "Stock.hpp"
#include <cmath>

using namespace std;


map<string,vector<string>> match(vector<vector<string>> &res);
vector<double> sqrt2(const vector<double>&x);
string convert_time(string time);
void bubblesort(vector<vector<string>> &v);
void swap(vector<string> *a,vector<string> *b);

vector<vector<string>> readfile(string filename);   // read stock name, announcement date, actual EPS, estimated EPS in order
void get_three_groups(vector<vector<string>> allstockinfo,vector<string> &group1,vector<string> &group2,vector<string> &group3);
void get_sample(vector<string> &group,vector<string> &samplegroup);
vector<double> daily_AAR(vector<string> &samplegroup,map<string,Stock>&stockpool_);
vector<double> daily_CAAR(vector<double> &aar);
vector<vector<vector<double>>> bootstrap(map<string,Stock>&stockpool_);
vector<string>get_trading_days(Stock &s);



#endif /* Group_hpp */
