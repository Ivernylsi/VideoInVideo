#ifndef BUFFER_H
#define BUFFER_H
#include <opencv2/highgui/highgui.hpp>
#include <mutex>
#include <string>
#include <queue>
#include <opencv2/video/tracking.hpp>
#include <boost/interprocess/sync/interprocess_semaphore.hpp>
#include <thread>

class buffer
{
public:
 std::queue<cv::Mat> frame;
 std::mutex a;
 boost::interprocess::interprocess_semaphore *Free;
 boost::interprocess::interprocess_semaphore *Used;
 int fps;//fps to show with natural speeed
 int rows;
 int cols;
 int fpsCounter;//Maximum count of frame to find point of stopping showing videos

 buffer(int );

 void add(cv::Mat);


 cv::Mat get();


};

#endif // BUFFER_H
