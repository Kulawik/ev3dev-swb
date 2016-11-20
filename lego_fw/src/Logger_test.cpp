#include <iostream>
#include <sstream>
#include <thread>
#include <vector>


#include "Logger.hpp"

int main() {
    INFO << "Ello!";

    std::vector<std::thread> threads;

    for (int i=0; i<100; ++i) {
        threads.emplace_back([i](){ INFO << "Hi!"; int n=100; while(--n) {DEBUG << "("<< i <<")";} WARNING << "Bye.";});
    }

    for(auto& th : threads) {
        th.join();
        ERROR << "Joined: " << th.get_id();
    }
    return 0;
}
