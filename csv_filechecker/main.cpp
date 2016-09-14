//==================================================================================================
// The MIT License (MIT)
//
// Copyright (c) 2016 Rajinder Yadav <rajinder.yadav@hotmail.com>
//==================================================================================================


#include <iostream>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <iterator>
#include <regex>
#include <iterator>

using namespace std;

void checkfile_read1();
void checkfile_read2();

/**
 * This is a CSV file checker, used after extraction to validate unzipped file.
 * This is the program used to validate the sample data provided mentioned in the README.
 */

int main(int argc, char *argv[])
{
  checkfile_read1();
  //checkfile_read2();
}

void checkfile_read1()
{
  std::ifstream in_fs("testdata");

  if (!in_fs.is_open())
  {
    cerr << "Aborting! Error: File does not exist\n";
    return;
  }

  std::string csv_data;
  std::size_t lines_read = 0;
  vector<string> words;

  std::string ticker;
  char exchange;
  char side;
  char condition;
  uint64_t time;
  uint64_t reptime;
  std::string price;
  uint32_t size;

  while (in_fs >> csv_data)
  {
    ++lines_read;
    cout << lines_read << " ";

    std::vector<std::string> tokens;
    std::regex ws_re(",");
    std::copy(std::sregex_token_iterator(csv_data.begin(), csv_data.end(), ws_re, -1),
              std::sregex_token_iterator(),
              std::back_insert_iterator<std::vector<std::string>>(tokens));

    ticker = tokens[0];
    cout << ticker << ",";
    exchange = static_cast<char>(tokens[1][0]);
    cout << exchange << ",";
    side = static_cast<char>(tokens[2][0]);
    cout << side << ",";
    condition = static_cast<char>(tokens[3][0]);
    cout << condition << ",";
    time = std::stoul(tokens[4]);
    cout << time << ",";
    reptime = std::stoul(tokens[5]);
    cout << reptime << ",";
    price = tokens[6];
    cout << price << ",";
    size = std::stoul(tokens[7]);
    cout << size << endl;
    tokens.clear();
  }
}

void checkfile_read2()
{
  std::ifstream in_fs("ebat.csv");

  if (!in_fs.is_open())
  {
    cerr << "Aborting! Error: File does not exist\n";
    return;
  }

  std::string csv_data;
  std::size_t lines_read = 0;
  vector<string> words;

  std::string ticker;
  char exchange;
  char side;
  char condition;
  uint64_t time;
  uint64_t reptime;
  std::string price;
  uint32_t size;

  while (in_fs >> csv_data)
  {
    ++lines_read;
    cout << lines_read << " " << csv_data << endl;

    // Tick data listed in CSV parse order
    std::string ticker;
    char exchange;
    char side;
    char condition;
    uint64_t time;
    uint64_t reptime;
    std::string price;
    uint32_t size;

    try
    {
      std::istringstream ss(csv_data);

      std::string tok;
      std::getline(ss, tok, ',');

      if (ss.fail())
      {
        cerr << "Aborting! Error: Invalid ticker input skipping input\n";
        return;
      }

      ticker = tok;

      std::getline(ss, tok, ',');

      if (ss.fail())
      {
        cerr << "Aborting! Error: Invalid exchange input skipping input\n";
        return;
      }

      exchange = static_cast<char>(tok[0]);

      std::getline(ss, tok, ',');

      if (ss.fail())
      {
        cerr << "Aborting! Error: Invalid side input skipping input\n";
        return;
      }

      side = static_cast<char>(tok[0]);

      std::getline(ss, tok, ',');

      if (ss.fail())
      {
        cerr << "Aborting! Error: Invalid condition input skipping input\n";
        return;
      }

      condition = static_cast<char>(tok[0]);

      std::getline(ss, tok, ',');

      if (ss.fail())
      {
        cerr << "Aborting! Error: Invalid time input skipping input\n";
        return;
      }

      time = std::stoul(tok);

      std::getline(ss, tok, ',');

      if (ss.fail())
      {
        cerr << "Aborting! Error: Invalid reptime input skipping input\n";
        return;
      }

      reptime = std::stoul(tok);

      std::getline(ss, tok, ',');

      if (ss.fail())
      {
        cerr << "Aborting! Error: Invalid price input skipping input\n";
        return;
      }

      price = tok;

      std::getline(ss, tok, ',');

      if (ss.fail())
      {
        cerr << "Aborting! Error: Invalid size input skipping input\n";
        return;
      }

      size = std::stoul(tok);
    }
    catch (...)
    {

      cerr << "Aborting! Error: Invalid input skipping input\n";
      return;
    }
  }
}
