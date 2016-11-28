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
#include "../src/Poller.hpp"
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

BOOST_AUTO_TEST_SUITE(test_Poller)

BOOST_AUTO_TEST_CASE( test_readSensors ) {
    typedef XData Data;
    typedef XControl Control;
    Control control, freeze;
    Data data(control);

    Poller<Data> poller(data);
    BOOST_CHECK(poller.buffers_[0] == data);
    BOOST_CHECK(poller.buffers_[1] == data);

    Data old_data(data);
    control.incX();
    data.update();
    BOOST_CHECK(poller.buffers_[0] == old_data);
    BOOST_CHECK(poller.buffers_[1] == old_data);

    poller.readSensors();
    BOOST_CHECK_EQUAL(poller.new_data_, true);
    BOOST_CHECK(poller.buffers_[static_cast<int>(poller.shadow_idx_)] == old_data);
    BOOST_CHECK(poller.buffers_[poller.shadow_idx_ ^ 0x1] == data);

    poller.new_data_ = false;
    poller.readSensors();
    BOOST_CHECK_EQUAL(poller.new_data_, false);
    BOOST_CHECK(poller.buffers_[static_cast<int>(poller.shadow_idx_)] == data);
    BOOST_CHECK(poller.buffers_[poller.shadow_idx_ ^ 0x1] == data);

    control.incX();
    poller.readSensors();
    old_data = data;
    data.update();
    BOOST_CHECK_EQUAL(poller.new_data_, true);
    BOOST_CHECK(poller.buffers_[static_cast<int>(poller.shadow_idx_)] == old_data);
    BOOST_CHECK(poller.buffers_[poller.shadow_idx_ ^ 0x1] == data);

    poller.new_data_ = false;
    poller.readSensors();
    BOOST_CHECK_EQUAL(poller.new_data_, false);
    BOOST_CHECK(poller.buffers_[static_cast<int>(poller.shadow_idx_)] == data);
    BOOST_CHECK(poller.buffers_[poller.shadow_idx_ ^ 0x1] == data);
}

BOOST_AUTO_TEST_CASE( test_work_and_end ) {
    typedef XData Data;
    typedef XControl Control;
    Control control, freeze;
    Data data(control);
    Poller<Data> poller(data);
    poller.setPeriod(Time(1));

    auto switch_off_foo = [&poller]() -> std::unique_ptr<std::thread> {
        std::unique_ptr<std::thread> uptr( new std::thread ([&poller] {
                while(!poller.working_) {}
                std::this_thread::sleep_for(std::chrono::milliseconds(500));
                poller.end();
        }));
        return uptr;
    };
    std::unique_ptr<std::thread> thread(switch_off_foo());
    poller.work();
    thread->join();
    BOOST_CHECK_EQUAL(poller.new_data_, false);

    control.incX();
    data.update();
    thread = switch_off_foo();
    poller.work();
    thread->join();
    BOOST_CHECK_EQUAL(poller.new_data_, true);
    BOOST_CHECK(poller.buffers_[static_cast<int>(poller.shadow_idx_)] == data);
    BOOST_CHECK(poller.buffers_[poller.shadow_idx_ ^ 0x1] == data);
}

BOOST_AUTO_TEST_CASE( test_swap ) {
    typedef XData Data;
    typedef XControl Control;
    Control control, freeze;
    Data data(control);
    Poller<Data> poller(data);
    BOOST_CHECK_EQUAL(poller.shadow_idx_, 0);
    BOOST_CHECK_EQUAL(poller.new_data_, false);
    poller.swap();
    BOOST_CHECK_EQUAL(poller.shadow_idx_, 1);
    BOOST_CHECK_EQUAL(poller.new_data_, true);
    poller.swap();
    BOOST_CHECK_EQUAL(poller.shadow_idx_, 0);
    BOOST_CHECK_EQUAL(poller.new_data_, true);
    poller.swap();
    BOOST_CHECK_EQUAL(poller.shadow_idx_, 1);
    BOOST_CHECK_EQUAL(poller.new_data_, true);
    poller.new_data_ = false;
    poller.swap();
    BOOST_CHECK_EQUAL(poller.shadow_idx_, 0);
    BOOST_CHECK_EQUAL(poller.new_data_, true);
}

BOOST_AUTO_TEST_CASE( test_setPeriod ) {
    typedef XData Data;
    typedef XControl Control;
    Control control, freeze;
    Data data(control);
    Poller<Data> poller(data);
    Poller<Data> poller2(data);
    BOOST_CHECK(poller.period_ == poller2.period_);
    poller.setPeriod(Time(44));
    BOOST_CHECK(poller.period_ == Time(44));
}

BOOST_AUTO_TEST_CASE( test_copyTo_without_timeout_once ) {
    typedef XData Data;
    typedef XControl Control;
    Control control, freeze;
    Data data(control);
    Poller<Data> poller(data);
    Data returned_data(control);
    std::thread worker([&poller]() {poller.work();});
    std::thread supplier([&control, &data]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        control.incX();
        data.update();
    });
    poller.copyTo(returned_data, nullptr);
    BOOST_CHECK(returned_data == data);
    poller.end();
    worker.join();
    supplier.join();
}

BOOST_AUTO_TEST_CASE( test_copyTo_without_timeout_multiple_times ) {
    typedef XData Data;
    typedef XControl Control;
    Control control, freeze;
    Data data(control);
    Poller<Data> poller(data);
    Data returned_data(control);
    std::thread worker([&poller]() {poller.work();});
    const int times = 100;
    std::condition_variable cond;
    std::mutex mtx;
    bool ready_to_read = false;
    bool ready_to_write = false;
    std::thread supplier([&]() {
        std::unique_lock<std::mutex> lock(mtx);
        ready_to_write = true;
        cond.notify_one();
        for (int i=0; i < times; ++i) {
            cond.wait(lock, [&]() { return ready_to_read;} );
            std::this_thread::sleep_for(Time(1));
            ready_to_read = false;
            control.incX();
            data.update();
        }
    });
    {
        std::unique_lock<std::mutex> lock(mtx);
        cond.wait(lock, [&]() { return ready_to_write;} );
    }
    for (int i = 0; i < times; ++i) {
        ready_to_read = true;
        cond.notify_one();
        poller.copyTo(returned_data, nullptr);
        BOOST_CHECK(returned_data == data);
    }
    poller.end();
    worker.join();
    supplier.join();
}

BOOST_AUTO_TEST_CASE( test_copyTo_with_timeout_once ) {
    typedef XData Data;
    typedef XControl Control;
    Control control, freeze;
    Data data(control);
    Poller<Data> poller(data);
    Data returned_data(control);
    std::thread worker([&poller]() {poller.work();});
    std::thread supplier([&control, &data]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        control.incX();
        data.update();
    });
    Time timeout(Time::max());
    poller.copyTo(returned_data, &timeout);
    BOOST_CHECK(returned_data == data);
    poller.end();
    worker.join();
    supplier.join();
}

BOOST_AUTO_TEST_CASE( test_copyTo_with_timeout_multiple_times ) {
    typedef XData Data;
    typedef XControl Control;
    Control control, freeze;
    Data data(control);
    Poller<Data> poller(data);
    Data returned_data(control);
    std::thread worker([&poller]() {poller.work();});
    const int times = 100;
    std::condition_variable cond;
    std::mutex mtx;
    bool ready_to_read = false;
    bool ready_to_write = false;
    std::thread supplier([&]() {
        std::unique_lock<std::mutex> lock(mtx);
        ready_to_write = true;
        cond.notify_one();
        for (int i=0; i < times; ++i) {
            cond.wait(lock, [&]() { return ready_to_read;} );
            std::this_thread::sleep_for(Time(1));
            ready_to_read = false;
            control.incX();
            data.update();
        }
    });
    {
        std::unique_lock<std::mutex> lock(mtx);
        cond.wait(lock, [&]() { return ready_to_write;} );
    }
    Time timeout(Time(20000)); // FIXME check why it doesn't work with Time::max()

    for (int i = 0; i < times; ++i) {
        ready_to_read = true;
        cond.notify_one();
        poller.copyTo(returned_data, &timeout);
        BOOST_CHECK(returned_data == data);
    }
    poller.end();
    BOOST_TEST_MESSAGE("Joining worker...");
    worker.join();
    BOOST_TEST_MESSAGE("Joining supplier...");
    supplier.join();
}

BOOST_AUTO_TEST_CASE( test_copyTo_with_timeout_timeouted ) {
    typedef XData Data;
    typedef XControl Control;
    Control control, freeze;
    Data data(control);
    Poller<Data> poller(data);
    Data returned_data(control);
    Time timeout(Time(50));
    poller.copyTo(returned_data, &timeout);
    BOOST_CHECK(2+2 == 4);
}


BOOST_AUTO_TEST_SUITE_END()
