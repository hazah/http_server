//
// request_parser.cpp
// ~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2015 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include "request_parser.hpp"
#include "request.hpp"

#include <boost/fusion/adapted.hpp>
#include <boost/spirit/include/qi.hpp>

BOOST_FUSION_ADAPT_STRUCT(
  http::request::url,
  (std::string, scheme)
  (std::string, host)
  (std::string, path)
  (boost::optional<std::string>, query)
)

BOOST_FUSION_ADAPT_STRUCT(
  http::server::request,
  (http::request::method, method)
  (http::request::url, url)
  (http::request::version, version)
)

namespace http {
namespace server {

using namespace boost::spirit::qi;
using namespace boost::spirit::ascii;
using namespace std;

namespace {

template <typename Iterator>
struct request_line_grammar : grammar<Iterator, request(), boost::spirit::ascii::space_type> {
  request_line_grammar() : request_line_grammar::base_type(start) {

  }

  rule<Iterator, request(), boost::spirit::ascii::space_type> start;
};

}

request_parser::request_parser()
  : state_(method_start) {
}

void request_parser::reset() {
}

bool request_parser::parse_request_line(request& request, std::string request_line) const {
  return true;
}

bool request_parser::parse_headers(request& request, std::string headers) const {
  return true;
}

} // namespace server
} // namespace http
