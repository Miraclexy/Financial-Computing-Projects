//
//  Stock.hpp
//  RetrieveData
//
//  Created by Yi Xu on 2019/11/30.
//  Copyright © 2019 Yi Xu. All rights reserved.
//

#ifndef Stock_hpp
#define Stock_hpp

#include <stdio.h>
#include <vector>
#include <string>
#include <map>
#include <cmath>


using namespace std;

class Stock
{
private:
    string symbol;
    string benchmark_symbol;
    string start_date;
    string end_date;
    
    vector<double> price;
    vector<double> benchmark_ret;
    vector<double> AR;  // abnormal returns
    
public:
    static string sCookies;
    static string sCrumb;
    static map<string,double> benchmark_price;
    static map<string,double> benchmark_returns;
    
    
    Stock(){};
    Stock(string symbol_,string benchmark_symbol_,string start_date_,string end_date_);
    Stock(const Stock &r);
    
    void Retrieve(string whichkind);  // whichkind="benchmark" to retrieve benchmark data
    vector<double>daily_returns(const vector<double> &s);  // input data is price
    
    void set_AR();
    void set_benchmark_returns();
    
    string get_symbol(){return symbol;}
    string get_benchmark_symbol(){return benchmark_symbol;}
    string get_start_date(){return start_date;}
    string get_end_date(){return end_date;}
    vector<double> &get_price(){return price;}
    vector<double> &get_benchmark_ret(){return benchmark_ret;}
    vector<double> &get_AR(){return AR;}
};




#endif /* Stock_hpp */

