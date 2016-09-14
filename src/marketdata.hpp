#ifndef _MARKETDATA_HPP_
#define _MARKETDATA_HPP_

/**
 * Market data structure for a given BAT tick.
 */
struct MarketData
{
   // C++11 Move-aware members
   std::string ticker;
   char exchange;
   char side;
   char condition;
   uint64_t time;
   uint64_t reptime;
   std::string price;
   uint32_t size;       // shares

   explicit MarketData()
      : ticker{}
      , exchange{}
      , side{}
      , condition{}
      , time{}
      , reptime{}
      , price{}
      , size{}
   {
   }

   explicit MarketData( std::string sym,
                        char ex,
                        char s,
                        char cond,
                        uint64_t tm,
                        uint64_t rtm,
                        std::string p,
                        uint32_t shares )
      : ticker{sym}
      , exchange{ex}
      , side{s}
      , condition{cond}
      , time{tm}
      , reptime{rtm}
      , price{p}
      , size{shares}
   {
   }

   void Dump() const
   {
      cout << ticker << ","
           << exchange << ","
           << side << ","
           << condition << ","
           << time << ","
           << reptime << ","
           << price << ","
           << size << "    "
           << reptime - time << endl;
   }
};

#endif // _MARKETDATA_HPP_
