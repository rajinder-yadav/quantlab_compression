#ifndef _bit_buffer_hpp_
#define _bit_buffer_hpp_

//#include <cassert>

class BitBuffer
{
   enum mode_e { WRITE_MODE, READ_MODE };
public:
   std::vector<uint32_t> packet;
   mode_e mode;
   uint32_t buffer;
   int buffer_size;

public:
   BitBuffer()
      : mode{WRITE_MODE}
      , buffer{}
      , buffer_size( 32 )
   {
   }

   /**
    * Prepare buffer to be written to (default).
    * In write mode buffer_size denotes space left to write into.
    */
   void SetWriteMode()
   {
      mode = WRITE_MODE;
      buffer_size = 32 - buffer_size;
      buffer << buffer_size;
   }

   /**
    * Prepare buffer to be read from.
    * In read more buffer_size denotes size of data remaining.
    */
   void SetReadMode()
   {
      mode = READ_MODE;
      buffer >> buffer_size;
      buffer_size = 32 - buffer_size;
   }

   /**
    * Pack value into 32bit buffer
    *
    * @param[in] val    - Value to be packed into buffer.
    *
    * @param[in] blocks - The bit size to use when writting value to the buffer.bits
    *                 If blocks is zero(0), the bit size will be calculated from the value.
    * @param[in] pad    - Bits to pad buffer before packing value.
    *
    * @return Status - Contain status of pack operation.bits:
    *                    Values {overflow, bits, value}
    *                    overflow - true if spilover in packing value.
    *                    bits     - Count of bits taken by value in the buffer.
    *                    value    - Zero(0) if no overflow, otherwise spilover value,
    *                               leftover bits that remain to be packed.
    *
    * Note: If value is zero(0) and bits > 0, then this become pad value for next iteration.
    * Important! Either blocks or pad MUST be zero(0).
    *
    * Padding: If block size passed, use it as # of bits to pack value in.
    *          Packed value will be zero bit padded at msb.
    *          1111 + 1bit pad -> [0|1|1|1|1] 5bits
    */
   Status pack32( const uint32_t val, const int blocks, const int pad )
   {
      // Error to write into a full buffer.
      assert( 0 < buffer_size && buffer_size <= 32 );
      // Make sure in correct mode.
      assert( mode == WRITE_MODE );

      uint32_t bits = blocks > 0 ? blocks : ceil( log2( val ) );
      bits += pad;

      // Invalid combination of value + blocks + pad
      assert( bits <= 64 );

      if ( bits <= buffer_size )
      {
         // Buffer still has space
         buffer_size -= bits;
         buffer |= val << buffer_size;
         return Status( false, bits, 0 );
      }

      // Buffer is full, ready to be written out & cleared.
      // bits will be the number of spillover bits.
      bits = bits - buffer_size;
      buffer |= val >> bits;
      uint32_t mask =  uint32_t( pow( 2, bits ) ) - 1;
      buffer_size = 0;
      return Status( true, bits, val & mask );
   }

   /**
    * Write value into packet.
    *
    * Important! Call Flush to after the last write, this will insure any remaining
    * data in the buffer is saved to the packet.bits
    *
    * @param[in] val - Interger value at most 32bits.
    *
    * @param[in] blocks - Bit size of value to write with.
    *
    * @param[in] pad - Padding in bits to use when writting.
    *
    * @return - The number of bits taken up by the value.
    */
   uint32_t Write( uint32_t val, const int blocks = 0, const int pad = 0 )
   {
      auto rv = pack32( val, blocks, pad );

      if ( rv.overflow )
      {
         packet.push_back( buffer );
         buffer = 0;
         buffer_size = 32;
         pack32( rv.val, 0 /*blocks*/, rv.val == 0 ? rv.bits : 0 );
      }
      else if ( buffer_size == 0 )
      {
         packet.push_back( buffer );
         buffer = 0;
         buffer_size = 32;
      }

      return uint32_t( ceil( log2( val ) ) );
   }

   /**
    * Write string into packet.
    *
    * @param[in] val - string to be packed.
    *
    * @return - The number of bits occupied by the string.
    */
   uint32_t Write( const std::string & val )
   {
      uint32_t bits = 0;

      for ( char c : val )
      {
         bits += Write( uint32_t( c ) );
      }

      return bits;
   }

   /**
    * Save what's remaining in the buffer to the packet.
    *
    * This MUST be called to finalize the Write operation.
    */
   void Flush()
   {
      if ( buffer_size < 32 )
      {
         packet.push_back( buffer );
         buffer = 0;
         buffer_size = 32;
      }
   }

};

#endif // _bit_buffer_hpp_
