#ifndef STATE_HPP
#define STATE_HPP

#include <chrono>
#include <functional>
#include <utility>
#include <vector>

typedef std::chrono::milliseconds Time;
typedef std::chrono::time_point<std::chrono::system_clock> TimePoint;

template<typename Data, typename Control>
class State {
   public:
    typedef std::function<void(Control& control, const Data& data)>
        ActionCallback;
    typedef std::function<void(const Data& data, const Time& elapsed_time)>
        ConditionCallback;
    typedef std::pair<ConditionCallback, State<Data, Control>*> Transition;
    typedef std::vector<ActionCallback> Actions;
    typedef std::vector<Transition> Transitions;

    State() = default;
    ~State() = default;

    /** Check if one of transitions' conditions is satisfied,
     *  if so returns next state of first applicable transition,
     *  otherwise returns "this" (the same state)
     */
    State<Data, Control>* next(const Data& data) {
        Time elapsed_time = std::chrono::system_clock::now() - time_entered_;
        for (auto& transition : transitions_) {
            if (transition.first(data, elapsed_time)) {
                return transition.second;
            }
        }
        return this;
    }

    /** Remember time of entrance to the state */
    State<Data, Control>& enter() {
        time_entered_ = std::chrono::system_clock::now();
        //TODO
    }

    void addAction(const ActionCallback& action) {
        actions_.push_back(action);
    }

    void addTransition(const ConditionCallback& condition,
                       State<Data, Control>* next_state) {
        transitions_.emplace_back(std::make_pair(condition, next_state));
    }

   private:
    State(const State&) = delete;

    Actions actions_;
    Transitions transitions_;
    TimePoint time_entered_;
};

#endif
