#include <iostream>
#include <asio.hpp>

#ifndef HTTPSERVER_H
#define HTTPSERVER_H

class HttpServer {
  public:
    asio::awaitable<void> handle_request(asio::ip::tcp::socket socket);
    asio::awaitable<void> start();
    HttpServer();
};

#endif //HTTPSERVER_H
