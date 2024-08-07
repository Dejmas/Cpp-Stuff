#include <iostream>
#include <thread>
#include <chrono>

int main (int argc, const char * argv[]) {
    int times = 0;
    if (argc > 1) {
        if (sscanf(argv[1], "%d", &times) != 1) {
            std::cerr << "Error: Usage: %s <numberOfSteps>" << std::endl;
            return 1;
        }
    }
    for (int i = 1; i <= times; i ++) {
        std::this_thread::sleep_for(std::chrono::seconds(3));
        std::cout << "step " << i << std::endl;
    }
    std::cout << "Done." << std::endl;
}