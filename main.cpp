#include <asio.hpp>
#include "HttpServer.h"

int main() {
  // Provides asynchronous I/O functionality
  asio::io_context event_loop(1);

  HttpServer httpserver;
  co_spawn(event_loop, httpserver.start(), asio::detached);

  event_loop.run();
}
