//==================================================================================================
// The MIT License (MIT)
//
// Copyright (c) 2016 Rajinder Yadav <rajinder.yadav@hotmail.com>
//==================================================================================================

#ifndef _bit_buffer_hpp_
#define _bit_buffer_hpp_

//#include <cassert>

class BitBuffer : public IProcessMarketData
{
public:
   enum mode_e { WRITE_MODE, READ_MODE };

   std::deque<buffer_ft> packet;
   mode_e mode;
   buffer_ft buffer;
   size_ft buffer_size;

public:
   BitBuffer()
      : mode{WRITE_MODE}
      , buffer{}
      , buffer_size( BUFFER_CAPACITY )
   {
   }

   // Interface method
   void Compress( MarketData md, bool eof )
   {
   }
   // Interface method
   void Save( const std::string & filename )
   {
   }
   // Interface method
   void Load( const std::string & filename )
   {
   }
   // Interface method
   void SaveTable( const std::string & filename )
   {
   }
   // Interface method
   void LoadTable( const std::string & filename )
   {
   }

   /**
    * Prepare buffer to be written to (default).
    * In write mode buffer_size denotes space left to write into.
    * (!) Avoid making use of this call unless you understand what you're doing!
    */
   void SetWriteMode()
   {
      mode = WRITE_MODE;

      if ( buffer != 0 && buffer_size == BUFFER_CAPACITY )
      {
         packet.push_back( buffer );
         buffer = 0;
         buffer_size = BUFFER_CAPACITY;
      }
      else
      {
         buffer_size = BUFFER_CAPACITY - buffer_size;
         buffer = buffer << buffer_size;
      }
   }

   /**
    * Prepare buffer to be read from.
    * In read mode buffer_size denotes size of data remaining.
    * (!) Buffer must have been flushed before this call. It is an error not to do so first.
    */
   void SetReadMode()
   {
      mode = READ_MODE;
      buffer = packet.front();
      packet.pop_front();
      buffer_size = BUFFER_CAPACITY;
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
   Status pack32( const buffer_ft val, const size_ft blocks )
   {
      // Error to write into a full buffer.
      assert( 0 < buffer_size && buffer_size <= BUFFER_CAPACITY );
      // Make sure in correct mode.
      assert( mode == WRITE_MODE );

      assert( blocks <= 2 * BUFFER_CAPACITY );

      size_ft bits = blocks;

      if ( bits <= buffer_size )
      {
         // Buffer has not overflowed, may be full.
         buffer_size -= bits;
         buffer |= val << buffer_size;
         return Status( false, 0, 0 );
      }

      // Buffer is full, ready to be written out & cleared.
      // bits will be the number of spillover bits.
      bits = bits - buffer_size;

      // BUG FIX : right-shift operator does not left fill with zero if bit size = base type
      // This bug will be implementation specific and is classified as undefined in C++
      if ( bits == 32 )
      {
         buffer = 0;
      }
      else
      {
         // Spill & merge, buffer is now full.
         buffer |= val >> bits;
      }

      // Pick up the spill
      buffer_ft mask =  uint32_t( pow( 2, bits ) ) - 1;
      buffer_size = 0;
      // The spill and its bit count.
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
    * Note: Buffer must be in read ready state, the buffer should have been
    *       flushed once.
    *
    *       (!) Uses SetReadMode() to prepare buffer for reading.
    */
   Status unpack32( buffer_ft & val, const size_ft bitsize )
   {
      // Error to read empty buffer.
      assert( 0 < buffer_size );
      // Make sure in correct mode.
      assert( mode == READ_MODE );

      size_ft bits = bitsize;

      if ( bits <= buffer_size )
      {
         buffer_ft mask = uint32_t( pow( 2, bits ) - 1 );
         mask = mask << ( 32 - bits );
         val = ( buffer & mask ) >> ( 32 - bits );
         buffer_size -= bits;

         // Bug with left shift operator for shift equal to bits of value. 
         if ( bits == 32 )
         {
            buffer = 0;
         }
         else
         {
            buffer = buffer << bits;
         }

         return Status( false, 0, 0 );
      }

      // Data cleared out of buffer, buffer is empty.
      bits -= buffer_size;
      val = buffer >> ( 32 - buffer_size );
      buffer_size = 0;
      buffer = 0;
      // Partial value along with bits remaining to be read once buffer is set with next data.
      return Status( true, bits, val );
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
   uint32_t Write( buffer_ft val, const size_ft blocks )
   {
      // Buffer is full and needs to be written out to the packet.
      if ( buffer_size == 0 )
      {
         packet.push_back( buffer );
         buffer = 0;
         buffer_size = BUFFER_CAPACITY;
      }

      auto rv = pack32( val, blocks );

      // Save full buffer to packet, clear out and write out spilled data to buffer.
      if ( rv.overflow )
      {
         packet.push_back( buffer );
         buffer = 0;
         buffer_size = BUFFER_CAPACITY;
         pack32( rv.val, rv.bits );
      }

      if ( buffer_size == 0 )
      {
         packet.push_back( buffer );
         buffer = 0;
         buffer_size = BUFFER_CAPACITY;
      }

      return blocks;
   }

   bool Read( uint32_t & val, const size_ft bitsize, bool & error )
   {
      // Alert caller of potential bug for reading from empty buffer.
      assert( packet.size() > 0 || buffer_size > 0 );

      if ( buffer_size == 0 )
      {
         assert( packet.size() > 0 );
         buffer = packet.front();
         packet.pop_front();
         buffer_size = BUFFER_CAPACITY;
      }

      auto status = unpack32( val, bitsize );

      if ( status.overflow )
      {
         assert( packet.size() > 0 );
         buffer = packet.front();
         packet.pop_front();
         buffer_size = BUFFER_CAPACITY;
         uint32_t tmp;
         unpack32( tmp, status.bits );
         val = ( val << status.bits ) | tmp;
      }

      return ( packet.size() > 0 || buffer != 0 );
   }

   /**
    * Save what's remaining in the buffer to the packet.
    *
    * This MUST be called only once to finalize the Write operation.
    */
   void Flush()
   {
      if ( buffer_size == 32 )
      {
         return;
      }

      packet.push_back( buffer );

      // We will need this when we start to read and have to prepare the buffer for the first time!
      //TODOpacket.push_back( buffer_size );
      buffer = 0;
      buffer_size = BUFFER_CAPACITY;
   }

};

#endif // _bit_buffer_hpp_
