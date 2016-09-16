//==================================================================================================
// The MIT License (MIT)
//
// Copyright (c) 2016 Rajinder Yadav <rajinder.yadav@hotmail.com>
//==================================================================================================

#ifndef _DATAPARSER_HPP_
#define _DATAPARSER_HPP_

using std::cerr;

/**
 * DataParser takes a CSV input string and return a MarketData structure.
 * Performs validation during parsing of input data.
 */
class DataParser
{
public:
   /**
    * Helper Function, parse CSV string to return market data object.
    * @param [in] data - string CSV market data, format "timestamp,symbol,quantity,price"
    * @param [out] err - error flag (false = success, true = error).
    * @return MarketData object.
    */
   static MarketData GetMarketData( const std::string & data, bool & err )
   {
      // Tick data listed in CSV parse order
      std::string ticker;
      char exchange;
      char side;
      char condition;
      uint64_t time;
      uint64_t reptime;
      std::string price;
      uint32_t size;

      try
      {
         char tick_csv[1000];
         strcpy(tick_csv, data.c_str()); 
         std::string seperator( "," );
         char * token = std::strtok( tick_csv, seperator.c_str() );

         if ( !token )
         {
            cerr << "Aborting! Error: Invalid ticker input skipping input\n";
            return MarketData{};
         }

         ticker = std::string(token);

         token = std::strtok( NULL, seperator.c_str() );

         if ( !token )
         {
            cerr << "Aborting! Error: Invalid exchange input skipping input\n";
            return MarketData{};
         }

         exchange = static_cast<char>( *token );

         token = std::strtok( NULL, seperator.c_str() );

         if ( !token )
         {
            cerr << "Aborting! Error: Invalid side input skipping input\n";
            return MarketData{};
         }

         side = static_cast<char>( *token );

         token = std::strtok( NULL, seperator.c_str() );

         if ( !token )
         {
            cerr << "Aborting! Error: Invalid condition input skipping input\n";
            return MarketData{};
         }

         condition = static_cast<char>( *token );

         token = std::strtok( NULL, seperator.c_str() );

         if ( !token )
         {
            cerr << "Aborting! Error: Invalid time input skipping input\n";
            return MarketData{};
         }

         time = std::stoul( std::string(token) );

         token = std::strtok( NULL, seperator.c_str() );

         if ( !token )
         {
            cerr << "Aborting! Error: Invalid reptime input skipping input\n";
            return MarketData{};
         }

         reptime = std::stoul( std::string(token) );

         token = std::strtok( NULL, seperator.c_str() );

         if ( !token )
         {
            cerr << "Aborting! Error: Invalid price input skipping input\n";
            return MarketData{};
         }

         price = std::string(token);

         token = std::strtok( NULL, seperator.c_str() );

         if ( !token )
         {
            cerr << "Aborting! Error: Invalid size input skipping input\n";
            return MarketData{};
         }

         size = std::stoul( std::string(token) );
      }
      catch ( ... )
      {
         err = true;
         cerr << "Aborting! Error: Invalid input skipping input\n";
         return MarketData{};
      }

      err = false;
      // C++11 uses move to return by value, efficent!
      return MarketData{ ticker,
                         exchange,
                         side,
                         condition,
                         time,
                         reptime,
                         price,
                         size };
   }
};

#endif // _DATAPARSER_HPP_
