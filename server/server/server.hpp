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

#include <boost/asio.hpp>
#include <string>
#include <boost/log/trivial.hpp>
#include <boost/log/sources/logger.hpp>
#include <boost/log/sources/severity_logger.hpp>

#include "logger.hpp"
#include "config.hpp"
#include "connection.hpp"
#include "connection_manager.hpp"
#include "request_handler.hpp"

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
  void do_accept() const;

  config configuration;

  /// The io_service used to perform asynchronous operations.
  mutable boost::asio::io_service io_service_;

  /// The signal_set is used to register for process termination notifications.
  mutable boost::asio::signal_set signals_;

  /// Acceptor used to listen for incoming connections.
  mutable boost::asio::ip::tcp::acceptor acceptor_;

  /// The connection manager which owns all live connections.
  mutable connection_manager connection_manager_;

  /// The next socket to be accepted.
  mutable boost::asio::ip::tcp::socket socket_;

  /// The handler for all incoming requests.
  mutable request_handler request_handler_;
};

} // namespace server
} // namespace http

#endif // HTTP_SERVER_HPP
