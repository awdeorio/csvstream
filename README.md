csvstream
=========
[![Build Status](https://travis-ci.com/awdeorio/csvstream.svg?branch=master)](https://travis-ci.com/awdeorio/csvstream)

An easy-to-use CSV file parser for C++

Andrew DeOrio <awdeorio@umich.edu><br>
http://andrewdeorio.com

## Quick start
```console
$ git clone https://github.com/awdeorio/csvstream.git
$ cd csvstream/
$ make test
```

## Example 1
This example reads one column from a CSV file.

```c++
// csvstream_example1.cpp
#include "csvstream.h"
#include <iostream>
#include <string>
#include <map>
using namespace std;


int main() {
  // Open file
  csvstream csvin("csvstream_example.csv");

  // Rows have key = column name, value = cell datum
  map<string, string> row;

  // Extract the "animal" column
  while (csvin >> row) {
    cout << row["animal"] << "\n";
  }

}
```

Input
```console
$ cat csvstream_example.csv
name,animal
Fergie,horse
Myrtle II,chicken
Oscar,cat
```

Compile
```console
$ make csvstream_example1
  # OR
$ g++ -std=c++11 csvstream_example1.cpp -o csvstream_example1
```

Output
```console
$ ./csvstream_example1
horse
chicken
cat
```


## Example 2
This example has an outer loop for each row and an inner loop for each column.

```c++
//csvstream_example2.cpp
#include "csvstream.h"
#include <iostream>
#include <string>
#include <map>
using namespace std;


int main() {
  // Open file
  csvstream csvin("csvstream_example.csv");

  // A row is a map<string, string>, key = column name, value = datum
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
```

Input
```console
$ cat csvstream_example.csv
name,animal
Fergie,horse
Myrtle II,chicken
Oscar,cat
```

Compile
```console
$ make csvstream_example2
  # OR
$ g++ -std=c++11 csvstream_example2.cpp -o csvstream_example2
```

Output
```console
$ ./csvstream_example2
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


## Example 3
This example uses a vector-of-pair to keep the order of the items read from the file.

```c++
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
    for (unsigned int i=0; i < row.size(); ++i) {
      const string &column_name = row[i].first;
      const string &datum = row[i].second;
      cout << "  " << column_name << ": " << datum << "\n";
    }
  }

}
```


## Constructor options
The constructor accepts several optional arguments.

Change the delimiter between values on a row.  Default=`','`.
```c++
csvstream csvin("csvstream_example.csv", delimiter='|');
```

Disable strict mode.  When `strict=true`, raise an exception if a row contains too many values or too few compared to the header.  When `strict=false`, ignore extra values and set missing values to empty string.
```c++
csvstream csvin("csvstream_example.csv", delimiter=',', strict=false);
```
