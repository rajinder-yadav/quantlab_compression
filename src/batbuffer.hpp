#ifndef _bat_buffer_hpp_
#define _bat_buffer_hpp_

class BatBuffer : public BitBuffer
{
public:

   size_ft WriteTicker( const ticker_ft index )
   {
      return Write( index, TICKER_SIZE );
   }

   size_ft WriteExchange( const exchange_ft ex )
   {
      return Write( static_cast<uint32_t>( ex ), 8 );
   }

   size_ft WriteSide( const side_ft side )
   {
      return Write( side, SIDE_SIZE );
   }

   size_ft WriteCondition( const condition_ft cond )
   {
      return Write( static_cast<uint32_t>( cond ), 8 );
   }

   size_ft WriteTime( const time_ft val )
   {
      // Verify field type, it may have changed
      assert( TIME_SIZE == 32 + 5 );

      // Mico-sec since midnight (24 hrs = 8.64e+10 = 37bits)
      buffer_ft hi = val >> 32;
      buffer_ft lo = static_cast<buffer_ft>( val );
      Write( hi, 5 );
      Write( lo, 32 );
      return 37;
   }

   uint32_t WritePrice( const std::string & val )
   {
      return WriteString( val, PRICE_SIZE );
   }

   size_ft WriteShares( const share_ft val )
   {
      return Write( val, SHARE_SIZE );
   }

   /**
    * Write string into packet.
    *
    * @param[in] val - String to be packed.
    *
    * @param[in] size - Count of characters to pack string into.
    *                   Count must be atlest string size.
    *
    * @return - The number of bits occupied by the string.
    */
   uint32_t WriteString( const std::string & val, const size_ft size )
   {
      assert( size >= val.size() );
      size_ft pad = size - val.size();

      for ( size_ft i = 0; i < pad; ++i )
      {
         Write( static_cast<uint32_t>( uint32_t( ' ' ) ), 8 );
      }

      for ( char c : val )
      {
         Write( static_cast<uint32_t>( c ), 8 );
      }

      return size > val.size() ? size : val.size();
   }

   bool ReadTicker( ticker_ft & ticker )
   {
      bool error;
      buffer_ft tmp;
      bool more = Read( tmp, TICKER_SIZE, error );
      ticker = static_cast<ticker_ft>( tmp );
      return more;
   }

   bool ReadExchance( exchange_ft & exchange )
   {
      std::string s;
      bool more = ReadString( s, EXCHANGE_SIZE );
      exchange = s[0];
      return more;
   }

   bool ReadSide( side_ft & side )
   {
      bool error;
      buffer_ft tmp;
      bool more = Read( tmp, SIDE_SIZE, error );
      side = static_cast<side_ft>( tmp );
      return more;
   }

   bool ReadCondition( condition_ft & cond )
   {
      std::string s;
      bool more = ReadString( s, CONDITION_SIZE );
      cond = s[0];
      return more;
   }

   bool ReadTime( time_ft & val )
   {
      // Verify field type, it may have changed
      assert( TIME_SIZE == 32 + 5 );
      bool error1;
      bool error2;
      bool more = false;
      buffer_ft tmp;

      more = Read( tmp, 32, error1 );
      assert( more );
      val = static_cast<buffer_ft>( tmp );
      more = Read( tmp, 5, error2 );
      time_ft hi = static_cast<buffer_ft>( tmp );
      val = ( hi << 32 ) | val;
      // error = error1 | error2;
      return more;
   }

   bool ReadPrice( std::string & price )
   {
      bool error;
      bool more = false;
      price.clear();
      buffer_ft ch;

      for ( size_ft i = 0; i < PRICE_SIZE; ++i )
      {
         more = Read( ch, 8, error );

         if ( ch == static_cast<char>( ' ' ) )
         {
            continue;
         }

         price = static_cast<char>( ch ) + price;
      }

      return more;
   }


   bool ReadShares( share_ft & val )
   {
      bool error;
      buffer_ft tmp;
      bool more = Read( tmp, SHARE_SIZE, error );
      val = static_cast<share_ft>( tmp );
      return more;
   }

   bool ReadString( std::string & s, const size_ft count )
   {
      bool error;
      bool more = false;
      s.clear();
      uint32_t ch;

      for ( int i = 0; i < count; ++i )
      {
         more = Read( ch, 8, error );
         s = static_cast<char>( ch ) + s;
      }

      return more;
   }

   void Save( const std::string & filename )
   {
      std::ofstream f( filename, std::ios::binary );

      if ( f.good() )
      {
         for ( buffer_ft v : packet )
         {
            f.write( reinterpret_cast<char*>(&v), sizeof( buffer_ft ) );
         }
      }
   }

   void Load( const std::string & filename )
   {
      std::ifstream f( filename, std::ios::binary );

      if ( f.good() )
      {
         packet.clear();
         buffer_ft buf;

         while ( true )
         {
            f.read( reinterpret_cast<char*>(&buf), sizeof( buffer_ft ) );

            if ( !f.good() )
            {
               break;
            }

            packet.push_back( buf );
         } // while
         SetReadMode();
      }
   }

};

#endif // _bat_buffer_hpp_
