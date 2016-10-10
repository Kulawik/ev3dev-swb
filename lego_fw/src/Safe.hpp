#ifndef SAFE_HPP
#define SAFE_HPP

#include <mutex>
#include <utility>

template<typename T>
class Safe {
public:
    Safe(std::mutex& mtx, T& dv) : lock_(mtx), device_(dv) { }
    Safe(Safe<T>&& s) : device_(s.device_), lock_(std::move(s.lock_)) { }
    T operator*() { return device_; }
    T* operator->() { return &device_; }
private:
    T& device_;
    std::unique_lock<std::mutex> lock_;
};

#endif
