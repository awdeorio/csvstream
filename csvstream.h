/* csvstream.h
 *
 * Andrew DeOrio <awdeorio@umich.edu>
 *
 * An easy-to-use CSV file parser for C++
 * https://github.com/awdeorio/csvstream
 */

#include <iostream>
#include <sstream>
#include <cassert>
#include <string>
#include <vector>
#include <map>


// A custom exception type
class csvstream_exception {
public:
  const std::string msg;
  csvstream_exception(const std::string &msg) : msg(msg) {};
};


// csvstream
class csvstream {
public:
  // A header is a vector of column names, in order
  typedef std::vector<std::string> header_type;

  // A row is a map of (column name, datum) pairs from one row
  typedef std::map<std::string, std::string> row_type;

  // Constructor
  csvstream(std::istream &is, char delimiter=',')
    : is(is), delimiter(delimiter), line_no(0) {

    // read first line, which is the header
    std::string line;
    if (!getline(is, line)) {
      throw csvstream_exception("error reading header");
    }

    // save header
    std::stringstream iss(line);
    std::string token;
    while (getline(iss, token, delimiter)) {
      header.push_back(token);
    }
  }

  // Returns whether an error flag on underlying stream is set
  explicit operator bool() const {
    return static_cast<bool>(is);
  }

  // Return header processed by constructor
  header_type getheader() const {
    return header;
  }

  // Stream extraction operator reads one row
  csvstream & operator>> (row_type& row) {
    // Read one line from stream, bail out if we're at the end
    std::string line;
    if (!getline(is, line)) return *this;
    line_no += 1;

    // Parse line using delimiter
    std::stringstream iss(line);
    size_t i = 0;
    std::string token;
    while (getline(iss, token, delimiter)) {
      row[header[i]] = token;
      i += 1;
    }

    // Check length of row with exception
    if (row.size() != header.size()) {
      throw csvstream_exception("Number of items in row does not match header: L" + std::to_string(line_no));
    }

    return *this;
  }

private:
  // Stream in CSV format
  std::istream &is;

  // Delimiter between columns
  char delimiter;

  // Line no in file.  Used for error messages
  size_t line_no;

  // Store header column names
  header_type header;
};
