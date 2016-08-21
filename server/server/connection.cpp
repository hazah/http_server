//
// connection.cpp
// ~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2015 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include "connection.hpp"

#include <utility>
#include <vector>
#include <sstream>
#include <string>
#include <iterator>
#include <iostream>

#include <boost/range/algorithm.hpp>
#include <boost/lexical_cast.hpp>

#include "connection_manager.hpp"
#include "request_handler.hpp"

namespace http {
namespace server {

using namespace boost;
using boost::system::error_code;
using namespace boost::asio;
using namespace boost::asio::ip;
using boost::asio::streambuf;
using namespace std;
using std::move;

connection::connection(tcp::socket&& socket,
    connection_manager& manager, request_handler& handler)
  : socket_(move(socket)),
    connection_manager_(manager),
    request_handler_(handler) {
}

void connection::start() {
  auto header_buffer = make_shared<streambuf>();
  
  async_read_until(socket_, *header_buffer, "\r\n",
      [this, self = shared_from_this(), header_buffer](
          error_code code, size_t bytes) {
        if (!code) {
          auto request = make_shared<http::server::request>();
          auto reply   = make_shared<http::server::reply>();
          
          auto write = [this, reply]() {
            log(info, "writing request");
            
            async_write(socket_, reply->to_buffers(),
                [this, self = shared_from_this()](error_code code, size_t) {
                  if (!code) {
                    // Initiate graceful connection closure.
                    error_code ignored_code;
                    socket_.shutdown(tcp::socket::shutdown_both, ignored_code);
                  }

                  if (code != error::operation_aborted) {
                    connection_manager_.stop(shared_from_this());
                  }
                });
            };
            
          istream input_stream(&*header_buffer);
          string request_line;
          
          getline(input_stream, request_line);
                    
          log(info, "request line: " + request_line);
          
          bool result = request_parser_.parse_request_line(
              *request, request_line);
          
          if (result) {
            async_read_until(socket_, *header_buffer, "\r\n\r\n",
                [this, self = shared_from_this(), header_buffer, request, reply, write](
                    error_code code, size_t bytes) {
                  if (!code) {
                    ostringstream output_stream;
                   
                    output_stream << &*header_buffer;
                    
                    log(trace, "bytes in buffer: " + output_stream.str().length());
                    log(debug, "bytes to retrieve: " + bytes);
                    
                    // extract a copy directly from the underlying string, leave
                    // buffer intact in case there is more content past the delimiter
                    string headers = output_stream.str().substr(0, bytes - ("\r\n"s).length());
                    
                    log(trace, "bytes in headers: " + headers.length());
                    log(debug, "headers: \n\n" + headers + "\n");
                    
                    bool result = request_parser_.parse_headers(
                        *request, headers);
                    
                    if (request->headers.count("content-length"))
                      log(trace, "content length: " + request->headers["content-length"]);
                    
                    if (result) {
                      auto handle_and_write = [this, request, reply, write]() {
                        log(trace, "handling request");
                        request_handler_.handle_request(*request, *reply);
                        write();
                      };

                      size_t content_length = request->headers.count("content-length") ?
                          lexical_cast<size_t>(request->headers["content-length"]) : 0;
                      
                      if (content_length > 0) {
                        request->payload.resize(content_length);
                        
                        // part of the payload may have already been recieved
                        request->payload = output_stream.str().substr(bytes);
                        
                        if (request->payload.length() < content_length) {
                          size_t remaining_bytes =
                              content_length - request->payload.length();
                          
                          auto rest = make_shared<vector<char>>();
                          rest->resize(remaining_bytes);
                          
                          async_read(socket_, buffer(*rest, remaining_bytes),
                              transfer_exactly(remaining_bytes),
                              [this, self = shared_from_this(), request, rest,
                                  handle_and_write](error_code code,
                                      size_t bytes) {
                                if (!code) {
                                  copy(*rest, back_inserter(request->payload));
                                      
                                  log(debug, "payload: \n\n" + request->payload);
                                  
                                  handle_and_write();
                                }
                                else if (code != error::operation_aborted) {
                                  connection_manager_.stop(shared_from_this());
                                }
                              });
                        }
                        else {
                          log(debug, "payload: \n\n" + request->payload);
                          handle_and_write();
                        }
                      }
                      else {
                        handle_and_write();
                      }
                    }
                    else {
                      log(boost::log::trivial::error, "cannot parse headers");
                      
                      *reply = reply::stock_reply(reply::bad_request);
                      write();
                    }
                  }
                  else if (code != error::operation_aborted) {
                    connection_manager_.stop(shared_from_this());
                  }
                });        
          } else {
            log(boost::log::trivial::error, "cannot parse request line");
            
            *reply = reply::stock_reply(reply::bad_request);
            write();
          }
        }
        else if (code != error::operation_aborted) {
          connection_manager_.stop(shared_from_this());
        }
      });
}

void connection::stop() {
  socket_.close();
}

} // namespace server
} // namespace http
