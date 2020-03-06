#ifndef CMD_SSIM_H
#define CMD_SSIM_H

#include "vce_commands_base.h"
#include <iostream>
#include <stdio.h>

typedef uint8_t pixel;

namespace vce {

class ssim : public Base {
public:
    void run(int argc, char *argv[]);

public:
    std::string refvideo;
    std::string mainvideo;

private:
    void params_check(int argc, char *argv[]);
    void pre_action();
    void do_action();
    void after_action();
    // 处理相关的函数，后面尽量放到lib中
    int get_video_info(std::string &video, int &w, int &h);
    void ssim_4x4x2_core(const pixel *pix1, intptr_t stride1, const pixel *pix2, intptr_t stride2, int sums[2][4]);
    float ssim_end1(int s1, int s2, int ss, int s12);
    float ssim_end4(int sum0[5][4], int sum1[5][4], int width);
    float ssim_plane(pixel *pix1, intptr_t stride1, pixel *pix2, intptr_t stride2, int width, int height, void *buf, int *cnt);
    uint64_t ssd_plane(const uint8_t *pix1, const uint8_t *pix2, int size);
    double ssd_to_psnr(uint64_t ssd, uint64_t denom);
    double ssim_db(double ssim, double weight);
    void print_results(std::ofstream &ssim_log_f, uint64_t ssd[3], double ssim[3], int frames, int w, int h, int frame_index);
    bool ssimVisualize(const std::string &ssimlog);
};

} // namespace vce

#endif /* CMD_SSIM_H */