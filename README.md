# Financial-Computing-Projects
C++ Projects

1. Evaluate the impact of quarterly earnings report on stock price movement

      Retrieve historical price data for all stocks of S&P500 from Yahoo Finance.

      Divide stock pool into three groups(Beat, Meet, Miss) based on the surprise value(difference between estimated EPS and         actual EPS) of each stock.
  
      Get data from 30 days before announcement day and 30 days after announcement day to see the impact of announcement.

      Calculate average of abnormal returns, std of AAR, cumulative average of abnormal returns and std of CAAR for each group. 

      Show the gnuplot graph with CAAR for all 3 groups. 
      
      Use multi-thread to improve the speed of program.

2. Pair trading strategy and backtesting

      Retrieve historical ohlc data for all stock pairs from Yahoo Finance and save them in the database.
      
      Execute pair trading strategy and calculate Profit and loss for each stock pair.
