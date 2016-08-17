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
            cerr << "[INFO] writing request" << endl;
            
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
                    
          cerr << "[INFO] request line: " << request_line << endl;
          
          bool result  = request_parser_.parse_request_line(
              *request, request_line);
          
          if (result) {
            async_read_until(socket_, *header_buffer, "\r\n\r\n",
                [this, self = shared_from_this(), header_buffer, request, reply, write](
                    error_code code, size_t bytes) {
                  if (!code) {
                    ostringstream output_stream;
                   
                    output_stream << &*header_buffer;
                    
                    cerr << "[INFO] bytes in buffer: " << output_stream.str().length() << endl;
                    cerr << "[INFO] bytes to retrieve: " << bytes << endl;
                    
                    // extract a copy directly from the underlying string, leave
                    // buffer intact in case there is more content past the delimiter
                    string headers = output_stream.str().substr(0, bytes - ("\r\n"s).length());
                    
                    cerr << "[INFO] bytes in headers: " << headers.length() << endl;
                    
                    cerr << "[INFO] headers: " << endl << endl << headers << endl;
                    //cerr << "[INFO] content length: " << request->headers.content_length << endl;
                    
                    bool result = request_parser_.parse_headers(
                        *request, headers);
                    
                    if (result) {
                      auto handle_and_write = [this, request, reply, write]() {
                        cerr << "[INFO] handling request" << endl;
                        request_handler_.handle_request(*request, *reply);
                        write();
                      };
                      
                      if (2/*request->headers.content_length*/ > 0) {
                        request->payload.resize(2/*request->headers.content_length*/);
                        
                        // part of the payload may have already been recieved
                        request->payload = output_stream.str().substr(bytes);
                        
                        if (request->payload.length() < 2/*request->headers.content_length*/) {
                          size_t remaining_length =
                              2/*request->headers.content_length*/ - request->payload.length();
                          
                          auto rest = make_shared<vector<char>>();
                          rest->resize(remaining_length);
                          
                          async_read(socket_, buffer(*rest, remaining_length),
                              transfer_exactly(remaining_length),
                              [this, self = shared_from_this(), request, rest,
                                  handle_and_write](error_code code,
                                      size_t bytes) {
                                if (!code) {
                                  copy(*rest, back_inserter(request->payload));
                                      
                                  cerr << "[INFO] payload: " << endl << endl << request->payload << endl;
                                  
                                  handle_and_write();
                                }
                                else if (code != error::operation_aborted) {
                                  connection_manager_.stop(shared_from_this());
                                }
                              });
                        }
                        else {
                          cerr << "[INFO] payload: " << endl << endl << request->payload << endl << endl;
                          handle_and_write();
                        }
                      }
                      else {
                        handle_and_write();
                      }
                    }
                    else {
                      cerr << "[ERROR] cannot parse headers" << endl;
                      
                      *reply = reply::stock_reply(reply::bad_request);
                      write();
                    }
                  }
                  else if (code != error::operation_aborted) {
                    connection_manager_.stop(shared_from_this());
                  }
                });        
          } else {
            cerr << "[ERROR] cannot parse request line" << endl;
            
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
