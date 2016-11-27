#ifndef CRANECTRL_HPP
#define CRANECTRL_HPP

#include <mutex>
#include <condition_variable>
#include <chrono>
#include <iostream>
#include <utility>

#include "Safe.hpp"
#include "ev3dev.h"

typedef std::chrono::milliseconds Time;
typedef Safe<ev3dev::motor> SafeMotor;
typedef Safe<ev3dev::infrared_sensor> SafeInfraSensor;
typedef Safe<ev3dev::touch_sensor> SafeTouchSensor;

// Used for array indices!  Don't change the numbers!
enum MotorLabel {
    A = 0,
    B,
    C,
    D
};

/*
 * Controler of the hardware. Stores all necessary motors and sensors together
 * with mutexes. This class is a singleton.
 */
class CraneControl {
public:
    // Singleton - can only be obtained by this method.
    static CraneControl& getInstance() {
        // Guaranteed to be destroyed.
        static CraneControl instance;
        // Instantiated on first use.
        return instance;
    }

    SafeMotor motor(MotorLabel idx) {
        return SafeMotor(motor_mutexes_[idx], motors_[idx]);
    }

    SafeTouchSensor touch_sensor() {
        return SafeTouchSensor(touch_sensor_mutex_, touch_sensor_);
    }

    SafeInfraSensor infra_sensor() {
        return SafeInfraSensor(infra_sensor_mutex_, infra_sensor_);
    }
private:
    CraneControl() {}
    CraneControl(const CraneControl&) = delete;
    void operator=(const CraneControl&) = delete;

    std::mutex motor_mutexes_[4];
    std::mutex touch_sensor_mutex_;
    std::mutex infra_sensor_mutex_;

    ev3dev::motor motors_[4] = {
        ev3dev::motor(ev3dev::OUTPUT_A),
        ev3dev::motor(ev3dev::OUTPUT_B),
        ev3dev::motor(ev3dev::OUTPUT_C),
        ev3dev::motor(ev3dev::OUTPUT_D)
    };

    ev3dev::touch_sensor touch_sensor_ = ev3dev::touch_sensor(ev3dev::INPUT_1);
    ev3dev::infrared_sensor infra_sensor_ = ev3dev::infrared_sensor(ev3dev::INPUT_3);
};
#endif
