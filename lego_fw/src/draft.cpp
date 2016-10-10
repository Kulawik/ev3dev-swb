

void go_up_action(CraneControl crane, CraneData data) {
	crane.vMotor()->set_position_sp(100);
	crane.vMotor()->set_command(", controlrun-to-abs-pos");
}

void go_down_action(CraneControl crane, CraneData data) {
	Safe<motor> motor = crane.vMotor();
	motor->set_position_sp(0);
	motor->set_command("run-to-abs-pos");
}

/*
void go_left_action(CraneControl crane, CraneData data) {
	crane.hMotor().set_position_sp(90);
	crane.hMotor().set_command("run-to-abs-pos");
}
*/

bool condition_A_to_B(CraneData data, ElapsedTime time_in_state) {
	return data.v_motor_position == 100 || data.v_motor_speed == 0;
}


int main() {

	State state_A, state_B, state_C;

	state_A.addActions(go_up_action);
	state_A.addTransition(condition_A_to_B, state_B); // (condition callback, target state) 

	state_B.addAction(go_down_action);
	//state_B.addAction(go_left_action);
	//state_B.addTransition([](CraneData, ElapsedTime)->bool{return true;}, state_C);
	CraneControl control;
	Poller<CraneData> poller(CraneData(control));
	EventLoop loop(poller, control);
	loop.run(state_A);	
	return 0;
}
