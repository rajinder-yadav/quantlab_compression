//==================================================================================================
// The MIT License (MIT)
//
// Copyright (c) 2016 Rajinder Yadav <rajinder.yadav@hotmail.com>
//==================================================================================================

#ifndef _test_runner_hpp_
#define _test_runner_hpp_

using std::cout;

class TestRunner
{
   // Test success & fail counts
   int pass;
   int fail;

   // To capture cerr output
   std::stringstream err_out;
   std::streambuf * cerr_buf;

public:
   TestRunner() : pass( 0 ), fail( 0 )
   {
      // Capture cerr, don't want test output polluted.
      cerr_buf = std::cerr.rdbuf( err_out.rdbuf() );
      cout << "Running tests\n"
           << "=============\n";
   }
   virtual ~TestRunner()
   {
      cout << "===================================\n";
      cout << "Tests run(" << pass + fail << ") "
           << "Passed(" << pass << ") "
           << "Failed(" << fail << ")\n\n";
      // Restore cerr
      std::cerr.rdbuf( cerr_buf );
   }

   void check( bool b, const std::string & msg  )
   {
      if ( b )
      {
         ++pass;
         cout << char(0x1B) << "[32mPass: " << msg << char(0x1B) << "[37m" << "\n";
      }
      else
      {
         ++fail;
         cout << char(0x1B) << "[31mFail: " << msg << char(0x1B) << "[37m" << "\n";
      }

      // Clear error buffer & error states
      err_out.str( "" );
      err_out.clear();
   }

   virtual void RunTests() = 0;
};

#endif // _test_runner_hpp_
