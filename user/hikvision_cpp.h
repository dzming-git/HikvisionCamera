#ifndef __HIKVISION_H__
#define __HIKVISION_H__
#include <opencv2\core\core.hpp>
#include <opencv2\highgui\highgui.hpp>
#include <opencv2\imgproc\imgproc.hpp>

using namespace cv;
using namespace std;

class HikCamera
{
public:
    HikCamera();
    ~HikCamera();

public:
    bool init();
    bool login(const char* sDeviceAddress, const char* sUserName, const char* sPassword, unsigned short wPort = 8000);
    bool getImgInit();
    bool getImg(Mat& img, bool shallowCopy);
private:
    long lUserID;           /*登录摄像头的用户标识*/
    long nPort;             /*视频流通道号*/
    void** pp_img;          /*二重指针，最终指向摄像头图像的源地址*/
    unsigned char* buffer;  /*解耦合之后的图像buffer*/
    int h;                  /*图像高*/
    int w;                  /*图像宽*/
};
#endif // HIKVISION_H__
