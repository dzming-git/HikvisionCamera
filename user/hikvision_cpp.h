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
    long lUserID;           /*��¼����ͷ���û���ʶ*/
    long nPort;             /*��Ƶ��ͨ����*/
    void** pp_img;          /*����ָ�룬����ָ������ͷͼ���Դ��ַ*/
    unsigned char* buffer;  /*�����֮���ͼ��buffer*/
    int h;                  /*ͼ���*/
    int w;                  /*ͼ���*/
};
#endif // HIKVISION_H__
