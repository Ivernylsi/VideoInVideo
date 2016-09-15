#ifndef VIDEO_H
#define VIDEO_H
#include <buffer.h>

class Video {
public:
    int fps;//fps to show with natural speeed
    int fpsCounter;//Maximum count of frame to find point of stopping showing videos
    int fpsStep;
    bool videoEnded = false;
    cv::Size size;
    cv::VideoCapture cap;
    buffer *b;
    Video(int );

};

#endif // VIDEO_H
