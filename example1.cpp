/* example1.cpp
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

  // Rows have key = column name, value = cell datum
  map<string, string> row;

  // Extract the "animal" column
  while (csvin >> row) {
    cout << row["animal"] << "\n";
  }

}
