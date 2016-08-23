//
// main.cpp
// ~~~~~~~~
//
// Copyright (c) 2016 Ivgeni Slabkovski 
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include "server.hpp"
#include <iostream>

using namespace http::server;
using namespace std;

int main(int argc, const char* argv[]) try {
  // Run the server until stopped.
  return server(argc, argv).start();
} catch (exception& error) {
  cerr << "[ERROR]: " << error.what() << endl;
  return -1;
}
