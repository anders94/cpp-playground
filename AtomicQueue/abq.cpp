#include <iostream>
#include <string>
#include <thread>
#include "pcqueue.h"

auto main() -> int {
  AtomicQueue<std::string, 10> aq;
  bool quit = false;

  auto reader_thread = std::thread([&aq, &quit]() {
    std::string s;

    while (aq.pop(s) || !quit) {
      std::cout << s << '\n';
    }

  });

  aq.push("a");
  aq.push("b");
  aq.push("c");

  quit = true;

  reader_thread.join();

}
