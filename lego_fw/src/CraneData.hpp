#ifndef CRANEDATA_HPP
#define CRANEDATA_HPP

#include <mutex>
#include <condition_variable>
#include <chrono>

typedef std::chrono::milliseconds Time;
struct CraneData {
    bool update();
    // data....
};
#endif
