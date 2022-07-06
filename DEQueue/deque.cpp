#include <iostream>
#include <deque>

void print_queue(std::deque<int> d)
{
    // Iterate and print values of deque
    for(int n : d) {
        std::cout << n << ' ';
    }
    std::cout << '\n';
}

int main()
{
    // Create a deque containing integers
    std::deque<int> d = {7, 5, 16, 8};

    print_queue(d);

    // Add an integer to the beginning and end of the deque
    d.push_front(13);
    d.push_back(25);

    print_queue(d);

}
