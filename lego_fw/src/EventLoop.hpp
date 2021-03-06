#ifndef EVENT_LOOP_HPP
#define EVENT_LOOP_HPP

#include "Poller.hpp"
#include "State.hpp"
#include <atomic>

/*
 * Model of the  state machine
 */
template<typename Data, typename Control>
class EventLoop {
   public:
    EventLoop(Control& control, Poller<Data>& poller)
        : control_(control), poller_(poller) {}
    /*
     * Run state machine
     */
    void run(State<Data, Control>* start_state) {
        running_.store(true);
        Data data(control_);
        State<Data, Control>* current_state = start_state;
        State<Data, Control>* previous_state = nullptr;
        while(running_.load() && current_state != nullptr) {
            if (previous_state != current_state) {
                current_state->enter(control_, data);
                previous_state = current_state;
            }
            poller_.copyTo(data, current_state->getTimeout());  // FIXME when
                                                                // timeout ==
                                                                // nullptr, can
                                                                // never stop
                                                                // running
            current_state = current_state->next(data);
        }
        running_.store(false);
    }

    /*
     * Stop state machine (requires additional thread)
     */
    void stop() {
        running_.store(false);
    }

   private:
    Control& control_;
    Poller<Data>& poller_;
    // running flag
    std::atomic<bool> running_;
};

#endif
