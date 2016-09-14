
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
   }
};
