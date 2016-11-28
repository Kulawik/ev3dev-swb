#ifndef CRANEDATA_HPP
#define CRANEDATA_HPP

#include <mutex>
#include <condition_variable>
#include <chrono>
#include <array>

#include "CraneControl.hpp"

/*
 * Container for sensors/motors data.
 */
template <class Control>
struct CraneData {
    CraneData(Control& control) : control_(control) {}
    CraneData(const CraneData&) = default;
    void update() {
        // get motors positions
        for ( int i = 0; i < MOTORS_COUNT; ++i ) {
            motors_positions[i] = control_.motor(static_cast<MotorLabel>(i))->position();
        }

        // get infra
        infra_proximity = control_.infra_sensor()->proximity();

        // get touch
        touch_is_pressed = control_.touch_sensor()->is_pressed();
    }

    bool operator==(const CraneData<Control> &other) const {
        return other.touch_is_pressed == touch_is_pressed &&
               other.infra_proximity == infra_proximity &&
               other.motors_positions == motors_positions;
    }

    // motors
    std::array<int, MOTORS_COUNT> motors_positions;

    // infra
    int infra_proximity;

    // touch
    bool touch_is_pressed;
    private:
        Control& control_;
};

#endif

