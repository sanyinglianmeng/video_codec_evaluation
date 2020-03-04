#include "Command1.h"

void vce::Command1::run(int argc, char *argv[]) {
    std::cout << "I am command 1"
              << "params: " << argv << std::endl;
}

void vce::Command1::params_check(int argc, char *argv[]) {
    std::cout << "I am command 1" << std::endl;
}
