//
//  main.cpp
//  RetrieveData
//
//  Created by Yi Xu on 2019/11/30.
//  Copyright © 2019 Yi Xu. All rights reserved.
//

#include <iostream>
#include "RetrieveData.hpp"
#include "Group.hpp"
#include "Stock.hpp"
#include "operator_overloading.hpp"
#include "gnuplot.hpp"

using namespace std;


int main()
{
    vector<vector<string>> epsdata;
    epsdata = readfile("EPS3.txt");     // epsdata is data from excel
    map<string,vector<string>> mapeps = match(epsdata);
    Stock benchmark("SPY","SPY","2018-11-01","2019-07-01");
    vector<string> trading_days = get_trading_days(benchmark);
    map<string,Stock> stockpool;
    vector<vector<vector<double>>> res;  // res is the matrix of AAR and CAAR with dimension 3*4

    // menu
    cout<<"~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"<<endl;
    cout<<"Enter 1 to retrieve historical price data for all selected stocks."<<endl;
    cout<<"Enter 2 to pull information for one stock from one group."<<endl;
    cout<<"Enter 3 to show AAR, AAR-SD, CAAR and CAAR-SD for one group."<<endl;
    cout<<"Enter 4 to show the gnuplot graph with CAAR for all 3 groups."<<endl;
    cout<<"Enter 5 to exit the program."<<endl;
    cout<<"~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"<<endl;
    
    while(true)
    {
        string num;
        cout<<"Enter your number:"; cin>>num;
        if(num == "1")
        {
            fast_retrievedata(mapeps,stockpool,trading_days);
            cout<<"Retrieve data finished!"<<endl;
        }
        else if(num == "2")
        {
            // if enter 2 before 1, retrieve data first
            if(stockpool.size()==0) fast_retrievedata(mapeps,stockpool,trading_days);
            string symbol;
            cout<<"Enter stock symbol:"; cin>>symbol;
            auto it = stockpool.find(symbol);
            if(it!=stockpool.end())
            {
                Stock s = stockpool[symbol];
                cout<<"Stock symbol: "<<s.get_symbol()<<endl;
                cout<<"Announcement date: "<<mapeps[symbol][0]<<endl;
                cout<<"Start date: "<<s.get_start_date()<<endl;  // in 'seconds' type
                cout<<"End date: "<<s.get_end_date()<<endl;      // in 'seconds' type
                cout<<"Estimated EPS: "<<mapeps[symbol][2]<<endl;
                cout<<"Actual EPS: "<<mapeps[symbol][1]<<endl;
                cout<<"Surprise: "<<mapeps[symbol][3]<<endl;
            }
            else
            {
                cout<<"Stock symbol is not in S&P500 components."<<endl;
            }
        }
        else if(num == "3")
        {
            // if enter 3 before 1, retrieve data first
            if(stockpool.size()==0) fast_retrievedata(mapeps,stockpool,trading_days);
            res = bootstrap(stockpool);
            cout<<endl;
            string groupname;
            cout<<"Enter group name(Beat/Meet/Miss): "; cin>>groupname;
            int g = 0;
            if(groupname=="Beat") g = 0;
            if(groupname=="Meet") g = 1;
            if(groupname=="Miss") g = 2;

            cout<<"Average AAR: "<<endl;
            for(int k=0;k<res[g][0].size();k++)
            {
                cout<<res[g][0][k]<<" ";
            }
            cout<<endl;
            cout<<"AAR Std: "<<endl;
            for(int k=0;k<res[g][1].size();k++)
            {
                cout<<res[g][1][k]<<" ";
            }
            cout<<endl;
            cout<<"Average CAAR: "<<endl;
            for(int k=0;k<res[g][2].size();k++)
            {
                cout<<res[g][2][k]<<" ";
            }
            cout<<endl;
            cout<<"CAAR Std: "<<endl;
            for(int k=0;k<res[g][3].size();k++)
            {
                cout<<res[g][3][k]<<" ";
            }
            cout<<endl;
        }
        else if(num == "4")
        {
            // if enter 4 before 1 and 3, retrieve data and do bootstrap first
            if(stockpool.size()==0) fast_retrievedata(mapeps,stockpool,trading_days);
            if(res.size()==0) res = bootstrap(stockpool);
            cout<<endl;
            double* xData = (double*) malloc((61)*sizeof(double));
            double* yData = (double*) malloc((61)*sizeof(double));
            double* yData2 = (double*) malloc((61)*sizeof(double));
            double* yData3 = (double*) malloc((61)*sizeof(double));

            for (int i = 0; i < 61; i++) xData[i] = -30+i;
            for (int i = 0; i < 61; i++) yData[i] = res[0][2][i];
            for (int i = 0; i < 61; i++) yData2[i] = res[1][2][i];
            for (int i = 0; i < 61; i++) yData3[i] = res[2][2][i];

            plotResults(xData, yData, yData2, yData3);
            cout<<endl;
        }
        else if(num == "5")
        {
            break;
        }
        else
        {
            cout<<"Re-enter the number."<<endl;
        }
    }
    return 0;
}
    
    
    




