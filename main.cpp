#include <iostream>
#include <opencv2/highgui/highgui.hpp>
#include <thread>
#include <string>
#include <opencv2/video/tracking.hpp>
#include <buffer.h>

bool videoEnded = true;//when it will be false video would ended
int nokFPS;// this 3 vars are needed to show videos in their real FPS
int fpsStep1;//
int fpsStep2;//

cv::Mat frame1;
cv::Mat frame2;

cv::Size size; // second frame size;

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

cv::Mat videoPaste(cv::Mat  bigFrame,cv::Mat  smallFrame)//Pasting small fideo to big one
{
resize(smallFrame,smallFrame,size);
smallFrame.copyTo(bigFrame(cv::Rect(0,0,smallFrame.cols, smallFrame.rows)));
return bigFrame;
}

cv::Mat frameChooser(buffer &b,buffer &b1,int counter1,int counter2,int &i){//function to choose wich frames better to show
cv::Mat frame;
if(counter1 >0 && counter2>0)
    {
     if(i%fpsStep1 == 0){
         b.fpsCounter --;
         b.get().copyTo(frame1);
     }
     if(i%fpsStep2 == 0)
     {
         b1.fpsCounter --;
         b1.get().copyTo(frame2);
     }
    videoPaste(frame1,frame2).copyTo(frame);
    }
if(counter1 ==0 && counter2>0) {
    if(i%fpsStep2 == 0)
    {
        b1.fpsCounter --;
        b1.get().copyTo(frame2);
    }
    cv::resize(frame2,frame2,size);
    frame2.copyTo(frame);
    }
if(counter1 >0 && counter2==0) {
    if(i%fpsStep1 == 0){
        b.fpsCounter --;
        b.get().copyTo(frame1);
    }
    frame1.copyTo(frame);
}
if(counter1 ==0 && counter2==0) videoEnded = false;
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

void getParam(buffer &b,buffer &b1){//get essential params to make system work
    b.a.lock();
    b1.a.lock();
    size = videoResizer(b.rows,b.cols,b1.rows,b1.cols);
    b1.rows = size.height;
    b1.cols = size.width;
    nokFPS = nok(b.fps,b1.fps);
    fpsStep1=nokFPS / b.fps;
    fpsStep2=nokFPS / b1.fps;
    b1.a.unlock();
    b.a.unlock();
}

void videoReader(std::string filename,buffer &b){
    cv::VideoCapture cap;///
    cap.open(filename);
    b.fps=(int)cap.get(cv::CAP_PROP_FPS);
    b.fpsCounter=(int)cap.get(cv::CAP_PROP_FRAME_COUNT);
    if(b.fps % 2 != 0) b.fps ++;// to avoid BIG NOK
    cv::Mat frame;
    cap>>frame;
    b.rows = frame.rows;
    b.cols = frame.cols;// finding essential information about both videos
    while(true){
        if(frame.empty()) break;
        b.add(frame);
        cap>>frame;
    }
}



void imageDisplay(buffer &b,buffer &b1){
    cv::Mat frame;
    getParam(b,b1);
    int i=nokFPS; // to control fps
    double delay = (double)1000 / (double)nokFPS;
    while(true){
       frameChooser(b,b1,b.fpsCounter,b1.fpsCounter,i).copyTo(frame);
       if(!videoEnded) break;
       cv::waitKey(delay);
       imshow("SuperBLackVideo",frame);
    }
}

int main()
{
    std::string file1 = "DSCN1369.MOV";
    std::string file2 = "MVI_6089.mov";
    std::string file3 = "DSCN1372.MOV";
    std::string file4 = "DSCN1378.MOV";
    std::string file5 = "MVI_6052.mov";
    std::string file6 = "Easy grass tricking combo.mp4";
    std::string file7 = "video6.mp4";
    std::string file8 = "IMG_5740.MOV";
    buffer b(200);
    buffer b1(200);
    std::thread firstVideoThread(videoReader,file1,std::ref(b));
    std::thread secondVideoThread(videoReader,file7,std::ref(b1));
    usleep(1000*2000);
    std::thread videoShowingThread(imageDisplay,std::ref(b),std::ref(b1));
    firstVideoThread.join();
    secondVideoThread.join();
    videoShowingThread.join();
    return 0;
}
