#define BOOST_TEST_MODULE LegoFrameworkTest

#include <boost/test/unit_test.hpp>

#include <vector>
#include <chrono>
#include <algorithm>
#include <thread>

#include "../src/State.hpp"

#define private public
#include "../src/EventLoop.hpp"
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
    XData(const XData& other) : x(other.x), control_(other.control_) {}
    void update() {
        x = control_.getX();
    }

    bool operator==(const XData& other) const { return this->x == other.x; }
    bool operator!=(const XData& other) const { return !((*this)==other); }

    XData& operator= (const XData& other) {
       this->x = other.x;
       return *this;
    }

   private:
        XControl& control_;
};



} // namespace

BOOST_AUTO_TEST_SUITE(test_State)

BOOST_AUTO_TEST_CASE( test_run_empty ) {
    typedef XControl Control;
    typedef XData Data;
    Control control;
    Poller<Data> poller((Data(control)));
    EventLoop<Data, Control> loop(control, poller);
    loop.run(nullptr);
    BOOST_CHECK_EQUAL(2+2, 4);
}

BOOST_AUTO_TEST_CASE( test_run_stop ) {
    typedef XControl Control;
    typedef XData Data;
    typedef State<Data, Control> State;
    Control control;
    Poller<Data> poller((Data(control)));
    EventLoop<Data, Control> loop(control, poller);
    State A;
    A.setTimeout(Time(100));
    std::thread swich_off([&loop](){
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            loop.stop();
    });
    loop.run(&A);
    BOOST_CHECK_EQUAL(2+2, 4);
    swich_off.join();
}

BOOST_AUTO_TEST_CASE( test_cyclic_state_machine ) {
    typedef XControl Control;
    typedef XData Data;
    typedef State<Data, Control> State;
    Control control;
    Poller<Data> poller((Data(control)));
    EventLoop<Data, Control> loop(control, poller);
    auto always = [](const Data&, const Time&) { return true; };
    State A, B, C;
    A.setTimeout(Time(100));
    B.setTimeout(Time(100));
    C.setTimeout(Time(100));
    A.addTransition(always, &B);
    B.addTransition(always, &C);
    C.addTransition(always, &A);

    std::thread swich_off([&loop](){
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
            loop.stop();
    });

    loop.run(&A);
    BOOST_CHECK_EQUAL(2+2, 4);
    swich_off.join();
}

//TODO Add more tests (i.a. real use tests) and add some checks in existing onces

BOOST_AUTO_TEST_SUITE_END()

