#ifndef CRANEDATA_HPP
#define CRANEDATA_HPP

#include <mutex>
#include <condition_variable>
#include <chrono>

/*
 * Container for sensors/motors data.
 */
template <typename T>
class CraneData {
    public:
    CraneData(T& cc) : control_(cc) {}
    CraneData(const CraneData&) = default;
    bool update();
    // data....
    private:
        T& control_;
};
#endif
