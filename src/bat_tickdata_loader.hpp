#ifndef _bat_tick_data_loader_hpp_
#define _bat_tick_data_loader_hpp_

class BatTickLoader
{
public:

   static void CompressData( IProcessMarketData & buffer,
                             const std::string & infile,
                             const std::string & outfile,
                             bool & err )
   {
      std::ifstream in_fs( infile );

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
         MarketData md = DataParser::GetMarketData( s, err );

         // Ignore invalid input data
         if ( !err )
         {
            buffer.Compress( std::move( md ), false );
         }
      } // while

      buffer.Compress( MarketData{}, true );

      err = !in_fs.eof();
      in_fs.close();

      buffer.Save( outfile );
      std::string symtable(outfile+".table");
      buffer.SaveTable(symtable);
   }

   static void Inflate( IProcessMarketData & buffer,
                        const std::string & infile,
                        const std::string & outfile,
                        bool & err )
   {
      buffer.Load(infile);
      std::string symtable(infile+".table");
      buffer.LoadTable(symtable);

   }

};

#endif // _bat_tick_data_loader_hpp_
