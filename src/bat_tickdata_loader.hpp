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
      buffer.OpenWriteStream( outfile );

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

      while ( std::getline( in_fs, s ) )
      {
         ++lines_read;
         cout << "Lines processed: " << lines_read << endl;
         MarketData md = DataParser::GetMarketData( s, err );

         if ( err )
         {
            cerr << "Aboting! Error with input line: " << lines_read << endl;
            exit( 1 );
         }


         // Ignore invalid input data
         if ( !err )
         {
            buffer.Compress( std::move( md ), false );
         }
         else
         {
            cerr << "Compression failed!\n";
            exit( 1 );
         }

         if ( lines_read > 0 && lines_read % 10 == 0 )
         {
            buffer.WriteStream();
         }
      } // while

      buffer.Compress( MarketData{}, true );
      buffer.CloseWriteStream( err );
      in_fs.close();

      std::string symtable( outfile + ".table" );
      buffer.SaveTable( symtable );
      cout << "Compressed successfully!\n";
      cout << "Filename: " << outfile << "\n\n";
   }

   static void Inflate( const std::string & infile,
                        const std::string & outfile,
                        bool & err )
   {
      BatBuffer buffer;
      buffer.InflateFile( infile, outfile, err );
   }

};

#endif // _bat_tick_data_loader_hpp_
