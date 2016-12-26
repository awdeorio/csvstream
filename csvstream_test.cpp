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
void test_tsv();
void test_too_few_cols_in_the_middle();
void test_too_few_cols_at_the_end();
void test_too_many_cols();
void test_no_newline_at_the_end();
void test_quotes();


int main() {
  test_filename_ctor();
  test_stream_ctor();
  test_getheader();
  test_emptyfields();
  test_tsv();
  test_too_few_cols_in_the_middle();
  test_too_few_cols_at_the_end();
  test_too_many_cols();
  test_no_newline_at_the_end();
  test_quotes();
  cout << "csvstream_test PASSED\n";
  return 0;
}


// data for next few unit tests
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

  // Read file
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

  // Open file
  ifstream fin(input_filename_animals.c_str());
  assert(fin.is_open());

  // Read file from stream
  csvstream csvin(fin);
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

  // Read stream
  csvstream csvin(iss);
  csvstream::row_type row;
  try {
    while (csvin >> row) {
      output_observed.push_back(row);
    }
  } catch(csvstream_exception e) {
    cout << e.msg << endl;
    assert(0);
  }

  // Check output
  assert(output_observed == output_correct);
}


void test_tsv() {
  // Input
  stringstream iss("a\tb\tc\nd\te\tf\n\t\t\n");

  // Correct answer
  const vector<map<string, string>> output_correct =
    {
      {{"a","d"},{"b","e"},{"c","f"}},
      {{"a",""},{"b",""},{"c",""}},
    }
  ;

  // Save actual output
  vector<map<string, string>> output_observed;

  // Read file
  csvstream csvin(iss, '\t');
  csvstream::row_type row;
  try {
    while (csvin >> row) {
      output_observed.push_back(row);
    }
  } catch(csvstream_exception e) {
    cout << e.msg << endl;
    assert(0);
  }

  // Check output
  assert(output_observed == output_correct);
}


void test_too_few_cols_in_the_middle() {
  // Input
  stringstream iss("a,b,c\n,\nd,e,f");

  // Create object
  csvstream csvin(iss);

  // Read file
  csvstream::row_type row;
  try {
    while (csvin >> row); // throw away data
  } catch(csvstream_exception e) {
    //if we caught an exception, then it worked
    return;
  }

  // if we made it this far, then it didn't work
  assert(0);
}


void test_too_few_cols_at_the_end() {
  // Input
  stringstream iss("a,b,c\n,");

  // Create object
  csvstream csvin(iss);

  // Read file
  csvstream::row_type row;
  try {
    while (csvin >> row); // throw away data
  } catch(csvstream_exception e) {
    //if we caught an exception, then it worked
    return;
  }

  // if we made it this far, then it didn't work
  assert(0);
}


void test_too_many_cols() {
  // Input
  stringstream iss("a,b,c\n,,,");

  // Create object
  csvstream csvin(iss);

  // Read file
  csvstream::row_type row;
  try {
    while (csvin >> row); // throw away data
  } catch(csvstream_exception e) {
    //if we caught an exception, then it worked
    return;
  }

  // if we made it this far, then it didn't work
  assert(0);
}

void test_no_newline_at_the_end() {
  // Input
  stringstream iss("a,b,c\n,,");

  // Create object
  csvstream csvin(iss);

  // Correct answer
  const vector<map<string, string>> output_correct =
    {
      {{"a",""},{"b",""},{"c",""}},
    }
  ;

  // Save actual output
  vector<map<string, string>> output_observed;

  // Read stream
  csvstream::row_type row;
  try {
    while (csvin >> row) {
      output_observed.push_back(row);
    }
  } catch(csvstream_exception e) {
    cout << e.msg << endl;
    assert(0);
  }

  // Check output
  assert(output_observed == output_correct);

}


void test_quotes() {
  // Input
  stringstream iss("\"a\",b,c\n\"1\",2,3\n\"4,44\",5,6\n");

  // Correct answer
  const vector<map<string, string>> output_correct =
    {
      {{"a","1"},{"b","2"},{"c","3"}},
      {{"a","4,44"},{"b","5"},{"c","6"}}, //quoted comma
    }
  ;

  // Save actual output
  vector<map<string, string>> output_observed;

  // Read stream
  csvstream csvin(iss);
  csvstream::row_type row;
  try {
    while (csvin >> row) {
      output_observed.push_back(row);
    }
  } catch(csvstream_exception e) {
    cout << e.msg << endl;
    assert(0);
  }

  // Check output
  assert(output_observed == output_correct);
}

void test_escape_quotes() {
  // Input
  stringstream iss("\"a\",b,c\n\"\\\"1\\\"\",2,3\n");

  // Correct answer
  const vector<map<string, string>> output_correct =
    {
      {{"a","\"1\""},{"b","2"},{"c","3"}},  //escaped quote
    }
  ;

  // Save actual output
  vector<map<string, string>> output_observed;

  // Read stream
  csvstream csvin(iss);
  csvstream::row_type row;
  try {
    while (csvin >> row) {
      output_observed.push_back(row);
    }
  } catch(csvstream_exception e) {
    cout << e.msg << endl;
    assert(0);
  }

  // Check output
  assert(output_observed == output_correct);
}
