//==================================================================================================
// The MIT License (MIT)
//
// Copyright (c) 2016 Rajinder Yadav <rajinder.yadav@hotmail.com>
//==================================================================================================

#ifndef _symbol_table_cpp_
#define _symbol_table_cpp_

using std::cout;
using std::endl;
using std::cerr;

class SymbolTable
{
   std::map< std::string, uint16_t > _table;
   uint16_t _index;

public:
   SymbolTable()
      : _table{}
      , _index{}
   {
   }

   uint16_t Add( const std::string & s )
   {
      auto r = _table.find( s );

      if ( r == _table.end() )
      {
         _table.insert( std::make_pair( s, _index++ ) );
         return _index - 1;
      }

      return r->second;
   }

   std::deque<std::string> Table()
   {
      std::priority_queue<SymbolNode> order;

      for ( auto v : _table )
      {
         order.push( SymbolNode( v.second, v.first ) );
      }

      std::deque<std::string> lookup;

      while ( !order.empty() )
      {
         auto v = order.top();
         lookup.push_back( v.symbol );
         order.pop();
      } // while

      return lookup;
   }

   void LoadTable( const std::string & filename )
   {
      std::ifstream in_fs( filename );

      if ( !in_fs.is_open() )
      {
         cerr << "Error: File does not exist\n";
         return;
      }

      std::string s;
      std::size_t lines_read = 0;

      _table.clear();
      _index = 0;

      while ( std::getline( in_fs, s ) )
      {
         Add( s );
      }
   }

   void SaveTable( const std::string & filename )
   {
      std::ofstream ofs( filename );

      if ( !ofs.is_open() )
      {
         cerr << "Error: Failed to save book to file!\n";
         return;
      }

      auto symtable = Table();
      for ( auto v : symtable )
      {
         ofs << v << "\n";
      }

      ofs.close();
   }
};

#endif // _symbol_table_cpp_
