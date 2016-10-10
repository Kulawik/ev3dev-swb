#ifndef EVENT_LOOP_HPP 
#define EVENT_LOOP_HPP 

#include "Poller.hpp"
#include "State.hpp"

template<typename T, typename E>
class EventLoop {
   public:
    EventLoop(Poller<T>& poller) : poller_(poller) {}

    void run(State<E>* start_state) {
        running_ = true;
        T data;
        State<E>* current_state = start_state;
        while(running_ && current_state != nullptr) {
            bool changed = poller_.copyTo(data, current_state.getTimeout());
            current_state = current_state.next(data);
        }
    }

    void stop() {
        running_ = false;
    }

   private:
    Poller<T>& poller_;
    bool running_;
};

#endif
