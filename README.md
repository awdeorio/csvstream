csvstream
=========
An easy-to-use CSV file parser for C++

Andrew DeOrio <awdeorio@umich.edu><br>
http://andrewdeorio.com

## Quick start
```
git clone https://github.com/awdeorio/csvstream.git
cd csvstream/
make test
```

## Example
```c++
//csvstream_example.cpp
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
```

Input
```
$ cat csvstream_example.csv 
name,animal
Fergie,horse
Myrtle II,chicken
Oscar,cat
```

Compile
```
$ make csvstream_example
  # OR
$ g++ -std=c++11 csvstream_example.cpp -o csvstream_example
```

Output
```
$ ./csvstream_example
row:
  animal: horse
  name: Fergie
row:
  animal: chicken
  name: Myrtle II
row:
  animal: cat
  name: Oscar
```
