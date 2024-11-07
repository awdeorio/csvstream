csvstream
=========
[![Build Status](https://travis-ci.com/awdeorio/csvstream.svg?branch=master)](https://travis-ci.com/awdeorio/csvstream)

An easy-to-use CSV file parser for C++

Andrew DeOrio <awdeorio@umich.edu><br>
http://andrewdeorio.com

**Table of Contents**
- [Quick start](#quick-start)
- [Example 1: Read one column](#example-1-read-one-column)
- [Example 2: Read each row and each column with nested loops](#example-2-read-each-row-and-each-column-with-nested-loops)
- [Example 3: Maintaining order of columns in each row](#example-3-maintaining-order-of-columns-in-each-row)
- [Changing the delimiter](#changing-the-delimiter)
- [Allow too many or too few values in a row](#allow-too-many-or-too-few-values-in-a-row)
- [Error handling](#error-handling)


## Quick start
```console
$ git clone https://github.com/awdeorio/csvstream.git
$ cd csvstream/
$ make test
```

## Example 1: Read one column
This example reads one column from a CSV file.

```c++
// example1.cpp
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
```

Input
```console
$ cat input.csv
name,animal
Fergie,horse
Myrtle II,chicken
Oscar,cat
```

Compile
```console
$ make example1
  # OR
$ g++ -std=c++11 example1.cpp -o example1
```

Output
```console
$ ./example1
horse
chicken
cat
```


## Example 2: Read each row and each column with nested loops
This example has an outer loop for each row and an inner loop for each column.

```c++
//example2.cpp
#include "csvstream.hpp"
#include <iostream>
#include <string>
#include <map>
using namespace std;


int main() {
  // Open file
  csvstream csvin("input.csv");

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
$ cat input.csv
name,animal
Fergie,horse
Myrtle II,chicken
Oscar,cat
```

Compile
```console
$ make example2
  # OR
$ g++ -std=c++11 example2.cpp -o example2
```

Output.  Notice output order within each row is `animal` followed by `name`.  This is because iterating over a `map` yields items in sorted order by key.
```console
$ ./example2
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


## Example 3: Maintaining order of columns in each row
This example uses a vector-of-pair to maintain the order of values read from each row.

```c++
// example3.cpp
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
```

Input
```console
$ cat input.csv
name,animal
Fergie,horse
Myrtle II,chicken
Oscar,cat
```

Compile
```console
$ make example3
  # OR
$ g++ -std=c++11 example3.cpp -o example3
```

Output.  Notice output order within each row is `name` followed by `animal`.  This is the order that the columns appear in the CSV file.
```console
$ ./example3
row:
  name: Fergie
  animal: horse
row:
  name: Myrtle II
  animal: chicken
row:
  name: Oscar
  animal: cat
```


## Changing the delimiter
By default, values in a row are delimited by a comma `,`.  Change the delimiter with the `delimiter` constructor parameter.

This example changes the delimiter to the `|` character.
```c++
csvstream csvin("input.csv", '|');
```

## Allow too many or too few values in a row
By default, if a row has too many or too few values, csvstream raises and exception.  With strict mode disabled, it will ignore extra values and set missing values to empty string.  You must specify a delimiter when using strict mode.
```c++
csvstream csvin("input.csv", ',', false);
```

## Error handling
If an error occurs, `csvstream` functions throw a `cstream_exception` .  For example:

```c++
// example4.cpp
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
    cerr << e.what() << "\n";
    return 1;
  }
}
```
