#ifndef _symbol_node_
#define _symbol_node_

struct SymbolNode
{
   uint16_t index;
   std::string symbol;

   SymbolNode( uint16_t i, const std::string & s )
      : index( i )
      , symbol( s )
   {
   }
};

bool operator < ( const SymbolNode & l, const SymbolNode & r )
{
   return l.index > r.index;
}

#endif // _symbol_node_
