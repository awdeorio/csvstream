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
void test_escape_quotes();
void test_multiline_quotes();
void test_osx_line_endings();
void test_windows_line_endings();


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
  test_escape_quotes();
  test_multiline_quotes();
  test_osx_line_endings();
  test_windows_line_endings();
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
  // Test creating a csvstream object from a filename string

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
  // Test creating a csvstream object from a stream

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
  // Test reading header from first line of CSV file

  csvstream csvin(input_filename_animals);
  auto header = csvin.getheader();
  assert(header == header_correct_animals);
}


void test_emptyfields() {
  // Test with input data containing empty fields (consecutive commas)

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
  // Test with tab separated data

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
  // Test error condition: a row in the middle of the file that doesn't have
  // enough fields

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
  // Test error condition: last row doesn't have enough fields

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
  // Test error condition: row with too many fields

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
  // Test with input that has no newline at the end

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
  // Test with input data containing escaped quotes

  // Input
  stringstream iss("\\\"a\\\",b,c\n\\\"1\\\",2,3\n\"4,\\\"44\",5,6");

  // Correct answer
  const vector<map<string, string>> output_correct =
    {
      {{"\\\"a\\\"","\\\"1\\\""},{"b","2"},{"c","3"}},  //escaped quote
      {{"\\\"a\\\"","4,\\\"44"},{"b","5"},{"c","6"}},  //escaped quote
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


void test_multiline_quotes() {
  // Test with input data containing a quoted field containing newlines

  stringstream iss("a,b\n\"hello\nworld\",\"b\"\n");

  const vector<map<string, string>> output_correct =
    {
      {{"a", "hello\nworld"},{"b", "b"}}
    }
  ;

  vector<map<string, string>> output_observed;

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


void test_osx_line_endings() {
  // Test with input data containing OSX line endings: \r

  // Input
  stringstream iss("a,b,c\r1,2,3\r,,\r");

  // Correct answer
  const vector<map<string, string>> output_correct =
    {
      {{"a","1"},{"b","2"},{"c","3"}},
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


void test_windows_line_endings() {
  // Test with input data containing Windows line endings: \r\n

  // Input
  stringstream iss("a,b,c\r\n1,2,3\r\n,,\r\n");

  // Correct answer
  const vector<map<string, string>> output_correct =
    {
      {{"a","1"},{"b","2"},{"c","3"}},
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
