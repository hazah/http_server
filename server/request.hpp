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
#include <map>
#include <ostream>


#include "header.hpp"

namespace http {

namespace request {

enum class method {GET, POST, PUT, DELETE, PATCH, HEAD, CONNECT, OPTIONS, TRACE};

inline std::ostream& operator<<(std::ostream& os, method m) {
  return os << static_cast<std::underlying_type<method>::type>(m);
}

}

namespace server {

/// A request received from a client.
struct request {
  http::request::method method;
  std::string uri;
  std::string version;

  std::map<std::string, std::string> headers;
  
  std::string payload;
};

} // namespace server
} // namespace http

#endif // HTTP_REQUEST_HPP
