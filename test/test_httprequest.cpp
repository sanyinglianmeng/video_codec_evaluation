#include <iostream>
#include <vector>
#include <string>
#include <boost/filesystem.hpp>
#include <unistd.h>
#include <stdio.h>

#include "../src/lib/ocrlib.h"
#include "../src/utils/cmdlineutils.h"
#include "../src/utils/matrixutils.h"
#include "../src/frame_drop_detect.h"

int main(int argc, char *argv[]) {
    std::string file1= "videoDB/t42.mp4";
	std::string file = "videoDB/t43.mp4";
	int oriWidth     = 720;
	int oriHeight    = 1280;
	int oriType      = 3;
	std::string dir  = "psnr/data";
	
    // 获取原始视频的帧数
    int frameNumber = GetVideoFrameNumber(file1);
    std::cout << "frameNumber: " << frameNumber << std::endl;

    // 清空目录
    DeleteFiles(dir);

	// 获取帧号图像，存储在dir目录下
	GetFrameLabel(file, oriWidth, oriHeight, EVideoType(oriType), dir);

	// ocr检测, 丢帧信息存放在v中
    std::vector<int> v;
    CheckFrameDrop(dir, frameNumber, v);

    for (auto &i : v) {
        std::cout << i << " ";
    }
    std::cout << std::endl;

    // 计算丢帧
    // ...

    // 计算psnr
    // ...
    
	return 0;
}

