#include <asio.hpp>
#include <asio/ssl.hpp>
#include <iostream>
#include <thread>
#include "HttpServer.h"

using namespace std;

HttpServer::HttpServer() {};

asio::awaitable<void> HttpServer::handle_request(asio::ssl::stream<asio::ip::tcp::socket> socket) {
  try {
    co_await socket.async_handshake(asio::ssl::stream_base::server, asio::use_awaitable);
    cout << "Server: handshake successful" << endl;

    for (;;) {
    std::string buffer;
    auto bytes_transferred = co_await asio::async_read_until(socket, asio::dynamic_buffer(buffer), "\r\n\r\n", asio::use_awaitable);
    cout << "Server: received: " << buffer << endl;

    // Close socket when "exit" is retrieved from client
    if (buffer.substr(0, bytes_transferred - 2) == "exit") {
      cout << "Server: closing connection" << endl;
      // Connection is closed when socket is destroyed
      co_return;
    } else if (buffer.find("GET / HTTP/1.1") != string::npos){
            buffer = "HTTP/1.1 200 OK\r\n"
                     "Content-Type: text/html\r\n"
                     "\r\n"
                     "<!DOCTYPE HTML>"
                     "<html>"
                     "  <head>"
                     "      <title>200 OK</title>"
                     "  </head>"
                     "  <body>"
                     "      <h1>Homepage</h1>"
                     "  </body>"
                     "</html>";
        } else if (buffer.find("GET /page1 HTTP/1.1") != string::npos){
            buffer ="HTTP/1.1 200 OK\r\n"
                    "Content-Type: text/html\r\n"
                    "\r\n"
                    "<!DOCTYPE HTML>"
                    "<html>"
                    "  <head>"
                    "      <title>200 OK</title>"
                    "  </head>"
                    "  <body>"
                    "      <h1>Page 1</h1>"
                    "  </body>"
                    "</html>";
        } else if (buffer.find("GET /page2 HTTP/1.1") != string::npos){
            buffer ="HTTP/1.1 200 OK\r\n"
                    "Content-Type: text/html\r\n"
                    "\r\n"
                    "<!DOCTYPE HTML>"
                    "<html>"
                    "  <head>"
                    "      <title>200 OK</title>"
                    "  </head>"
                    "  <body>"
                    "      <h1>Page 2</h1>"
                    "  </body>"
                    "</html>";
        } else {
            buffer ="HTTP/1.1 400 Bad request\r\n"
                    "Content-Type: text/html\r\n"
                    "\r\n"
                    "<!DOCTYPE HTML>"
                    "<html>"
                    "  <head>"
                    "      <title>400 Bad request</title>"
                    "  </head>"
                    "  <body>"
                    "      <h1>Page doesn't exist</h1>"
                    "  </body>"
                    "</html>";
        }
        bytes_transferred = co_await asio::async_write(socket, asio::buffer(buffer), asio::use_awaitable);
        cout << "Server: sent: " << buffer << endl;
     }
   } catch (const exception &e) {
    cerr << "Server: " << e.what() << endl;
  }

}




asio::awaitable<void> HttpServer::start() {
  try {
    auto executor = co_await asio::this_coro::executor;
    asio::ip::tcp::acceptor acceptor(executor, {asio::ip::tcp::v6(), 3000});
    asio::ssl::context ssl_context(asio::ssl::context::tlsv13_server);
    ssl_context.use_certificate_chain_file("../cert.crt");
    ssl_context.use_private_key_file("../cert.key", asio::ssl::context::pem);
    cout << "Server: waiting for connection" << endl;

    for (;;) {
      asio::ssl::stream<asio::ip::tcp::socket> socket(co_await acceptor.async_accept(asio::use_awaitable),
                                                            ssl_context);
      cout << "Server: connection from " << socket.lowest_layer().remote_endpoint().address() << ':'
           << socket.lowest_layer().remote_endpoint().port() << endl;

      co_spawn(executor, handle_request(std::move(socket)), asio::detached);
    }
  } catch (exception &e) {
    cerr << "Server: " <<e.what() <<endl;
  }



}
