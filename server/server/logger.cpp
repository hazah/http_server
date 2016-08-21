//
// logger.cpp
// ~~~~~~~~~~
//
// Copyright (c) 2016 Ivgeni Slabkovski
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include "logger.hpp"

#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/console.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/utility/formatting_ostream.hpp>
#include <boost/log/utility/manipulators/to_log.hpp>

#include <boost/log/expressions.hpp>

#include <map>

namespace http {
namespace server {


using namespace boost;
using namespace boost::log;
using namespace boost::log::sources;
using namespace boost::log::sinks;
using namespace boost::log::keywords;
using namespace boost::log::sinks;
using namespace boost::log::trivial;
using namespace boost::log::expressions;

using namespace std;

using boost::log::trivial::severity;
using boost::log::keywords::filter;
using boost::log::keywords::format;

namespace {

struct severity_tag;

formatting_ostream& operator<<(formatting_ostream& stream,
    const to_log_manip<severity_level, severity_tag>& manip) {
  map<severity_level, string> mapping;
  
  mapping[trace] = "TRACE";
  mapping[debug] = "DEBUG";
  mapping[info] = "INFO";
  mapping[warning] = "WARNING";
  mapping[severity_level::error] = "ERROR";
  mapping[fatal] = "FATAL";

  return stream << mapping[manip.get()];
}

}


void logger::init_logger() {
  auto severity = attr<severity_level, severity_tag>("Severity");
  auto frmt = stream << "[" << severity << "] " << smessage;
  
  add_file_log(
    file_name = "logs/development.log",
    auto_flush = true,
    format = frmt,
    filter = has_attr("Severity"));
  add_console_log(
    clog,
    auto_flush = true,
    format = frmt,
    filter = has_attr("Severity"));
  add_console_log(cout,
    auto_flush = true,
    format = stream << "=> " << smessage,
    filter = !has_attr("Severity"));
  
  add_common_attributes();
}

}
}