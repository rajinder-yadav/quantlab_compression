#ifndef _symbol_table_cpp_
#define _symbol_table_cpp_

using std::cout;
using std::endl;

class SymbolTable
{
   std::map< std::string, uint16_t > _table;
   uint16_t _index;

   uint16_t _index;  
public:
   SymbolTable()
      : _table{}
      , _index{}
   {
   }

   void Add( const std::string & s )
   {
   }

   std::vector<std::string> Table()
   {
   }
};

#endif // _symbol_table_cpp_
