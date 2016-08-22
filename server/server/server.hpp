//
// server.hpp
// ~~~~~~~~~~
//
// Copyright (c) 2016 Ivgeni Slabkovski
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef HTTP_SERVER_HPP
#define HTTP_SERVER_HPP

#include "logger.hpp"
#include "config.hpp"
#include "connection_manager.hpp"
#include "request_handler.hpp"

#include <boost/asio.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/sources/logger.hpp>
#include <boost/log/sources/severity_logger.hpp>

#include <string>
#include <utility>

namespace http {
namespace server {

/// The top-level class of the HTTP server.
class server : logger {
public:
  server(const server&) = delete;
  server& operator=(const server&) = delete;

  /// Construct the server to serve up files based on the given options.
  explicit server(int argc, const char* argv[]);

  /// Run the server's loop.
  int start() const;

private:
  /// Perform an asynchronous accept operation.
  void do_accept(std::shared_ptr<http::server::connection_manager> connection_manager,
      std::shared_ptr<boost::asio::ip::tcp::acceptor> acceptor) const;

  config configuration;

  /// The io_service used to perform asynchronous operations.
  mutable boost::asio::io_service io_service;
};

} // namespace server
} // namespace http

#endif // HTTP_SERVER_HPP
