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

      if ( buffer != 0 && buffer_size == 32 )
      {
         packet.push_back( buffer );
         buffer = 0;
         buffer_size = 32;
      }
      else
      {
         buffer_size = 32 - buffer_size;
         buffer = buffer << buffer_size;
      }
   }

   /**
    * Prepare buffer to be read from.
    * In read more buffer_size denotes size of data remaining.
    */
   void SetReadMode()
   {
      mode = READ_MODE;
      buffer_size = packet.back();
      packet.pop_back();

      buffer = packet.back();
      packet.pop_back();

      buffer = buffer >> buffer_size;
      buffer_size = 32 - buffer_size;

      if ( buffer_size == 0 )
      {
         buffer = packet.back();
         packet.pop_back();
         buffer_size = 32;
      }
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
   Status pack32( const uint32_t val, const int blocks )
   {
      // Error to write into a full buffer.
      assert( 0 < buffer_size && buffer_size <= 32 );
      // Make sure in correct mode.
      assert( mode == WRITE_MODE );

      uint32_t bits = blocks;

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

      // BUG FIX : right-shift operator does not left fill with zero is bit size = base type
      if ( bits == 32 )
      {
         buffer = 0;
      }
      else
      {
         buffer |= val >> bits;
      }

      uint32_t mask =  uint32_t( pow( 2, bits ) ) - 1;
      buffer_size = 0;
      return Status( true, bits, val & mask );
   }

   /**
    * Unpack value from buffer given bit size.
    *
    * @input[out] val -Fetched value register.
    *
    * @input[in] bitsize - Bit size of value to unpack.
    *
    * @return Status - Contain status of unpack operation.bits:
    *                    Values {overflow, bits, value}
    *                    overflow - true if underflow, mean more bits to be read.
    *                    bits     - Count of bits remaining to be read.
    *                    value    - Value read, will be partial if underflow true.
    *
    * Note: Buffer must be in read ready state, what this mean is empty space
    *       in the buffer must be on the msb side. When data is being packed
    *       the empty space will be on the lsb side, so buffer must be right-shifted
    *       before unpack operation!
    *
    *       (!) Uses SetReadMode() to prepare buffer for reading.
    *
    *               <not ready>        <ready>
    *  Buffer =>  [datat|......]  => [......|data]
    *               msb | lsb          msb  |lsb
    */
   Status unpack32( uint32_t & val, const int bitsize )
   {
      // Error to read empty buffer.
      assert( 0 < buffer_size );
      // Make sure in correct mode.
      assert( mode == READ_MODE );

      uint32_t bits = bitsize;

      if ( bits <= buffer_size )
      {
         val = buffer & uint32_t( pow( 2, bits ) - 1 );
         buffer_size -= bits;
         buffer = buffer >> bits;
         return Status( false, bitsize, val );
      }

      bits -= buffer_size;
      val = buffer;
      buffer_size = 0;
      buffer = 0;
      return Status( true, bits, val );
   }

   bool Read( uint32_t & val, const int bitsize, bool & error )
   {
      // Alert caller of potential bug for reading from empty buffer.
      assert( packet.size() > 0 || buffer_size > 0 );
      assert( mode == READ_MODE );

      uint32_t size = buffer_size;
      auto status = unpack32( val, bitsize );

      if ( status.overflow )
      {
         assert( packet.size() > 0 );

         if ( packet.size() == 0 )
         {
            error = true;
            return false;
         }

         buffer = packet.back();
         buffer_size = 32;
         packet.pop_back();
         uint32_t tmp;
         unpack32( tmp, status.bits );
         val |= tmp << size;
      }

      return ( packet.size() > 0 || buffer_size > 0 );
   }
   std::string ReadString( const int count )
   {
      bool error;
      std::string s;
      uint32_t ch;

      for ( int i = 0; i < count; ++i )
      {
         Read( ch, 8, error );
         s = char( ch ) + s;
      }

      return s;
   }

   bool ReadSize( uint32_t & val )
   {
      bool error;
      Read( val, 14, error );
      return error;
   }

   bool ReadTime( uint64_t & val )
   {
      bool error;
      uint32_t tmp;

      Read( tmp, 32, error );
      val = tmp;
      Read( tmp, 5, error );
      uint64_t hi = tmp;
      val = ( hi << 32 ) | val;
      return error;
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
   uint32_t Write( uint32_t val, const int blocks )
   {
      auto rv = pack32( val, blocks );

      if ( rv.overflow )
      {
         packet.push_back( buffer );
         buffer = 0;
         buffer_size = 32;
         pack32( rv.val, rv.bits );
      }
      else if ( buffer_size == 0 )
      {
         packet.push_back( buffer );
         buffer = 0;
         buffer_size = 32;
      }

      return blocks;
   }

   uint32_t WriteTime( uint64_t val )
   {
      // Mico-sec since midnight (24 hrs = 8.64e+10 = 37bits)
      uint32_t hi = val >> 32;
      uint32_t lo = static_cast<uint32_t>( val );
      Write( hi, 5 );
      Write( lo, 32 );
      return 37;
   }
   uint32_t WriteSize( uint32_t val )
   {
      // Max 10000 (14bits) ???
      Write( val, 14 );
      return 14;
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
         bits += Write( uint32_t( c ), 8 );
      }

      return bits / 8; // chars
   }

   /**
    * Save what's remaining in the buffer to the packet.
    *
    * This MUST be called only once to finalize the Write operation.
    */
   void Flush()
   {
      packet.push_back( buffer );

      // We will need this when we start to read and have to prepare the buffer for the first time!
      packet.push_back( buffer_size );
      buffer = 0;
      buffer_size = 32;
   }

};

#endif // _bit_buffer_hpp_
