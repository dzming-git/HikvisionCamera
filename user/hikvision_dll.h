#pragma once
#ifndef _HIKVISION_DLL_H_
#define _HIKVISION_DLL_H_
#ifdef LIBDLL
#define LIBDLL _declspec(dllimport)
#else
#define LIBDLL _declspec(dllexport)
#endif

class LIBDLL HikCamera
{
public:
    HikCamera();
    ~HikCamera();

public:
    int nHeight;        /*图像高*/
    int nWidth;         /*图像宽*/

    static bool init();
    bool login(const char* sDeviceAddress, const char* sUserName, const char* sPassword, unsigned short wPort = 8000);
    bool getImgInit();
    bool getImgBuf(unsigned char*& p_buffer, bool shallowCopy = false);
    bool PTZCtrl(int cmd, int speed, bool stop);
private:
    long handle;                       /*句柄*/
    long lUserID;                      /*登录摄像头的用户标识*/
    long nPort;                        /*视频流通道号*/
    void** pp_img;                     /*二重指针，最终指向摄像头图像的源地址*/
    unsigned char* p_decoupledBuffer;  /*解耦合之后的图像buffer*/
};

#endif // _HIKVISION_DLL_H_

