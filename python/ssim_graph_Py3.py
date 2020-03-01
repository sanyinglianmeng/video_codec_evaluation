#!/usr/bin/env python
# -*- coding: utf-8 -*-

"""
@author: Gerren Liu( https://github.com/GerenLiu )

"""

import matplotlib.pyplot as plt
import os
import sys
from matplotlib.ticker import MultipleLocator, FormatStrFormatter

def ssimFileAnalysis(ssimLogFilePath):
    """
    读取ssim.log文件，识别各分量的值
    Args:
        ssimLogFilePath: ssim数据文件
    Returns:
        List
    """
    if not os.path.exists(ssimLogFilePath):
        return
    pic_x_list = []
    ssim_y_list = []
    ssim_u_list = []
    ssim_v_list = []
    ssim_all_list = []
    ssim_db_list = []

    with open(ssimLogFilePath, 'r') as f:
        for line in f.readlines():
            pic_x = int(line.split('n:')[1].split(" ")[0])
            pic_x_list.append(pic_x)

            ssim_all = float(line.split('Y:')[1].split(' ')[0])
            ssim_all_list.append(ssim_all)

            ssim_y = float(line.split('U:')[1].split(' ')[0])
            ssim_y_list.append(ssim_y)

            ssim_u = float(line.split('V:')[1].split(' ')[0])
            ssim_u_list.append(ssim_u)

            ssim_db =float(line.split('All:')[1].split(' ')[0])
            ssim_v_list.append(ssim_db)

            ssim_db = float(line.split('ssim_db:')[1].split(' ')[0])
            ssim_db_list.append(ssim_db)

        return pic_x_list, ssim_all_list, ssim_y_list, ssim_u_list, ssim_v_list


def ssim_graph(res, dir):
    """
    每一帧图片及yuv每个通道的ssim值可视化图
    Args:
        pic_x_list, ssim_all_list, ssim_y_list, ssim_u_list, ssim_v_list
    Returns:
        None
    """
    
    pic_x_list    = res[0]
    ssim_y_list   = res[2]
    ssim_u_list   = res[3]
    ssim_v_list   = res[4]
    ssim_all_list = res[1]

    y_all = []
    y_all.extend(ssim_y_list)
    y_all.extend(ssim_u_list)
    y_all.extend(ssim_v_list)
    y_all.extend(ssim_all_list)

    y_min = min(y_all)
    y_max = max(y_all)
    y_min -= 0.1
    y_max += 0.1

    if (y_min <= 0.0):
        y_min = 0.0
    if (y_max >= 1.0):
        y_max = 1.0

    y_min = round(y_min, 1)
    y_max = round(y_max, 1)

    # 创建绘图对象
    plt.figure(figsize=(10,8))
    
    plt.plot(list(map(int, pic_x_list)), list(map(float, ssim_all_list)), "r",linewidth=1,label='ssim_all')
    plt.plot(list(map(int, pic_x_list)), list(map(float, ssim_y_list)), "g",linewidth=1,label='ssim_y')
    plt.plot(list(map(int, pic_x_list)), list(map(float, ssim_u_list)), "b",linewidth=1,label='ssim_u')
    plt.plot(list(map(int, pic_x_list)), list(map(float, ssim_v_list)),color='black',linewidth=1,label='ssim_v')

    
    plt.xlabel("Frame Number")
    plt.ylabel("ssim Value")
    plt.title("ssim Temporal Graph")
    plt.legend()

    xmajorLocator = MultipleLocator(int(int(len(pic_x_list) / 10) / 10) * 10)
    
    ymajorLocator = MultipleLocator(0.1)
    ax=plt.gca()
    ax.xaxis.set_major_locator(xmajorLocator)
    ax.yaxis.set_major_locator(ymajorLocator)
    # todo 根据y轴峰峰值做动态调整
    plt.ylim(y_min,y_max)
    plt.grid(True)

    # plt.show()
    plt.savefig(dir + '/ssim.png')

def get_ssim_graph(ssimlog, ssimdir):
    res = ssimFileAnalysis(ssimlog)
    ssim_graph(res, ssimdir)

if __name__ == '__main__':
    get_ssim_graph(sys.argv[1], sys.argv[2])

