/* example2.cpp
 *
 * Andrew DeOrio <awdeorio@umich.edu>
 *
 * csvstream: An easy-to-use CSV file parser for C++
 * https://github.com/awdeorio/csvstream
 */

#include "csvstream.hpp"
#include <iostream>
#include <string>
#include <map>
using namespace std;


int main() {
  // Open file
  csvstream csvin("input.csv");

  // A row is a map<string, string>, key = column name, value = cell datum
  map<string, string> row;

  // Read file
  while (csvin >> row) {
    cout << "row:" << "\n";
    for (auto &col:row) {
      const string &column_name = col.first;
      const string &datum = col.second;
      cout << "  " << column_name << ": " << datum << "\n";
    }
  }

}
