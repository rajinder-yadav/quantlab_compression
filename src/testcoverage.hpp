
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
       * Test class BatBuffer.
       */
      test = "Packing zero value (1 bit)";
      {
         BatBuffer buf;
         uint32_t val = 0;
         auto b = buf.pack32( val, 1 ); // 1bit including 1bit padding
         bool result = buf.buffer == 0
                       && !b.overflow
                       && b.bits == 0
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
                       && b.bits == 0  // How many bits were packed
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
                       && b.bits == 0
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
                       && b.bits == 0
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
                       && b.bits == 0
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
                       && b.bits == 0
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
                       && b2.bits == 0  // size of last pack
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

      /**
       * Test BatBuffer::Write method.
       */
      test = "Write value < 32bits and check buffer.";
      {
         BatBuffer buf;
         buf.Write( 3756, 12 ); // 12bits
         uint32_t data = 3756 << ( 32 - 12 );

         bool result = buf.buffer == data
                       && buf.packet.size() == 0
                       && buf.buffer_size == 32 - 12;
         check( result, test );
      }
      test = "Write 32bit value and check buffer.";
      {
         BatBuffer buf;
         buf.Write( 3982135763, 32 ); // 32bits
         uint32_t data = 3982135763;

         bool result = buf.buffer == 0
                       && buf.packet.size() == 1
                       && buf.packet.back() == 3982135763
                       && buf.buffer_size == 32;
         check( result, test );
      }
      test = "Write 2 values < 32bits and check buffer.";
      {
         BatBuffer buf;
         buf.Write( 3756, 12 );  // 12bits
         buf.Write( 57483, 16 ); // 16bits
         uint32_t data1 = 3756 << ( 32 - 12 );
         uint32_t data2 = 57483 << ( ( 32 - 12 ) - 16 );

         bool result = buf.buffer == data1 | data2
                       && buf.packet.size() == 0
                       && buf.buffer_size == 32 - ( 12 + 16 );
         check( result, test );
      }
      test = "Write 3 values == 32bits and check buffer.";
      {
         BatBuffer buf;
         buf.Write( 3756, 12 );  // 12bits
         buf.Write( 57483, 16 ); // 16bits
         buf.Write( 8, 4 );      // 4bits
         uint32_t data1 = 3756 << ( 32 - 12 );
         uint32_t data2 = 57483 << ( ( 32 - 12 ) - 16 );
         uint32_t data3 = 4;

         bool result = buf.buffer == 0
                       && buf.packet.size() == 1
                       && buf.packet.back() == data1 | data2 | data3;
         check( result, test );
      }
      test = "Write 2 values + pad == 32bits and check buffer.";
      {
         BatBuffer buf;
         buf.Write( 3756, 12 );  // 12bits
         buf.Write( 57483, 20 ); // 16bits
         uint32_t data1 = 3756 << ( 32 - 12 );
         uint32_t data2 = 57483;

         bool result = buf.buffer == 0
                       && buf.packet.size() == 1
                       && buf.packet.back() == data1 | data2
                       && buf.buffer_size == 32;
         check( result, test );
      }
      test = "Write 1 values + pad > 32bits and check buffer.";
      {
         BatBuffer buf;
         buf.Write( 15021, 14 + 25 ); // 14bits + 25 bits padding
         uint32_t data1 = 15021 >> 7; // spill 7bits
         uint32_t data2 = ( 15021 & uint32_t( pow( 2, 7 ) - 1 ) ) << ( 32 - 7 );

         bool result = buf.buffer == data2
                       && buf.packet.size() == 1
                       && buf.packet.back() == data1
                       && buf.buffer_size == 32 - 7;
         check( result, test );
      }
      test = "Write 3 values > 32bits and check buffer.";
      {
         BatBuffer buf;
         buf.Write( 3756, 12 );  // 12bits
         buf.Write( 57483, 16 ); // 16bits
         buf.Write( 9586, 14 );  // 14bits
         uint32_t data1 = 3756 << ( 32 - 12 );
         uint32_t data2 = 57483 << ( ( 32 - 12 ) - 16 );
         uint32_t data3 = 9586 >> 10;
         uint32_t data4 = ( 9586 & uint32_t( pow( 2, 10 ) - 1 ) ) << ( 32 - 10 );

         bool result = buf.buffer == data4
                       && buf.packet.size() == 1
                       && buf.packet.back() == data1 | data2 | data3
                       && buf.buffer_size == 22;
         check( result, test );
      }
      test = "Write string 'Hello' into buffer";
      {
         BatBuffer buf;
         buf.WriteString( "Hello", 5 );
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
                       && buf.buffer == 111 << ( 32 - 8 );
         check( result, test );
      }
      test = "Write string 'Hello' into buffer & flush buffer";
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

      /**
       * Test BatBuffer::Read method.
       */
      test = "Switching write to read modes with 11bit value";
      {
         BatBuffer buf;
         uint32_t val = 1995; // 11bits
         buf.Write( val, 11 );
         buf.Flush();
         buf.SetReadMode();
         uint32_t data;
         bool error;
         buf.Read( data, 11, error );

         bool result = ( data == val )
                       && !error
                       && ( buf.buffer == 0 );       // No more data
         //&& (buf.packet.size() == 0); // Empty packet
         check( result, test );
      }
      test = "Switching write to read modes with zero value using 11bit";
      {
         BatBuffer buf;
         uint32_t val = 0;
         buf.Write( val, 11 );
         buf.Flush();
         buf.SetReadMode();
         uint32_t data;
         bool error;
         buf.Read( data, 11, error );

         bool result = data == val && !error
                       && buf.buffer == 0         // No more data
                       && buf.packet.size() == 0; // Empty packet
         check( result, test );
      }
      test = "Switching write to read modes with 32bit value";
      {
         BatBuffer buf;
         uint32_t val = 2863333375; // 11bits
         buf.Write( val, 32 );
         buf.Flush();
         buf.SetReadMode();
         assert( buf.packet.size() == 0 );
         uint32_t data;
         bool error;
         buf.Read( data, 32, error );

         bool result = ( data == val )
                       && !error
                       && ( buf.buffer_size == 0 )       // No more data
                       && ( buf.packet.size() == 0 ); // Empty packet
         check( result, test );
      }
      test = "Switching write to read modes with zero value using 32bit";
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

         bool result = data == val && !error
                       && buf.buffer == 0    // No more data
                       && buf.packet.size() == 0; // Empty packet
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
         buf.Read( val1, 4, error );
         buf.Read( val2, 8, error );

         bool result = val1 == 13
                       && val2 == 220;
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
         bool more1 = buf.Read( val[0], 8, error );
         bool more2 = buf.Read( val[1], 4, error );
         bool more3 = buf.Read( val[2], 11, error );
         bool more4 = buf.Read( val[3], 2, error );
         bool more5 = buf.Read( val[4], 7, error );

         bool result = val[0] == 220
                       && val[1] == 13
                       && val[2] == 1978
                       && val[3] == 0
                       && val[4] == 82
                       && more1 && more2 && more3 && more4 && !more5;

         check( result, test );
      }

      /**
       * Test Writing a BAT packets.
       */
      test = "BAT entry 1";
      {
         BatBuffer buf;
         bool error;
         uint32_t size;
         share_ft shares;
         std::string price;
         time_ft timereport;
         time_ft time;
         char condition;
         side_ft side;
         char ex;
         ticker_ft index;

         uint32_t s1 = buf.WriteTicker( 1 );          // index
         uint32_t s2 = buf.WriteExchange( 'q' );      // ex
         uint32_t s3 = buf.WriteSide( 3 );            // side B,A,T
         uint32_t s4 = buf.WriteCondition( 'O' );     // condition
         uint32_t s5 = buf.WriteTime( 42180525828 );  // time
         uint32_t s6 = buf.WriteTime( 42180526502 );  // time-report
         uint32_t s7 = buf.WritePrice( "13.06" );     // price
         uint32_t s8 = buf.WriteShares( 730 );        // shares
         buf.Flush();
         buf.SetReadMode();

         buf.ReadTicker( index );
         buf.ReadExchance( ex );
         buf.ReadSide( side );
         buf.ReadCondition( condition );
         buf.ReadTime( time );
         buf.ReadTime( timereport );
         buf.ReadPrice( price );
         buf.ReadShares( shares );

         bool result = index == 1
                       && ex == 'q'
                       && side == 3
                       && condition == 'O'
                       && time == 42180525828
                       && timereport == 42180526502
                       && price == "13.06"
                       && shares == 730;

         check( result, test );
      }
      test = "BAT entry 2";
      {
         BatBuffer buf;
         bool error;
         uint32_t size[2];
         share_ft shares[2];
         std::string price[2];
         time_ft timereport[2];
         time_ft time[2];
         char condition[2];
         side_ft side[2];
         char ex[2];
         ticker_ft index[2];

         uint32_t s1 = buf.WriteTicker( 3 );          // index
         uint32_t s2 = buf.WriteExchange( 'r' );      // ex
         uint32_t s3 = buf.WriteSide( 3 );            // side B,A,T
         uint32_t s4 = buf.WriteCondition( 'O' );     // condition
         uint32_t s5 = buf.WriteTime( 42180525828 );  // time
         uint32_t s6 = buf.WriteTime( 42180526502 );  // time-report
         uint32_t s7 = buf.WritePrice( "12.56" );     // price
         uint32_t s8 = buf.WriteShares( 1102 );       // size

         uint32_t r1 = buf.WriteTicker( 12 );         // index
         uint32_t r2 = buf.WriteExchange( 'm' );      // ex
         uint32_t r3 = buf.WriteSide( 1 );            // ride B,A,T
         uint32_t r4 = buf.WriteCondition( 'C' );     // condition
         uint32_t r5 = buf.WriteTime( 42180526503 );  // time
         uint32_t r6 = buf.WriteTime( 42180526672 );  // time-report
         uint32_t r7 = buf.WritePrice( "3.09" );      // price
         uint32_t r8 = buf.WriteShares( 243 );        // size
         buf.Flush();
         buf.SetReadMode();

         for ( int i = 0; i < 2; ++i )
         {
            buf.ReadTicker( index[i] );
            buf.ReadExchance( ex[i] );
            buf.ReadSide( side[i] );
            buf.ReadCondition( condition[i] );
            buf.ReadTime( time[i] );
            buf.ReadTime( timereport[i] );
            buf.ReadPrice( price[i] );
            buf.ReadShares( shares[i] );
         } // for

         bool result = index[0] == 3
                       && ex[0] == 'r'
                       && side[0] == 3
                       && condition[0] == 'O'
                       && time[0] == 42180525828
                       && timereport[0] == 42180526502
                       && price[0] == "12.56"
                       && shares[0] == 1102

                       && index[1] == 12
                       && ex[1] == 'm'
                       && side[1] == 1
                       && condition[1] == 'C'
                       && time[1] == 42180526503
                       && timereport[1] == 42180526672
                       && price[1] == "3.09"
                       && shares[1] == 243;

         check( result, test );
      }

      /**
       * Test Save and load of buffer.
       */
      test = "BAT single record save and load";
      {
         BatBuffer buf;
         bool error;
         uint32_t size;
         share_ft shares;
         std::string price;
         time_ft timereport;
         time_ft time;
         char condition;
         side_ft side;
         char ex;
         ticker_ft index;

         uint32_t s1 = buf.WriteTicker( 1 );          // index
         uint32_t s2 = buf.WriteExchange( 'q' );      // ex
         uint32_t s3 = buf.WriteSide( 3 );            // side B,A,T
         uint32_t s4 = buf.WriteCondition( 'O' );     // condition
         uint32_t s5 = buf.WriteTime( 42180525828 );  // time
         uint32_t s6 = buf.WriteTime( 42180526502 );  // time-report
         uint32_t s7 = buf.WritePrice( "13.06" );     // price
         uint32_t s8 = buf.WriteShares( 730 );        // shares
         buf.Flush();
         buf.Save( "filetest1" );

         BatBuffer buf2;
         buf2.Load( "filetest1" );

         buf2.ReadTicker( index );
         buf2.ReadExchance( ex );
         buf2.ReadSide( side );
         buf2.ReadCondition( condition );
         buf2.ReadTime( time );
         buf2.ReadTime( timereport );
         buf2.ReadPrice( price );
         buf2.ReadShares( shares );

         bool result = index == 1
                       && ex == 'q'
                       && side == 3
                       && condition == 'O'
                       && time == 42180525828
                       && timereport == 42180526502
                       && price == "13.06"
                       && shares == 730;

         check( result, test );
      }
      test = "BAT 2 records save and load";
      {
         BatBuffer buf;
         bool error;
         uint32_t size[2];
         share_ft shares[2];
         std::string price[2];
         time_ft timereport[2];
         time_ft time[2];
         char condition[2];
         side_ft side[2];
         char ex[2];
         ticker_ft index[2];

         uint32_t s1 = buf.WriteTicker( 3 );          // index
         uint32_t s2 = buf.WriteExchange( 'r' );      // ex
         uint32_t s3 = buf.WriteSide( 3 );            // side B,A,T
         uint32_t s4 = buf.WriteCondition( 'O' );     // condition
         uint32_t s5 = buf.WriteTime( 42180525828 );  // time
         uint32_t s6 = buf.WriteTime( 42180526502 );  // time-report
         uint32_t s7 = buf.WritePrice( "12.56" );     // price
         uint32_t s8 = buf.WriteShares( 1102 );       // size

         uint32_t r1 = buf.WriteTicker( 12 );         // index
         uint32_t r2 = buf.WriteExchange( 'm' );      // ex
         uint32_t r3 = buf.WriteSide( 1 );            // ride B,A,T
         uint32_t r4 = buf.WriteCondition( 'C' );     // condition
         uint32_t r5 = buf.WriteTime( 42180526503 );  // time
         uint32_t r6 = buf.WriteTime( 42180526672 );  // time-report
         uint32_t r7 = buf.WritePrice( "3.09" );      // price
         uint32_t r8 = buf.WriteShares( 243 );        // size
         buf.Flush();
         buf.Save( "filetest2" );

         BatBuffer buf2;
         buf2.Load( "filetest2" );

         for ( int i = 0; i < 2; ++i )
         {
            buf2.ReadTicker( index[i] );
            buf2.ReadExchance( ex[i] );
            buf2.ReadSide( side[i] );
            buf2.ReadCondition( condition[i] );
            buf2.ReadTime( time[i] );
            buf2.ReadTime( timereport[i] );
            buf2.ReadPrice( price[i] );
            buf2.ReadShares( shares[i] );
         }

         bool result = index[0] == 3
                       && ex[0] == 'r'
                       && side[0] == 3
                       && condition[0] == 'O'
                       && time[0] == 42180525828
                       && timereport[0] == 42180526502
                       && price[0] == "12.56"
                       && shares[0] == 1102

                       && index[1] == 12
                       && ex[1] == 'm'
                       && side[1] == 1
                       && condition[1] == 'C'
                       && time[1] == 42180526503
                       && timereport[1] == 42180526672
                       && price[1] == "3.09"
                       && shares[1] == 243;

         check( result, test );
      }
      test = "BAT 5 records write and read";
      {
         // STIBX,i,A,0,42180550149,42180550822,27.04,500
         // CNX,i,B,0,42180555115,42180555793,35.87,436
         // FFG,i,B,0,42180562216,42180562892,8.85,4218
         // TES,i,B,0,42180564054,42180564732,10.25,7100
         // CCC,i,T,0,42180564198,42180564812,13.12,540
         BatBuffer buf;
         bool error;
         share_ft shares[5];
         std::string price[5];
         time_ft timereport[5];
         time_ft time[5];
         char condition[5];
         side_ft side[5];
         char ex[5];
         ticker_ft index[5];

         // STIBX,i,A,0,42180550149,42180550822,27.04,500
         buf.WriteTicker( 1 );          // index
         buf.WriteExchange( 'i' );      // ex
         buf.WriteSide( 1 );            // side B,A,T
         buf.WriteCondition( '0' );     // condition
         buf.WriteTime( 42180550149 );  // time
         buf.WriteTime( 42180550822 );  // time-report
         buf.WritePrice( "27.04" );     // price
         buf.WriteShares( 500 );        // size

         // CNX,i,B,0,42180555115,42180555793,35.87,436
         buf.WriteTicker( 2 );          // index
         buf.WriteExchange( 'i' );      // ex
         buf.WriteSide( 0 );            // side B,A,T
         buf.WriteCondition( '0' );     // condition
         buf.WriteTime( 42180555115 );  // time
         buf.WriteTime( 42180555793 );  // time-report
         buf.WritePrice( "35.87" );     // price
         buf.WriteShares( 436 );        // size

         // FFG,i,B,0,42180562216,42180562892,8.85,4218
         buf.WriteTicker( 3 );          // index
         buf.WriteExchange( 'i' );      // ex
         buf.WriteSide( 0 );            // side B,A,T
         buf.WriteCondition( '0' );     // condition
         buf.WriteTime( 42180562216 );  // time
         buf.WriteTime( 42180562892 );  // time-report
         buf.WritePrice( "8.85" );     // price
         buf.WriteShares( 4218 );        // size

         // TES,i,B,0,42180564054,42180564732,10.25,7100
         buf.WriteTicker( 4 );          // index
         buf.WriteExchange( 'i' );      // ex
         buf.WriteSide( 0 );            // side B,A,T
         buf.WriteCondition( '0' );     // condition
         buf.WriteTime( 42180564054 );  // time
         buf.WriteTime( 42180564732 );  // time-report
         buf.WritePrice( "10.25" );     // price
         buf.WriteShares( 7100 );        // size

         // CCC,i,T,0,42180564198,42180564812,13.12,540
         buf.WriteTicker( 5 );          // index
         buf.WriteExchange( 'i' );      // ex
         buf.WriteSide( 2 );            // side B,A,T
         buf.WriteCondition( '0' );     // condition
         buf.WriteTime( 42180564198 );  // time
         buf.WriteTime( 42180564812 );  // time-report
         buf.WritePrice( "13.12" );     // price
         buf.WriteShares( 540 );        // size
         buf.Flush();
         buf.SetReadMode();

         for ( int i = 0; i < 5; ++i )
         {
            buf.ReadTicker( index[i] );
            buf.ReadExchance( ex[i] );
            buf.ReadSide( side[i] );
            buf.ReadCondition( condition[i] );
            buf.ReadTime( time[i] );
            buf.ReadTime( timereport[i] );
            buf.ReadPrice( price[i] );
            buf.ReadShares( shares[i] );
         } // for

         // STIBX,i,A,0,42180550149,42180550822,27.04,500
         // CNX,i,B,0,42180555115,42180555793,35.87,436
         // FFG,i,B,0,42180562216,42180562892,8.85,4218
         // TES,i,B,0,42180564054,42180564732,10.25,7100
         // CCC,i,T,0,42180564198,42180564812,13.12,540
         bool result = index[4] == 5
                       && ex[4] == 'i'
                       && side[4] == 2
                       && condition[4] == '0'
                       && time[4] == 42180564198
                       && timereport[4] == 42180564812
                       && price[4] == "13.12"
                       && shares[4] == 540

                       && index[3] == 4
                       && ex[3] == 'i'
                       && side[3] == 0
                       && condition[3] == '0'
                       && time[3] == 42180564054
                       && timereport[3] == 42180564732
                       && price[3] == "10.25"
                       && shares[3] == 7100

                       && index[2] == 3
                       && ex[2] == 'i'
                       && side[2] == 0
                       && condition[2] == '0'
                       && time[2] == 42180562216
                       && timereport[2] == 42180562892
                       && price[2] == "8.85"
                       && shares[2] == 4218

                       && index[1] == 2
                       && ex[1] == 'i'
                       && side[1] == 0
                       && condition[1] == '0'
                       && time[1] == 42180555115
                       && timereport[1] == 42180555793
                       && price[1] == "35.87"
                       && shares[1] == 436

                       && index[0] == 1
                       && ex[0] == 'i'
                       && side[0] == 1
                       && condition[0] == '0'
                       && time[0] == 42180550149
                       && timereport[0] == 42180550822
                       && price[0] == "27.04"
                       && shares[0] == 500;
         check( result, test );
      }
      test = "BAT 5 records save and load";
      {
         // STIBX,i,A,0,42180550149,42180550822,27.04,500
         // CNX,i,B,0,42180555115,42180555793,35.87,436
         // FFG,i,B,0,42180562216,42180562892,8.85,4218
         // TES,i,B,0,42180564054,42180564732,10.25,7100
         // CCC,i,T,0,42180564198,42180564812,13.12,540
         BatBuffer buf;
         bool error;
         share_ft shares[5];
         std::string price[5];
         time_ft timereport[5];
         time_ft time[5];
         char condition[5];
         side_ft side[5];
         char ex[5];
         ticker_ft index[5];

         // STIBX,i,A,0,42180550149,42180550822,27.04,500
         buf.WriteTicker( 1 );          // index
         buf.WriteExchange( 'i' );      // ex
         buf.WriteSide( 1 );            // side B,A,T
         buf.WriteCondition( '0' );     // condition
         buf.WriteTime( 42180550149 );  // time
         buf.WriteTime( 42180550822 );  // time-report
         buf.WritePrice( "27.04" );     // price
         buf.WriteShares( 500 );        // size

         // CNX,i,B,0,42180555115,42180555793,35.87,436
         buf.WriteTicker( 2 );          // index
         buf.WriteExchange( 'i' );      // ex
         buf.WriteSide( 0 );            // side B,A,T
         buf.WriteCondition( '0' );     // condition
         buf.WriteTime( 42180555115 );  // time
         buf.WriteTime( 42180555793 );  // time-report
         buf.WritePrice( "35.87" );     // price
         buf.WriteShares( 436 );        // size

         // FFG,i,B,0,42180562216,42180562892,8.85,4218
         buf.WriteTicker( 3 );          // index
         buf.WriteExchange( 'i' );      // ex
         buf.WriteSide( 0 );            // side B,A,T
         buf.WriteCondition( '0' );     // condition
         buf.WriteTime( 42180562216 );  // time
         buf.WriteTime( 42180562892 );  // time-report
         buf.WritePrice( "8.85" );     // price
         buf.WriteShares( 4218 );        // size

         // TES,i,B,0,42180564054,42180564732,10.25,7100
         buf.WriteTicker( 4 );          // index
         buf.WriteExchange( 'i' );      // ex
         buf.WriteSide( 0 );            // side B,A,T
         buf.WriteCondition( '0' );     // condition
         buf.WriteTime( 42180564054 );  // time
         buf.WriteTime( 42180564732 );  // time-report
         buf.WritePrice( "10.25" );     // price
         buf.WriteShares( 7100 );        // size

         // CCC,i,T,0,42180564198,42180564812,13.12,540
         buf.WriteTicker( 5 );          // index
         buf.WriteExchange( 'i' );      // ex
         buf.WriteSide( 2 );            // side B,A,T
         buf.WriteCondition( '0' );     // condition
         buf.WriteTime( 42180564198 );  // time
         buf.WriteTime( 42180564812 );  // time-report
         buf.WritePrice( "13.12" );     // price
         buf.WriteShares( 540 );        // size
         buf.Flush();
         buf.Save( "filetest2" );

         BatBuffer buf2;
         buf2.Load( "filetest2" );

         for ( int i = 0; i < 5; ++i )
         {
            buf2.ReadTicker( index[i] );
            buf2.ReadExchance( ex[i] );
            buf2.ReadSide( side[i] );
            buf2.ReadCondition( condition[i] );
            buf2.ReadTime( time[i] );
            buf2.ReadTime( timereport[i] );
            buf2.ReadPrice( price[i] );
            buf2.ReadShares( shares[i] );
         } // for

         // STIBX,i,A,0,42180550149,42180550822,27.04,500
         // CNX,i,B,0,42180555115,42180555793,35.87,436
         // FFG,i,B,0,42180562216,42180562892,8.85,4218
         // TES,i,B,0,42180564054,42180564732,10.25,7100
         // CCC,i,T,0,42180564198,42180564812,13.12,540
         bool result = index[4] == 5
                       && ex[4] == 'i'
                       && side[4] == 2
                       && condition[4] == '0'
                       && time[4] == 42180564198
                       && timereport[4] == 42180564812
                       && price[4] == "13.12"
                       && shares[4] == 540

                       && index[3] == 4
                       && ex[3] == 'i'
                       && side[3] == 0
                       && condition[3] == '0'
                       && time[3] == 42180564054
                       && timereport[3] == 42180564732
                       && price[3] == "10.25"
                       && shares[3] == 7100

                       && index[2] == 3
                       && ex[2] == 'i'
                       && side[2] == 0
                       && condition[2] == '0'
                       && time[2] == 42180562216
                       && timereport[2] == 42180562892
                       && price[2] == "8.85"
                       && shares[2] == 4218

                       && index[1] == 2
                       && ex[1] == 'i'
                       && side[1] == 0
                       && condition[1] == '0'
                       && time[1] == 42180555115
                       && timereport[1] == 42180555793
                       && price[1] == "35.87"
                       && shares[1] == 436

                       && index[0] == 1
                       && ex[0] == 'i'
                       && side[0] == 1
                       && condition[0] == '0'
                       && time[0] == 42180550149
                       && timereport[0] == 42180550822
                       && price[0] == "27.04"
                       && shares[0] == 500;
         check( result, test );
      }

      test = "BAT single record save and load 10,000 times";
      {
         BatBuffer buf;
         bool error;
         uint32_t size;
         share_ft shares;
         std::string price;
         time_ft timereport;
         time_ft time;
         char condition;
         side_ft side;
         char ex;
         ticker_ft index;

         int count = 10000;

         for ( int i = 0; i < count; ++i )
         {
            uint32_t s1 = buf.WriteTicker( 1 );          // index
            uint32_t s2 = buf.WriteExchange( 'q' );      // ex
            uint32_t s3 = buf.WriteSide( 3 );            // side B,A,T
            uint32_t s4 = buf.WriteCondition( 'O' );     // condition
            uint32_t s5 = buf.WriteTime( 42180525828 );  // time
            uint32_t s6 = buf.WriteTime( 42180526502 );  // time-report
            uint32_t s7 = buf.WritePrice( "13.06" );     // price
            uint32_t s8 = buf.WriteShares( 730 );        // shares
         }

         buf.Flush();
         buf.Save( "filetest1" );

         BatBuffer buf2;
         buf2.Load( "filetest1" );

         bool pass = true;

         for ( int i = 0; i < count; ++i )
         {
            buf2.ReadTicker( index );
            buf2.ReadExchance( ex );
            buf2.ReadSide( side );
            buf2.ReadCondition( condition );
            buf2.ReadTime( time );
            buf2.ReadTime( timereport );
            buf2.ReadPrice( price );
            buf2.ReadShares( shares );

            bool result = index == 1
                          && ex == 'q'
                          && side == 3
                          && condition == 'O'
                          && time == 42180525828
                          && timereport == 42180526502
                          && price == "13.06"
                          && shares == 730;
            pass = pass & result;
         }

         check( pass, test );
      }

   }
};
