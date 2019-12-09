//
//  operator_overloading.cpp
//  Project
//
//  Created by Yi Xu on 2019/12/3.
//  Copyright © 2019 Song Tang. All rights reserved.
//

#include "operator_overloading.hpp"
#include <stdio.h>
#include <vector>
#include <iomanip>
#include <cmath>
using namespace std;

vector<double> operator-(const vector<double>&x1,const vector<double>&x2)
{
    vector<double> res(x1.size());
    for(int i=0;i<res.size();i++)
    {
        res[i] = x1[i] - x2[i];
    }
    return res;
}

vector<double> operator+(const vector<double>&c,const vector<double>&d)
{
    vector<double> res(c.size());
    for(int i=0;i<res.size();i++)
    {
        res[i] = c[i] + d[i];
    }
    return res;
}


vector<double> operator/(const vector<double>&a, const double &b)
{
    vector<double> res(a.size());
    for(int i=0;i<res.size();i++)
    {
        res[i] = a[i]/b;
    }
    return res;
}


vector<double> operator*(const vector<double>&x,const vector<double>&y)
{
    vector<double> res(x.size());
    for(int i=0;i<res.size();i++)
    {
        res[i] = x[i]*y[i];
    }
    return res;
}


