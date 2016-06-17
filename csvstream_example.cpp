/* csvstream_example.cpp
 *
 * Andrew DeOrio <awdeorio@umich.edu>
 *
 * An easy-to-use CSV file parser for C++
 * https://github.com/awdeorio/csvstream
 */

#include "csvstream.h"
#include <iostream>
#include <string>
#include <map>
using namespace std;


int main() {
  // Open file
  csvstream csvin("csvstream_example.csv");

  // A row is a map<string, string>, key = column name, value = datum
  csvstream::row_type row;

  // Read file
  while (csvin >> row) {
    cout << "row:" << "\n";
    for (auto i:row) {
      string column_name = i.first;
      string datum = i.second;
      cout << "  " << column_name << ": " << datum << "\n";
    }
  }

}
