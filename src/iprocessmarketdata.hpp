//==================================================================================================
// The MIT License (MIT)
//
// Copyright (c) 2016 Rajinder Yadav <rajinder.yadav@hotmail.com>
//==================================================================================================

#ifndef _IPROCESSMARKETDATA_HPP_
#define _IPROCESSMARKETDATA_HPP_

struct IProcessMarketData
{
   virtual void Compress( MarketData md, bool eof ) = 0;
   virtual void Save( const std::string & filename ) = 0;
   virtual void Load( const std::string & filename ) = 0;
   virtual void SaveTable( const std::string & filename ) = 0;
   virtual void LoadTable( const std::string & filename ) = 0;   
};

#endif // _IPROCESSMARKETDATA_HPP_