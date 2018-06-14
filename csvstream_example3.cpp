/* csvstream_example3.cpp
 *
 * Andrew DeOrio <awdeorio@umich.edu>
 *
 * An easy-to-use CSV file parser for C++
 * https://github.com/awdeorio/csvstream
 */

#include "csvstream.h"
#include <iostream>
#include <string>
#include <vector>
#include <utility>
using namespace std;


int main() {
  // Open file
  csvstream csvin("csvstream_example.csv");

  // A row is a vector<pair<string, string>>
  // key = column name, value = cell datum
  vector<pair<string, string>> row;

  // Read file
  while (csvin >> row) {
    cout << "row:" << "\n";
    for (auto &col:row) {
      string column_name = col.first;
      string datum = col.second;
      cout << "  " << column_name << ": " << datum << "\n";
    }
  }

}
