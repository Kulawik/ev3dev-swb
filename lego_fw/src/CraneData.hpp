#ifndef CRANEDATA_HPP
#define CRANEDATA_HPP

#include <mutex>
#include <condition_variable>
#include <chrono>

struct CraneData {
    CraneData(CraneControl& control) : control_(control);
    CraneData(const CraneData&) = default;
    bool update();
    // data....
    private:
        CraneControl& control_;
};
#endif
