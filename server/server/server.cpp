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
  : configuration(argc, argv) {
  
  init_logger();
  
  log("Booting Server");
  log("Ctrl-C to shutdown server");

  // Register to handle the signals that indicate when the server should exit.
  auto signals = make_shared<signal_set>(io_service);
  signals->add(SIGINT);
  signals->add(SIGTERM);
#if defined(SIGQUIT)
  signals->add(SIGQUIT);
#endif // defined(SIGQUIT)

  signals->async_wait(
      [this, signals](error_code /*ec*/, int /*signo*/) {
        log(info, "going to shutdown");
        io_service.stop();
      });
}

int server::start() const {
  // Open the acceptor with the option to reuse the address (i.e. SO_REUSEADDR).
  tcp::resolver resolver(io_service);
  tcp::endpoint endpoint = *resolver.resolve({configuration.host, configuration.port});
  
  auto acceptor = make_shared<tcp::acceptor>(io_service);
  acceptor->open(endpoint.protocol());
  acceptor->set_option(tcp::acceptor::reuse_address(true));
  acceptor->bind(endpoint);
  acceptor->listen();
  {
    ostringstream stream;
    stream << "server started at " << configuration.host << " on port " << configuration.port;
    log(stream.str());
  } 
  auto connection_manager = make_shared<http::server::connection_manager>(io_service);
  
  do_accept(connection_manager, acceptor);
  
  io_service.run();

  return errc::success;
}

void server::do_accept(shared_ptr<connection_manager> connection_manager,
    shared_ptr<tcp::acceptor> acceptor) const {

  auto socket = make_shared<tcp::socket>(io_service);
  acceptor->async_accept(*socket,
      [this, connection_manager, acceptor, socket](error_code ec) {
        // Check whether the server was stopped by a signal before this
        // completion handler had a chance to run.
        if (!acceptor->is_open()) {
          return;
        }

        if (!ec) {
          log(info, "connection accepted");
          
          connection_manager->start(socket, make_shared<connection>(*connection_manager));
        }

        do_accept(connection_manager, acceptor);
      });
}

} // namespace server
} // namespace http
