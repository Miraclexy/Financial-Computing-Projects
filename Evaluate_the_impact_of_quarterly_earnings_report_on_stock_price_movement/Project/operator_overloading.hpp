//
//  operator_overloading.hpp
//  Project
//
//  Created by Yi Xu on 2019/12/3.
//  Copyright © 2019 Song Tang. All rights reserved.
//

#ifndef operator_overloading_hpp
#define operator_overloading_hpp

#include <stdio.h>
#include <vector>
#include <cmath>
using namespace std;

vector<double> operator-(const vector<double>&x1,const vector<double>&x2);
vector<double> operator+(const vector<double>&c,const vector<double>&d);
vector<double> operator/(const vector<double>&a, const double &b);
vector<double> operator*(const vector<double>&x,const vector<double>&y);






#endif /* operator_overloading_hpp */
