#include <asio.hpp>
#include <iostream>
#include <thread>

using namespace std;

class EchoServer {
  asio::awaitable<void> handle_request(asio::ip::tcp::socket socket) {
    for (;;) {
      std::string buffer;
      auto bytes_transferred = co_await asio::async_read_until(socket, asio::dynamic_buffer(buffer), "\r\n", asio::use_awaitable);
      auto message = buffer.substr(0, bytes_transferred - 2); // Strip \r\n at end of buffer
      cout << "Server: received: " << message << endl;

      istringstream request_stream(buffer);
      string method, path, version;
      request_stream >> method >> path >> version;

      if (method != "GET") {
        co_return;
      }

      string response;
      if (path == "/") {
        response = "HTTP/1.1 200 OK \r\nContent-Type: text/html\r\n\Content-Length: 19\r\n\r\n<h1>Homepage</h1>";
      } else if (path == "/page1") {
        response = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: 18\r\n\r\n<h1>Page 1</h1>";
      } else if (path == "/page2") {
        response = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: 18\r\n\r\n<h1>Page 2</h1>";
      } else {
        response = "HTTP/1.1 404 Not Found\r\nContent-Type: text/html\r\nContent-Length: 30\r\n\r\n<h1>404 - Page Not Found</h1>";
      }


      bytes_transferred = co_await asio::async_write(socket, asio::buffer(response), asio::use_awaitable);
      cout << "Server: sent: " << response << endl;
    }
  }

public:
  EchoServer() {}

  asio::awaitable<void> start() {
    auto executor = co_await asio::this_coro::executor;
    asio::ip::tcp::acceptor acceptor(executor, {asio::ip::tcp::v6(), 3000});

    cout << "Server: waiting for connection" << endl;
    for (;;) {
      auto socket = co_await acceptor.async_accept(asio::use_awaitable);
      cout << "Server: connection from " << socket.remote_endpoint().address() << ':' << socket.remote_endpoint().port() << endl;
      co_spawn(executor, handle_request(std::move(socket)), asio::detached);
    }
  }
};

class EchoClient {

public:
  asio::awaitable<void> start() {
    auto executor = co_await asio::this_coro::executor;
    asio::ip::tcp::resolver resolver(executor);

    // Resolve query (DNS-lookup if needed)
    auto results = co_await resolver.async_resolve("localhost", to_string(3000), asio::use_awaitable);

    asio::ip::tcp::socket socket(executor);
    co_await asio::async_connect(socket, results, asio::use_awaitable);
    cout << "Client: connected" << endl;

    std::string message("GET / HTTP/1.1");
    auto bytes_transferred = co_await asio::async_write(socket, asio::buffer(message + "\r\n"), asio::use_awaitable);
    cout << "Client: sent: " << message << endl;

    std::string buffer;
    bytes_transferred = co_await asio::async_read_until(socket, asio::dynamic_buffer(buffer), "\r\n", asio::use_awaitable);
    message = buffer.substr(0, bytes_transferred - 2); // Strip \r\n at end of buffer
    cout << "Client: received: " << message << endl;

    message = "exit";
    bytes_transferred = co_await asio::async_write(socket, asio::buffer(message + "\r\n"), asio::use_awaitable);
    cout << "Client: sent: " << message << endl;
  }
};

int main() {
  // Provides asynchronous I/O functionality
  asio::io_context event_loop(1);

  EchoServer echo_server;
  co_spawn(event_loop, echo_server.start(), asio::detached);

  EchoClient echo_client;
  co_spawn(event_loop, echo_client.start(), asio::detached);

  event_loop.run();
}
