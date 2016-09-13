#include "buffer.h"
 


    buffer::buffer(int s){
            Free = new boost::interprocess::interprocess_semaphore (s);
            Used = new boost::interprocess::interprocess_semaphore (0);
    }

    void buffer::add(cv::Mat video){
        Free->wait();
        a.lock();
        cv::Mat ans;
        video.copyTo(ans);
        frame.push(ans);
        a.unlock();
        Used->post();
    }

    cv::Mat buffer::get(){
        Used->wait();
        a.lock();
        cv::Mat ans;
        frame.front().copyTo(ans);
        frame.pop();
        a.unlock();
        Free->post();
        return ans;
    }
