#ifndef _bit_buffer_hpp_
#define _bit_buffer_hpp_

//#include <cassert>

class BitBuffer
{
public:
   std::vector<uint32_t> packet;
   uint32_t buffer;
   int buffer_size;

public:
   BitBuffer()
      : buffer{}
      , buffer_size( 32 )
   {
   }

   /**
    * Pack value into 32bit buffer
    *
    * @param val    - Value to be packed into buffer.
    * @param blocks - The bit size to use when writting value to the buffer.bits
    *                 If blocks is zero(0), the bit size will be calculated from the value.
    * @param pad    - Bits to pad buffer before packing value.
    * @param reset  - Clear out buffer.
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
    */
   Status pack32( const uint32_t val, const int blocks, const int pad )
   {
   }

   /**
    * Write value into packet.
    *
    * Important! Call Flush to after the last write, this will insure any remaining
    * data in the buffer is saved to the packet.bits
    *
    * @input val - Interger value at most 32bits.
    */
   void Write( uint32_t val, const int blocks = 0, const int pad = 0 )
   {
   }

   void Write( const std::string & val ) {
   }

   /**
    * Save what's remaining in the buffer to the packet.
    *
    * This MUST be called to finalize the Write operation.
    */
   void Flush()
   {
   }

};

#endif // _bit_buffer_hpp_
