#ifndef STATE_HPP 
#define STATE_HPP 

template<typename Data, typename Control>
class State {
   public:
    State() = default;
    State<Control>* next(const Data& d);
    State<Control>& enter();

   private:
    std::vector<std::function<bool(const Data&, Time > 
};

#endif
