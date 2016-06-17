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


// pretty print std::vector
template <typename T>
std::ostream & operator<<(std::ostream &os, const std::vector<T> &v) {
  os << "[";
  size_t j=0;
  for (auto i:v) {
    os << i;
    if (j++ == v.size() - 1) break;
    os << ", ";
  }
  os << "]";
  return os;
}

// pretty print std::pair
template <typename K, typename V>
std::ostream & operator<<(std::ostream &os, const std::pair<K,V> &p) {
  os << p.first << ":" << p.second;
  return os;
}

// pretty print std::map
template <typename K, typename V>
std::ostream & operator<<(std::ostream &os, const std::map<K,V> &m) {
  os << "{";
  size_t j=0;
  for (auto i:m) {
    os << i;
    if (j++ == m.size() - 1) break;
    os << ", ";
  }
  os << "}";
  return os;
}


int main() {

  // Open file
  string filename = "test00-train.csv";
  ifstream fin;
  fin.open(filename.c_str());
  if (!fin.is_open()) {
    cout << "Error opening " << filename << "\n";
    exit(1);
  }

  // Create object
  csvstream csvin(fin);

  // Check header
  auto header = csvin.getheader();
  cout << header << "\n";

  // Read file
  csvstream::row_type row;
  while (csvin >> row) {
    if (row.size() == 0) break;
    cout << row << "\n";
  }

  // Clean up
  fin.close();
  return 0;
}
