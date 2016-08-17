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

#include "header.hpp"

namespace http {
namespace request {

enum class method {GET, POST, PUT, DELETE, PATCH, HEAD, CONNECT, OPTIONS, TRACE};

struct asterisk {};

struct path {
  std::string value;
  
  path(std::string value) : value(value) {}
  operator std::string() const { return value; }
};

struct query {
  std::string value;
  
  query(std::string value) : value(value) {}
  operator std::string() const { return value; }
};

typedef boost::variant<
  asterisk,
  path,
  query
    > uri;

struct version {
  std::string major;
  std::string minor;
};

struct host {
  std::string value;
  
  host(std::string value) : value(value) {}
  operator std::string() const { return value; }
};

struct accept {
  std::string value;
  
  accept(std::string value) : value(value) {}
  operator std::string() const { return value; }
};

struct content_length {
  size_t value;
  
  content_length(size_t value) : value(value) {}
  operator size_t() const { return value; }
};

typedef boost::variant<
  host,
  accept,
  content_length
    > headers;
}


namespace server {

/// A request received from a client.
struct request {
  //http::request::method  method;
  std::string method;
  //http::request::uri     uri;
  std::string uri;
  ///http::request::version http_version;
  int http_version_major;
  int http_version_minor;
  //http::request::headers headers;
  std::vector<http::server::header> headers;
  std::string            payload;
};

} // namespace server
} // namespace http

#endif // HTTP_REQUEST_HPP
