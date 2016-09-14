#ifndef _symbol_table_cpp_
#define _symbol_table_cpp_

using std::cout;
using std::endl;

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

   void Add( const std::string & s )
   {
      auto r = _table.find( s );

      if ( r == _table.end() )
      {
         _table.insert( std::make_pair( s, _index++ ) );
      }
   }

   std::vector<std::string> Table()
   {
      std::priority_queue<SymbolNode> order;

      for ( auto v : _table )
      {
         order.push( SymbolNode( v.second, v.first ) );
      }

      std::vector<std::string> lookup;

      while ( !order.empty() )
      {
         auto v = order.top();
         lookup.push_back( v.symbol );
         order.pop();
      } // while

      return lookup;

   }
};

#endif // _symbol_table_cpp_
