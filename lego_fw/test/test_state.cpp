#define BOOST_TEST_MODULE ElementTest

#include <boost/test/unit_test.hpp>

#include <vector>

#define private public
#include "../src/State.hpp"
#undef private


namespace {

struct XData {
    int x;
    bool update() {
        ++x;
        return true;
    }
};

} // namespace

BOOST_AUTO_TEST_SUITE(test_state)

/** Test if State::addAction adds actions to actions container */
BOOST_AUTO_TEST_CASE( test_addAction ) {
    typedef XData Data;
    typedef int Control;
    State<Data, Control> state;
    BOOST_CHECK_EQUAL(state.actions_.size(), 0);
    auto action = [](Control& c, const Data& d) {};
    state.addAction(action);
    BOOST_CHECK_EQUAL(state.actions_.size(), 1);
    state.addAction(action);
    BOOST_CHECK_EQUAL(state.actions_.size(), 2);
    for (int i=2; i<10; ++i) {
        state.addAction(action);
    }
    BOOST_CHECK_EQUAL(state.actions_.size(), 10);
}

BOOST_AUTO_TEST_SUITE_END()
