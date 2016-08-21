//
// server.hpp
// ~~~~~~~~~~
//
// Copyright (c) 2003-2015 Christopher M. Kohlhoff (chris at kohlhoff dot com)
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

#include "connection.hpp"
#include "connection_manager.hpp"
#include "request_handler.hpp"

namespace http {
namespace server {

/// The top-level class of the HTTP server.
class server {
public:
  server(const server&) = delete;
  server& operator=(const server&) = delete;

  /// Construct the server to serve up files from the given directory.
  explicit server(const std::string& doc_root);

  /// Run the server's io_service loop and listen on the specified
  /// TCP address and port.
  void start(const std::string& address, const std::string& port);

private:
  /// Perform an asynchronous accept operation.
  void do_accept();

  /// The io_service used to perform asynchronous operations.
  boost::asio::io_service io_service_;

  /// The signal_set is used to register for process termination notifications.
  boost::asio::signal_set signals_;

  /// Acceptor used to listen for incoming connections.
  boost::asio::ip::tcp::acceptor acceptor_;

  /// The connection manager which owns all live connections.
  connection_manager connection_manager_;

  /// The next socket to be accepted.
  boost::asio::ip::tcp::socket socket_;

  /// The handler for all incoming requests.
  request_handler request_handler_;


  mutable boost::log::sources::logger logger;
  mutable boost::log::sources::severity_logger<boost::log::trivial::severity_level> severity_logger;

  void log(boost::log::trivial::severity_level level, std::string message) const {
    BOOST_LOG_SEV(severity_logger, level) << message;
  }

  void log(std::string message) const {
    BOOST_LOG(logger) << message;
  }
};

} // namespace server
} // namespace http

#endif // HTTP_SERVER_HPP
