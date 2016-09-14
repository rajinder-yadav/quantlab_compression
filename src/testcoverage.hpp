
struct Runner : public TestRunner
{
   void RunTests()
   {
      std::string test;

      /**
       * Test class SymbolTable
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
       * Test class BatBuffer
       */
      test = "Packing zero value (1 bit)";
      {
         BatBuffer buf;
         uint32_t val = 0;
         auto b = buf.pack32( val, 1 ); // 1bit including 1bit padding
         bool result = buf.buffer == 0
                       && !b.overflow
                       && b.bits == 1
                       && b.val == 0;

         check( result, test );
      }
      test = "Packing value 15 with 1bit pad, testing pad bit is on msb end of value";
      {
         BatBuffer buf;
         uint32_t val = 15;             // 4bits
         auto b = buf.pack32( val, 5 ); // 5bits including 1bit padding
         bool result = buf.buffer == 2013265920
                       && !b.overflow
                       && b.bits == 5  // How many bits were packed
                       && b.val == 0;
         check( result, test );
      }
      test = "Packing value 37 < 32bits";
      {
         BatBuffer buf;
         uint32_t val = 37;             // 6bits
         auto b = buf.pack32( val, 6 ); // 6bits
         bool result = buf.buffer == 2483027968
                       && !b.overflow
                       && b.bits == 6
                       && b.val == 0;
         check( result, test );
      }
      test = "Packing value 37 < 32bits, fixed bits 7";
      {
         BatBuffer buf;
         uint32_t val = 37;              // 6bits
         auto b = buf.pack32( val, 7 );  // 7bits including 1bit padding
         bool result = buf.buffer == 37 << ( 32 - 7 )
                       && !b.overflow
                       && b.bits == 7
                       && b.val == 0;
         check( result, test );
      }
      test = "Packing value 2^32 == 32bits";
      {
         BatBuffer buf;
         uint32_t val = pow( 2, 32 ) - 1; // 32bits
         auto b = buf.pack32( val, 32 );  // 32bits
         bool result = buf.buffer == val
                       && !b.overflow
                       && b.bits == 32
                       && b.val == 0;
         check( result, test );
      }
      test = "Packing value 2^32 == 32bits, fixed bits 32";
      {
         BatBuffer buf;
         uint32_t val = pow( 2, 32 ) - 1; // 32bits
         auto b = buf.pack32( val, 32 );  // 32bits
         bool result = buf.buffer == val
                       && !b.overflow
                       && b.bits == 32
                       && b.val == 0;
         check( result, test );
      }
      test = "Packing value 4294945450 with padding of 5 bits";
      {
         // 4294945450 (0b11111111111111111010101010101010) 32bits
         BatBuffer buf;
         uint32_t val = 4294945450;       // 32bits
         auto b = buf.pack32( val, 37 );  // 37bits including 5bits padding

         uint32_t data1 = val >> 5;
         uint32_t data2 = val & uint32_t( pow( 2, 5 ) - 1 );

         bool result = buf.buffer == data1
                       && b.overflow
                       && b.bits == 5
                       && b.val == data2;
         check( result, test );
      }
      test = "Packing value 4294945450 with padding of 9 bits";
      {
         // 4294945450 (0b11111111111111111010101010101010) 32bits
         BatBuffer buf;
         uint32_t val = 4294945450;      // 32bits
         auto b = buf.pack32( val, 41 ); // 41bits including 9bits padding

         uint32_t data1 = val >> 9;
         uint32_t data2 = val & uint32_t( pow( 2, 9 ) - 1 );

         bool result = buf.buffer == data1
                       && b.overflow
                       && b.bits == 9
                       && b.val == data2;
         check( result, test );
      }
      test = "Packing value 4294945450 with padding of 32 bits";
      {
         // 4294945450 (0b11111111111111111010101010101010) 32bits
         BatBuffer buf;
         uint32_t val = 4294945450;      // 32bits
         auto b = buf.pack32( val, 64 ); // 64bits including 9bits padding

         uint32_t data1 = 0;
         uint32_t data2 = 4294945450;

         bool result = buf.buffer == data1
                       && b.overflow
                       && b.bits == 32
                       && b.val == data2;
         check( result, test );
      }
      test = "Packing value 6577, 2863333375 with padding of 32 bits";
      {
         // 2863333375 (0b 1010 1010 1010 1010 1111 1111 1111 1111) 32bits
         BatBuffer buf;
         uint32_t val = 2863333375;
         auto b1 = buf.pack32( 6577, 13 ); // 13bits
         auto b2 = buf.pack32( val, 32 );  // 32bits

         uint32_t data1 = 6577 << ( 32 - 13 );
         data1 = data1 | ( val >> 13 );
         uint32_t data2 = val & uint32_t( pow( 2, 13 ) - 1 );

         bool result = buf.buffer == data1
                       && b2.overflow
                       && b2.bits == 13
                       && b2.val == data2;
         check( result, test );
      }
      test = "Packing value 6577 (15bit), 2863333375 with padding of 32 bits";
      {
         // 2863333375 (0b 1010 1010 1010 1010 1111 1111 1111 1111) 32bits
         BatBuffer buf;
         uint32_t val = 2863333375;
         auto b1 = buf.pack32( 6577, 15 ); // 15bits
         auto b2 = buf.pack32( val, 32 );  // 32bits

         uint32_t data1 = 6577 << ( 32 - 15 );
         data1 = data1 | ( val >> 15 );
         uint32_t data2 = val & uint32_t( pow( 2, 15 ) - 1 );

         bool result = buf.buffer == data1
                       && b2.overflow
                       && b2.bits == 15
                       && b2.val == data2;
         check( result, test );
      }
      test = "Packing value 442, 2863333375 with padding of 32 bits";
      {
         // 2863333375 (0b 1010 1010 1010 1010 1111 1111 1111 1111) 32bits
         BatBuffer buf;
         uint32_t val = 2863333375;
         auto b1 = buf.pack32( 442, 9 );  // 9bits
         auto b2 = buf.pack32( val, 32 );  // 32bits

         uint32_t data1 = 442 << ( 32 - 9 );
         data1 = data1 | ( val >> 9 );
         uint32_t data2 = val & uint32_t( pow( 2, 9 ) - 1 );

         bool result = buf.buffer == data1
                       && b2.overflow
                       && b2.bits == 9
                       && b2.val == data2;
         check( result, test );
      }
      test = "Packing value 61865903, 43(0b1100010) to create a 32bit value";
      {
         // 61865903 (0b11 1010 1111 1111 1111 1010 1111) 26bits
         BatBuffer buf;
         uint32_t val = 61865903;
         auto b1 = buf.pack32( val, 26 ); // 26bits
         auto b2 = buf.pack32( 43, 6 );   // 6bits

         uint32_t data1 = 61865903 << ( 32 - 26 );
         data1 = data1 | 43;
         uint32_t data2 = 0;

         bool result = buf.buffer == data1
                       && !b2.overflow
                       && b2.bits == 6  // size of last pack
                       && b2.val == data2;
         check( result, test );
      }
      test = "Packing value 61865903, 98(0b1100010) so that 1 bit of zero value spills-over";
      {
         // 61865903 (0b11 1010 1111 1111 1111 1010 1111) 26bits
         BatBuffer buf;
         uint32_t val = 61865903;
         auto b1 = buf.pack32( val, 26 ); // 26bits
         auto b2 = buf.pack32( 98, 7 );   // 7bits

         uint32_t data1 = 61865903 << ( 32 - 26 );
         data1 = data1 | 98 >> 1;
         uint32_t data2 = 98 & 1;

         bool result = buf.buffer == data1
                       && b2.overflow
                       && b2.bits == 1
                       && b2.val == data2;
         check( result, test );
      }
      test = "Packing value 61865903, 816(0b1100110000) so that 1 bit of zero value spills-over";
      {
         // 61865903 (0b11 1010 1111 1111 1111 1010 1111)
         BatBuffer buf;
         uint32_t val = 61865903;
         auto b1 = buf.pack32( val, 26 ); // 26bits
         auto b2 = buf.pack32( 816, 10 ); // 10bits

         uint32_t data1 = val << ( 32 - 26 );
         data1 = data1 | 816 >> ( 10 - ( 32 - 26 ) );
         uint32_t data2 = 816 & uint32_t( pow( 2, 4 ) - 1 );

         bool result = buf.buffer == data1
                       && b2.overflow
                       && b2.bits == 4
                       && b2.val == data2;
         check( result, test );
      }
      test = "Write string 'Hello' into buffer";
      {
         BatBuffer buf;
         buf.WriteString( "Hello", 5 );
         buf.Flush();
         // H = 72   8bit
         // e = 101  8bit
         // l = 108  8bit
         // l = 108  8bit
         // o = 111  8bit

         uint32_t n = 72;
         n = ( n << 8 ) | 101;
         n = ( n << 8 ) | 108;
         n = ( n << 8 ) | 108;

         bool result = buf.packet[0] == n
                       && buf.packet[1] == 111 << ( 32 - 8 );
         check( result, test );
      }
      test = "Switching read/write modes with 11bit value";
      {
         BatBuffer buf;
         uint32_t val = 1995; // 11bits
         buf.Write( val, 11 );
         buf.Flush();
         buf.SetReadMode();
         uint32_t data;
         bool error;
         buf.Read( data, 11, error );

         bool result = data == val && !error;
         check( result, test );
      }
      test = "Switching read/write modes with zero value using 11bit";
      {
         BatBuffer buf;
         uint32_t val = 0;
         buf.Write( val, 11 );
         buf.Flush();
         buf.SetReadMode();
         uint32_t data;
         bool error;
         buf.Read( data, 11, error );

         bool result = data == val && !error;
         check( result, test );
      }
      test = "Switching read/write modes with 32bit value";
      {
         BatBuffer buf;
         uint32_t val = 2863333375; // 11bits
         buf.Write( val, 32 );
         buf.Flush();
         buf.SetReadMode();
         uint32_t data;
         bool error;
         buf.Read( data, 32, error );

         bool result = data == val && !error;
         check( result, test );
      }
      test = "Switching read/write modes with zero value using 32bit";
      {
         BatBuffer buf;
         uint32_t val = 0;
         uint32_t bufval = 0;
         buf.Write( val, 32 );
         buf.Flush();
         buf.SetReadMode();
         uint32_t data;
         bool error;
         buf.Read( data, 32, error );

         bool result = data == val && !error;
         check( result, test );
      }
      test = "Read two values from buffer 13, 220";
      {
         BatBuffer buf;
         buf.Write( 13, 4 );     // 4bits
         buf.Write( 220, 8 );    // 8bits
         buf.Flush();
         buf.SetReadMode();
         uint32_t val1;
         uint32_t val2;
         bool error;
         buf.Read( val1, 8, error );
         buf.Read( val2, 4, error );

         bool result = val1 == 220
                       && val2 == 13;
         check( result, test );
      }
      test = "Read 5 values from buffer";
      {
         BatBuffer buf;
         buf.Write( 220, 8 );    // 8bits
         buf.Write( 13, 4 );     // 4bits
         buf.Write( 1978, 11 );  // 11bits
         buf.Write( 0, 2 );      // 2bits
         buf.Write( 82, 7 );     // 7bits
         buf.Flush();
         buf.SetReadMode();
         uint32_t val[4];
         bool error;
         bool more1 = buf.Read( val[0], 7, error );
         bool more2 = buf.Read( val[1], 2, error );
         bool more3 = buf.Read( val[2], 11, error );
         bool more4 = buf.Read( val[3], 4, error );
         bool more5 = buf.Read( val[4], 8, error );

         bool result = val[0] == 82
                       && val[1] == 0
                       && val[2] == 1978
                       && val[3] == 13
                       && val[4] == 220
                       && more1 && more2 && more3 && more4 && !more5;
         check( result, test );
      }
      test = "BAT entry 1";
      {
         BatBuffer buf;
         uint32_t s1 = buf.WriteTicker( 1 );      // index
         uint32_t s2 = buf.WriteExchange( 'q' );   // ex
         uint32_t s3 = buf.WriteSide( 3 );         // side B,A,T
         uint32_t s4 = buf.WriteCondition( 'O' );  // condition
         uint32_t s5 = buf.WriteTime( 42180525828 );  // time
         uint32_t s6 = buf.WriteTime( 42180526502 );  // time-report
         uint32_t s7 = buf.WritePrice( "13.06" );     // price
         uint32_t s8 = buf.WriteShares( 730 );        // shares
         buf.Flush();
         buf.SetReadMode();

         bool error;
         share_ft shares;
         buf.ReadShares( shares );
         cout << shares << "\n";

         std::string price;
         buf.ReadPrice( price );
         cout << price << endl;

         time_ft timereport;
         buf.ReadTime( timereport );
         cout << timereport << "\n";

         time_ft time;
         buf.ReadTime( time );
         cout << time << "\n";

         char condition;
         buf.ReadCondition( condition );
         cout << condition << "\n";

         side_ft side;
         buf.ReadSide( side );
         cout << long( side ) << "\n";

         char ex;
         buf.ReadExchance( ex );
         cout << ex << "\n";

         ticker_ft index;
         buf.ReadTicker( index );
         cout << index << "\n";
      }
      test = "BAT entry 2";
      {
         BatBuffer buf;
         uint32_t s1 = buf.WriteTicker( 3 );            // index
         uint32_t s2 = buf.WriteExchange( 'r' );     // ex
         uint32_t s3 = buf.WriteSide( 3 );             // side B,A,T
         uint32_t s4 = buf.WriteCondition( 'O' );     // condition
         uint32_t s5 = buf.WriteTime( 42180525828 );  // time
         uint32_t s6 = buf.WriteTime( 42180526502 );  // time-report
         uint32_t s7 = buf.WritePrice( "12.56" );     // price
         uint32_t s8 = buf.WriteShares( 1102 );       // size

         uint32_t r1 = buf.WriteTicker( 12 );           // index
         uint32_t r2 = buf.WriteExchange( 'm' );     // ex
         uint32_t r3 = buf.WriteSide( 1 );             // ride B,A,T
         uint32_t r4 = buf.WriteCondition( 'C' );     // condition
         uint32_t r5 = buf.WriteTime( 42180526503 );  // time
         uint32_t r6 = buf.WriteTime( 42180526672 );  // time-report
         uint32_t r7 = buf.WritePrice( "3.09" );      // price
         uint32_t r8 = buf.WriteShares( 243 );        // size
         buf.Flush();
         buf.SetReadMode();

         bool error;
         uint32_t size;

         for ( int i = 0; i < 2; ++i )
         {
            share_ft shares;
            buf.ReadShares( shares );
            cout << shares << "\n";

            std::string price;
            buf.ReadPrice( price );
            cout << price << endl;

            time_ft timereport;
            buf.ReadTime( timereport );
            cout << timereport << "\n";

            time_ft time;
            buf.ReadTime( time );
            cout << time << "\n";

            char condition;
            buf.ReadCondition( condition );
            cout << condition << "\n";

            side_ft side;
            buf.ReadSide( side );
            cout << long( side ) << "\n";

            char ex;
            buf.ReadExchance( ex );
            cout << ex << "\n";

            ticker_ft index;
            buf.ReadTicker( index );
            cout << index << "\n";
         } // for
      }
   }
};
