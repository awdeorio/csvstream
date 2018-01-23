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
#include <vector>
using namespace std;


int main() {
  // Open file
  csvstream csvin("csvstream_example.csv");

  // Alternative option for a row: vector<string>, values only, with no
  // column names.
  vector<string> row;

  // Read file
  while (csvin >> row) {
    cout << "row:";
    for (auto col:row) {
      string datum = col;
      cout  << '\t' << datum;
    }
    cout << endl;
  }
}
