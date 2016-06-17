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

const string input_filename = "csvstream_example.csv";
const vector<string> header_correct = {"name", "animal"};
const vector<map<string, string>> output_correct =
  {
    {{"name","Fergie"},{"animal","horse"}},
    {{"name","Myrtle II"},{"animal","chicken"}},
    {{"name","Oscar"},{"animal","cat"}},
  }
  ;


int main() {
  test_filename_ctor();
  test_stream_ctor();
  test_getheader();
  cout << "csvstream_test PASSED";
  return 0;
}


void test_filename_ctor() {
  // Save actual output
  vector<map<string, string>> output_observed;

  csvstream csvin(input_filename);
  csvstream::row_type row;
  while (csvin >> row) {
    output_observed.push_back(row);
  }

  // Check output
  assert(output_observed == output_correct);
}


void test_stream_ctor() {
  // Save actual output
  vector<map<string, string>> output_observed;

  ifstream fin(input_filename.c_str());
  if (!fin.is_open()) {
    cout << "Error opening " << input_filename << "\n";
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
  assert(output_observed == output_correct);

  // Clean up
  fin.close();
}


void test_getheader() {
  csvstream csvin(input_filename);
  auto header = csvin.getheader();
  assert(header == header_correct);
}
