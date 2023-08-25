/* example3.cpp
 *
 * Andrew DeOrio <awdeorio@umich.edu>
 *
 * csvstream: An easy-to-use CSV file parser for C++
 * https://github.com/awdeorio/csvstream
 */

#include "csvstream.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <utility>
using namespace std;


int main() {
  // Open file
  csvstream csvin("input.csv");

  // A row is a vector<pair<string, string>>
  // key = column name, value = cell datum
  vector<pair<string, string>> row;

  // Read file
  while (csvin >> row) {
    cout << "row:" << "\n";
    for (unsigned int i=0; i < row.size(); ++i) {
      const string &column_name = row[i].first;
      const string &datum = row[i].second;
      cout << "  " << column_name << ": " << datum << "\n";
    }
  }

}
