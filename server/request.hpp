//
// request.hpp
// ~~~~~~~~~~~
//
// Copyright (c) 2003-2015 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef HTTP_REQUEST_HPP
#define HTTP_REQUEST_HPP

#include <string>
#include <vector>

#include <boost/variant.hpp>
#include <boost/optional.hpp>
#include <boost/fusion/include/vector.hpp>

#include "header.hpp"

namespace http {
namespace request {

enum class method {GET, POST, PUT, DELETE, PATCH, HEAD, CONNECT, OPTIONS, TRACE};

struct asterisk {};

struct url {
  std::string scheme;
  std::string host;
  std::string path;
  boost::optional<std::string> query;
};

typedef boost::variant<
  asterisk,
  url
    > uri;

typedef boost::fusion::vector<std::string, std::string> version;
  
struct host {
  std::string value;
  
  host(std::string value) : value(value) {}
  host& operator= (std::string new_value) { value = new_value; return *this; }
  operator std::string() const { return value; }
};

struct accept {
  std::string value;
  
  accept(std::string value) : value(value) {}
  accept& operator= (std::string new_value) { value = new_value; return *this; }
  operator std::string() const { return value; }
};

struct content_length {
  size_t value;
  
  content_length(size_t value) : value(value) {}
  content_length& operator= (size_t new_value) { value = new_value; return *this; }
  operator size_t() const { return value; }
};

typedef boost::variant<
  host,
  accept,
  content_length
    > header;

typedef std::vector<header> headers;

}


namespace server {

/// A request received from a client.
struct request {
  http::request::method  method;
  http::request::uri     uri;
  http::request::version http_version;
  http::request::headers headers;
  std::string            payload;
};

} // namespace server
} // namespace http

#endif // HTTP_REQUEST_HPP
