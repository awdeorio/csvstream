/* csvstream_example.cpp
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


int main() {

  // Open file
  string filename = "csvstream_example.csv";
  ifstream fin;
  fin.open(filename.c_str());
  if (!fin.is_open()) {
    cout << "Error opening " << filename << "\n";
    exit(1);
  }

  // Initialize
  auto csvin = csvstream(fin);

  // See header (optional)
  auto header = csvin.getheader();
  cout << "header:\n";
  for (auto i:header) {
    cout << "  " << i << "\n";
  }

  // A row is a map<string, string>, key = column name, value = datum
  csvstream::row_type row;

  // Read file
  while (csvin >> row) {
    cout << "row:" << "\n";
    for (auto i:row) {
      auto column_name = i.first;
      auto datum = i.second;
      cout << "  " << column_name << ": " << datum << "\n";
    }
  }

  // Clean up
  fin.close();
  return 0;
}
