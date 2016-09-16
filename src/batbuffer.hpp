//==================================================================================================
// The MIT License (MIT)
//
// Copyright (c) 2016 Rajinder Yadav <rajinder.yadav@hotmail.com>
//==================================================================================================

#ifndef _bat_buffer_hpp_
#define _bat_buffer_hpp_

class BatBuffer : public BitBuffer
{
   SymbolTable table;
   std::ofstream fstreamer;

public:
   enum BAT { BID = 0x0, ASK = 0x1, TRADE = 0x2 };

   std::deque<std::string> Table()
   {
      return table.Table();
   }

   // Interface method
   void Compress( MarketData md, bool eof )
   {
      if ( !eof )
      {
         ticker_ft index = table.Add( md.ticker );

         uint32_t s1 = WriteTicker( index );
         uint32_t s2 = WriteExchange( static_cast<exchange_ft>( md.exchange ) );

         side_ft side;

         switch ( md.side )
         {
         case 'B':
            side = BAT::BID;
            break;

         case 'A':
            side = BAT::ASK;
            break;

         case 'T':
            side = BAT::TRADE;
            break;

         default:
            assert( false );
         }

         uint32_t s3 = WriteSide( side );
         uint32_t s4 = WriteCondition( static_cast<condition_ft>( md.condition ) );
         uint32_t s5 = WriteTime( static_cast<time_ft>( md.time ) );
         uint32_t s6 = WriteTime( static_cast<time_ft>( md.reptime ) );
         uint32_t s7 = WritePrice( md.price );
         uint32_t s8 = WriteShares( static_cast<share_ft>( md.size ) );

         /* Debug logging
         cout << index << "  "
              << md.ticker << ","
              << md.exchange << ","
              << int( side ) << ","
              << md.condition << ","
              << md.time << ","
              << md.reptime << ","
              << md.price << ","
              << md.size << endl;
               */
      }
      else
      {
         Flush();
      }
   }

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
      // Verify field size, it may have changed
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
      price_ft price = std::stod( val );
      uint64_t * ptr1 = reinterpret_cast<uint64_t *>( &price );
      uint32_t high = reinterpret_cast<uint64_t>( *ptr1 ) >> 32;
      uint32_t low  = static_cast<uint32_t>( reinterpret_cast<uint64_t>( *ptr1 ) );

      Write( high, 32 );
      Write( low, 32 );
      return PRICE_SIZE;
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
    *                   Count must be atleast string size.
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

      more = Read( tmp, 5, error2 );
      assert( more );
      time_ft hi = static_cast<buffer_ft>( tmp );
      more = Read( tmp, 32, error1 );
      val = static_cast<buffer_ft>( tmp );
      val = ( hi << 32 ) | val;
      // error = error1 | error2;
      return more;
   }

   bool ReadPrice( std::string & price )
   {
      // Verify field type, it may have changed
      assert( PRICE_SIZE == 64 );
      bool error1;
      bool error2;
      bool more = false;
      buffer_ft tmp;

      more = Read( tmp, 32, error1 );
      assert( more );
      uint64_t hi = static_cast<uint64_t>( tmp );
      more = Read( tmp, 32, error2 );
      uint64_t lo = static_cast<uint64_t>( tmp );
      uint64_t val = ( hi << 32 ) | lo;

      price_ft * ptr = reinterpret_cast<price_ft *>( &val );

      std::ostringstream ss;
      ss << std::setprecision( PRICE_PRECISION ) << static_cast<price_ft>( *ptr );
      price = ss.str();

      // error = error1 | error2;
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

   void OpenWriteStream( const std::string & filename )
   {
      fstreamer.open( filename, std::ios::binary );

      if ( !fstreamer.good() )
      {
         cerr << "Aborting! Unable to open file for streaming.\n";
         assert( false );
         exit( 1 );
      }
   }

   void WriteStream()
   {
      if ( fstreamer.good() )
      {
         for ( buffer_ft v : packet )
         {
            fstreamer.write( reinterpret_cast<char *>( &v ), sizeof( buffer_ft ) );
         }

         packet.clear();
      }
   }

   void CloseWriteStream( bool & error )
   {
      WriteStream();
      error = !fstreamer.good();
      fstreamer.close();
   }

   void Save( const std::string & filename )
   {
      std::ofstream f( filename, std::ios::binary );

      if ( f.good() )
      {
         for ( buffer_ft v : packet )
         {
            f.write( reinterpret_cast<char *>( &v ), sizeof( buffer_ft ) );
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
            f.read( reinterpret_cast<char *>( &buf ), sizeof( buffer_ft ) );

            if ( !f.good() )
            {
               break;
            }

            packet.push_back( buf );
         } // while

         SetReadMode();
      }
   }

   void InflateFile( const std::string & infile,
                     const std::string & outfile,
                     bool & err )
   {
      cout << "Inflating file " << infile << " ... " << std::flush;

      // Load symbol table
      std::string symtable_file( infile + ".table" );
      SymbolTable st;
      st.LoadTable( symtable_file );
      auto symtable = st.Table();

      std::ofstream ofs( outfile );

      // Prepare inflated file for decompressing to
      if ( !ofs.is_open() )
      {
         cerr << "Error: Failed to save book to file!\n";
         err = true;
         return;
      }

      bool more;
      share_ft shares;
      std::string price;
      time_ft timereport;
      time_ft time;
      char condition;
      side_ft side;
      char ex;
      ticker_ft index;

      // Prepare reading of compressed binary file
      std::ifstream f( infile, std::ios::binary );
      std::size_t lines_read = 0;

      buffer_ft readbuffer_cache = 0;
      size_ft readbuffer_size_cache = 0;
      bool first_read = true;

      if ( f.good() )
      {
         packet.clear();
         buffer_ft buf;

         while ( true )
         {
            ++lines_read;
            mode = WRITE_MODE;
            f.read( reinterpret_cast<char *>( &buf ), sizeof( buffer_ft ) );

            if ( !f.good() )
            {
               break;
            }

            packet.push_back( buf );

            if ( lines_read > 0 && ( lines_read % 50 == 0 ) && packet.size() > 7 )
            {
               buffer_ft buffer_cache     = buffer;
               size_ft  buffer_size_cache = buffer_size;

               if ( first_read )
               {
                  SetReadMode();
                  first_read = false;
               }
               else
               {
                  mode = READ_MODE;
                  buffer = readbuffer_cache;
                  buffer_size = readbuffer_size_cache;
               }

               for ( int i = 0; i < packet.size(); ++i )
               {
                  ReadTicker( index );
                  ReadExchance( ex );
                  ReadSide( side );
                  ReadCondition( condition );
                  ReadTime( time );
                  ReadTime( timereport );
                  ReadPrice( price );
                  ReadShares( shares );

                  char sidetok[] = {'B', 'A', 'T'};

                  ofs  << symtable[index] << ","
                       << ex << ","
                       << sidetok[side] << ","
                       << condition << ","
                       << time << ","
                       << timereport << ","
                       << price << ","
                       << shares << endl;
               }

               readbuffer_cache      = buffer;
               readbuffer_size_cache = buffer_size;

               buffer      = buffer_cache;
               buffer_size = buffer_size_cache;
            }
         } // while

         mode = READ_MODE;
         buffer = readbuffer_cache;
         buffer_size = readbuffer_size_cache;

         if ( packet.size() > 0 )
         {
            do
            {
               ReadTicker( index );
               ReadExchance( ex );
               ReadSide( side );
               ReadCondition( condition );
               ReadTime( time );
               ReadTime( timereport );
               ReadPrice( price );
               more = ReadShares( shares );

               char sidetok[] = {'B', 'A', 'T'};

               ofs  << symtable[index] << ","
                    << ex << ","
                    << sidetok[side] << ","
                    << condition << ","
                    << time << ","
                    << timereport << ","
                    << price << ","
                    << shares << endl;
            }
            while ( more );
         }

      }

      err = !ofs.good();
      ofs.close();
      cout << "Inflated successfully!\n";
      cout << "Filename: " << outfile << "\n\n";
   }

   void SaveTable( const std::string & filename )
   {
      table.SaveTable( filename );
   }

   void LoadTable( const std::string & filename )
   {
      table.LoadTable( filename );
   }

};

#endif // _bat_buffer_hpp_
