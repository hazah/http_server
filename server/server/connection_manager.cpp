//
// connection_manager.cpp
// ~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2015 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include "connection_manager.hpp"

namespace http {
namespace server {

using namespace boost::asio;

connection_manager::connection_manager(io_service& io_service)
  : io_service_(io_service) {
}

void connection_manager::start(connection_ptr c) {
  connections_.insert(c);
  c->start();
}

void connection_manager::stop(connection_ptr c) {
  connections_.erase(c);
  c->stop();
}

void connection_manager::stop_all() {
  io_service_.stop();
}

} // namespace server
} // namespace http
