#include <iostream>
#include <deque>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <chrono>

std::mutex m;
std::condition_variable cv;
std::deque<int> q;
bool ready = false;
bool processed = false;

template<typename T>
void print(T const & xs) {
  std::cout << "[ ";
  for (auto const & x : xs) {
    std::cout << x << ' ';
  }
  std::cout << "]\n";
}

void worker_thread() {
  while (1) {
    // Wait until main() sends data
    std::unique_lock lk(m);
    cv.wait(lk, []{
      std::cout << "waiting\n";
      return ready;
    });

    // after the wait, we own the lock.
    if (!q.empty()) {
      int x = q[0];
      q.pop_front();

      // do some stuff
      std::cout << "Working... x=" << x << "\n";
      std::this_thread::sleep_for(std::chrono::milliseconds(500));

      print(q);
    }

    // Send data back to main()
    processed = true;

    lk.unlock();
    cv.notify_one();
  }
}
 
int main() { 
  // start the thread
  std::thread worker(worker_thread);

  // fill the queue
  q.push_back(1);
  q.push_back(2);
  q.push_back(3);

  print(q);

  // send data to the worker thread
  {
    std::lock_guard lg(m);
    ready = true;
  }
  cv.notify_one();

  // wait for the worker
  {
    std::unique_lock lg(m);
    cv.wait(lg, [] {
      return processed;
    });
  }

  std::this_thread::sleep_for(std::chrono::milliseconds(2000));

  q.push_back(11);
  q.push_back(22);
  q.push_back(33);

  print(q);

  // send data to the worker thread
  {
    std::lock_guard lg(m);
    ready = true;
  }
  cv.notify_one();

  // wait for the worker
  {
    std::unique_lock lg(m);
    cv.wait(lg, [] {
      return processed;
    });
  }

  worker.join();
}
