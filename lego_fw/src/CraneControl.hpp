#ifndef CRANECTRL_HPP
#define CRANECTRL_HPP

#include <mutex>
#include <condition_variable>
#include <chrono>
#include <iostream>
#include <utility>

#include "Safe.hpp"

typedef std::chrono::milliseconds Time;

class motor {
public:
    motor() : j(42) {}
    int j;
};


class CraneControl {
public:
    CraneControl() {}
    Safe<motor> vMotor() { return Safe<motor>(motor_mtx_, vMotor_); }
private:
    CraneControl(const CraneControl&) = delete;

    std::mutex motor_mtx_;
    motor vMotor_;
};
#endif
