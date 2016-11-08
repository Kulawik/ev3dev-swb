#ifndef EVENT_LOOP_HPP 
#define EVENT_LOOP_HPP 

#include "Poller.hpp"
#include "State.hpp"

/*
 * Model of the  state machine
 */
template<typename Data, typename Control>
class EventLoop {
   public:
    EventLoop(Poller<Data>& poller) : poller_(poller) {}

    /*
     * Run state machine
     */
    void run(State<Control>* start_state) {
        running_ = true;
        T data;
        State<Control>* current_state = start_state;
        State<Control>* previous_state = nullptr;
        while(running_ && current_state != nullptr) {
            if (previous_state != current_state) {
                current_state.enter();
                previous_state = current_state;
            }
            poller_.copyTo(data, current_state.getTimeout());
            current_state = current_state.next(data);
        }
    }

    /*
     * Stop state machine (requires additional thread)
     */
    void stop() {
        running_ = false;
    }

   private:
    Poller<Data>& poller_;
    // running flag
    bool running_;
};

#endif
