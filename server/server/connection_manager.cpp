//
// connection_manager.cpp
// ~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2016 Ivgeni Slabkovski
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include "connection_manager.hpp"

namespace http {
namespace server {

using namespace boost::asio;
using namespace boost::asio::ip;
using namespace std;

connection_manager::connection_manager(io_service& io_service)
  : io_service_(io_service) {
}

void connection_manager::start(shared_ptr<tcp::socket> socket, connection_ptr c) {
  connections_.insert(c);
  c->start(socket);
}

void connection_manager::stop(shared_ptr<tcp::socket> socket, connection_ptr c) {
  connections_.erase(c);
  c->stop(socket);
}

void connection_manager::stop_all() {
  io_service_.stop();
}

} // namespace server
} // namespace http