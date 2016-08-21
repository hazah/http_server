//
// server.cpp
// ~~~~~~~~~~
//
// Copyright (c) 2003-2015 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include "server.hpp"

#include <signal.h>
#include <utility>
#include <iostream>
#include <map>
#include <string>

namespace http {
namespace server {

using boost::system::error_code;
using namespace boost::asio;
using namespace boost::asio::ip;
using namespace std;


server::server(const std::string& app_root)
  : io_service_(),
    signals_(io_service_),
    acceptor_(io_service_),
    connection_manager_(io_service_),
    socket_(io_service_),
    request_handler_(app_root) {
  init_logger();
  log("Booting Server");
  log("Ctrl-C to shutdown server");

  // Register to handle the signals that indicate when the server should exit.
  signals_.add(SIGINT);
  signals_.add(SIGTERM);
#if defined(SIGQUIT)
  signals_.add(SIGQUIT);
#endif // defined(SIGQUIT)

  signals_.async_wait(
      [this](error_code /*ec*/, int /*signo*/) {
        log(info, "going to shutdown");
        io_service_.stop();
      });
}

void server::start(const string& address, const string& port) {
  // Open the acceptor with the option to reuse the address (i.e. SO_REUSEADDR).
  tcp::resolver resolver(io_service_);
  tcp::endpoint endpoint = *resolver.resolve({address, port});
  
  acceptor_.open(endpoint.protocol());
  acceptor_.set_option(tcp::acceptor::reuse_address(true));
  acceptor_.bind(endpoint);
  acceptor_.listen();

  log("server started at " + address + " on port " + port);
  
  do_accept();
  
  io_service_.run();
}

void server::do_accept() {
  acceptor_.async_accept(socket_,
      [this](error_code ec) {
        // Check whether the server was stopped by a signal before this
        // completion handler had a chance to run.
        if (!acceptor_.is_open()) {
          return;
        }

        if (!ec) {
          log(info, "connection accepted");
          
          connection_manager_.start(make_shared<connection>(move(socket_),
              connection_manager_, request_handler_));
        }

        do_accept();
      });
}

} // namespace server
} // namespace http
