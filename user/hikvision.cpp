#include "HCNetSDK.h"
#include "WindowsPlayM4.h"
#ifdef DLLGENERATE_EXPORTS
#include "hikvision_dll.h"
#else
#include "hikvision_cpp.h"
#endif // DLLGENERATE_EXPORTS
#include <iostream>

using namespace std;
/*==================================================================
�� �� ����DecCBFun
�����������ص����������ڽ��룬����������ͼ�񴫳�
���������
          pBuf:       PlayCtrl�ڲ�����
          pFrameInfo��PlayCtrl�ڲ�����
          pp_img��     ��getImgInit�����е�PlayM4_SetDecCallBackMend��Ϊ�������룬���ͼ���˫��ָ��
��    �ߣ�Dzm
��    �ڣ�2022.05.22
��    ������������SDK���õ���PlayCtrlģ���е�4��PlayM4_SetDecCallBack��û��void*����
          https:qcsdn.com/q/a/324952.html#download
          ������վ�е�PlayCtrlģ���е�PlayM4_SetDecCallBack��void* �����������滻����
==================================================================*/
void CALLBACK DecCBFun(long, char* pBuf, long, FRAME_INFO* pFrameInfo, void* pp_img, void*)
{
    Mat* pImg = *(Mat**)pp_img;
    if (nullptr == pImg)
    {
        *(Mat**)pp_img = new Mat;
        pImg = *(Mat**)pp_img;
        pImg->create(pFrameInfo->nHeight, pFrameInfo->nWidth, CV_8UC3);
    }
    Mat* pBGRImg = pImg;
    if (T_YV12 == pFrameInfo->nType)
    {
        Mat YUVImg(pFrameInfo->nHeight + pFrameInfo->nHeight / 2, pFrameInfo->nWidth, CV_8UC1, (unsigned char*)pBuf);
        cvtColor(YUVImg, *pBGRImg, COLOR_YUV2BGR_YV12);
        YUVImg.~Mat();
    }
}
/*==================================================================
�� �� ����fRealDataCallBack_V30
�����������ص�����������ʵʱ��Ƶ�������ݻ�ȡ
���������
          dwDataType:    ��ȡ�����ݶ�����
          pBuffer��      ���ݶλ�����
          dwBufSize��    ���ݶδ�С
          p_nPort��      ��Ƶ��ͨ����ָ��
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
��    ����
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
          sDeviceAddress����������ͷip��ַ
          sUserName����¼�û���
          sPassword����¼����
          wPort���˿ںţ�һ��Ϊ8000
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
���������
          sDeviceAddress����������ͷip��ַ
          sUserName����¼�û���
          sPassword����¼����
          wPort���˿ںţ�һ��Ϊ8000
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
    Mat **temp = new Mat*;
    pp_img = (void**)(void*)temp;
    //���ý���ص�
    if (false == (\
        PlayM4_GetPort(&nPort) /* ��ȡ���ſ�ͨ���� */ && \
        PlayM4_SetStreamOpenMode(nPort, STREAME_REALTIME) /* ������ģʽ */ && \
        PlayM4_OpenStream(nPort, nullptr, 0, 1024 * 1024) /* ���� ���һ���������˽⺬�塾mark�� */ && \
        PlayM4_SetDecCallBackMend(nPort, DecCBFun, pp_img) /* ������Ƶ����ص����� */ && \
        PlayM4_Play(nPort, nullptr) /* ��ʼ���� */))
    {
        return false;
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
        return false;
    }

    int waitMax = 0;
    int WAITMAX = 100;
    while (nullptr == *pp_img) // �ؼ�һ�����ȴ��ص�����DecCBFun����֮��ſ��Խ���֮�����
    {
        waitMax++;
        if (WAITMAX == waitMax)
        {
            NET_DVR_StopRealPlay(handle);
            NET_DVR_Logout(lUserID);
            NET_DVR_Cleanup();
            return false;
        }
        Sleep(100);
    }
}
/*==================================================================
�� �� ����HikCamera::getImg
������������ȡͼ��
���������
----------img��        ͼ��������÷�ʽ��Ϊ���
----------shallowCopy���Ƿ����ǳ����
�� �� ֵ���Ƿ��ȡͼƬ
��    �ߣ�Dzm
��    �ڣ�2022.05.22
��    ��������Լ�ʵ�֣�����ʹ��copyTo
==================================================================*/
bool HikCamera::getImg(Mat& img, bool shallowCopy)
{
    Mat* pImg = *(Mat**)pp_img;
    if (pImg == nullptr)
    {
        return false;
    }
    if (true == shallowCopy)
    {
        // ǳ������Ч�ʸߣ����Ǵ������ݿ��ܻ�������������ݵ��ٶȣ���ɴ�����������ݲ��ֱ������ɵĸ���
        img = *pImg;
    }
    else
    {
        // ���������洢�ռ������Ч�ʵͣ������ȶ�
//         (*pUserParaDecCBFun->pp_img)->copyTo(*(Mat*)pCameraInfo->pImg);
        // ��һ��������ʱ��ᵼ�³�ͻ�������Լ�ʵ��
        // Mat�����ݴ洢��ַ����������uchar��8λ������ȡ��ֻ��Ҫ���׵�ַ�����ȡrows*cols*3�μ���
        // ���յ�ַ��ȡʵʱ���ݣ���Ӱ�����ݵ�д�룬������ֳ�ͻ
        int rows = pImg->rows;
        int cols = pImg->cols;
        img.create(rows, cols, CV_8UC3);
        unsigned char* pImgDataSrc = &(pImg)->at<uchar>(0, 0);
        unsigned char* pImgDataDst = &img.at<uchar>(0, 0);
        memcpy(pImgDataDst, pImgDataSrc, rows * cols * 3);
    }
    return true;
}

HikCamera::HikCamera()
{}

HikCamera::~HikCamera()
{
    delete* pp_img;
    delete pp_img;
}