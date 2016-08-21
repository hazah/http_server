#include "config.hpp"
#include <boost/system/error_code.hpp>
#include <boost/filesystem.hpp>
#include <cstdlib>
#include <string>
#include <iostream>

namespace http {
namespace server {

using namespace boost::program_options;
using namespace boost::system;
using namespace boost::filesystem;
using namespace std;

config::config(const int argc, const char* argv[]) {
  auto display_version = [&](bool display) {
    if (display) {
      cout << "0" << endl;
      exit(errc::success);
    }
  };

  auto display_help = [&](bool display) {
    if (display) {
      options_description
          help("Usage: http_server <host> <port> <application_root>");
      
      help.add(information).add(configuration);
      
      cout << help;
      exit(errc::success);
    }
  };

  information.add_options()
      ("version,v", bool_switch()->default_value(false)->notifier(display_version),
          "Print version string and exit.")
      ("help,?", bool_switch()->default_value(false)->notifier(display_help),
          "Display help information and exit.")
      ;
    
  configuration.add_options()
      ("host,h", value<string>(&host)->default_value("0.0.0.0"),
          "The IP address of the host the server is running on.")
      ("port,p", value<string>(&port)->default_value("3000"),
          "The port the server will bind to.")
      ("app-root,r", value<string>(&application_root)->default_value("./"),
          "Application root directory.")
      ;
  
  options.add(information).add(configuration);
  
  variables_map variables;
  
  auto parser = command_line_parser(argc, argv).options(options);
  
  store(parser.run(), variables);
  notify(variables);
  
  path app_root(application_root);
  application_root = canonical(app_root).string();
}

}
}