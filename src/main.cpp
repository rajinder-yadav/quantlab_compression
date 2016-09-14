//==================================================================================================
// The MIT License (MIT)
//
// Copyright (c) 2016 Rajinder Yadav <rajinder.yadav@hotmail.com>
//==================================================================================================

#include <iostream>
#include <iomanip>
#include <map>
#include <deque>
#include <queue>
#include <string>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cassert>
#include <regex>
#include <iterator>

#include "fieldtypes.hpp"
#include "fieldsizes.hpp"
#include "symbolnode.hpp"
#include "symboltable.hpp"
#include "status.hpp"
#include "marketdata.hpp"
#include "iprocessmarketdata.hpp"
#include "bitbuffer.hpp"
#include "batbuffer.hpp"
#include "dataparser.hpp"
#include "bat_tickdata_loader.hpp"

#include "testrunner.hpp"
#include "testcoverage.hpp"

using std::clog;
using std::cout;
using std::endl;

#define TEST
#define COMPRESS

/**
 *  (!) Make sure to name the test input file 'testdata'.
 *  (!) This is the file to be compressed.
 *
 * 1) Compressed file is called:   "compressed".
 * 2) Lookup table file is called: "compressed.table".
 * 3) Uncompressed file is called: "inflated".
 *
 * (!) Both files #1 & #2 are required to uncompress the file.
 *
 * Program Arguments: quantlab_compression <filename> <mode>
 *
 * <mode> must be one of:
 *
 * c = Compress file.
 * i = Inflate compressed file.
 * b = Compress then inflate.
 * t = Run tests.
 * a = Run all, Test, Compress, Inflate.
 *
 */
int main( int argc, char * argv[] )
{
   if ( argc < 3 )
   {
      cout << "\nAuthor: Rajinder Yadav <rajinder.yadav@hotmail.com>\n\n"
           << "Program Arguments: quantlab_compression <filename> <mode>\n\n"
           << "<mode> must be one of:\n\n"
           << "Pass single argument to run the following:\n\n"
           << "  c = Compress file.\n"
           << "  i = Inflate compressed file.\n"
           << "  b = Compress than inflate.\n"
           << "  t = Run tests.\n"
           << "  a = Run all, Test, Compress, Inflate.\n\n";
      return 0;
   }

   bool error;

   const char c = argv[2][0];

   if ( c == 't' || c == 'a' )
   {
      // Run tests
      Runner tr;
      tr.RunTests();
   }

   if ( c == 'c' || c == 'b' || c == 'a' )
   {
      std::string filename(argv[1]);
      BatTickLoader::Compress( filename, "compressed", error );
   }

   if ( c == 'i' || c == 'b' || c == 'a' )
   {
      BatTickLoader::Inflate( "compressed", "inflated", error );
   }

   return 0;
}
