#include <iostream>
#include <opencv2/highgui/highgui.hpp>
#include <thread>
#include <string>
#include <opencv2/video/tracking.hpp>
#include <buffer.h>
#include <video.h>
#include <chrono>>

cv::Mat frame1;
cv::Mat frame2;

cv::Size videoResizer(double rows1,double cols1,double rows2,double cols2){ // Find optimal size of second image
   if(rows1 >= cols1 and rows2 >= cols2 ){
       cols2 *= cols1 / (1.5 *rows2) ;
       rows2 = cols1 /1.5;
   }
   if(rows1 <= cols1 and rows2 >= cols2 ){
       cols2 *= rows1 / (1.5 *rows2) ;
       rows2 = rows1 /1.5;
   }
   if(rows1 >= cols1 and rows2 <= cols2 ){
       rows2 *= cols1 / (1.5 *cols2) ;
       cols2 = cols1 /1.5;
   }
   if(rows1 <= cols1 and rows2 <= cols2 ){
       rows2 *= rows1 / (1.5 *cols2) ;
       cols2 = rows1 /1.5;
   }
   return cv::Size((int)cols2,(int)rows2);
}

cv::Mat videoPaste(cv::Mat  bigFrame,cv::Mat  smallFrame,cv::Size size)//Pasting small fideo to big one
{
resize(smallFrame,smallFrame,size);
smallFrame.copyTo(bigFrame(cv::Rect(0,0,smallFrame.cols, smallFrame.rows)));
return bigFrame;
}

cv::Mat frameChooser(Video &video1,Video &video2,int &i){//function to choose wich frames better to show
cv::Mat frame;
    if(video1.fpsCounter > 0){
        if(i%video1.fpsStep == 0){
            video1.fpsCounter--;
            video1.b->get().copyTo(frame1);
        }
    }
    else{
        video1.videoEnded = true;
    }

    if(video2.fpsCounter > 0){
        if(i%video2.fpsStep == 0){
           video2.fpsCounter--;
            video2.b->get().copyTo(frame2);
        }
    }
    else{
         video2.videoEnded = true;
    }

videoPaste(frame1,frame2,video2.size).copyTo(frame);
i++;
return frame;
}

int nok(int a, int b)
{
    int max = b;
    for(int i = max; i > 0; i++){

        if((i % a == 0) && (i % b == 0)){
            return i;
        }
    }
}

void getParam(Video &video1,Video &video2){//get essential params to make system work

}

void videoReader(Video &video,std::string filename){
    video.cap.open(filename);
    video.fps=(int)video.cap.get(cv::CAP_PROP_FPS);
    video.fpsCounter=(int)video.cap.get(cv::CAP_PROP_FRAME_COUNT);
    if(video.fps % 2 != 0) video.fps ++;// to avoid BIG NOK
    cv::Mat frame;
    video.cap>>frame;
    video.size=cv::Size(frame.cols,frame.rows);
    while(!frame.empty()){
        video.b->add(frame);
        video.cap>>frame;
    }
}



void imageDisplay(Video &video1,Video &video2){
    cv::Mat frame;
    int workingTime;
    int nokFPS;// this  var is needed to show videos in their real FPS
    video2.size = videoResizer(video1.size.height,video1.size.width,video2.size.height,video2.size.width);
    nokFPS = nok(video1.fps,video2.fps);
    video1.fpsStep = nokFPS / video1.fps;
    video2.fpsStep = nokFPS / video2.fps;
    int i=video1.fpsStep + video2.fpsStep; // to control fps
    double delay = (double)1000 / (double)nokFPS;
    while(!video1.videoEnded || !video2.videoEnded){
       auto t1= std::chrono::high_resolution_clock::now();
       frameChooser(video1,video2,i).copyTo(frame);
       auto t2= std::chrono::high_resolution_clock::now();
       workingTime = (int)((t1 - t2).count()/1e6);
       if(delay - workingTime < 1 ) workingTime =0;
       cv::waitKey(delay-workingTime);
       imshow("SuperBLackVideo",frame);
    }
}

int main()
{
    std::string file1 = "DSCN1369.MOV";
    std::string file2 = "DSCN1372.MOV";
    std::string file3 = "DSCN1378.MOV";
    std::string file4 = "Easy grass tricking combo.mp4";
    std::string file5 = "video6.mp4";
    std::string file6 = "IMG_5740.MOV";
    std::string file7 = "MVI_6052.mov";
    std::string file8 = "MVI_6089.mov";
    std::string file9 = "MVI_8301.MOV";
    Video video1(200);
    Video video2(200);

   std::thread firstVideoThread(videoReader,std::ref(video1),file8);
   std::thread secondVideoThread(videoReader,std::ref(video2),file9);
   video1.b->Used->wait();
   video1.b->Used->post();
   video2.b->Used->wait();
   video2.b->Used->post();
   std::thread videoShowingThread(imageDisplay,std::ref(video1),std::ref(video2));
    firstVideoThread.join();
    secondVideoThread.join();
    videoShowingThread.join();
    std::cout<<"The Programm was ended successfully"<<std::endl;
    return 0;
}
