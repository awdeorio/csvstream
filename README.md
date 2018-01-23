csvstream
=========
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
    for (auto col:row) {
      string column_name = col.first;
      string datum = col.second;
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
