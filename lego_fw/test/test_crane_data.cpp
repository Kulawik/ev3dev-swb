#define BOOST_TEST_MODULE LegoFrameworkTest

#include <boost/test/unit_test.hpp>

#include <vector>
#include <chrono>
#include <thread>
#include <string>
#include <chrono>
#include <memory>
#include <condition_variable>
#include <mutex>

#define private public
#include "../src/CraneData.hpp"
#undef private

namespace {
    struct dummyMotor {
        dummyMotor() : j(0) {}
        int position() {
            return j++;
        }
        private:
            int j;
    };
    struct dummyInfra {
        dummyInfra() : j(10) {}
        int proximity() {
            return j++;
        }
        private:
            int j;
    };
    struct dummyTouch {
        dummyTouch() : b(true) {}
        bool is_pressed() {
            b = !b;
            return b;
        }
        private:
            bool b;
    };
    struct XControl {
        dummyMotor* motor(MotorLabel) {
            return &mot;
        }
        dummyInfra* infra_sensor() {
            return &infra;
        }
        dummyTouch* touch_sensor() {
            return &touch;
        }
        private:
            dummyMotor mot;
            dummyInfra infra;
            dummyTouch touch;
    };
}

BOOST_AUTO_TEST_SUITE(test_CraneData)

BOOST_AUTO_TEST_CASE( test_update ) {
    typedef XControl Control;
    Control control;
    CraneData<Control> data(control);
    data.update();
    BOOST_CHECK_EQUAL(data.motors_positions[0], 0);
    BOOST_CHECK_EQUAL(data.motors_positions[1], 1);
    BOOST_CHECK_EQUAL(data.motors_positions[2], 2);
    BOOST_CHECK_EQUAL(data.motors_positions[3], 3);
    BOOST_CHECK_EQUAL(data.infra_proximity, 10);
    BOOST_CHECK_EQUAL(data.touch_is_pressed, false);
    data.update();
    BOOST_CHECK_EQUAL(data.motors_positions[0], 4);
    BOOST_CHECK_EQUAL(data.motors_positions[1], 5);
    BOOST_CHECK_EQUAL(data.motors_positions[2], 6);
    BOOST_CHECK_EQUAL(data.motors_positions[3], 7);
    BOOST_CHECK_EQUAL(data.infra_proximity, 11);
    BOOST_CHECK_EQUAL(data.touch_is_pressed, true);
}

BOOST_AUTO_TEST_CASE( test_equal_operator ) {
    typedef XControl Control;
    Control control1, control2;
    CraneData<Control> data1(control1);
    CraneData<Control> data2(control2);
    data1.update();
    data2.update();
    BOOST_CHECK_EQUAL(data1.motors_positions[0], data2.motors_positions[0]);
    BOOST_CHECK_EQUAL(data1.motors_positions[1], data2.motors_positions[1]);
    BOOST_CHECK_EQUAL(data1.motors_positions[2], data2.motors_positions[2]);
    BOOST_CHECK_EQUAL(data1.motors_positions[3], data2.motors_positions[3]);
    BOOST_CHECK_EQUAL(data1.infra_proximity, data2.infra_proximity);
    BOOST_CHECK_EQUAL(data1.touch_is_pressed, data2.touch_is_pressed);
    BOOST_CHECK(data1 == data2);
    data1.update();
    data2.update();
    BOOST_CHECK_EQUAL(data1.motors_positions[0], data2.motors_positions[0]);
    BOOST_CHECK_EQUAL(data1.motors_positions[1], data2.motors_positions[1]);
    BOOST_CHECK_EQUAL(data1.motors_positions[2], data2.motors_positions[2]);
    BOOST_CHECK_EQUAL(data1.motors_positions[3], data2.motors_positions[3]);
    BOOST_CHECK_EQUAL(data1.infra_proximity, data2.infra_proximity);
    BOOST_CHECK_EQUAL(data1.touch_is_pressed, data2.touch_is_pressed);
    BOOST_CHECK(data1 == data2);
}

BOOST_AUTO_TEST_SUITE_END()
