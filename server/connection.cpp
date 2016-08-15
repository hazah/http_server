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
#include "connection_manager.hpp"
#include "request_handler.hpp"

namespace http {
namespace server {

using boost::system::error_code;
using namespace boost::asio;
using namespace boost::asio::ip;
using boost::asio::streambuf;
using namespace std;

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
                    
          bool result  = request_parser_.parse_request_line(
              *request, request_line);
          
          if (result) {
            async_read_until(socket_, *header_buffer, "\r\n\r\n",
                [this, self = shared_from_this(), header_buffer, request, reply, write](
                    error_code code, size_t bytes) {
                  if (!code) {
                    ostringstream output_stream;
                   
                    output_stream << &*header_buffer;
                    
                    // extract a copy directly from the underlying string, leave
                    // buffer intact in case there is more content past the delimiter
                    string headers = output_stream.str().substr(0, bytes - ("\r\n"s).length());
                    
                    bool result = request_parser_.parse_headers(
                        *request, headers);
                    
                    if (result) {
                      if (request->headers.content_length > 0) {
                        request->payload.resize(request->headers.content_length);
                        
                        // part of the payload may have already been recieved
                        request->payload = output_stream.str().substr(bytes);
                        
                        auto handle_and_write = [this, request, reply, write]() {
                          request_handler_.handle_request(*request, *reply);
                          write();
                        };
                        
                        if (request->payload.length() < request->headers.content_length) {
                          size_t remaining_length =
                              request->headers.content_length - request->payload.length();
                          
                          auto rest = make_shared<string>();
                          rest->resize(remaining_length);
                          
                          async_read(socket_, buffer(&(*rest)[0], remaining_length),
                              transfer_exactly(remaining_length),
                              [this, self = shared_from_this(), request, rest,
                                  handle_and_write](error_code code,
                                      size_t bytes) {
                                if (!code) {
                                  request->payload += *rest;
                                  handle_and_write();
                                }
                                else if (code != error::operation_aborted) {
                                  connection_manager_.stop(shared_from_this());
                                }
                              });
                        }
                        else {
                          handle_and_write();
                        }
                      }
                    }
                    else {
                      *reply = reply::stock_reply(reply::bad_request);
                      write();
                    }
                  }
                  else if (code != error::operation_aborted) {
                    connection_manager_.stop(shared_from_this());
                  }
                });        
          } else {
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
