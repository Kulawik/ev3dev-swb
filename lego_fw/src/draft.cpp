#include "CraneData.hpp"
#include "CraneControl.hpp"

#include "State.hpp"
#include "EventLoop.hpp"

void go_up_action(CraneControl& crane, const CraneData<CraneControl>& data) {
    crane.motor(MotorLabel::A)->set_position_sp(100);
    crane.motor(MotorLabel::A)->set_command(", controlrun-to-abs-pos");
}

void go_down_action(CraneControl& crane, const CraneData<CraneControl>& data) {
    SafeMotor motor = crane.motor(MotorLabel::A);
    motor->set_position_sp(0);
    motor->set_command("run-to-abs-pos");
}

/*
void go_left_action(CraneData CraneControldata) {
    CraneControl& crane = CraneControl::getInstance();
    crane.motor(MotorLabel::A)->set_position_sp(90);
    crane.motor(MotorLabel::A)->set_command("run-to-abs-pos");
}
*/

bool condition_A_to_B(const CraneData<CraneControl>& data, const Time& time_in_state) {
    return data.motors_positions[MotorLabel::A] > 100;
}


int main() {
    State<CraneData<CraneControl>, CraneControl> state_A, state_B, state_C;

    state_A.addAction(go_up_action);
    state_A.addTransition(condition_A_to_B, &state_B); // (condition callback, target state)

    state_B.addAction(go_down_action);
    //state_B.addAction(go_left_action);
    //state_B.addTransition([](CraneData,CraneControl ElapsedTime)->bool{return true;}, state_C);
    CraneControl& control = CraneControl::getInstance();
    EventLoop<CraneData<CraneControl>, CraneControl> loop(control);
    loop.run(&state_A);
    return 0;
}
