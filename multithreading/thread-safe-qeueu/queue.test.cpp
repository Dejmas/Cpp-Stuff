#include "queue.h"
#include <cassert>
#include <iostream>

void queueTestSeq() {
    ThreadedLimitedQueue<int> queue;
    assert(queue.isEmpty());
    assert(!queue.isFull());
    queue.pushItem(1);
    assert(!queue.isEmpty());
    assert(!queue.isFull());
    
    for (int i = 2; i <= 20; i ++) {
        assert(!queue.isFull());
        queue.pushItem(i);
    }
    assert(queue.isFull());

    for (int i = 1; i <= 20; i ++) {
        int x = queue.popItem();
        assert(x == i);
    }
    assert(queue.isEmpty());
    assert(!queue.isFull());
}

void blockingPopItem() {
    ThreadedLimitedQueue<int> queue;
    int const expected = 666;
    std::thread t1 ([&](){
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        queue.pushItem(expected);
    });
    int val = queue.popItem();
    assert(val == expected);
    t1.join();
    std::cout << "val " << val << "\n";
}

static void sleepLittle() {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
}

void blockingPushItem() {
    ThreadedLimitedQueue<int, 1> queue;
    int const data[] = { 1,2,3,4,5,6,7,8,9,10 };
    size_t len = sizeof(data) / sizeof(data[0]);
    std::thread t1 ([&](){
        for (size_t i = 0; i < len; i ++) {
            queue.pushItem(data[i]);
        }
    });
    for (size_t i = 0; i < len; i ++) {
        sleepLittle();
        int item = queue.popItem();
        assert(item == data[i]);
        std::cout << " item " << item << "\n";
    }
    t1.join();
}

int main() {
    queueTestSeq();
    blockingPopItem();
    blockingPushItem();
}