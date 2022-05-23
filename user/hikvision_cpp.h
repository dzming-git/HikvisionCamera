#ifndef __HIKVISION_H__
#define __HIKVISION_H__


class HikCamera
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
private:
    long lUserID;                      /*��¼����ͷ���û���ʶ*/
    long nPort;                        /*��Ƶ��ͨ����*/
    void** pp_img;                     /*����ָ�룬����ָ������ͷͼ���Դ��ַ*/
    unsigned char* p_decoupledBuffer;  /*�����֮���ͼ��buffer*/
};
#endif // HIKVISION_H__
