//==================================================================================================
// The MIT License (MIT)
//
// Copyright (c) 2016 Rajinder Yadav <rajinder.yadav@hotmail.com>
//==================================================================================================

#ifndef _symbol_node_
#define _symbol_node_

struct SymbolNode
{
   uint16_t index;
   std::string symbol;

   /**
    * @param[in] i - Index value of symbol in symbol table.
    *
    * @param[in] s - Name of symbol.
    */
   SymbolNode( uint16_t i, const std::string & s )
      : index( i )
      , symbol( s )
   {
   }
};

/**
 * Global Comparator
 */
bool operator < ( const SymbolNode & l, const SymbolNode & r )
{
   return l.index > r.index;
}

#endif // _symbol_node_
