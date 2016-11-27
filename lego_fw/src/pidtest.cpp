#include "CraneControl.hpp"

#include <iostream>


void printMotorStats(SafeMotor& m) {
    std::cout << "motor " << m->address() << std::endl;
    std::cout << "speed: " << m->speed() << std::endl;
    std::cout << "position: " << m->position() << std::endl;
}

bool dummyConnectionTester(SafeMotor& m) {
    return m->connected();
}

int main() {
    CraneControl& cc = CraneControl::getInstance();
    SafeMotor m = cc.motor(MotorLabel::A);
    SafeInfraSensor i = cc.infra_sensor();
    if (i->connected()) {
        std::cout << "Infra connected." << std::endl;
    } else {
        std::cout << "Infra not connected." << std::endl;
        return 1;
    }
    if (dummyConnectionTester(m)) {
        std::cout << "Motor connected." << std::endl;
    } else {
        std::cout << "Motor not connected." << std::endl;
        return 1;
    }
    printMotorStats(m);
    std::cout << "commands: ";
    for (auto cmd : m->commands()) {
        std::cout << cmd;
    }
    std::cout << std::endl;

    std::cout << "P I D "
        << m->position_p()
        << " " << m->position_i()
        << " " << m->position_d() << std::endl;

    m->set_position_sp(0);
    m->set_speed_sp(100);
    char choice;
    do {
        printMotorStats(m);
        std::cout << "P I D "
            << m->position_p()
            << " " << m->position_i()
            << " " << m->position_d() << std::endl;
        std::cin >> choice;
        if (choice == '+') {
            m->set_position_d(m->position_d() + 1000);
        } else if (choice == '-') {
            m->set_position_d(m->position_d() - 1000);
        } else if (choice == 's') {
            m->set_command("stop");
        } else if (choice == 'r') {
            m->set_command("run-to-abs-pos");
        }
    } while (choice != 'q');
    m->set_command("stop");
    return 0;
}
