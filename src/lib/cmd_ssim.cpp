#include "cmd_ssim.h"
#include "../utils/cmdlineutils.h"

void vce::ssim::run(int argc, char *argv[]) {
    params_check(argc, argv);
}

void vce::ssim::params_check(int argc, char *argv[]) {
    std::cout << "begin params check..." << std::endl;

    cmdline::parser cmdPara;
    cmdPara.add<std::string>("refVideo", 'r', "计算ssim的视频文件1(暂时只支持mp4格式)", true, "");
    cmdPara.add<std::string>("mainVideo", 'm', "计算ssim的视频文件2(暂时只支持mp4格式)", true, "");

    cmdPara.parse_check(argc, argv);

    vce::ssim::refvideo = cmdPara.get<std::string>("refVideo");
    vce::ssim::mainvideo = cmdPara.get<std::string>("mainVideo");
}
