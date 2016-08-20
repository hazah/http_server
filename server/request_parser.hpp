//
// request_parser.hpp
// ~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2015 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef HTTP_REQUEST_PARSER_HPP
#define HTTP_REQUEST_PARSER_HPP

#include <string>

namespace http {
namespace server {

struct request;

/// Parser for incoming requests.
class request_parser {
public:
  bool parse_request_line(request& request, std::string request_line) const;
  bool parse_headers(request& request, std::string headers) const;
};

} // namespace server
} // namespace http

#endif // HTTP_REQUEST_PARSER_HPP
