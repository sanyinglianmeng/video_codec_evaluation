/*
 * Copyright (c) 2003-2013 Loren Merritt
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110 USA
 */
/*
 * tiny_msssim.c
 * Computes the Multi-Scale Structural Similarity Metric between two rawYV12 video files.
 * original algorithm:
 * Z. Wang, E. P. Simoncelli and A. C. Bovik,
 *   "Multi-scale structural similarity for image quality assessment,"
 *   The Thrity-Seventh Asilomar Conference on Signals, Systems & Computers, 
 *       2003, Pacific Grove, CA, USA, 2003, pp. 1398-1402 Vol.2.
 *
 * To improve speed, this implementation uses the standard approximation of
 * overlapped 8x8 block sums, rather than the original gaussian weights.
 */

#include <inttypes.h>
#include <limits.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <cstring>
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "lib/psnrlib.h"


#define FFSWAP(type, a, b) \
    do                     \
    {                      \
        type SWAP_tmp = b; \
        b = a;             \
        a = SWAP_tmp;      \
    } while (0)
#define FFMIN(a, b) ((a) > (b) ? (b) : (a))

#define BIT_DEPTH 8                      // 位深 使用几位来定位一个像素点 8位的话 像素值范围就是0-255
#define PIXEL_MAX ((1 << BIT_DEPTH) - 1) // 像素值最大值 公式里的L
typedef uint8_t pixel;                   // 8位无符号 表示像素值

const float WEIGHT[] = {0.0448, 0.2856, 0.3001, 0.2363, 0.1333};

typedef struct
{
    float L;
    float C_S;
} ssim_value;

/****************************************************************************
 * structural similarity metric
 ****************************************************************************/
static void ssim_4x4x2_core(const pixel *pix1, intptr_t stride1,
                            const pixel *pix2, intptr_t stride2,
                            int sums[2][4])
{
    int x, y, z;

    for (z = 0; z < 2; z++)
    {
        uint32_t s1 = 0, s2 = 0, ss = 0, s12 = 0;
        for (y = 0; y < 4; y++)
            for (x = 0; x < 4; x++)
            {
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

static ssim_value ssim_end1(int s1, int s2, int ss, int s12)
{
    ssim_value value;
/* Maximum value for 10-bit is: ss*64 = (2^10-1)^2*16*4*64 = 4286582784, which will overflow in some cases.
 * s1*s1, s2*s2, and s1*s2 also obtain this value for edge cases: ((2^10-1)*16*4)^2 = 4286582784.
 * Maximum value for 9-bit is: ss*64 = (2^9-1)^2*16*4*64 = 1069551616, which will not overflow. */
#if BIT_DEPTH > 9
    typedef double type;
    static const double ssim_c1 = .01 * .01 * PIXEL_MAX * PIXEL_MAX * 64 * 64;
    static const double ssim_c2 = .03 * .03 * PIXEL_MAX * PIXEL_MAX * 64 * 63;
#else
    typedef int type;
    // k1=0.01, k2=0.03
    static const int ssim_c1 = (int)(.01 * .01 * PIXEL_MAX * PIXEL_MAX * 64 * 64 + .5);
    static const int ssim_c2 = (int)(.03 * .03 * PIXEL_MAX * PIXEL_MAX * 64 * 63 + .5);
#endif
    type fs1 = s1;
    type fs2 = s2;
    type fss = ss;
    type fs12 = s12;
    type vars = fss * 64 - fs1 * fs1 - fs2 * fs2;
    type covar = fs12 * 64 - fs1 * fs2;
    
    value.L = (float)(2 * fs1 * fs2 + ssim_c1) / (float)(fs1 * fs1 + fs2 * fs2 + ssim_c1);
    value.C_S = (float)(2 * covar + ssim_c2) / (float)(vars + ssim_c2);

    return value;
}

static ssim_value ssim_end4(int sum0[5][4], int sum1[5][4], int width)
{
    ssim_value ssim;
    ssim.L = 0.0;
    ssim.C_S = 0.0;

    int i;
    for (i = 0; i < width; i++)
    {
        ssim_value tmp;
        tmp = ssim_end1(sum0[i][0] + sum0[i + 1][0] + sum1[i][0] + sum1[i + 1][0],
                        sum0[i][1] + sum0[i + 1][1] + sum1[i][1] + sum1[i + 1][1],
                        sum0[i][2] + sum0[i + 1][2] + sum1[i][2] + sum1[i + 1][2],
                        sum0[i][3] + sum0[i + 1][3] + sum1[i][3] + sum1[i + 1][3]);
        ssim.L   += tmp.L;
        ssim.C_S += tmp.C_S;
    }

    return ssim;
}

ssim_value ssim_plane(
    pixel *pix1, intptr_t stride1,
    pixel *pix2, intptr_t stride2,
    int width, int height, void *buf, int *cnt)
{
    int z = 0;
    int x, y;
    ssim_value ssim;
    ssim.L = 0.0;
    ssim.C_S = 0.0;

    int(*sum0)[4] = (int(*)[4])buf; 
    int(*sum1)[4] = sum0 + (width >> 2) + 3;
    width >>= 2;
    height >>= 2; 
    for (y = 1; y < height; y++)
    {
        for (; z <= y; z++)
        {
            // FFSWAP( (int (*)[4]), sum0, sum1 );
            int(*tmp)[4] = sum0;
            sum0 = sum1;
            sum1 = tmp;

            for (x = 0; x < width; x += 2)
                ssim_4x4x2_core(&pix1[4 * (x + z * stride1)], stride1, &pix2[4 * (x + z * stride2)], stride2, &sum0[x]);
        }

        for (x = 0; x < width - 1; x += 4)
        {
            ssim_value tmp;
            tmp = ssim_end4(sum0 + x, sum1 + x, FFMIN(4, width - x - 1));
            ssim.L   += tmp.L;
            ssim.C_S += tmp.C_S;
        }
    }

    ssim.L /= (height - 1) * (width - 1);
    ssim.C_S /= (height - 1) * (width - 1);
    return ssim;
}

static void print_results(std::ofstream& msssim_log_f, float ms_ssim[3], int frames, int w, int h, int frame_index)
{
    printf("MS-SSIM Y:%.5f U:%.5f V:%.5f All:%.5f",
           ms_ssim[0] / frames,
           ms_ssim[1] / frames,
           ms_ssim[2] / frames,
           (ms_ssim[0] * 4 + ms_ssim[1] + ms_ssim[2]) / (frames * 6));

    msssim_log_f  << "n:" << frame_index + 1
            << std::setiosflags(std::ios::fixed) << std::setprecision(5)
            << " Y:"            << ms_ssim[0] / frames
            << " U:"            << ms_ssim[1] / frames
            << " V:"            << ms_ssim[2] / frames
            << " All:"          << (ms_ssim[0]*4 + ms_ssim[1] + ms_ssim[2]) / (frames*6)
            << std::endl;
    std::cout << "\r\033[k"; // 清空命令行.
}

static void downsample_2x2_mean(pixel *input, int width, int height, pixel *output) 
{
    int downsample_width =  width >> 1;
    int downsample_height = height >> 1;

    for (int y = 0; y < downsample_height; y++) 
    {
        for (int x =0; x < downsample_width; x++) 
        {
            output[y * downsample_width + x] = (input[2 * y * width + 2 * x] +
                                                input[2 * y * width + 2 * x + 1] +
                                                input[(2 * y + 1) * width + 2 * x] +
                                                input[(2 * y + 1) * width + 2 * x + 1]) / 4;
        }
    }
}

bool msssimVisualize(const std::string &ssimlog) {
	Py_Initialize();    //初始化

    #ifdef DEBUG
    std::cout << "......为了ssim能够正确生成时间维度的分析图，请检查ssim和python在同级目录下." << std::endl;
    #endif

	std::string path    = "python";
	std::string cmd_dir = std::string("sys.path.append(\"" + path + "\")");
    std::string cmd_dir1= std::string("sys.path.append(\"bin/" + path + "\")");	
	std::string ssim_dir = ssimlog.substr(0, ssimlog.find_last_of('/'));

    PyRun_SimpleString("import sys");
    PyRun_SimpleString("import os");
    PyRun_SimpleString(cmd_dir.c_str());
    PyRun_SimpleString(cmd_dir1.c_str());

    // 加载模块
    PyObject* moduleName = PyUnicode_FromString("msssim_graph_Py3");
    PyObject* pModule = PyImport_Import(moduleName);
    if (!pModule) {
        std::cout << "Python get module [msssim_graph_Py3] failed." << std::endl;
        return false;
    }

    #ifdef DEBUG
    std::cout << "Python get module [msssim_graph_Py3] succeed." <<std::endl;
    #endif

    // 加载函数
    PyObject* pv = PyObject_GetAttrString(pModule, "get_ssim_graph");
    if (!pv || !PyCallable_Check(pv)) {
        std::cout << "Can't find funftion [get_ssim_graph]" << std::endl;
        return false;
    }

    #ifdef DEBUG
    std::cout << "Python get function [get_ssim_graph] succeed." << std::endl;
    #endif

    // 设置参数
    PyObject* args = PyTuple_New(2); 
    PyObject* arg1 = Py_BuildValue("s", ssimlog.c_str());    
    PyObject* arg2 = Py_BuildValue("s", ssim_dir.c_str());
    PyTuple_SetItem(args, 0, arg1);
    PyTuple_SetItem(args, 1, arg2);

    #ifdef DEBUG    
    std::cout << "第一个参数：" << ssimlog << std::endl;
    std::cout << "第二个参数：" << ssim_dir << std::endl;
    #endif

    // 调用函数
    PyObject* pRet = PyObject_CallObject(pv, args);
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

    Py_Finalize();      //释放资源

    return true;
}

static float ms_ssim_plane(pixel *pix1, pixel *pix2, int width, int height, int scale = 5)
{
    ssim_value value;
    float result = 1.0;
    float luminance_value[5];
    int w = width;
    int h = height;

    int   *temp;
    pixel *ori_img1;
    pixel *ori_img2;
    pixel *sample_img1;
    pixel *sample_img2;

    if (scale < 1 || scale > 5) 
    {
        scale = 5;
    }

    temp        = (int *)malloc((2*w+12)*sizeof(*temp));
    ori_img1    = (pixel*)malloc(w * h * sizeof(pixel));
    ori_img2    = (pixel*)malloc(w * h * sizeof(pixel));
    sample_img1 = (pixel*)malloc(w * h * sizeof(pixel));
    sample_img2 = (pixel*)malloc(w * h * sizeof(pixel));

    for (int i = 0; i < w * h; i++) 
    {
        ori_img1[i] = pix1[i];
        ori_img2[i] = pix2[i];
    }

    // 计算每个尺度的ssim值.
    for (int i = 1; i <= scale; i++) 
    {
        if (i != 1) 
        {
            memset(sample_img1, 0, width * height);
            memset(sample_img2, 0, width * height);

            downsample_2x2_mean(ori_img1, w, h, sample_img1);
            downsample_2x2_mean(ori_img2, w, h, sample_img2);

            w = w >> 1;
            h = h >> 1;

            memset(ori_img1, 0, width * height);
            memset(ori_img2, 0, width * height);

            for (int j = 0; j < w * h; j++) 
            {
                ori_img1[j] = sample_img1[j];
                ori_img2[j] = sample_img2[j];
            }
        }

        value = ssim_plane(ori_img1, w, ori_img2, w, w, h, temp, NULL);
        result *= pow(value.C_S, WEIGHT[i-1]);
        luminance_value[i-1] = value.L;
    }

    free(temp);
    free(ori_img1);
    free(ori_img2);
    free(sample_img1);
    free(sample_img2);
    temp = NULL;
    ori_img1 = NULL;
    ori_img2 = NULL;
    sample_img1 = NULL;
    sample_img2 = NULL;

    result *= pow(luminance_value[scale-1], WEIGHT[scale-1]);
    return result;
}

static int get_video_info(const std::string& video, int& w, int& h){
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
    std::cout << s.width << std::endl;
    std::cout << s.height << std::endl;
    #endif

    capture.release();
    return 0;
}

int main(int argc, char *argv[])
{
    FILE *f[2];
    uint8_t *buf[2], *plane[2][3];
    int *temp;
    float ms_ssim[3] = {0, 0, 0};
    int frame_size, w1, h1, w2, h2;
    int frames, seek;
    int i;

    // 输入格式
    if (argc < 3)
    {
        printf("msssim <file1.yuv> <file2.yuv> \n");
        return -1;
    }

    // todo: 根据文件后缀来判断是否要转
    std::string yuv0 = "";
    std::string yuv1 = "";
    if (!mp42yuv(argv[1], yuv0) || !mp42yuv(argv[2], yuv1)) {
        std::cout << "生成yuv文件错误." << std::endl;
        return 0;
    }

    w1 = 0;
    h1 = 0;
    w2 = 0;
    h2 = 0;
    int res01 = get_video_info(argv[1], w1, h1);
    int res02 = get_video_info(argv[2], w2, h2);

    if(res01 != 0 || res02 != 0 || w1 != w2 || h1 != h2){
        std::cout << "file1.mp4 file2.mp4 have different size, check file rotated..." << std::endl;
        if(w1 == h2 && w2 == h1)
            std::cout << "file1.mp4 file2.mp4 rotated ..." << std::endl;
            // 原来计算的时候，默认就是按翻转的，所以这里先不翻转
            // todo: 翻转
        else
            return -1;
    }

    // 读入两个文件 长x宽
    f[0] = fopen(yuv0.c_str(), "rb");
    f[1] = fopen(yuv1.c_str(), "rb");

    if (w1 <= 0 || h1 <= 0 || w1 * (int64_t)h1 >= INT_MAX / 3 || 2LL * w1 + 12 >= INT_MAX / sizeof(*temp))
    {
        fprintf(stderr, "Dimensions are too large, or invalid\n");
        return -2;
    }

    // 一帧的内存大小
    // yuv420格式：先w*h个Y，然后1/4*w*h个U，再然后1/4*w*h个
    frame_size = w1 * h1 * 3LL / 2;

    // plane[i][0] Y分量信息
    // plane[i][1] U分量信息
    // plane[i][2] V分量信息
    for (i = 0; i < 2; i++)
    {
        buf[i] = (uint8_t *)malloc(frame_size);
        plane[i][0] = buf[i]; // plane[i][0] = buf[i]
        plane[i][1] = plane[i][0] + w1 * h1;
        plane[i][2] = plane[i][1] + w1 * h1 / 4;
    }

    seek = argc < 5 ? 0 : atoi(argv[4]);
    fseek(f[seek < 0], seek < 0 ? -seek : seek, SEEK_SET);

    // todo: 不写死了，从入参读
    std::string msssim_log     = "ssimDir/msssim.log";
    std::ofstream msssim_log_f(msssim_log, std::ios::out);
    if(!msssim_log_f) {
        std::cout << "打开文件: " << msssim_log << "失败!" << std::endl;
        return false;
    }

    // 逐帧计算
    for (frames = 0;; frames++)
    {
        float ms_ssim_one[3]; // Y U V 三个向量一帧ms-ssim的结果
        // 分别读入这一帧Y向量的地址，随之也获得了UV向量的起始地址
        if (fread(buf[0], frame_size, 1, f[0]) != 1)
            break;
        if (fread(buf[1], frame_size, 1, f[1]) != 1)
            break;
        for (int i = 0; i < 3; i++)
        {
            ms_ssim_one[i] = ms_ssim_plane(plane[0][i], plane[1][i], w1 >> !!i, h1 >> !!i);
            ms_ssim[i] += ms_ssim_one[i];
        }

        printf("Frame %d | ", frames);
        print_results(msssim_log_f, ms_ssim_one, 1, w1, h1, frames);
        printf("                \r");
        fflush(stdout);
    }

    if (!frames)
        return 0;

    printf("Total: %d frames | ", frames);
    print_results(msssim_log_f, ms_ssim, frames, w1, h1, frames);
    printf("\n");

    msssimVisualize(msssim_log);

    return 0;
}