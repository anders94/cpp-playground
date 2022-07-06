#include <iostream>
#include <thread>

auto main() -> int {
  int x = 0;

  auto t0 = std::thread([&]() {
    x++;
    std::cout << x << std::endl;
  });

  auto t1 = std::thread([&]() {
    x++;
    std::cout << x << std::endl;
  });

  t0.join();
  t1.join();

  std::cout << "hardware can support " << t0.hardware_concurrency() << " threads\n";

}
