//
//  Database.cpp
//  FRE7831_MarketData_JSON_MAC
//
//  Created by Yi Xu on 2020/4/19.
//  Copyright © 2020 NYU. All rights reserved.
//

#include "Database.hpp"
#include <string>
#include <vector>
#include <iostream>
#include "curl/curl.h"
#include <sqlite3.h>

using namespace std;



int OpenDatabase(sqlite3*& db)
{
    int rc = 0;
    // Open Database
    cout << "Opening PairTrading.db ..." << endl;
    rc = sqlite3_open("PairTrading.db", &db);
    if (rc)
    {
        cerr << "Error opening SQLite3 database: " << sqlite3_errmsg(db) << endl << endl;
        sqlite3_close(db);
        return -1;
    }
    else
    {
        cout << "Opened PairTrading.db." << endl << endl;
    }
    return rc;
}



int CreateTables(sqlite3*& db)
{
    int rc = 0;
    int rc2 = 0;
    int rc3 = 0;
//    int rc4 = 0;
    char *error = NULL;
    
    // Drop the table if exists
    string sql_dropatable = "DROP TABLE IF EXISTS PairOnePrices";
    if (sqlite3_exec(db, sql_dropatable.c_str(), 0, 0, 0) != SQLITE_OK) {
        cout << "SQLite can't drop PairOnePrices table" << std::endl;
        sqlite3_close(db);
        return -1;
    }
    
    // Create the PairOnePrices table
    cout << "Creating PairOnePrices table ..." << endl;
    const char *sqlCreateTable = "CREATE TABLE PairOnePrices (" \
        "symbol CHAR(20) NOT NULL," \
        "date CHAR(20) NOT NULL," \
        "open REAL NOT NULL," \
        "high REAL NOT NULL," \
        "low REAL NOT NULL," \
        "close REAL NOT NULL," \
        "adjusted_close REAL NOT NULL," \
        "volume INT NOT NULL," \
        "PRIMARY KEY(symbol, date)" \
        "Foreign Key(Symbol) references Stocks(Symbol)\n" \
        "ON DELETE CASCADE\n" \
        "ON UPDATE CASCADE\n);";
    
    rc = sqlite3_exec(db, sqlCreateTable, NULL, NULL, &error);
    if (rc)
    {
        cerr << "Error executing SQLite3 statement: " << sqlite3_errmsg(db) << endl << endl;
        sqlite3_free(error);
    }
    else
    {
        cout << "Created PairOnePrices table." << endl << endl;
    }
    
    
    // Drop the table if exists
    string sql_dropatable2 = "DROP TABLE IF EXISTS PairTwoPrices";
    if (sqlite3_exec(db, sql_dropatable2.c_str(), 0, 0, 0) != SQLITE_OK) {
        cout << "SQLite can't drop PairTwoPrices table" << std::endl;
        sqlite3_close(db);
        return -1;
    }
    
    // Create the PairTwoPrices table
    cout << "Creating PairTwoPrices table ..." << endl;
    const char *sqlCreateTable2 = "CREATE TABLE PairTwoPrices (" \
        "symbol CHAR(20) NOT NULL," \
        "date CHAR(20) NOT NULL," \
        "open REAL NOT NULL," \
        "high REAL NOT NULL," \
        "low REAL NOT NULL," \
        "close REAL NOT NULL," \
        "adjusted_close REAL NOT NULL," \
        "volume INT NOT NULL," \
        "PRIMARY KEY(symbol, date)" \
        "Foreign Key(Symbol) references Stocks(Symbol)\n" \
        "ON DELETE CASCADE\n" \
        "ON UPDATE CASCADE\n);";
    
    rc2 = sqlite3_exec(db, sqlCreateTable2, NULL, NULL, &error);
    if (rc2)
    {
        cerr << "Error executing SQLite3 statement: " << sqlite3_errmsg(db) << endl << endl;
        sqlite3_free(error);
    }
    else
    {
        cout << "Created PairTwoPrices table." << endl << endl;
    }
    
    // Drop the table if exists
    string sql_dropatable3 = "DROP TABLE IF EXISTS StockPairs";
    if (sqlite3_exec(db, sql_dropatable3.c_str(), 0, 0, 0) != SQLITE_OK) {
        cout << "SQLite can't drop StockPairs table" << std::endl;
        sqlite3_close(db);
        return -1;
    }
    // Create StockPairs table
    cout<<"Creating StockPairs table..."<<endl;
    const char * sqlCreateTable3 = "CREATE TABLE StockPairs (" \
        "id INT NOT NULL," \
        "symbol1 CHAR(20) NOT NULL," \
        "symbol2 CHAR(20) NOT NULL," \
        "volatility FLOAT NOT NULL," \
        "profit_loss FLOAT NOT NULL," \
        "PRIMARY KEY(symbol1,symbol2));";
    
    rc3 = sqlite3_exec(db, sqlCreateTable3, NULL, NULL, &error);
    if (rc3)
    {
        cerr << "Error executing SQLite3 statement: " << sqlite3_errmsg(db) << endl << endl;
        sqlite3_free(error);
    }
    else
    {
        cout << "Created StockPairs table." << endl << endl;
    }
    
    /*
    // Drop the table if exists
    string sql_dropatable4 = "DROP TABLE IF EXISTS PairPrices";
    if (sqlite3_exec(db, sql_dropatable4.c_str(), 0, 0, 0) != SQLITE_OK) {
        cout << "SQLite can't drop PairPrices table" << std::endl;
        sqlite3_close(db);
        return -1;
    }

    // Create PairPrices table
    cout<<"Creating PairPrices table..."<<endl;
    const char *sqlCreateTable4 = "CREATE TABLE PairPrices (" \
        "symbol1 CHAR(20) NOT NULL," \
        "symbol2 CHAR(20) NOT NULL," \
        "date CHAR(20) NOT NULL," \
        "open1 REAL NOT NULL," \
        "close1 REAL NOT NULL," \
        "open2 REAL NOT NULL," \
        "close2 REAL NOT NULL," \
        "profit_loss FLOAT NOT NULL," \
        "PRIMARY KEY(symbol1,symbol2,date));";
    
    rc4 = sqlite3_exec(db, sqlCreateTable4, NULL, NULL, &error);
    if (rc4)
    {
        cerr << "Error executing SQLite3 statement: " << sqlite3_errmsg(db) << endl << endl;
        sqlite3_free(error);
    }
    else
    {
        cout << "Created PairPrices table." << endl << endl;
    }
    */
    return 0;
}


int InsertTable(sqlite3* db, const char * sqlInsert)
{
    char *error = NULL;
    int rc = sqlite3_exec(db, sqlInsert, NULL, NULL, &error);
    if (rc)
    {
        cerr << "Error executing SQLite3 statement: " << sqlite3_errmsg(db) << endl << endl;
        sqlite3_free(error);
    }
    
    return 0;
}


int DisplayTables(sqlite3* db, const char * sqlSelect)
{
    int rc = 0;
    char *error = NULL;

    cout << "Retrieving values in table DailyTrades ..." << endl;
    char **results = NULL;
    int rows, columns;
    sqlite3_get_table(db, sqlSelect, &results, &rows, &columns, &error);
    if (rc)
    {
        cerr << "Error executing SQLite3 query: " << sqlite3_errmsg(db) << endl << endl;
        sqlite3_free(error);
    }
    else
    {
        // Display Table
        for (int rowCtr = 0; rowCtr <= rows; ++rowCtr)
        {
            for (int colCtr = 0; colCtr < columns; ++colCtr)
            {
                // Determine Cell Position
                int cellPosition = (rowCtr * columns) + colCtr;

                // Display Cell Value
                cout.width(12);
                cout.setf(ios::left);
                cout << results[cellPosition] << " ";
            }

            // End Line
            cout << endl;

            // Display Separator For Header
            if (0 == rowCtr)
            {
                for (int colCtr = 0; colCtr < columns; ++colCtr)
                {
                    cout.width(12);
                    cout.setf(ios::left);
                    cout << "~~~~~~~~~~~~ ";
                }
                cout << endl;
            }
        }
    }
    sqlite3_free_table(results);
    
    // Close Database
    cout << "Closing PairTrading.db ..." << endl;
    sqlite3_close(db);
    cout << "Closed PairTrading.db" << endl << endl;

    return 0;
}


int CalculateVar(sqlite3 *db)
{
    char *error = NULL;
    string calculate_volatility_for_pair = string("Update StockPairs SET volatility =")
    + "(SELECT(AVG((close1/close2)*(close1/close2)) -AVG(close1/close2)*AVG(close1/close2)) as variance "
    + "FROM PairPrices "
    + "WHERE StockPairs.symbol1 = PairPrices.symbol1 AND StockPairs.symbol2 = PairPrices.symbol2 AND PairPrices.date <= \'"
    + "2019-12-31" + "\');";

    const char * sqlUpdate = calculate_volatility_for_pair.c_str();
    int rc = sqlite3_exec(db, sqlUpdate, NULL, NULL, &error);
    if (rc)
    {
        cerr << "Error executing SQLite3 statement: " << sqlite3_errmsg(db) << endl << endl;
        sqlite3_free(error);
    }
    
    return 0;
}
