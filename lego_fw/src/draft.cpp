#include "CraneData.hpp"
#include "CraneControl.hpp"

#include "State.hpp"
#include "EventLoop.hpp"

#include "Logger.hpp"

typedef CraneControl Control;
typedef CraneData<Control> Data;

void go_left_action(Control& crane, const Data& data) {
    crane.motor(MotorLabel::A)->set_speed_sp(100);
    crane.motor(MotorLabel::A)->set_position_sp(20);
    crane.motor(MotorLabel::A)->set_command("run-to-rel-pos");
}

void go_right_action(Control& crane, const Data& data) {
    SafeMotor motor = crane.motor(MotorLabel::A);
    motor->set_speed_sp(100);
    motor->set_position_sp(-20);
    motor->set_command("run-to-rel-pos");
}

/*
void go_left_action(CraneData CraneControldata) {
    Control& crane = Control::getInstance();
    crane.motor(MotorLabel::A)->set_position_sp(90);
    crane.motor(MotorLabel::A)->set_command("run-to-abs-pos");
}
*/

bool condition_A_to_B(const Data& data, const Time& time_in_state) {
    return data.touch_is_pressed;
}

bool always(const Data&, const Time&) {
    return true;
}

int main() {
    State<Data, Control> state_A, state_B, state_C;

    state_A.addAction(go_left_action);
    state_A.addTransition(condition_A_to_B, &state_B); // (condition callback, target state)
    
    state_B.addAction(go_right_action);
    state_B.addTransition(condition_A_to_B, &state_A); // (condition callback, target state)

    //state_B.addAction(go_down_action);
    //state_B.addTransition([](CraneData,Control ElapsedTime)->bool{return true;}, state_C);
    Control& control = Control::getInstance();
    Poller<Data> poller((Data(control)));
    EventLoop<Data, Control> loop(control, poller);

    std::thread poller_thread([&poller](){poller.work();});
    loop.run(&state_A);

    poller.end();
    poller_thread.join();
    return 0;
}
