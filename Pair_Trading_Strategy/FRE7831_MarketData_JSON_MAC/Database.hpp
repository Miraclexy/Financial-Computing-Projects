//
//  Database.hpp
//  FRE7831_MarketData_JSON_MAC
//
//  Created by Yi Xu on 2020/4/19.
//  Copyright © 2020 NYU. All rights reserved.
//

#ifndef Database_hpp
#define Database_hpp

#include <stdio.h>
#pragma once
#include<sqlite3.h>
#include <string>
#include "Stock.hpp"

class Stock;
int OpenDatabase(sqlite3*& db);
int CreateTables(sqlite3*& db);
int InsertTable(sqlite3* db, const char * sqlInsert);
int DisplayTables(sqlite3* db, const char * sqlSelect);

int CalculateVar(sqlite3 *db);


#endif /* Database_hpp */
