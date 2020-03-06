#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <inttypes.h>
#include <limits.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "cmd_ssim.h"
#include "psnrlib.h"
#include "../utils/cmdlineutils.h"

#define FFSWAP(type, a, b)                                                                                                                 \
    do {                                                                                                                                   \
        type SWAP_tmp = b;                                                                                                                 \
        b = a;                                                                                                                             \
        a = SWAP_tmp;                                                                                                                      \
    } while (0)
#define FFMIN(a, b) ((a) > (b) ? (b) : (a))

#define BIT_DEPTH 8
#define PIXEL_MAX ((1 << BIT_DEPTH) - 1)

void vce::ssim::run(int argc, char *argv[]) {
    std::cout << "begin ssim run..." << std::endl;
    params_check(argc, argv);
    do_action();
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

void vce::ssim::do_action() {
#ifdef DEBUG
    std::cout << "begin do_action ..." << std::endl;
#endif
    // 获取video1和video2的分辨率宽高
    int w1 = 0, w2 = 0, h1 = 0, h2 = 0;
    int r0 = get_video_info(vce::ssim::refvideo, w1, h1);
    int r1 = get_video_info(vce::ssim::mainvideo, w2, h2);
    if (r0 != 0 || r1 != 0 || w1 != w2 || h1 != h2) {
        std::cout << "check file rotated..." << std::endl;
        if (w1 == h2 && w2 == h1)
            // 视频处理过程中有翻转
            std::cout << "file1.mp4 file2.mp4 rotated ..." << std::endl;
        else {
            std::cout << "file1.mp4 file2.mp4 have different size, can\' be calculate";
            exit(-1);
        }
    }

    // todo: 根据文件后缀来判断是否要转
    std::string yuv0 = "";
    std::string yuv1 = "";
    if (!mp42yuv(vce::ssim::refvideo, yuv0) || !mp42yuv(vce::ssim::mainvideo, yuv1)) {
        std::cout << "生成yuv文件错误." << std::endl;
        exit(-1);
    }

    // 指向转换后的yuv文件
    FILE *f[2];
    f[0] = fopen(yuv0.c_str(), "rb");
    f[1] = fopen(yuv1.c_str(), "rb");

    int *temp;
    if (w1 <= 0 || h1 <= 0 || w1 * (int64_t)h1 >= INT_MAX / 3 || 2LL * w1 + 12 >= INT_MAX / sizeof(*temp)) {
        fprintf(stderr, "Dimensions are too large, or invalid\n");
        // todo: error_code规范化
        exit(-2);
    }

    int i = 0;
    int frame_size = w1 * h1 * 3LL / 2;
    uint8_t *buf[2], *plane[2][3];
    int frames, seek;
    uint64_t ssd[3] = {0, 0, 0};
    double ssim[3] = {0, 0, 0};
    for (i = 0; i < 2; i++) {
        buf[i] = (uint8_t *)malloc(frame_size);
        plane[i][0] = buf[i];
        plane[i][1] = plane[i][0] + w1 * h1;
        plane[i][2] = plane[i][1] + w1 * h1 / 4;
    }
    temp = (int *)malloc((2 * w1 + 12) * sizeof(*temp));

    std::string ssim_log = "ssimDir/ssim.log";
    std::ofstream ssim_log_f(ssim_log, std::ios::out);
    if (!ssim_log_f) {
        std::cout << "打开文件: " << ssim_log << "失败!" << std::endl;
        exit(-1);
    }

    for (frames = 0;; frames++) {
        uint64_t ssd_one[3];
        double ssim_one[3];
        if (fread(buf[0], frame_size, 1, f[0]) != 1)
            break;
        if (fread(buf[1], frame_size, 1, f[1]) != 1)
            break;
        for (i = 0; i < 3; i++) {
            ssd_one[i] = ssd_plane(plane[0][i], plane[1][i], w1 * h1 >> 2 * !!i);
            ssim_one[i] = ssim_plane(plane[0][i], w1 >> !!i, plane[1][i], w1 >> !!i, w1 >> !!i, h1 >> !!i, temp, NULL);
            ssd[i] += ssd_one[i];
            ssim[i] += ssim_one[i];
        }

        printf("Frame: %d | ", frames);
        print_results(ssim_log_f, ssd_one, ssim_one, 1, w1, h1, frames);
        printf("                \r");
        fflush(stdout);
    }

    if (!frames) {
        std::cout << "frames 为空" << std::endl;
        exit(-1);
    }

    printf("Total: %d frames | ", frames);
    print_results(ssim_log_f, ssd, ssim, frames, w1, h1, frames);
    printf("\n");

    ssimVisualize(ssim_log);

#ifdef DEBUG
    std::cout << "do_action run successfully..."
#endif
}

int vce::ssim::get_video_info(std::string &video, int &w, int &h) {
    cv::VideoCapture capture;
    capture.open(video);
    if (!capture.isOpened()) {
        std::cout << video << "readin failed, please check it..！\n" << std::endl;
        return -1;
    }
    cv::Size s = cv::Size((int)capture.get(cv::CAP_PROP_FRAME_WIDTH), (int)capture.get(cv::CAP_PROP_FRAME_HEIGHT));
    w = s.width;
    h = s.height;

#ifdef DEBUG
    std::cout << "get_video_info, width: " << s.width << std::endl;
    std::cout << "get_video_info, height: " << s.height << std::endl;
#endif

    capture.release();
    return 0;
}

/****************************************************************************
 * structural similarity metric
 ****************************************************************************/
void vce::ssim::ssim_4x4x2_core(const pixel *pix1, intptr_t stride1, const pixel *pix2, intptr_t stride2, int sums[2][4]) {
    int x, y, z;

    for (z = 0; z < 2; z++) {
        uint32_t s1 = 0, s2 = 0, ss = 0, s12 = 0;
        for (y = 0; y < 4; y++)
            for (x = 0; x < 4; x++) {
                int a = pix1[x + y * stride1];
                int b = pix2[x + y * stride2];
                s1 += a;
                s2 += b;
                ss += a * a;
                ss += b * b;
                s12 += a * b;
            }
        sums[z][0] = s1;
        sums[z][1] = s2;
        sums[z][2] = ss;
        sums[z][3] = s12;
        pix1 += 4;
        pix2 += 4;
    }
}

float vce::ssim::ssim_end1(int s1, int s2, int ss, int s12) {
/* Maximum value for 10-bit is: ss*64 = (2^10-1)^2*16*4*64 = 4286582784, which will overflow in some cases.
 * s1*s1, s2*s2, and s1*s2 also obtain this value for edge cases: ((2^10-1)*16*4)^2 = 4286582784.
 * Maximum value for 9-bit is: ss*64 = (2^9-1)^2*16*4*64 = 1069551616, which will not overflow. */
#if BIT_DEPTH > 9
    typedef float type;
    static const float ssim_c1 = .01 * .01 * PIXEL_MAX * PIXEL_MAX * 64 * 64;
    static const float ssim_c2 = .03 * .03 * PIXEL_MAX * PIXEL_MAX * 64 * 63;
#else
    typedef int type;
    static const int ssim_c1 = (int)(.01 * .01 * PIXEL_MAX * PIXEL_MAX * 64 * 64 + .5);
    static const int ssim_c2 = (int)(.03 * .03 * PIXEL_MAX * PIXEL_MAX * 64 * 63 + .5);
#endif
    type fs1 = s1;
    type fs2 = s2;
    type fss = ss;
    type fs12 = s12;
    type vars = fss * 64 - fs1 * fs1 - fs2 * fs2;
    type covar = fs12 * 64 - fs1 * fs2;
    return (float)(2 * fs1 * fs2 + ssim_c1) * (float)(2 * covar + ssim_c2) /
           ((float)(fs1 * fs1 + fs2 * fs2 + ssim_c1) * (float)(vars + ssim_c2));
}

float vce::ssim::ssim_end4(int sum0[5][4], int sum1[5][4], int width) {
    float ssim = 0.0;
    int i;

    for (i = 0; i < width; i++)
        ssim +=
            ssim_end1(sum0[i][0] + sum0[i + 1][0] + sum1[i][0] + sum1[i + 1][0], sum0[i][1] + sum0[i + 1][1] + sum1[i][1] + sum1[i + 1][1],
                      sum0[i][2] + sum0[i + 1][2] + sum1[i][2] + sum1[i + 1][2], sum0[i][3] + sum0[i + 1][3] + sum1[i][3] + sum1[i + 1][3]);
    return ssim;
}

float vce::ssim::ssim_plane(pixel *pix1, intptr_t stride1, pixel *pix2, intptr_t stride2, int width, int height, void *buf, int *cnt) {
    int z = 0;
    int x, y;
    float ssim = 0.0;
    int(*sum0)[4] = (int(*)[4])buf;
    int(*sum1)[4] = sum0 + (width >> 2) + 3;
    width >>= 2;
    height >>= 2;
    for (y = 1; y < height; y++) {
        for (; z <= y; z++) {
            // FFSWAP( (int (*)[4]), sum0, sum1 );
            int(*tmp)[4] = sum0;
            sum0 = sum1;
            sum1 = tmp;

            for (x = 0; x < width; x += 2)
                ssim_4x4x2_core(&pix1[4 * (x + z * stride1)], stride1, &pix2[4 * (x + z * stride2)], stride2, &sum0[x]);
        }
        for (x = 0; x < width - 1; x += 4)
            ssim += ssim_end4(sum0 + x, sum1 + x, FFMIN(4, width - x - 1));
    }
    //     *cnt = (height-1) * (width-1);
    return ssim / ((height - 1) * (width - 1));
}

uint64_t vce::ssim::ssd_plane(const uint8_t *pix1, const uint8_t *pix2, int size) {
    uint64_t ssd = 0;
    int i;
    for (i = 0; i < size; i++) {
        int d = pix1[i] - pix2[i];
        ssd += d * d;
    }
    return ssd;
}

double vce::ssim::ssd_to_psnr(uint64_t ssd, uint64_t denom) {
    return -10 * log((double)ssd / (denom * 255 * 255)) / log(10);
}

double vce::ssim::ssim_db(double ssim, double weight) {
    return 10 * (log(weight) / log(10) - log(weight - ssim) / log(10));
}

void vce::ssim::print_results(std::ofstream &ssim_log_f, uint64_t ssd[3], double ssim[3], int frames, int w, int h, int frame_index) {
    // printf( "PSNR Y:%.3f  U:%.3f  V:%.3f  All:%.3f | ",
    //         ssd_to_psnr( ssd[0], (uint64_t)frames*w*h ),
    //         ssd_to_psnr( ssd[1], (uint64_t)frames*w*h/4 ),
    //         ssd_to_psnr( ssd[2], (uint64_t)frames*w*h/4 ),
    //         ssd_to_psnr( ssd[0] + ssd[1] + ssd[2], (uint64_t)frames*w*h*3/2 ) );
    printf("SSIM Y:%.5f U:%.5f V:%.5f All:%.5f (%.5f)", ssim[0] / frames, ssim[1] / frames, ssim[2] / frames,
           (ssim[0] * 4 + ssim[1] + ssim[2]) / (frames * 6), ssim_db(ssim[0] * 4 + ssim[1] + ssim[2], frames * 6));

    ssim_log_f << "n:" << frame_index + 1 << std::setiosflags(std::ios::fixed) << std::setprecision(5) << " Y:" << ssim[0] / frames
               << " U:" << ssim[1] / frames << " V:" << ssim[2] / frames << " All:" << (ssim[0] * 4 + ssim[1] + ssim[2]) / (frames * 6)
               << " ssim_db:" << (ssim_db(ssim[0] * 4 + ssim[1] + ssim[2], frames * 6)) << std::endl;
    std::cout << "\r\033[k"; // 清空命令行.
}

bool vce::ssim::ssimVisualize(const std::string &ssimlog) {
    Py_Initialize(); //初始化

#ifdef DEBUG
    std::cout << "......为了ssim能够正确生成时间维度的分析图，请检查ssim和python在同级目录下." << std::endl;
#endif

    std::string path = "python";
    std::string cmd_dir = std::string("sys.path.append(\"" + path + "\")");
    std::string cmd_dir1 = std::string("sys.path.append(\"bin/" + path + "\")");
    std::string ssim_dir = ssimlog.substr(0, ssimlog.find_last_of('/'));

    PyRun_SimpleString("import sys");
    PyRun_SimpleString("import os");
    PyRun_SimpleString(cmd_dir.c_str());
    PyRun_SimpleString(cmd_dir1.c_str());

    // 加载模块
    PyObject *moduleName = PyUnicode_FromString("ssim_graph_Py3");
    PyObject *pModule = PyImport_Import(moduleName);
    if (!pModule) {
        std::cout << "Python get module [ssim_graph-Py3] failed." << std::endl;
        return false;
    }

#ifdef DEBUG
    std::cout << "Python get module [ssim_graph-Py3] succeed." << std::endl;
#endif

    // 加载函数
    PyObject *pv = PyObject_GetAttrString(pModule, "get_ssim_graph");
    if (!pv || !PyCallable_Check(pv)) {
        std::cout << "Can't find funftion [get_ssim_graph]" << std::endl;
        return false;
    }

#ifdef DEBUG
    std::cout << "Python get function [get_ssim_graph] succeed." << std::endl;
#endif

    // 设置参数
    PyObject *args = PyTuple_New(2);
    PyObject *arg1 = Py_BuildValue("s", ssimlog.c_str());
    PyObject *arg2 = Py_BuildValue("s", ssim_dir.c_str());
    PyTuple_SetItem(args, 0, arg1);
    PyTuple_SetItem(args, 1, arg2);

#ifdef DEBUG
    std::cout << "第一个参数：" << ssimlog << std::endl;
    std::cout << "第二个参数：" << ssim_dir << std::endl;
#endif

    // 调用函数
    PyObject *pRet = PyObject_CallObject(pv, args);
    if (pRet) {
#ifdef DEBUG
        long result = PyLong_AsLong(pRet);
        std::cout << "result:" << result << std::endl;
#endif

        std::cout << "...ssim帧维度可视化执行成功" << std::endl;
#ifdef DEBUG
        std::cout << "..." << ssimlog << "<===>" << ssim_dir << "/ssim.png" << std::endl;
#endif
    }

    Py_Finalize(); //释放资源

    return true;
}

void vce::ssim::pre_action() {
#ifdef DEBUG
    std::cout << "pre_action " << std::endl;
#endif
}

void vce::ssim::after_action() {
#ifdef DEBUG
    std::cout << "after_action " << std::endl;
#endif
}