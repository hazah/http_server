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

#include <boost/fusion/include/vector.hpp>
#include <boost/fusion/adapted.hpp>
#include <boost/variant.hpp>
#include <boost/optional.hpp>

//#define BOOST_SPIRIT_DEBUG

#include <boost/spirit/include/qi.hpp>

BOOST_FUSION_ADAPT_STRUCT(
  http::server::request,
  (http::request::method, method)
  (std::string, uri)
  (std::string, version)
)

namespace http {
namespace server {

using namespace boost;
using namespace boost::spirit::qi;
using boost::spirit::qi::on_error;
using boost::spirit::ascii::no_case;
using boost::spirit::ascii::blank_type;
using boost::spirit::ascii::blank;
using boost::spirit::qi::rule;
using namespace std;

namespace {

template <typename Iterator>
struct request_line_grammar : grammar<Iterator, http::server::request(), blank_type> {
  request_line_grammar() : request_line_grammar::base_type(request_line, "request line") {
    method.add("GET",     http::request::method::GET);
    method.add("POST",    http::request::method::POST);
    method.add("PUT",     http::request::method::PUT);
    method.add("DELETE",  http::request::method::DELETE);
    method.add("PATCH",   http::request::method::PATCH);
    method.add("HEAD",    http::request::method::HEAD);
    method.add("CONNECT", http::request::method::CONNECT);
    method.add("OPTIONS", http::request::method::OPTIONS);
    method.add("TRACE",   http::request::method::TRACE);

    uri %= +graph;
    version %= no_case["HTTP/"] >> +char_("0-9.");

    request_line %= lexeme[no_case[method]] >> uri >> version;

    method.name("method");
    uri.name("uri");
    version.name("version");

    BOOST_SPIRIT_DEBUG_NODES((request_line)(uri)(version));
  }

  rule<Iterator, http::server::request(), blank_type> request_line;

  symbols<char,  http::request::method> method;
  rule<Iterator, std::string()>         uri;
  rule<Iterator, std::string()>         version;
};

}

bool request_parser::parse_request_line(request& request, std::string request_line) const {
  typedef std::string::const_iterator iterator;

  iterator position = request_line.begin(),
           end = request_line.end();

  request_line_grammar<iterator> grammar;
  bool result = phrase_parse(position, end, grammar, blank, request);

  if (!result) {
    cerr << "[ERROR] request line parsing stopped at: " << std::string(position, end) << endl;
  }

  return result;
}

namespace {

template <typename Iterator>
struct headers_grammar : grammar<Iterator, request::headers_type(), blank_type> {
  headers_grammar() : headers_grammar::base_type(headers, "headers") {
    name %= +char_("-0-9a-zA-Z");
    value %= +~char_("\r\n");

    headers %= *(name >> ":" >> value >> -lexeme["\r\n"]);

    name.name("name");
    value.name("value");

    BOOST_SPIRIT_DEBUG_NODES((name)(value));
  }

  rule<Iterator, request::headers_type(), blank_type> headers;
  rule<Iterator, std::string()> name, value;
};

}

bool request_parser::parse_headers(request& request, std::string headers) const {
  typedef std::string::const_iterator iterator;

  iterator position = headers.begin(),
           end = headers.end();

  headers_grammar<iterator> grammar;
  bool result = phrase_parse(position, end, grammar, blank, request.headers);

  if (!result) {
    cerr << "[ERROR] request line parsing stopped at: " << std::string(position, end) << endl;
  }

  return result;
  return true;
}

} // namespace server
} // namespace http
