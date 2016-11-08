#ifndef STATE_HPP 
#define STATE_HPP 

#include <chrono>

typedef std::chrono::milliseconds Time;

template<typename Data, typename Control>
class State {
   public:
	typedef std::function<void(CraneControl& crane, CraneData& data)> ActionCallback;
	typedef std::function<void(CraneData& data, const Time& elapsed_time)> ConditionCallback;
	typedef std::pair<ConditionCallback, State*> Transition;
    State() = default;
    State<Control>* next(const Data& d);
    State<Control>& enter();
	
	void addAction(const ActionCallback& action) {
		actions_.push_back(action);
	}
	void addTransition(const ConditionCallback& condition, State* next_state) {
		transitions_.emplace_back(std::make_pair(condition, next_state));
	}
   private:
    std::vector<ActionCallback> actions_;
	std::vector<Transition> transitions_; 
};

#endif
