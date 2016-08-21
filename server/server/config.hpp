//
// request.hpp
// ~~~~~~~~~~~
//
// Copyright (c) 2016 Ivgeni Slabkovski
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef HTTP_CONFIG_HPP
#define HTTP_CONFIG_HPP

#include <boost/program_options.hpp>


namespace http {
namespace server {

struct config {
  config(const int, const char*[]);
	boost::program_options::options_description information, configuration, options;
	std::string host, port, application_root;
};

}
}


#endif