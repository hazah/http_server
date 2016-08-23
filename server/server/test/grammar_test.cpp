#define BOOST_TEST_MODULE test request grammar
#include <boost/test/unit_test.hpp>

#include <string>
#include "../request.hpp"
#include "../grammar.hpp"

BOOST_AUTO_TEST_CASE(test_request_line_grammar) {
  std::string request_line = "GET /resource/1?param=value HTTP/1.1";

  typedef std::string::const_iterator iterator;

  iterator position = request_line.begin(),
           end = request_line.end();

  http::server::request_line_grammar<iterator> grammar;
  http::server::request request;

  BOOST_TEST((true == boost::spirit::qi::phrase_parse(position, end, grammar,
      boost::spirit::ascii::blank, request)));
}

BOOST_AUTO_TEST_CASE(test_request_headers) {
  std::string headers = 
    "Host: 0.0.0.0:3000\r\n"
    "User-Agent: Mozilla/5.0 (X11; Ubuntu; Linux x86_64; rv:48.0) Gecko/20100101 Firefox/48.0\r\n"
    "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8\r\n"
    "Accept-Language: en-US,en;q=0.5\r\n"
    "Accept-Encoding: gzip, deflate\r\n"
    "Connection: keep-alive\r\n"
    "Upgrade-Insecure-Requests: 1\r\n"
    "Cache-Control: max-age=0\r\n"
    "\r\n"  
    ;

  typedef std::string::const_iterator iterator;

  iterator position = headers.begin(),
           end = headers.end();

  http::server::headers_grammar<iterator> grammar;
  http::server::request request;
  
  BOOST_TEST((true == boost::spirit::qi::phrase_parse(position, end, grammar,
      boost::spirit::ascii::blank, request.headers)));
}

