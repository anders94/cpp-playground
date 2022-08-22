#include <asio/co_spawn.hpp>
#include <asio/detached.hpp>
#include <asio/io_context.hpp>
#include <asio/ip/tcp.hpp>
#include <asio/signal_set.hpp>
#include <asio/write.hpp>
#include <cstdio>
#include <thread>

using asio::ip::tcp;
using asio::awaitable;
using asio::co_spawn;
using asio::detached;
using asio::use_awaitable;
namespace this_coro = asio::this_coro;

awaitable<void> echo(tcp::socket& socket) {
  char data[128];
  std::size_t n = co_await socket.async_read_some(asio::buffer(data), use_awaitable);
  std::printf("got [%s]\n", data);
  std::this_thread::sleep_for(std::chrono::milliseconds(1000));
  std::printf("send [%s]\n", data);
  co_await async_write(socket, asio::buffer(data, n), use_awaitable);
}

awaitable<void> consume(tcp::socket socket) {
  try {
    for (;;) {
      co_await echo(socket);
    }
  }
  catch (std::exception& e) {
    std::printf("echo Exception: %s\n", e.what());
  }
}

awaitable<void> listener() {
  auto executor = co_await this_coro::executor;
  tcp::acceptor acceptor(executor, {tcp::v4(), 8888});
  for (;;) {
    tcp::socket socket = co_await acceptor.async_accept(use_awaitable);
    co_spawn(executor, consume(std::move(socket)), detached);
  }
}

int main() {
  try {
    asio::io_context io_context(1);

    asio::signal_set signals(io_context, SIGINT, SIGTERM);
    signals.async_wait([&](auto, auto) {
      io_context.stop();
    });

    co_spawn(io_context, listener(), detached);

    io_context.run();
  }
  catch (std::exception& e) {
    std::printf("Exception: %s\n", e.what());
  }
}
