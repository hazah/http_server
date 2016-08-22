//
// connection.hpp
// ~~~~~~~~~~~~~~
//
// Copyright (c) 2016 Ivgeni Slabkovski
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef HTTP_CONNECTION_HPP
#define HTTP_CONNECTION_HPP

#include <array>
#include <memory>
#include <boost/asio.hpp>
#include "logger.hpp"
#include "reply.hpp"
#include "request.hpp"
#include "request_handler.hpp"
#include "request_parser.hpp"

namespace http {
namespace server {

class connection_manager;

/// Represents a single connection from a client.
class connection
  : logger, public std::enable_shared_from_this<connection> {
public:
  connection(const connection&) = delete;
  connection& operator=(const connection&) = delete;

  /// Construct a connection with the given socket.
  explicit connection(connection_manager& manager);

private:

  /// Start the first asynchronous operation for the connection.
  void start(std::shared_ptr<boost::asio::ip::tcp::socket> socket);

  /// Stop all asynchronous operations associated with the connection.
  void stop(std::shared_ptr<boost::asio::ip::tcp::socket> socket);
  
  /// The manager for this connection.
  connection_manager& connection_manager_;

  /// The parser for the incoming request.
  request_parser request_parser_;

  friend class connection_manager;
};

typedef std::shared_ptr<connection> connection_ptr;

} // namespace server
} // namespace http

#endif // HTTP_CONNECTION_HPP
