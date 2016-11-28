#define BOOST_TEST_MODULE LegoFrameworkTest

#include <boost/test/unit_test.hpp>

#include <vector>
#include <chrono>
#include <algorithm>
#include <thread>

#define private public
#include "../src/State.hpp"
#undef private


namespace {

struct XControl {
    XControl() : x_(0) {}
    int getX() {
        return x_;
    }
    void incX() {
        ++x_;
    }
   private:
    int x_;
};

struct XData {
    int x;
    XData(XControl& control) : x(0), control_(control) {}
    void update() {
        x = control_.getX();
    }
   private:
        XControl& control_;
};


} // namespace

BOOST_AUTO_TEST_SUITE(test_State)

/** Test if State::addAction adds actions to actions' container */
BOOST_AUTO_TEST_CASE( test_addAction ) {
    typedef XData Data;
    typedef XControl Control;
    State<Data, Control> state;
    BOOST_CHECK_EQUAL(state.actions_.size(), 0);
    auto action = [](Control&, const Data&) {};
    state.addAction(action);
    BOOST_CHECK_EQUAL(state.actions_.size(), 1);
    state.addAction(action);
    BOOST_CHECK_EQUAL(state.actions_.size(), 2);
    for (int i=2; i<10; ++i) {
        state.addAction(action);
    }
    BOOST_CHECK_EQUAL(state.actions_.size(), 10);
}

/** Test if State::addTransition adds transitions to transitions' container */
BOOST_AUTO_TEST_CASE( test_addTransition ) {
    typedef XData Data;
    typedef XControl Control;
    State<Data, Control> state;
    BOOST_CHECK_EQUAL(state.transitions_.size(), 0);
    auto condition = [](const Data&, const Time&) { return true; };
    state.addTransition(condition, nullptr);
    BOOST_CHECK_EQUAL(state.transitions_.size(), 1);
    state.addTransition(condition, nullptr);
    BOOST_CHECK_EQUAL(state.transitions_.size(), 2);
    for (int i=2; i<10; ++i) {
        state.addTransition(condition, nullptr);
    }
    BOOST_CHECK_EQUAL(state.transitions_.size(), 10);
}

BOOST_AUTO_TEST_CASE( test_get_set_Timeout ) {
    typedef XData Data;
    typedef XControl Control;
    State<Data, Control> state;
    BOOST_CHECK(state.getTimeout() == nullptr);
    state.setTimeout(Time(1));
    BOOST_REQUIRE(state.timeout_ != nullptr);
    BOOST_CHECK(*state.timeout_ == Time(1));
    Time* timeout = state.getTimeout();
    BOOST_REQUIRE(timeout != nullptr);
    BOOST_CHECK(*timeout == Time(1));
    timeout = state.getTimeout();
    BOOST_REQUIRE(timeout != nullptr);
    BOOST_CHECK(*timeout == Time(1));
    state.setTimeout(Time(2));
    timeout = state.getTimeout();
    BOOST_REQUIRE(timeout != nullptr);
    BOOST_CHECK(*timeout == Time(2));
}

BOOST_AUTO_TEST_CASE( test_enter ) {
    typedef XData Data;
    typedef XControl Control;
    State<Data, Control> state;
    Control control;
    Data data(control);
    auto enter_time = std::chrono::system_clock::now();
    state.enter(control, data);
    BOOST_CHECK(state.time_entered_ - enter_time < std::chrono::milliseconds(100));
    auto action = [](Control& c, const Data& d) {
        if (d.x < 4) {
            c.incX();
        }
    };
    state.addAction(action);
    state.enter(control, data);
    enter_time = std::chrono::system_clock::now();
    BOOST_CHECK(state.time_entered_ - enter_time < std::chrono::milliseconds(100));
    BOOST_CHECK_EQUAL(control.getX(), 1);
    data.update();
    state.addAction(action);
    state.addAction(action);
    enter_time = std::chrono::system_clock::now();
    state.enter(control, data);
    BOOST_CHECK(state.time_entered_ - enter_time < std::chrono::milliseconds(100));
    BOOST_CHECK_EQUAL(control.getX(), 4);
    data.update();
    state.enter(control, data);
    BOOST_CHECK_EQUAL(control.getX(), 4);
}

BOOST_AUTO_TEST_CASE( test_next ) {
    typedef XData Data;
    typedef XControl Control;
    State<Data, Control> state, A, B, C, T;
    Control control;
    Data data(control);
    BOOST_CHECK(state.next(data) == &state);
    auto cond_A = [](const Data& d, const Time&) {return d.x == 1;};
    auto cond_B = [](const Data& d, const Time&) {return d.x == 2;};
    auto cond_C = [](const Data& d, const Time&) {return d.x == 3;};
    auto cond_X = [](const Data& d, const Time&) {return d.x > 0;};

    state.addTransition(cond_A, &A);
    data.x = 0;
    BOOST_CHECK(state.next(data) == &state);
    data.x = 1;
    BOOST_CHECK(state.next(data) == &A);

    state.addTransition(cond_B, &B);
    state.addTransition(cond_C, &C);
    state.addTransition(cond_X, nullptr);

    data.x = 0;
    BOOST_CHECK(state.next(data) == &state);
    data.x = 1;
    BOOST_CHECK(state.next(data) == &A);
    data.x = 2;
    BOOST_CHECK(state.next(data) == &B);
    data.x = 3;
    BOOST_CHECK(state.next(data) == &C);
    data.x = 4;
    BOOST_CHECK(state.next(data) == nullptr);

    data.x = -1;
    BOOST_CHECK(state.next(data) == &state);
    auto cond_T = [](const Data&, const Time& t) {
        return t > std::chrono::milliseconds(400);
    };
    state.enter(control, data);
    state.addTransition(cond_T, &T);
    BOOST_CHECK(state.next(data) == &state);
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    BOOST_CHECK(state.next(data) == &T);
}

BOOST_AUTO_TEST_SUITE_END()
