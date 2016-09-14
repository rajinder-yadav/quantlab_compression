#ifndef _bat_buffer_hpp_
#define _bat_buffer_hpp_

class BatBuffer : public BitBuffer
{
public:

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
    * @param[in] val - String to be packed.
    *
    * @param[in] size - Count of characters to pack string into.
    *                   Count must be atlest string size.
    *
    * @return - The number of bits occupied by the string.
    */
   uint32_t WriteString( const std::string & val, const int size )
   {
      assert( size >= val.size() );
      const int pad = size - val.size();

      for ( int i = 0; i < pad; ++i )
      {
         Write( uint32_t( uint32_t( ' ' ) ), 8 );
      }

      for ( char c : val )
      {
         Write( uint32_t( c ), 8 );
      }

      return size > val.size() ? size : val.size();
   }

   uint32_t WritePrice( const std::string & val, const int size )
   {
      WriteString( val, size );
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

   std::string ReadPrice( const int count )
   {
      bool error;
      std::string s;
      uint32_t ch;

      for ( int i = 0; i < count; ++i )
      {
         Read( ch, 8, error );

         if ( ch == char( ' ' ) )
         {
            continue;
         }

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

};

#endif // _bat_buffer_hpp_
