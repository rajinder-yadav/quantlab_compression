#ifndef __status_hpp__
#define __status_hpp__

using std::cout;
using std::endl;

struct Status
{
   bool overflow;
   int bits;
   uint32_t val;

   Status( bool f, int b, uint32_t v )
      : overflow( f )
      , bits( b )
      , val( v )
   {
   }

   void Dump() const
   {
      cout << "overflow " << std::boolalpha << overflow
           << " bits "    << std::setw( 2 ) << bits
           << " val "     << val            << endl;
   }
};

#endif // __status_hpp__
