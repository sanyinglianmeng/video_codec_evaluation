#include "format_factory.h"


void Rotate90(const AVFrame* src, AVFrame* dst){
    int half_width   = src->width >> 1;
    int half_height  = src->height >> 1;
    int size         = src->linesize[0] * src->height;
    int half_size    = size >> 2;

    for (int j = 0, n = 0; j < src->width; j++) {
        int pos = size;
        for (int i = src->height - 1; i >= 0; i--) {
            pos -= src->linesize[0];
            dst->data[0][n++] = src->data[0][pos + j];
        }
    }

    for (int j = 0, n = 0; j < half_width; j++) {
        int pos = half_size;
        for (int i = half_height - 1; i >= 0; i--) {
            pos -= src->linesize[1];
            dst->data[1][n] = src->data[1][pos + j];
            dst->data[2][n++] = src->data[2][pos + j];
        }
    }

    dst->height = src->width;
    dst->width  = src->height;
}

void Rotate270(const AVFrame* src, AVFrame* dst){
    int half_width = src->linesize[0] >> 1;
    int half_height = src->height >> 1;

    for (int i = src->width - 1, n = 0; i >= 0; i--) {
        for (int j = 0, pos = 0; j < src->height; j++) {
            dst->data[0][n++] = src->data[0][pos + i];
            pos += src->linesize[0];
        }
    }

    for (int i = (src->width >> 1) - 1, n = 0; i >= 0; i--) {
        for (int j = 0, pos = 0; j < half_height; j++) {
            dst->data[1][n] = src->data[1][pos + i];
            dst->data[2][n++] = src->data[2][pos + i];
            pos += half_width;
        }
    }
    dst->width  = src->height;
    dst->height = src->width;
}

bool mp42yuv(const std::string &mp4, const std::string &yuv){
    // todo: 将格式转换迁到这里来
    return true;
}