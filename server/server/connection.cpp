//
// connection.cpp
// ~~~~~~~~~~~~~~
//
// Copyright (c) 2016 Ivgeni Slabkovski
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
using std::shared_ptr;

connection::connection(connection_manager& manager)
  : connection_manager_(manager) {
}

void connection::start(shared_ptr<tcp::socket> socket) {
  auto header_buffer = make_shared<streambuf>();
  
  async_read_until(*socket, *header_buffer, "\r\n",
      [this, self = shared_from_this(), socket, header_buffer](
          error_code code, size_t bytes) {
        if (!code) {
          auto request = make_shared<http::server::request>();
          auto reply   = make_shared<http::server::reply>();
          
          auto write = [this, socket, reply]() {
            log(info, "writing request");
            
            async_write(*socket, reply->to_buffers(),
                [this, self = shared_from_this(), socket](error_code code, size_t) {
                  if (!code) {
                    // Initiate graceful connection closure.
                    error_code ignored_code;
                    socket->shutdown(tcp::socket::shutdown_both, ignored_code);
                  }

                  if (code != error::operation_aborted) {
                    connection_manager_.stop(socket, shared_from_this());
                  }
                });
            };
            
          istream input_stream(&*header_buffer);
          string request_line;
          
          getline(input_stream, request_line);
                    
          {
            ostringstream stream;
            stream << "request line: " << request_line;
            log(debug, stream.str());
          }
          
          bool result = request_parser_.parse_request_line(
              *request, request_line);
          
          if (result) {
            async_read_until(*socket, *header_buffer, "\r\n\r\n",
                [this, self = shared_from_this(), socket, header_buffer,
                    request, reply, write](error_code code, size_t bytes) {
                  if (!code) {
                    ostringstream output_stream;
                   
                    output_stream << &*header_buffer;
                    {
                      ostringstream stream;
                      stream << "bytes in buffer: " << output_stream.str().length();
                      log(trace, stream.str());
                    }
                    {
                      ostringstream stream;
                      stream << "bytes to retrieve: " << bytes;
                      log(debug, stream.str());
                    }
                    // extract a copy directly from the underlying string, leave
                    // buffer intact in case there is more content past the delimiter
                    string headers = output_stream.str().substr(0, bytes - ("\r\n"s).length());
                    
                    {
                      ostringstream stream;
                      stream << "bytes in headers: " << headers.length();
                      log(trace, stream.str());
                    }
                    {
                      ostringstream stream;
                      stream << "headers: " << endl << endl << headers;
                      log(debug, stream.str());
                    }
                    
                    bool result = request_parser_.parse_headers(
                        *request, headers);
                    
                    if (request->headers.count("content-length")) {
                      ostringstream stream;
                      stream << "content length: " << request->headers["content-length"];
                      log(debug, stream.str());
                    }
                    
                    if (result) {
                      auto handle_and_write = [this, request, reply, write]() {
                        log(trace, "handling request");
                        request_handler().handle_request(*request, *reply);
                        write();
                      };

                      size_t content_length = request->headers.count("content-length") ?
                          lexical_cast<size_t>(request->headers["content-length"]) : 0;
                      
                      if (content_length > 0) {
                        request->payload.reserve(content_length);

                        // part of the payload may have already been recieved
                        copy(output_stream.str().substr(bytes),
                            back_inserter(request->payload));
                        
                        if (request->payload.length() < content_length) {
                          size_t remaining_bytes =
                              content_length - request->payload.length();
                          
                          auto rest = make_shared<vector<char>>(remaining_bytes);
                          
                          async_read(*socket, buffer(*rest, remaining_bytes),
                              transfer_exactly(remaining_bytes),
                              [this, self = shared_from_this(), socket, request, rest,
                                  handle_and_write](error_code code,
                                      size_t bytes) {
                                if (!code) {
                                  copy(*rest, back_inserter(request->payload));
                                      
                                  log(debug, "payload: \n\n"s + request->payload + "\n");
                                  
                                  handle_and_write();
                                }
                                else if (code != error::operation_aborted) {
                                  connection_manager_.stop(socket, shared_from_this());
                                }
                              });
                        }
                        else {
                          log(debug, "payload: \n\n"s + request->payload + "\n");
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
                    connection_manager_.stop(socket, shared_from_this());
                  }
                });        
          } else {
            log(boost::log::trivial::error, "cannot parse request line");
            
            *reply = reply::stock_reply(reply::bad_request);
            write();
          }
        }
        else if (code != error::operation_aborted) {
          connection_manager_.stop(socket, shared_from_this());
        }
      });
}

void connection::stop(shared_ptr<tcp::socket> socket) {
  socket->close();
}

} // namespace server
} // namespace http