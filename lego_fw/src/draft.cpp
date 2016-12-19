#include <chrono>

#include "CraneData.hpp"
#include "CraneControl.hpp"

#include "State.hpp"
#include "EventLoop.hpp"

#include "Logger.hpp"

typedef CraneControl Control;
typedef CraneData<Control> Data;

void go_right_action(Control& crane, const Data& data) {
    crane.motor(MotorLabel::C)->set_speed_sp(200);
//    crane.motor(MotorLabel::A)->set_position_sp(20);
    crane.motor(MotorLabel::C)->set_command("run-forever");
}

void stop_rl_action(Control& crane, const Data& data) {
    SafeMotor motor = crane.motor(MotorLabel::C);
    motor->set_command("stop");
}

void go_up_action(Control& crane, const Data& data) {
    crane.motor(MotorLabel::B)->set_speed_sp(-50);
    crane.motor(MotorLabel::B)->set_command("run-forever");
}

void stop_ud_action(Control& crane, const Data& data) {
    SafeMotor motor = crane.motor(MotorLabel::B);
    motor->set_command("stop");
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

bool condition_B_to_A(const Data& data, const Time& time_in_state) {
    return !data.touch_is_pressed && time_in_state > std::chrono::seconds(10);
}

bool condition_C_to_D(const Data& data, const Time& time_in_state) {
    return data.color_intensity > 40;
}
bool condition_D_to_C(const Data& data, const Time& time_in_state) {
    return data.color_intensity <= 40 && time_in_state > std::chrono::seconds(5);
}

bool always(const Data&, const Time&) {
    return true;
}

bool after1s(const Data&, const Time& time) {
    return time > std::chrono::seconds(1);
}

int main() {
    State<Data, Control> state_A, state_B, state_C, state_D;

    state_C.addAction(go_up_action);
    state_D.addAction(stop_ud_action);
    state_C.addTransition(condition_C_to_D, &state_D);
    state_D.addTransition(condition_D_to_C, &state_C);
    state_D.setTimeout(std::chrono::seconds(1));

    state_A.addAction(go_right_action);
    state_A.addTransition(condition_A_to_B, &state_B); // (condition callback, target state)
    
    state_B.addAction(stop_rl_action);
    state_B.addTransition(condition_B_to_A, &state_A); // (condition callback, target state)
    state_B.setTimeout(std::chrono::seconds(1));

    //state_B.addAction(go_down_action);
    //state_B.addTransition([](CraneData,Control ElapsedTime)->bool{return true;}, state_C);
    Control& control = Control::getInstance();
    Poller<Data> poller((Data(control)));
    EventLoop<Data, Control> loop(control, poller);

    std::thread poller_thread([&poller](){poller.work();});
    loop.run(&state_C);

    poller.end();
    poller_thread.join();
    return 0;
}
