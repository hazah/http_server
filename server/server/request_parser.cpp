//
// request_parser.cpp
// ~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2016 Ivgeni Slabkovski
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include "request_parser.hpp"
#include "request.hpp"
#include "grammar.hpp"

namespace http {
namespace server {

using namespace std;
using std::string;

bool request_parser::parse_request_line(request& request, string request_line) const {
  typedef string::const_iterator iterator;

  iterator position = request_line.begin(),
           end = request_line.end();

  request_line_grammar<iterator> grammar;
  bool result = phrase_parse(position, end, grammar, blank, request);

  if (!result) {
    log(error, "request line parsing stopped at: " + string(position, end));
  }

  return result;
}

bool request_parser::parse_headers(request& request, string headers) const {
  typedef string::const_iterator iterator;

  iterator position = headers.begin(),
           end = headers.end();

  headers_grammar<iterator> grammar;
  bool result = phrase_parse(position, end, grammar, blank, request.headers);

  if (!result) {
    log(error, "request line parsing stopped at: " + string(position, end));
  }

  return result;
}

} // namespace server
} // namespace http
