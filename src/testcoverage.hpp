
struct Runner : public TestRunner
{
   void RunTests()
   {
    std::string test;

      /**
       * Test class SymbolTable.
       */
      test = "Create table verify index order";
      {
         SymbolTable t;
         t.Add( "max" );
         t.Add( "top" );
         t.Add( "garden" );
         t.Add( "glass" );
         t.Add( "door" );

         auto lookup = t.Table();

         bool result = lookup.size() == 5
                       && lookup[0] == "max"
                       && lookup[1] == "top"
                       && lookup[2] == "garden"
                       && lookup[3] == "glass"
                       && lookup[4] == "door";
         check( result, test );
      }
      test = "Create table with duplicate entries, verify index order";
      {
         SymbolTable t;
         t.Add( "max" );
         t.Add( "top" );
         t.Add( "max" );
         t.Add( "garden" );
         t.Add( "top" );
         t.Add( "glass" );
         t.Add( "door" );
         t.Add( "glass" );
         t.Add( "glass" );
         t.Add( "door" );

         auto lookup = t.Table();

         bool result = lookup.size() == 5
                       && lookup[0] == "max"
                       && lookup[1] == "top"
                       && lookup[2] == "garden"
                       && lookup[3] == "glass"
                       && lookup[4] == "door";
         check( result, test );
      }
      test = "Create table different order, verify index order";
      {
         SymbolTable t;

         t.Add( "glass" );
         t.Add( "top" );
         t.Add( "garden" );
         t.Add( "max" );
         t.Add( "door" );

         auto lookup = t.Table();

         bool result = lookup.size() == 5
                       && lookup[0] == "glass"
                       && lookup[1] == "top"
                       && lookup[2] == "garden"
                       && lookup[3] == "max"
                       && lookup[4] == "door";
         check( result, test );
      }

    /**
       * Test class BitBuffer
       */
      test = "Packing zero value (1 bit)";
      {
         BitBuffer buf;
         uint32_t val = 0;
         auto b = buf.pack32( val, 0, 1 ); // 1bit including 1bit padding
         bool result = buf.buffer == 0
                       && !b.overflow
                       && b.bits == 1
                       && b.val == 0;

         check( result, test );
      }
      // 2013265920 (0b1111000000000000000000000000000) 31bits
      test = "Packing value 15 with 1bit pad, testing pad bit is on msb end of value";
      {
         BitBuffer buf;
         uint32_t val = 15;                // 4bits
         auto b = buf.pack32( val, 0, 1 ); // 5bits including 1bit padding
         bool result = buf.buffer == 2013265920
                       && !b.overflow
                       && b.bits == 5  // How many bits were packed
                       && b.val == 0;
         check( result, test );
      }            
      test = "Packing value 37 < 32bits";
      {
         BitBuffer buf;
         uint32_t val = 37;
         auto b = buf.pack32( val, 0, 0 ); // 6bits
         bool result = buf.buffer == 2483027968
                       && !b.overflow
                       && b.bits == 6
                       && b.val == 0;
         check( result, test );
      }
      test = "Packing value 37 < 32bits, fixed bits 7";
      {
         BitBuffer buf;
         uint32_t val = 37;
         auto b = buf.pack32( val, 7, 0 );  // 7bit including 1bit padding
         bool result = buf.buffer == 37 << (32-7)
                       && !b.overflow
                       && b.bits == 7
                       && b.val == 0;
         check( result, test );
      }
      test = "Packing value 2^32 == 32bits";
      {
         BitBuffer buf;
         uint32_t val = pow(2,32) - 1;
         auto b = buf.pack32( val, 0, 0 );  // 32bits
         bool result = buf.buffer == val
                       && !b.overflow
                       && b.bits == 32
                       && b.val == 0;
         check( result, test );
      }
      test = "Packing value 2^32 == 32bits, fixed bits 32";
      {
         BitBuffer buf;
         uint32_t val = pow(2,32) - 1;
         auto b = buf.pack32( val, 32, 0 );  // 32bits
         bool result = buf.buffer == val
                       && !b.overflow
                       && b.bits == 32
                       && b.val == 0;
         check( result, test );
      }      
      test = "Packing value 2^32-1 with padding of 5 bits";
      {
         // 4294945450 (0b11111111111111111010101010101010) 32bits
         BitBuffer buf;
         uint32_t pad = 5;
         uint32_t val = 4294945450;
         auto b = buf.pack32( val, 0, pad );  // 37bits including 5bits padding

         uint32_t data1 = val >> pad;
         uint32_t data2 = val & uint32_t(pow( 2, pad ) - 1);

         bool result = buf.buffer == data1
                       && b.overflow
                       && b.bits == pad
                       && b.val == data2;
         check( result, test );
      }
      test = "Packing value 2^32-1 with padding of 9 bits";
      {
         // 4294945450 (0b11111111111111111010101010101010) 32bits
         BitBuffer buf;
         uint32_t pad = 9;
         uint32_t val = 4294945450;
         auto b = buf.pack32( val, 0, pad );

         uint32_t data1 = val >> pad;
         uint32_t data2 = val & uint32_t(pow( 2, pad ) - 1);

         bool result = buf.buffer == data1
                       && b.overflow
                       && b.bits == pad
                       && b.val == data2;
         check( result, test );
      }
      test = "Packing value 2^32-1 with padding of 32 bits";
      {
         // 4294945450 (0b11111111111111111010101010101010) 32bits
         BitBuffer buf;
         uint32_t pad = 32;
         uint32_t val = 4294945450;
         auto b = buf.pack32( val, 0, pad );

         uint32_t data1 = val >> pad;
         uint32_t data2 = val & uint32_t(pow( 2, pad ) - 1);

         bool result = buf.buffer == data1
                       && b.overflow
                       && b.bits == pad
                       && b.val == data2;
         check( result, test );
      }
      test = "Packing value 6577, 2863333375 with padding of 32 bits";
      {
         // 2863333375 (0b 1010 1010 1010 1010 1111 1111 1111 1111) 32bits
         BitBuffer buf;
         uint32_t pad = 0;
         uint32_t val = 2863333375;
         auto b1 = buf.pack32( 6577, 0, pad ); // 13bits
         auto b2 = buf.pack32( val, 0, pad );  // 32bits

         uint32_t data1 = 6577 << (32-13);
         data1 = data1 | (val >> 13);
         uint32_t data2 = val & uint32_t(pow(2,13)-1);

         bool result = buf.buffer == data1
                       && b2.overflow
                       && b2.bits == 13
                       && b2.val == data2;
         check( result, test );
      }
      test = "Packing value 6577 (15bit), 2^32-1 with padding of 32 bits";
      {
         // 2863333375 (0b 1010 1010 1010 1010 1111 1111 1111 1111) 32bits
         BitBuffer buf;
         uint32_t pad = 0;
         uint32_t val = 2863333375;
         auto b1 = buf.pack32( 6577, 15, pad ); // 15bits
         auto b2 = buf.pack32( val, 0, pad );   // 32bits

         uint32_t data1 = 6577 << (32-15);
         data1 = data1 | (val >> 15);
         uint32_t data2 = val & uint32_t(pow(2,15)-1);

         bool result = buf.buffer == data1
                       && b2.overflow
                       && b2.bits == 15
                       && b2.val == data2;
         check( result, test );
      }
      test = "Packing value 442, 2^32-1 with padding of 32 bits";
      {
         // 2863333375 (0b 1010 1010 1010 1010 1111 1111 1111 1111) 32bits
         BitBuffer buf;
         uint32_t pad = 0;
         uint32_t val = 2863333375;
         auto b1 = buf.pack32( 442, 0, pad );  // 9bits
         auto b2 = buf.pack32( val, 0, pad );  // 32bits

         uint32_t data1 = 442 << (32-9);
         data1 = data1 | (val >> 9);
         uint32_t data2 = val & uint32_t(pow(2,9)-1);

         bool result = buf.buffer == data1
                       && b2.overflow
                       && b2.bits == 9
                       && b2.val == data2;
         check( result, test );
      }
      test = "Packing value 2^26-1, 43(0b1100010) to create a 32bit value";
      {
         // 61865903 (0b11 1010 1111 1111 1111 1010 1111) 26bits
         BitBuffer buf;
         uint32_t pad = 0;
         uint32_t val = 61865903;
         auto b1 = buf.pack32( val, 0, pad ); // 26bits
         auto b2 = buf.pack32( 43, 0, pad );  // 6bits

         uint32_t data1 = 61865903 << (32-26);
         data1 = data1 | 43;
         uint32_t data2 = 0;

         bool result = buf.buffer == data1
                       && !b2.overflow
                       && b2.bits == 6  // size of last pack
                       && b2.val == data2;
         check( result, test );
      }
      test = "Packing value 2^26-1, 98(0b1100010) so that 1 bit of zero value spills-over";
      {
         // 61865903 (0b11 1010 1111 1111 1111 1010 1111) 26bits
         BitBuffer buf;
         uint32_t pad = 0;
         uint32_t val = 61865903;
         auto b1 = buf.pack32( val, 0, pad ); // 26bits
         auto b2 = buf.pack32( 98, 0, pad );  // 7bits

         uint32_t data1 = 61865903 << (32-26);
         data1 = data1 | 98 >> 1;
         uint32_t data2 = 98 & 1;

         bool result = buf.buffer == data1
                       && b2.overflow
                       && b2.bits == 1
                       && b2.val == data2;
         check( result, test );
      }
      test = "Packing value 2^26-1, 816(0b1100110000) so that 1 bit of zero value spills-over";
      {
         // 61865903 (0b11 1010 1111 1111 1111 1010 1111)
         BitBuffer buf;
         uint32_t pad = 0;
         uint32_t val = 61865903;
         auto b1 = buf.pack32( val, 0, pad ); // 26bits
         auto b2 = buf.pack32( 816, 0, pad ); // 10bits

         uint32_t data1 = val << (32-26);
         data1 = data1 | 816 >> (10 - (32-26));
         uint32_t data2 = 816 & uint32_t(pow(2,4)-1);

         bool result = buf.buffer == data1
                       && b2.overflow
                       && b2.bits == 4
                       && b2.val == data2;
         check( result, test );
      }
      test = "Write string 'Hello' into buffer";
      {
         BitBuffer buf;
         buf.Write( "Hello" );
         buf.Flush();
         // H = 72   7bit
         // e = 101  7bit
         // l = 108  7bit
         // l = 108  7bit
         // o = 111  7bit

         uint32_t n = 72;
         n = ( n << 7 ) | 101;
         n = ( n << 7 ) | 108;
         n = ( n << 7 ) | 108;
         // 4bit left in buffer
         n = ( n << 4 ) | ( 111 >> 3 );

         bool result = buf.packet[0] == n
                       && buf.packet[1] == ( 111 & 7 ) << ( 32 - 3 );
         check( result, test );
      }           
   }
};
