#include <iostream>
#include <iomanip>
#include <map>
#include <queue>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <algorithm>

#include <cassert>

// Add this at the end
#include "testrunner.hpp"
#include "testcoverage.hpp"

using std::clog;
using std::cout;
using std::endl;

int main(int argc, char *argv[])
{
  // Run tests
  Runner tr;
  tr.RunTests();
  return 0;
}
