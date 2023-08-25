/* example4.cpp
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
  string filename = "input.csv";
  try {
    csvstream csvin(filename);
    map<string, string> row;
    while (csvin >> row) {
      cout << row["animal"] << "\n";
    }
  } catch(const csvstream_exception &e) {
    cerr << "Error: " << e.what() << "\n";
    return 1;    
  }
}
