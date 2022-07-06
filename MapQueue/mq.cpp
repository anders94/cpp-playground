#include <iostream>
#include <map>
#include <deque>

struct order {
  uint64_t id;
  uint64_t volume;
};

void print_queue(std::deque<order> q)
{
    for(order o : q) {
      std::cout << o.id << ':' << o.volume << ' ';
    }
    std::cout << '\n';
}

void print_map(const std::map<uint64_t, std::deque<order>>& m)
{
    for (const auto& [k, q] : m) {
        std::cout << '[' << k << "] ";
        print_queue(q);
    }
    std::cout << '\n';
}

int main()
{
    order o1;
    o1.id = 100;
    o1.volume = 20000;

    order o2;
    o2.id = 101;
    o2.volume = 35000;

    order o3;
    o3.id = 102;
    o3.volume = 27000;

    order o4;
    o4.id = 103;
    o4.volume = 46000;

    order o5;
    o5.id = 104;
    o5.volume = 21000;

    std::deque<order> q1 = {o1, o2};
    std::deque<order> q2 = {o3, o4, o5};
    std::map<uint64_t, std::deque<order>> m { {2999000, q1}, {3000100, q2} };
    print_map(m);

    order o6;
    o6.id = 105;
    o6.volume = 239999;

    // insert entry into existing deque
    m[3000100].push_back(o6);

    /* // add a new entry to the map
    auto it = q2.begin();
    it = q2.insert(it, 3000200, {o6});
    */

    print_map(m);

}
