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
    typedef typename std::function<void(Control& control, const Data& data)>
        ActionCallback;
    typedef typename std::function<bool(const Data& data, const Time& elapsed_time)>
        ConditionCallback;
    typedef typename std::pair<ConditionCallback, State<Data, Control>*> Transition;
    typedef typename std::vector<ActionCallback> Actions;
    typedef typename std::vector<Transition> Transitions;

    State() : timeout_(nullptr) {}
    ~State() { delete timeout_; }

    /** Check if one of transitions' conditions is satisfied,
     *  if so returns next state of first applicable transition,
     *  otherwise returns "this" (the same state)
     */
    State<Data, Control>* next(const Data& data) {
        const TimePoint now = std::chrono::system_clock::now();
        Time elapsed_time =
            std::chrono::duration_cast<Time>(now - time_entered_);
        for (auto& transition : transitions_) {
            if (transition.first(data, elapsed_time)) {
                return transition.second;
            }
        }
        return this;
    }

    /** Save time of entrence and perform state's actions
     */
    void enter(Control& control, const Data& data) {
        /** save time of entrance to the state */
        time_entered_ = std::chrono::system_clock::now();
        /** perform all actions */
        for (auto& action : actions_) {
            action(control, data);
        }
    }

    void addAction(const ActionCallback& action) {
        actions_.push_back(action);
    }

    void addTransition(const ConditionCallback& condition,
                       State<Data, Control>* next_state) {
        transitions_.emplace_back(std::make_pair(condition, next_state));
    }

    /** return pointer to timeout, if none set return nullptr
     */
    Time* getTimeout() {
        return timeout_;
    }

    void setTimeout(const Time& timeout) {
        delete timeout_;
        timeout_ = new Time(timeout);
    }

   private:
    State(const State&) = delete;

    Actions actions_;
    Transitions transitions_;
    TimePoint time_entered_;
    Time* timeout_;
};

#endif
