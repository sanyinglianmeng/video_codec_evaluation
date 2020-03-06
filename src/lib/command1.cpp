#include "Command1.h"

void vce::Command1::run(int argc, char *argv[]) {
    std::cout << "begin run..." << std::endl;
}

void vce::Command1::params_check(int argc, char *argv[]) {
    std::cout << "begin params check..." << std::endl;
}

void vce::Command1::pre_action() {
    std::cout << "pre_action " << std::endl;
}

void vce::Command1::do_action() {
    std::cout << "do_action " << std::endl;
}

void vce::Command1::after_action() {
    std::cout << "after_action " << std::endl;
}