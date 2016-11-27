#ifndef SAFE_HPP
#define SAFE_HPP

#include <mutex>
#include <utility>

/*
 * Wrapper around object, allows for safe sharing of the underlying data T
 * across threads
 */
template<typename T>
class Safe {
public:
    // c-tor. User should pass both data reference as well as the mutex
    Safe(std::mutex& mtx, T& dv) : lock_(mtx), device_(dv) { }
    // move c-tor
    explicit Safe(Safe<T>&& s) : lock_(std::move(s.lock_)), device_(s.device_) { }
    // operator overloads
    T operator*() const { return device_; }
    T* operator->() const { return &device_; }
private:
    std::unique_lock<std::mutex> lock_;
    T& device_;
};

#endif
