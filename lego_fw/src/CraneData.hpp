#ifndef CRANEDATA_HPP
#define CRANEDATA_HPP

#include <mutex>
#include <condition_variable>
#include <chrono>
#include <array>

#include "CraneControl.hpp"

#include "Logger.hpp"

/*
 * Container for sensors/motors data.
 */
template <class Control>
struct CraneData {
    CraneData(Control& control) : control_(control) {}
    void update() {
        DEBUG << "Getting motors' positions...";
        // get motors positions
        for ( unsigned i = 0; i < MOTORS_COUNT; ++i ) {
            motors_positions[i] = control_.motor(static_cast<MotorLabel>(i))->position();
        }

        DEBUG << "Getting color sensor data...";
        // get color
        color_intensity = control_.color_sensor()->reflected_light_intensity();

        DEBUG << "Getting touch sensor data...";
        // get touch
        touch_is_pressed = control_.touch_sensor()->is_pressed();
    }

    bool operator==(const CraneData<Control> &other) const {
        return other.touch_is_pressed == touch_is_pressed &&
               other.color_intensity == color_intensity;// &&
               //other.motors_positions == motors_positions;
    }

    bool operator!=(const CraneData<Control> &other) const {
        return !(*this == other);
    }

    CraneData& operator=(const CraneData<Control> &other) {
        color_intensity = other.color_intensity;
        touch_is_pressed = other.touch_is_pressed;
        std::copy(other.motors_positions.begin(),
                  other.motors_positions.end(),
                  motors_positions.begin());
        return *this; 
    }

    // motors
    std::array<int, MOTORS_COUNT> motors_positions;

    // color
    int color_intensity;

    // touch
    bool touch_is_pressed;
    private:
        Control& control_;
};

#endif

