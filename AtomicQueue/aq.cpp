#include <iostream>
#include <string>
#include "pcqueue.h"

auto main() -> int {
  AtomicQueue<std::string, 10> aq; // = new AtomicQueue();

  std::string a1 = "a1";
  std::string a2 = "a2";
  std::string a3 = "a3";

  aq.push(a1);
  aq.push(a2);
  aq.push(a3);

  std::string b;

  while (aq.pop(b))
    std::cout << b << '\n';

}
