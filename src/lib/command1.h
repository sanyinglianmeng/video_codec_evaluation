#ifndef Command1_h
#define Command1_h

#include "vce_commands_base.h"
#include <iostream>
#include <stdio.h>

namespace vce {

class Command1 : public Base {
public:
    void run(int argc, char *argv[]);
};

} // namespace vce

#endif /* Command1_h */
