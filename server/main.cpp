//
// main.cpp
// ~~~~~~~~
//
// Copyright (c) 2016 Ivgeni Slabkovski 
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <cstdlib>
#include <iostream>
#include <string>
#include <boost/asio.hpp>
#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>
#include "server.hpp"


int main(int argc, const char* argv[]) {
  using namespace http::server;
  using namespace boost::program_options;
  using namespace boost::filesystem;
  using namespace std;

  try {
    // Check command line arguments.
    options_description info, config, cl_options;
    
    info.add_options()
        ("version,v", bool_switch()->default_value(false)->notifier(
                [&](bool display) {
                  if (display) {
                    cerr << "0" << endl;
                    exit(0);
                  }
                }
            ),
            "Print version string and exit.")
        ("help,?", bool_switch()->default_value(false)->notifier(
                [&](bool display) {
                  if (display) {
                    options_description
                        help("Usage: http_server <host> <port> <application_root>");
                    
                    help.add(info).add(config);
                    
                    cerr << help;
                    exit(0);
                  }
                }
            ),
            "Display help information and exit.")
        ;
    
    string host, port, application_root;
    
    config.add_options()
        ("host,h", value<string>(&host)->default_value("0.0.0.0"),
            "The IP address of the host the server is running on.")
        ("port,p", value<string>(&port)->default_value("3000"),
            "The port the server will bind to.")
        ("app-root,r", value<string>(&application_root)->default_value("./"),
            "Application root directory.")
        ;
    
    cl_options.add(info).add(config);
    
    variables_map cl_vars;
    
    auto cl_parser = command_line_parser(argc, argv).options(cl_options);
    
    store(cl_parser.run(), cl_vars);
    notify(cl_vars);
    
    path app_root(application_root);
    application_root = canonical(app_root).string();
    
    cerr << "[INFO]: initializing http_server[" << host << ":" << port << ":"
         << application_root << "]" << endl;

    // Initialize the server.
    server http_server(host, port, application_root);

    // Run the server until stopped.
    http_server.run();
  } catch (exception& error) {
    cerr << "error: " << error.what() << "\n";
    return 1;
  }
}
