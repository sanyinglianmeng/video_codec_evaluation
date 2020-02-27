#ifndef FORMAT_FACTORY_H_
#define FORMAT_FACTORY_H_

#include <iostream>
#include <vector>
#include <string>
#include <boost/filesystem.hpp>
#include <unistd.h>
#include <stdio.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdint.h>
#include <cstring>
#include <cmath>
#include <complex>
#include <iomanip>
#include <fstream>
#include <opencv2/opencv.hpp>
#include <boost/lexical_cast.hpp>
#include <Python.h>


namespace vce {

/**
 * 对src 旋转90度，然后存储在dst中.
 */
void Rotate90(const AVFrame* src, AVFrame* dst);

/**
 * 对src 旋转270度，然后存储在dst中.
 */
void Rotate270(const AVFrame* src, AVFrame* dst);

/**
 * 将MP4文件转为对应的yuv原始数据文件, 
 * 并存储在conf/psnr.yaml中配置的conf["psnr"]["resDir"]目录中。
 * @param mp4: 待转为yuv格式的MP4文件的路径.
 * @param yuv: yuv文件的存放地址.
 * @return: 成功返回true，失败返回false.
 * @TODO: 目前没有对t参数做处理，因此只能处理没有旋转的视频的mp4转yuv.
 */
bool mp42yuv(const std::string &mp4, const std::string &yuv);

} /* namespace vce */


#endif

