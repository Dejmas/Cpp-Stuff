
#include <thread>
#include <iostream>
#include <mutex>
#include <condition_variable>

struct ThreadTask {
    private:
        std::mutex mM;
        std::condition_variable mCV;
        bool isNextOdd = true;
        
    public:
    void printOdd() {
        for (int i = 1; i <= 20; i ++) {
            std::unique_lock lk(mM);
            mCV.wait(lk, [this]{ return isNextOdd; });
            PrintNextOddNumber();
            isNextOdd = false;
            lk.unlock();
            mCV.notify_one();
        }
    }

    void printEven() {
        for (int i = 1; i <= 20; i ++) {
            std::unique_lock lk(mM);
            mCV.wait(lk, [this]{ return !isNextOdd; });
            PrintNextEvenNumber();
            isNextOdd = true;
            lk.unlock();
            mCV.notify_one();
        }
    }

    static void PrintNextOddNumber() {
        static int val = 1;
        std::cout << val << "\n";
        val += 2;
    }

    static void PrintNextEvenNumber() {
        static int val = 2;
        std::cout << val << "\n";
        val += 2;
    }

};



int main () {
    ThreadTask task;

    std::thread oddThread([&task] {
        task.printOdd();
    });

    std::thread evenThread([&task] {
        task.printEven();
    });

    oddThread.join();
    evenThread.join();
}