#ifndef __status_hpp__
#define __status_hpp__

using std::cout;
using std::endl;
/**
 * This class returns status info by class BitBuffer.
 */
struct Status
{
   bool overflow;    // Underflow|Overflow
   int bits;         // Bits packed|unpacked
   uint32_t val;     // Value spillover(write), read

   /**
    * Create status value
    * @param[in] f - Underflow for readm, Overflow for write.
    * @param[in] b - Bits written (spilled if overflow) or bits read.
    * @param[in] v - Value to write or read.
    */
   Status( bool f, int b, uint32_t v )
      : overflow( f )
      , bits( b )
      , val( v )
   {
   }

   /**
    * Debug helper
    */
   void Dump() const
   {
      cout << "overflow " << std::boolalpha << overflow
           << " bits "    << std::setw( 2 ) << bits
           << " val "     << val            << endl;
   }
};

#endif // __status_hpp__
