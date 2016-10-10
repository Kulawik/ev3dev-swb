#ifndef POLLER_HPP
#define POLLER_HPP

#include <mutex>
#include <condition_variable>
#include <chrono>

typedef std::chrono::milliseconds Time;
template <typename T>
class Poller {
public:
    // 0 = inf
    bool copyTo(T& dst, Time timeout) {
        std::unique_lock<std::mutex> lock(swap_mutex_, std::defer_lock);
        lock.try_lock_for(timeout);
        if (lock.owns_lock()) {
            dst = buffors_[shadow_idx_ ^ 0x1];
            return true;
        }
        else {
            return false;
        }
    }
    void work() {
        while(1) {
            readSensors();
        }
    }
private:
    void swap() {
        std::lock_guard<std::mutex> guard(swap_mutex_);
        shadow_idx_ ^= 0x1;
    }
    void readSensors();
    T buffors_[2];
    char shadow_idx_;
    std::mutex swap_mutex_;
};
#endif
