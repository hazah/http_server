//
// server.cpp
// ~~~~~~~~~~
//
// Copyright (c) 2016 Ivgeni Slabkovski
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include "server.hpp"

#include <signal.h>
#include <utility>
#include <map>
#include <string>
#include <sstream>

namespace http {
namespace server {

using namespace boost::system;
using namespace boost::asio;
using namespace boost::asio::ip;
using namespace std;

using boost::system::error_code;

server::server(const int argc, const char* argv[])
  : configuration(argc, argv),
    io_service_(),
    signals_(io_service_),
    acceptor_(io_service_),
    connection_manager_(io_service_),
    socket_(io_service_),
    request_handler_() {
  
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

int server::start() const {
  // Open the acceptor with the option to reuse the address (i.e. SO_REUSEADDR).
  tcp::resolver resolver(io_service_);
  tcp::endpoint endpoint = *resolver.resolve({configuration.host, configuration.port});
  
  acceptor_.open(endpoint.protocol());
  acceptor_.set_option(tcp::acceptor::reuse_address(true));
  acceptor_.bind(endpoint);
  acceptor_.listen();
  {
    ostringstream stream;
    stream << "server started at " << configuration.host << " on port " << configuration.port;
    log(stream.str());
  }  
  do_accept();
  
  io_service_.run();

  return errc::success;
}

void server::do_accept() const {
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
