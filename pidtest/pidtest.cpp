#include "ev3dev.h"

#include <iostream>


void printMotorStats(ev3dev::motor& m) {
    std::cout << "motor " << m.address() << std::endl; 
    std::cout << "speed: " << m.speed() << std::endl;
    std::cout << "position: " << m.position() << std::endl;
}

int main() {
    ev3dev::motor m(ev3dev::OUTPUT_A);
    if (m.connected()) {
        std::cout << "Motor connected." << std::endl;
    } else {
        std::cout << "Motor not connected." << std::endl;
    }
    printMotorStats(m);
    std::cout << "commands: ";
    for (auto cmd : m.commands()) {
        std::cout << cmd;
    }
    std::cout << std::endl;
   
    std::cout << "P I D " 
        << m.position_p() 
        << " " << m.position_i() 
        << " " << m.position_d() << std::endl;
    
    m.set_position_sp(0);
    m.set_speed_sp(100);
    char choice;
    do {
        printMotorStats(m);
        std::cout << "P I D " 
            << m.position_p() 
            << " " << m.position_i() 
            << " " << m.position_d() << std::endl;
        std::cin >> choice;
        if (choice == '+') {
            m.set_position_d(m.position_d() + 1000);
        } else if (choice == '-') {
            m.set_position_d(m.position_d() - 1000);
        } else if (choice == 's') {
            m.set_command("stop");
        } else if (choice == 'r') {
            m.set_command("run-to-abs-pos");
        } 
    } while (choice != 'q');
    m.set_command("stop");
    return 0;
}
