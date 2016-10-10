#ifndef CRANECTRL_HPP
#define CRANECTRL_HPP

#include <mutex>
#include <condition_variable>
#include <chrono>
#include <iostream>
#include <utility>

typedef std::chrono::milliseconds Time;

class motor {
public:
    motor() : j(42) {}
    int j;
};

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


// template <typename N>
// using UnqPtr = std::unique_ptr<N>;

class CraneControl {
public:
    CraneControl() {}
    Safe<motor> vMotor() { return Safe<motor>(motor_mtx_, vMotor_); }
private:
    std::mutex motor_mtx_;
    motor vMotor_;
};
#endif
