#ifndef CMD_SSIM_H
#define CMD_SSIM_H

#include "vce_commands_base.h"
#include <iostream>
#include <stdio.h>

namespace vce {

class ssim : public Base {
public:
    void run(int argc, char *argv[]);

public:
    std::string refvideo;
    std::string mainvideo;

private:
    void params_check(int argc, char *argv[]);
};

} // namespace vce

#endif /* CMD_SSIM_H */