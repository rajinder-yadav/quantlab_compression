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
         std::istringstream ss( data );

         err = true;
         std::string tok;
         std::getline( ss, tok, ',' );

         if ( ss.fail() )
         {
            cerr << "Error: Invalid input skipping input\n";
            return MarketData{};
         }

         ticker = tok;

         std::getline( ss, tok, ',' );

         if ( ss.fail() )
         {
            cerr << "Error: Invalid input skipping input\n";
            return MarketData{};
         }

         exchange = static_cast<char>( tok[0] );

         std::getline( ss, tok, ',' );

         if ( ss.fail() )
         {
            cerr << "Error: Invalid input skipping input\n";
            return MarketData{};
         }

         side = static_cast<char>( tok[0] );

         std::getline( ss, tok, ',' );

         if ( ss.fail() )
         {
            cerr << "Error: Invalid input skipping input\n";
            return MarketData{};
         }

         condition = static_cast<char>( tok[0] );

         std::getline( ss, tok, ',' );

         if ( ss.fail() )
         {
            cerr << "Error: Invalid input skipping input\n";
            return MarketData{};
         }

         time = std::stoul( tok );

         std::getline( ss, tok, ',' );

         if ( ss.fail() )
         {
            cerr << "Error: Invalid input skipping input\n";
            return MarketData{};
         }

         reptime = std::stoul( tok );

         std::getline( ss, tok, ',' );

         if ( ss.fail() )
         {
            cerr << "Error: Invalid input skipping input\n";
            return MarketData{};
         }

         price = tok;

         std::getline( ss, tok, ',' );

         if ( ss.fail() )
         {
            cerr << "Error: Invalid input skipping input\n";
            return MarketData{};
         }

         size = std::stoul( tok );

      }
      catch ( ... )
      {
         err = true;
         cerr << "Error: Invalid input skipping input\n";
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
