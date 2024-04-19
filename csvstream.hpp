/* -*- mode: c++ -*- */
#ifndef CSVSTREAM_HPP
#define CSVSTREAM_HPP
/* csvstream.hpp
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
#include <exception>


// A custom exception type
class csvstream_exception : public std::exception {
public:
  const char * what () const noexcept override {
    return msg.c_str();
  }
  const std::string msg;
  csvstream_exception(const std::string &msg) : msg(msg) {};
};


// csvstream interface
class csvstream {
public:
  // Constructor from filename. Throws csvstream_exception if open fails.
  csvstream(const std::string &filename, char delimiter=',', bool strict=true)
    : filename(filename),
      is(fin),
      delimiter(delimiter),
      strict(strict),
      line_no(0) {

    // Open file
    fin.open(filename.c_str());
    if (!fin.is_open()) {
      throw csvstream_exception("Error opening file: " + filename);
    }

    // Process header
    read_header();
  }

  // Constructor from stream
  csvstream(std::istream &is, char delimiter=',', bool strict=true)
    : filename("[no filename]"),
      is(is),
      delimiter(delimiter),
      strict(strict),
      line_no(0) {
    read_header();
  }

  // Destructor
  ~csvstream() {
    if (fin.is_open()) fin.close();
  }

  // Return false if an error flag on underlying stream is set
  explicit operator bool() const {
    return static_cast<bool>(is);
  }

  // Return header processed by constructor
  std::vector<std::string> getheader() const {
    return header;
  }

  // Stream extraction operator reads one row. Throws csvstream_exception if
  // the number of items in a row does not match the header.
  csvstream & operator>> (std::map<std::string, std::string>& row) {
    return extract_row(row);
  }

  // Stream extraction operator reads one row, keeping column order. Throws
  // csvstream_exception if the number of items in a row does not match the
  // header.
  csvstream & operator>> (std::vector<std::pair<std::string, std::string> >& row) {
    return extract_row(row);
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

  // Strictly enforce the number of values in each row.  Raise an exception if
  // a row contains too many values or too few compared to the header.  When
  // strict=false, ignore extra values and set missing values to empty string.
  bool strict;

  // Line no in file.  Used for error messages
  size_t line_no;

  // Store header column names
  std::vector<std::string> header;

  // Disable copying because copying streams is bad!
  csvstream(const csvstream &);
  csvstream & operator= (const csvstream &);

  /////////////////////////////////////////////////////////////////////////////
  // Implementation

  // Read and tokenize one line from a stream
  static bool read_csv_line(std::istream &is,
                            std::vector<std::string> &data,
                            char delimiter
                            ) {

    // Add entry for first token, start with empty string
    data.clear();
    data.push_back(std::string());

    // Process one character at a time
    char c = '\0';
    enum State {BEGIN, QUOTED, QUOTED_ESCAPED, UNQUOTED, UNQUOTED_ESCAPED, END};
    State state = BEGIN;
    while(is.get(c)) {
      switch (state) {
      case BEGIN:
        // We need this state transition to properly handle cases where nothing
        // is extracted.
        state = UNQUOTED;

        // Intended switch fallthrough.  Beginning with GCC7, this triggers an
        // error by default.  Disable the error for this specific line.
#if __GNUG__ && __GNUC__ >= 7
        [[fallthrough]];
#endif

      case UNQUOTED:
        if (c == '"') {
          // Change states when we see a double quote
          state = QUOTED;
        } else if (c == '\\') { //note this checks for a single backslash char
          state = UNQUOTED_ESCAPED;
          data.back() += c;
        } else if (c == delimiter) {
          // If you see a delimiter, then start a new field with an empty string
          data.push_back("");
        } else if (c == '\n' || c == '\r') {
          // If you see a line ending *and it's not within a quoted token*, stop
          // parsing the line.  Works for UNIX (\n) and OSX (\r) line endings.
          // Consumes the line ending character.
          state = END;
        } else {
          // Append character to current token
          data.back() += c;
        }
        break;

      case UNQUOTED_ESCAPED:
        // If a character is escaped, add it no matter what.
        data.back() += c;
        state = UNQUOTED;
        break;

      case QUOTED:
        if (c == '"') {
          // Change states when we see a double quote
          state = UNQUOTED;
        } else if (c == '\\') {
          state = QUOTED_ESCAPED;
          data.back() += c;
        } else {
          // Append character to current token
          data.back() += c;
        }
        break;

      case QUOTED_ESCAPED:
        // If a character is escaped, add it no matter what.
        data.back() += c;
        state = QUOTED;
        break;

      case END:
        if (c == '\n') {
          // Handle second character of a Windows line ending (\r\n).  Do
          // nothing, only consume the character.
        } else {
          // If this wasn't a Windows line ending, then put character back for
          // the next call to read_csv_line()
          is.unget();
        }

        // We're done with this line, so break out of both the switch and loop.
        goto multilevel_break; //This is a rare example where goto is OK
        break;

      default:
        assert(0);
        throw state;

      }//switch
    }//while

  multilevel_break:
    // Clear the failbit if we extracted anything.  This is to mimic the
    // behavior of getline(), which will set the eofbit, but *not* the failbit
    // if a partial line is read.
    if (state != BEGIN) is.clear();

    // Return status is the underlying stream's status
    return static_cast<bool>(is);
  }

  // Process header, the first line of the file
  void read_header() {
    // read first line, which is the header
    if (!read_csv_line(is, header, delimiter)) {
      throw csvstream_exception("error reading header");
    }
  }

  // Extract a row into a map
  csvstream & extract_row(std::map<std::string, std::string>& row) {
    // Clear input row
    row.clear();

    // Read one line from stream, bail out if we're at the end
    std::vector<std::string> data;
    if (!read_csv_line(is, data, delimiter)) return *this;
    line_no += 1;

    // When strict mode is disabled, coerce the length of the data.  If data is
    // larger than header, discard extra values.  If data is smaller than header,
    // pad data with empty strings.
    if (!strict) {
      data.resize(header.size());
    }

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

  // Extract a row into a vector of pairs
  csvstream & extract_row(std::vector<std::pair<std::string, std::string> >& row) {
    // Clear input row
    row.clear();
    row.resize(header.size());

    // Read one line from stream, bail out if we're at the end
    std::vector<std::string> data;
    if (!read_csv_line(is, data, delimiter)) return *this;
    line_no += 1;

    // When strict mode is disabled, coerce the length of the data.  If data is
    // larger than header, discard extra values.  If data is smaller than header,
    // pad data with empty strings.
    if (!strict) {
      data.resize(header.size());
    }

    // Check length of data
    if (row.size() != header.size()) {
      auto msg = "Number of items in row does not match header. " +
        filename + ":L" + std::to_string(line_no) + " " +
        "header.size() = " + std::to_string(header.size()) + " " +
        "row.size() = " + std::to_string(row.size()) + " "
        ;
      throw csvstream_exception(msg);
    }

    // combine data and header into a row object
    for (size_t i=0; i<data.size(); ++i) {
      row[i] = make_pair(header[i], data[i]);
    }

    return *this;
  }
};

#endif
