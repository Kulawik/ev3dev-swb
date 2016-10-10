#ifndef POLLER_HPP
#define POLLER_HPP

#include <mutex>
#include <condition_variable>
#include <chrono>

typedef std::chrono::milliseconds Time;
template <typename T>
class Poller {
public:
    void copyTo(T& dst) {
		std::unique_lock<std::mutex> lock(swap_mutex_);
		cond.wait_for(lock, [this](){return new_data_;});
		dst = buffors_[shadow_idx_ ^ 0x1];
		new_data_ = false;
    }

    bool copyTo(T& dst, Time timeout) {
        std::unique_lock<std::mutex> lock(swap_mutex_);
		cond.wait_for(lock, timeout, [this](){return new_data_;});
        if (lock.owns_lock()) {
            dst = buffors_[shadow_idx_ ^ 0x1];
			new_data_ = false;
            return true;
        }
        else {
            return false;
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

private:
    void swap() {
        std::lock_guard<std::mutex> guard(swap_mutex_);
        shadow_idx_ ^= 0x1;
		new_data_ = true;
    }

    void readSensors() {
		bool new_data = buffors_[shadow_idx_].update();
		if(new_data) {
			swap();
			cond.notify_one();
		}
    }

    T buffors_[2];
    char shadow_idx_;
	bool new_data_;
	Time period_; // from last readSensors end to next readSensors
    std::mutex swap_mutex_;
	bool working_;
};

#endif

