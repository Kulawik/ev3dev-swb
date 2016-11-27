#ifndef POLLER_HPP
#define POLLER_HPP

#include <mutex>
#include <condition_variable>
#include <chrono>
#include <thread>

typedef std::chrono::milliseconds Time;

/*
 * Periodically reads state of all sensors and alllows for safe copy of them
 */
template <typename Data>
class Poller {
   public:
    Poller(Data data)
        : buffers_({data, data}),
          shadow_idx_(0),
          new_data_(false),
          period_(Time(1)),
          working_(false) {}

    // Cope the data to destination. Returns if timeout has been reached. no
    // timeout is representated by nullptr.
    void copyTo(Data& dst, Time* timeout) {
        std::unique_lock<std::mutex> lock(swap_mutex_);
        if (timeout) {
            cond.wait_for(lock, *timeout, [this]() { return new_data_; });
        } else {
            cond.wait(lock, [this]() { return new_data_; });
        }

        if (lock.owns_lock()) {
            dst = buffers_[shadow_idx_ ^ 0x1];
            new_data_ = false;
        }
    }

    void work() {
        working_ = true;
        while(working_) {
            readSensors();
            std::this_thread::sleep_for(period_);
        }
    }

    void end() {
        working_ = false;
    }

    void setPeriod(const Time& period) {
        period_ = period;
    }

private:
    void swap() {
        std::lock_guard<std::mutex> guard(swap_mutex_);
        shadow_idx_ ^= 0x1;
        new_data_ = true;
    }

    void readSensors() {
        Data& shadow = buffers_[shadow_idx_];
        const Data& visible = buffers_[shadow_idx_ ^ 0x1];
        shadow.update();
        if(shadow != visible) {
            swap();
            cond.notify_one();
        }
    }

    Data buffers_[2];
    char shadow_idx_;
    bool new_data_;
    Time period_; // from last readSensors end to next readSensors
    bool working_;
    std::mutex swap_mutex_;
    std::condition_variable cond;
};

#endif
