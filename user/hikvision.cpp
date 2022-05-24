#include "HCNetSDK.h"
#include "WindowsPlayM4.h"
#ifdef DLLGENERATE_EXPORTS
#include "hikvision_dll.h"
#else
#include "hikvision_cpp.h"
#endif // DLLGENERATE_EXPORTS
#include<ctime>
#include <opencv2\core\core.hpp>
#include <opencv2\highgui\highgui.hpp>
#include <opencv2\imgproc\imgproc.hpp>

/*==================================================================
�� �� ����DecCBFun
�����������ص����������ڽ��룬����������ͼ�񴫳�
���������
----------pBuf:       ͼ�񻺳���ָ��
----------pFrameInfo��ͼ�����Ϣ�������nWidth�����ߣ�nHeight����ͼ�����ͣ�nType��
----------pp_img��    ��getImgInit�����е�PlayM4_SetDecCallBackMend��Ϊ�������룬���ͼ���˫��ָ��
��    �ߣ�Dzm
��    �ڣ�2022.05.22
��    ������������SDK���õ���PlayCtrlģ���е�4��PlayM4_SetDecCallBack��û��void*����
          https:qcsdn.com/q/a/324952.html#download
          ������վ�е�PlayCtrlģ���е�PlayM4_SetDecCallBack��void* �����������滻����
==================================================================*/
void CALLBACK DecCBFun(long, char* pBuf, long, FRAME_INFO* pFrameInfo, void* pp_img, void*)
{
    cv::Mat* pImg = *(cv::Mat**)pp_img;
    if (nullptr == pImg)
    {
        *(cv::Mat**)pp_img = new cv::Mat;
        pImg = *(cv::Mat**)pp_img;
        pImg->create(pFrameInfo->nHeight, pFrameInfo->nWidth, CV_8UC3);
    }
    cv::Mat* pBGRImg = pImg;
    if (T_YV12 == pFrameInfo->nType)
    {
        cv::Mat YUVImg(pFrameInfo->nHeight + pFrameInfo->nHeight / 2, pFrameInfo->nWidth, CV_8UC1, (unsigned char*)pBuf);
        cv::cvtColor(YUVImg, *pBGRImg, cv::COLOR_YUV2BGR_YV12);
        YUVImg.~Mat();
    }
}
/*==================================================================
�� �� ����fRealDataCallBack_V30
�����������ص�����������ʵʱ��Ƶ�������ݻ�ȡ
���������
----------dwDataType:    ��ȡ�����ݶ�����
----------pBuffer��      ���ݶλ�����ָ��
----------dwBufSize��    ���ݶδ�С
----------p_nPort��      ��Ƶ��ͨ����ָ��
��    �ߣ�Dzm
��    �ڣ�2022.05.22
��    ����
==================================================================*/
void CALLBACK fRealDataCallBack_V30(LONG, DWORD dwDataType, BYTE* pBuffer, DWORD dwBufSize, void* p_nPort)
{
    LONG nPort = *(LONG*)p_nPort;
    if (false == (\
        NET_DVR_STREAMDATA == dwDataType && \
        dwBufSize > 0 && nPort != -1 \
        ))
    {
        return;
    }
    PlayM4_InputData(nPort, pBuffer, dwBufSize);
}
/*==================================================================
�� �� ����HikCamera::init
��������������������������ͷ��ʼ��
�� �� ֵ���Ƿ��ʼ���ɹ�
��    �ߣ�Dzm
��    �ڣ�2022.05.22
��    ������̬�����������Աֻ��Ҫ����һ��
==================================================================*/
bool HikCamera::init()
{
    bool flag = NET_DVR_Init();
    NET_DVR_SetConnectTime(2000, 1);
    NET_DVR_SetReconnect(10000, true);
    return flag;
}
/*==================================================================
�� �� ����HikCamera::login
��������������������������ͷ��¼
���������
----------sDeviceAddress����������ͷip��ַ
----------sUserName����¼�û���
----------sPassword����¼����
----------wPort���˿ںţ�һ��Ϊ8000
�� �� ֵ���Ƿ��¼�ɹ�
��    �ߣ�Dzm
��    �ڣ�2022.05.22
��    ����
==================================================================*/
bool HikCamera::login(const char* sDeviceAddress, const char* sUserName, const char* sPassword, unsigned short wPort)
{
    NET_DVR_USER_LOGIN_INFO pLoginInfo;
    NET_DVR_DEVICEINFO_V40 lpDeviceInfo;

    pLoginInfo.bUseAsynLogin = 0;     //ͬ����¼��ʽ
    strcpy_s(pLoginInfo.sDeviceAddress, sDeviceAddress);
    strcpy_s(pLoginInfo.sUserName, sUserName);
    strcpy_s(pLoginInfo.sPassword, sPassword);
    pLoginInfo.wPort = wPort;

    lUserID = NET_DVR_Login_V40(&pLoginInfo, &lpDeviceInfo);

    return lUserID >= 0;
}
/*==================================================================
�� �� ����HikCamera::getImgInit
������������ȡͼƬ֮ǰ���ĳ�ʼ��
�� �� ֵ���Ƿ��ʼ���ɹ�
��    �ߣ�Dzm
��    �ڣ�2022.05.22
��    �����ú�����Ҫ��Ϊ�������֣������Ⱥ�˳��
          1.���ý���ص�
          2.����ʵʱԤ��������ʵʩ�ص�
          ��������ͷ��֧�����������䣬��������������Ż���mark��
==================================================================*/
bool HikCamera::getImgInit()
{
    cv::Mat **temp = new cv::Mat*;
    pp_img = (void**)(void*)temp;
    //���ý���ص�
    if (false == (\
        PlayM4_GetPort(&nPort) /* ��ȡ���ſ�ͨ���� */ && \
        PlayM4_SetStreamOpenMode(nPort, STREAME_REALTIME) /* ������ģʽ */ && \
        PlayM4_OpenStream(nPort, nullptr, 0, 1024 * 1024) /* ���� ���һ���������˽⺬�塾mark�� */ && \
        PlayM4_SetDecCallBackMend(nPort, DecCBFun, pp_img) /* ������Ƶ����ص����� */ && \
        PlayM4_Play(nPort, nullptr) /* ��ʼ���� */))
    {
        return false;  // ���ý���ص�ʧ�ܣ�����
    }

    //����ʵʱԤ��������ʵʩ�ص�
    NET_DVR_PREVIEWINFO* pStruPlayInfo = new NET_DVR_PREVIEWINFO;
    pStruPlayInfo->hPlayWnd = nullptr; //����Ϊ�գ��豸SDK������ֻȡ��
    pStruPlayInfo->lChannel = 1; //Channel number �豸ͨ��
    pStruPlayInfo->dwStreamType = 0;// �������ͣ�0-��������1-��������2-����3��3-����4, 4-����5,5-����6,7-����7,8-����8,9-����9,10-����10
    pStruPlayInfo->dwLinkMode = 0;// 0-TCP��ʽ,1-UDP��ʽ,2-�ಥ��ʽ,3-RTP��ʽ��4-RTP/RTSP,5-RSTP/HTTP
    pStruPlayInfo->bBlocked = 0; // 0-������ȡ��, 1-����ȡ��
    LONG handle = NET_DVR_RealPlay_V40(lUserID, pStruPlayInfo, fRealDataCallBack_V30, &nPort);
    if (handle < 0)
    {
        NET_DVR_Logout(lUserID);
        NET_DVR_Cleanup();
        return false;  // ʵʱ����ʧ�ܣ�����
    }
    clock_t t0, t1;
    t0 = clock();
    while (0 == nHeight * nWidth)  // �ؼ�һ�����ȴ��ص�����DecCBFun����֮��ſ��Խ���֮�����
    {
        t1 = clock();
        if (t1 - t0 > 2000)
        {
            NET_DVR_StopRealPlay(handle);
            NET_DVR_Logout(lUserID);
            NET_DVR_Cleanup();
            return false;  // �ص���������ʧ�ܣ�����
        }
        cv::Mat* p_img = *(cv::Mat**)pp_img;
        if (nullptr != p_img && false == p_img->empty())
        {
            nHeight = p_img->rows;
            nWidth = p_img->cols;
        }
    }
    p_decoupledBuffer = new unsigned char[nHeight * nWidth * 3];
    return true;
}
/*==================================================================
�� �� ����HikCamera::getImgBuf
������������ȡͼ��buffer
���������
----------buffer��     ͼ��buffer�����÷�ʽ��Ϊ���
----------shallowCopy���Ƿ����ǳ����
�� �� ֵ���Ƿ��ȡͼƬ
��    �ߣ�Dzm
��    �ڣ�2022.05.23
��    ��������Լ�ʵ�֣�����ʹ��copyTo
==================================================================*/
bool HikCamera::getImgBuf(unsigned char*& buffer, bool shallowCopy)
{
    cv::Mat* p_img = *(cv::Mat**)pp_img;
    if (nullptr == p_img)
    {
        return false;
    }
    if (true == shallowCopy)
    {
        // ǳ������Ч�ʸߣ����Ǵ������ݿ��ܻ�������������ݵ��ٶȣ���ɴ�����������ݲ��ֱ������ɵĸ���
        buffer = p_img->data;
    }
    else
    {
        // ���������洢�ռ������Ч�ʵͣ������ȶ�
        unsigned char* pImgDataSrc = &(p_img->at<uchar>(0, 0));
        int ucharCount = nHeight * nWidth * 3;
        memcpy(p_decoupledBuffer, pImgDataSrc, ucharCount);
        buffer = p_decoupledBuffer;
    }
    return true;
}

/*==================================================================
�� �� ����HikCamera::PTZCtrl
��������������ͷ��̨����
���������
----------cmd��   ���������ͷ�ļ��Ժ궨�����ʽ����
----------speed�� ת���ٶ�
----------stop��  ֹͣ��־λ��Ϊfalseʱ��ʼ��Ϊtrueʱֹͣ
�� �� ֵ���Ƿ���̨���Ƶ��óɹ�
��    �ߣ�Dzm
��    �ڣ�2022.05.24
��    ����
ZOOM_IN 11 ������(���ʱ��) 
ZOOM_OUT 12 �����С(���ʱ�С) 
FOCUS_NEAR 13 ����ǰ�� 
FOCUS_FAR 14 ������ 
IRIS_OPEN 15 ��Ȧ���� 
IRIS_CLOSE 16 ��Ȧ��С 
TILT_UP 21 ��̨���� 
TILT_DOWN 22 ��̨�¸� 
PAN_LEFT 23 ��̨��ת 
PAN_RIGHT 24 ��̨��ת 
UP_LEFT 25 ��̨��������ת 
UP_RIGHT 26 ��̨��������ת 
DOWN_LEFT 27 ��̨�¸�����ת 
DOWN_RIGHT 28 ��̨�¸�����ת 
==================================================================*/
bool HikCamera::PTZCtrl(int cmd, int speed, bool stop)
{
    return NET_DVR_PTZControlWithSpeed(handle, cmd, stop, speed);
}

HikCamera::HikCamera()
{}

HikCamera::~HikCamera()
{
    delete* pp_img;
    delete pp_img;
}