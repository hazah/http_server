//
// logger.hpp
// ~~~~~~~~~~
//
// Copyright (c) 2016 Ivgeni Slabkovski
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef HTTP_LOGGER_HPP
#define HTTP_LOGGER_HPP


#include <boost/log/trivial.hpp>
#include <boost/log/sources/logger.hpp>
#include <boost/log/sources/severity_logger.hpp>

namespace http {
namespace server {

using namespace boost::log::trivial;

struct logger {
  mutable boost::log::sources::logger console;
  mutable boost::log::sources::severity_logger<severity_level> severity_logger;

  void init_logger();

  void log(boost::log::trivial::severity_level level, std::string message) const {
    BOOST_LOG_SEV(severity_logger, level) << message;
  }

  void log(std::string message) const {
    BOOST_LOG(console) << message;
  }
};

}
}

#endif