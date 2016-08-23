//
// grammar.hpp
// ~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2016 Ivgeni Slabkovski
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef HTTP_GRAMMAR_HPP
#define HTTP_GRAMMAR_HPP

#include "request.hpp"

#include <boost/spirit/include/qi.hpp>

#include <boost/fusion/include/vector.hpp>
#include <boost/fusion/adapted.hpp>
#include <boost/variant.hpp>
#include <boost/optional.hpp>

BOOST_FUSION_ADAPT_STRUCT(
  http::server::request,
  (http::request::method, method)
  (std::string, uri)
  (std::string, version)
)

namespace http {
namespace server {

namespace {

using namespace boost::spirit::qi;
using boost::spirit::qi::on_error;
using boost::spirit::ascii::no_case;
using boost::spirit::ascii::blank_type;
using boost::spirit::ascii::blank;
using boost::spirit::qi::rule;

template <typename Iterator>
struct request_line_grammar : grammar<Iterator, request(), blank_type> {
  request_line_grammar() : request_line_grammar::base_type(request_line, "request line") {
    method.add("get",     http::request::method::GET);
    method.add("post",    http::request::method::POST);
    method.add("put",     http::request::method::PUT);
    method.add("delete",  http::request::method::DELETE);
    method.add("patch",   http::request::method::PATCH);
    method.add("head",    http::request::method::HEAD);
    method.add("connect", http::request::method::CONNECT);
    method.add("options", http::request::method::OPTIONS);
    method.add("trace",   http::request::method::TRACE);

    uri %= +graph;
    version %= no_case["HTTP/"] >> +char_("0-9.");

    request_line %= lexeme[no_case[method]] >> uri >> version;

    method.name("method");
    uri.name("uri");
    version.name("version");

    BOOST_SPIRIT_DEBUG_NODES((request_line)(uri)(version));
  }

  rule<Iterator, request(), blank_type> request_line;

  symbols<char,  http::request::method> method;
  rule<Iterator, std::string()>         uri;
  rule<Iterator, std::string()>         version;
};

template <typename Iterator>
struct headers_grammar : grammar<Iterator, http::request::headers_type(), blank_type> {
  headers_grammar() : headers_grammar::base_type(headers, "headers") {
    name %= +char_("-0-9a-zA-Z");
    value %= +~char_("\r\n");

    headers %= *(name >> ":" >> value >> lexeme["\r\n"]);

    name.name("name");
    value.name("value");

    BOOST_SPIRIT_DEBUG_NODES((name)(value));
  }

  rule<Iterator, http::request::headers_type(), blank_type> headers;
  rule<Iterator, std::string()> name, value;
};

}


}
}

#endif
