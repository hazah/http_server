// ~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2016 Ivgeni Slabkovski
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef HTTP_REQUEST_PARSER_HPP
#define HTTP_REQUEST_PARSER_HPP

#include "logger.hpp"
#include <string>

namespace http {
namespace server {

struct request;

/// Parser for incoming requests.
class request_parser : logger {
public:
  bool parse_request_line(request& request, std::string request_line) const;
  bool parse_headers(request& request, std::string headers) const;
};

} // namespace server
} // namespace http

#endif // HTTP_REQUEST_PARSER_HPP
