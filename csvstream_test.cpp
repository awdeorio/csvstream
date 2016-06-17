/* csvstream_test.cpp
 *
 * Andrew DeOrio <awdeorio@umich.edu>
 *
 * An easy-to-use CSV file parser for C++
 * https://github.com/awdeorio/csvstream
 */

#include "csvstream.h"
#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <vector>
using namespace std;


void test_filename_ctor();
void test_stream_ctor();
void test_getheader();
void test_emptyfields();


int main() {
  test_filename_ctor();
  test_stream_ctor();
  test_getheader();
  test_emptyfields();
  cout << "csvstream_test PASSED\n";
  return 0;
}


// data for test_filename_ctor() and test_stream_ctor()
const string input_filename_animals = "csvstream_example.csv";
const vector<string> header_correct_animals = {"name", "animal"};
const vector<map<string, string>> output_correct_animals =
  {
    {{"name","Fergie"},{"animal","horse"}},
    {{"name","Myrtle II"},{"animal","chicken"}},
    {{"name","Oscar"},{"animal","cat"}},
  }
  ;


void test_filename_ctor() {
  // Save actual output
  vector<map<string, string>> output_observed;

  csvstream csvin(input_filename_animals);
  csvstream::row_type row;
  while (csvin >> row) {
    output_observed.push_back(row);
  }

  // Check output
  assert(output_observed == output_correct_animals);
}


void test_stream_ctor() {
  // Save actual output
  vector<map<string, string>> output_observed;

  ifstream fin(input_filename_animals.c_str());
  if (!fin.is_open()) {
    cout << "Error opening " << input_filename_animals << "\n";
    exit(1);
  }

  // Create object
  csvstream csvin(fin);

  // Read file
  csvstream::row_type row;
  while (csvin >> row) {
    output_observed.push_back(row);
  }

  // Check output
  assert(output_observed == output_correct_animals);

  // Clean up
  fin.close();
}


void test_getheader() {
  csvstream csvin(input_filename_animals);
  auto header = csvin.getheader();
  assert(header == header_correct_animals);
}


void test_emptyfields() {
  // Input
  stringstream iss("a,b,c\n,,\n,,\n");

  // Correct answer
  const vector<map<string, string>> output_correct =
    {
      {{"a",""},{"b",""},{"c",""}},
      {{"a",""},{"b",""},{"c",""}},
    }
  ;

  // Save actual output
  vector<map<string, string>> output_observed;

  // Create object
  csvstream csvin(iss);

  // Check header
  auto header = csvin.getheader();

  // Read file
  csvstream::row_type row;
  try {
    while (csvin >> row) {
      output_observed.push_back(row);
    }
  } catch(csvstream_exception e) {
    cout << e.msg << endl;
    throw;
  }  

  // Check output
  assert(output_observed == output_correct);
}
