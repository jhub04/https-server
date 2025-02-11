#include <iostream>
#include <asio.hpp>
#include <asio/ssl.hpp>

#ifndef HTTPSERVER_H
#define HTTPSERVER_H

class HttpServer {
  public:
    asio::awaitable<void> handle_request(asio::ssl::stream<asio::ip::tcp::socket> socket);
    asio::awaitable<void> start();
    HttpServer();
};

#endif //HTTPSERVER_H
