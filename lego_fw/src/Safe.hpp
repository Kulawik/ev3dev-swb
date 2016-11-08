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
    Safe(Safe<T>&& s) : device_(s.device_), lock_(std::move(s.lock_)) { }
    // operator overloads
    T operator*() { return device_; }
    T* operator->() { return &device_; }
private:
    T& device_;
    std::unique_lock<std::mutex> lock_;
};

#endif
