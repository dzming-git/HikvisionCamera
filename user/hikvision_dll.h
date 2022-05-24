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
    int nHeight;        /*ͼ���*/
    int nWidth;         /*ͼ���*/

    static bool init();
    bool login(const char* sDeviceAddress, const char* sUserName, const char* sPassword, unsigned short wPort = 8000);
    bool getImgInit();
    bool getImgBuf(unsigned char*& p_buffer, bool shallowCopy = false);
    bool PTZCtrl(int cmd, int speed, bool stop);
private:
    long handle;                       /*���*/
    long lUserID;                      /*��¼����ͷ���û���ʶ*/
    long nPort;                        /*��Ƶ��ͨ����*/
    void** pp_img;                     /*����ָ�룬����ָ������ͷͼ���Դ��ַ*/
    unsigned char* p_decoupledBuffer;  /*�����֮���ͼ��buffer*/
};

#endif // _HIKVISION_DLL_H_

