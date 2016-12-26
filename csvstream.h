#ifndef CSVSTREAM_H
#define CSVSTREAM_H
/* csvstream.h
 *
 * Andrew DeOrio <awdeorio@umich.edu>
 *
 * An easy-to-use CSV file parser for C++
 * https://github.com/awdeorio/csvstream
 */

#include <iostream>
#include <fstream>
#include <sstream>
#include <cassert>
#include <string>
#include <vector>
#include <map>
#include <regex>


// A custom exception type
class csvstream_exception {
public:
  const std::string msg;
  csvstream_exception(const std::string &msg) : msg(msg) {};
};


// Read and tokenize one line from a stream
static bool read_csv_line(std::istream &is, std::vector<std::string> &data,
                   char delimiter=',', char newline='\n') {

  // Add entry for first token, start with empty string
  data.clear();
  data.push_back(std::string());

  // Process one character at a time
  char c = '\0';
  enum State {QUOTED, UNQUOTED};
  State state = UNQUOTED;
  bool extracted_anything = false;
  while(is.get(c)) {
    extracted_anything = true;
    switch (state) {
    case UNQUOTED:
      if (c == '"') {
        // Change states when we see a double quote
        state = QUOTED;
      } else if (c == delimiter) {
        data.push_back("");
      } else if (c == newline) {
        // If you see a newline *and it's not within a quoted token*, stop.
        // This will consume the newline character.  NOTE: this won't work
        // properly for windows-style newlines "\n\r".
        goto multilevel_break; //This is a rare example where goto is OK
      } else {
        // Append character to current token
        data.back() += c;
      }
      break;

    case QUOTED:
      if (c == '"') {
        // Change states when we see a double quote
        state = UNQUOTED;
      } else {
        // Append character to current token
        data.back() += c;
      }
      break;

    default:
      assert(0);
      throw state;
    }//switch
  }//while

 multilevel_break:
  // Clear the failbit.  This is to mimic the behavior of getline(), which will
  // set the eofbit, but *not* the failbit if a partial line is read.
  if (extracted_anything) is.clear();

  // Return status is the underlying stream's status
  return static_cast<bool>(is);
}


// csvstream
class csvstream {
public:
  // A header is a vector of column names, in order
  typedef std::vector<std::string> header_type;

  // A row is a map of (column name, datum) pairs from one row
  typedef std::map<std::string, std::string> row_type;

  // Constructor from filename
  csvstream(const std::string &filename, char delimiter=',', char newline='\n')
    : filename(filename), is(fin),
      delimiter(delimiter), newline(newline), line_no(0) {

    // Open file
    fin.open(filename.c_str());
    if (!fin.is_open()) {
      throw csvstream_exception("Error opening file: " + filename);
    }

    // Process header
    read_header();
  }

  // Constructor from stream
  csvstream(std::istream &is, char delimiter=',', char newline='\n')
    : filename("[no filename]"), is(is),
      delimiter(delimiter), newline(newline), line_no(0) {
    read_header();
  }

  // Destructor
  ~csvstream() {
    if (fin.is_open()) fin.close();
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
    // Clear input row
    row.clear();

    // Read one line from stream, bail out if we're at the end
    std::vector<std::string> data;
    if (!read_csv_line(is, data, delimiter, newline)) return *this;
    line_no += 1;

    // Check length of data
    if (data.size() != header.size()) {
      auto msg = "Number of items in row does not match header. " +
        filename + ":L" + std::to_string(line_no) + " " +
        "header.size() = " + std::to_string(header.size()) + " " +
        "row.size() = " + std::to_string(data.size()) + " "
          ;
      throw csvstream_exception(msg);
    }

    // combine data and header into a row object
    for (size_t i=0; i<data.size(); ++i) {
      row[header[i]] = data[i];
    }

    return *this;
  }

private:
  // Filename.  Used for error messages.
  std::string filename;

  // File stream in CSV format, used when library is called with filename ctor
  std::ifstream fin;

  // Stream in CSV format
  std::istream &is;

  // Delimiter between columns
  char delimiter;

  // Delimiter between rows
  char newline;

  // Line no in file.  Used for error messages
  size_t line_no;

  // Store header column names
  header_type header;

  // Process header, the first line of the file
  void read_header() {
    // read first line, which is the header
    if (!read_csv_line(is, header, delimiter, newline)) {
      throw csvstream_exception("error reading header");
    }
  }

  // Disable copying because copying streams is bad!
  csvstream(const csvstream &);
  csvstream & operator= (const csvstream &);
};

#endif
