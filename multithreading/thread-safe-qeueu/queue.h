#pragma once

#include <thread>
#include <mutex>
#include <condition_variable>

#include <iostream>

template <typename T, int MAX = 20>
class ThreadedLimitedQueue {
    private:
        std::mutex mMutex;
        std::condition_variable mCV;
        T data[MAX];
        size_t mBegin = 0;
        size_t mEnd = 0;
        size_t mCount = 0;
    public:

    void pushItem(const T & item) {
        std::unique_lock locker(mMutex);
        mCV.wait(locker, [this](){return !isFull();});
        data[mEnd % MAX] = item;
        mEnd = (mEnd + 1) % MAX;
        mCount ++;
        locker.unlock();
        mCV.notify_one();
    }

    int popItem() {
        std::unique_lock locker(mMutex);
        mCV.wait(locker, [this](){return !isEmpty();});
        int temp = data[mBegin % MAX];
        mBegin = (mBegin + 1) % MAX;
        mCount --;
        locker.unlock();
        mCV.notify_one();
        return temp;
    }


    bool isFull() const { return size() == MAX; }
    bool isEmpty() const { return size() == 0; }
    size_t size() const {
        return mCount; 
    }
};
