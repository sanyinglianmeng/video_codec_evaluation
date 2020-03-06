#ifndef Command1_h
#define Command1_h

#include "vce_commands_base.h"
#include <iostream>
#include <stdio.h>

namespace vce {

class Command1 : public Base {
public:
    void run(int argc, char *argv[]);

private:
    void params_check(int argc, char *argv[]);
    void pre_action();
    void do_action();
    void after_action();
};

} // namespace vce

#endif /* Command1_h */
