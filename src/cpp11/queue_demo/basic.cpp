#include <iostream>

#include "concurrentqueue/concurrentqueue.h"
using namespace moodycamel;

int main() {
    ConcurrentQueue<int> q;

    for (int i = 0; i != 123; ++i)
        q.enqueue(i);

    std::cout << "queue size: " << q.size_approx() << std::endl;

    int item;
    for (int i = 0; i != 123; ++i) {
        std::cout << "queue try_dequeue: " << q.try_dequeue(item) << ", item: " << item << std::endl;
        assert(item == i);
    }
}
