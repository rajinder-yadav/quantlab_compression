//==================================================================================================
// The MIT License (MIT)
//
// Copyright (c) 2016 Rajinder Yadav <rajinder.yadav@hotmail.com>
//==================================================================================================

#ifndef _bat_tick_data_loader_hpp_
#define _bat_tick_data_loader_hpp_

class BatTickLoader
{
public:

   static void Compress( const std::string & infile,
                         const std::string & outfile,
                         bool & err )
   {
      BatBuffer buffer;
      std::ifstream in_fs( infile );

      cout << "Compressing file: " << infile << " ... ";
      if ( !in_fs.is_open() )
      {
         cerr << "Error: File does not exist\n";
         err = true;
         return;
      }

      std::string s;
      std::size_t lines_read = 0;

      while ( in_fs >> s )
      {
         ++lines_read;
         cout << "Lines processed: " << lines_read << endl;
         MarketData md = DataParser::GetMarketData( s, err );

         if(err) {
            cerr << "Aboting! Error with input line: " << lines_read << endl;
            exit(1);
         }
  

         // Ignore invalid input data
         if ( !err )
         {
            buffer.Compress( std::move( md ), false );
         }
         else
         {
            cerr << "Compression failed!\n";
            exit(1);
         }
      } // while

      buffer.Compress( MarketData{}, true );

      err = !in_fs.eof();
      in_fs.close();

      std::string symtable( outfile + ".table" );
      buffer.SaveTable( symtable );
      buffer.Save( outfile );
      cout << "Compressed successfully!\n";
      cout << "Filename: " << outfile << "\n\n";
   }

   static void Inflate( const std::string & infile,
                        const std::string & outfile,
                        bool & err )
   {
      BatBuffer buffer;
      buffer.Load( infile );

      std::string symtable_file( infile + ".table" );
      SymbolTable st;
      st.LoadTable( symtable_file );
      auto symtable = st.Table();

      cout << "Inflating file " << infile << " ... ";
      std::ofstream ofs( "inflated" );

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

      do
      {
         buffer.ReadTicker( index );
         buffer.ReadExchance( ex );
         buffer.ReadSide( side );
         buffer.ReadCondition( condition );
         buffer.ReadTime( time );
         buffer.ReadTime( timereport );
         buffer.ReadPrice( price );
         more = buffer.ReadShares( shares );

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

      err = !ofs.good();
      ofs.close();
      cout << "Inflated successfully!\n";
      cout << "Filename: " << outfile << "\n\n";
   }

};

#endif // _bat_tick_data_loader_hpp_
