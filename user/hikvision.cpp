#include "HCNetSDK.h"
#include "WindowsPlayM4.h"
#ifdef DLLGENERATE_EXPORTS
#include "hikvision_dll.h"
#else
#include "hikvision_cpp.h"
#endif // DLLGENERATE_EXPORTS
#include <ctime>
#include <thread>

/*==================================================================
�� �� ����DecCBFun
�����������ص����������ڽ��룬����������ͼ�񴫳�
���������
----------pBuf:              ͼ�񻺳���ָ��
----------pFrameInfo��       ͼ�����Ϣ�������nWidth�����ߣ�nHeight����ͼ�����ͣ�nType��
----------pp_yuv420��        ��getImgInit�����е�PlayM4_SetDecCallBackMend��Ϊ�������룬���ͼ���˫��ָ��
                             ǰ HEAD_LENGTH λΪ��Ϣλ��Ŀǰ���ţ�0-3λ�洢�߿�4λ������5-end�洢yuv420��buffer
��    �ߣ�Dzm
��    �ڣ�2022.05.26
��    ������������SDK���õ���PlayCtrlģ���е�4��PlayM4_SetDecCallBack��û��void*����
          https:qcsdn.com/q/a/324952.html#download
          ������վ�е�PlayCtrlģ���е�PlayM4_SetDecCallBack��void* �����������滻����
==================================================================*/
#define HEAD_LENGTH 5
void CALLBACK DecCBFun(long, char* pBuf, long, FRAME_INFO* pFrameInfo, void* pp_yuv420, void*)
{
    static int bufLen = (pFrameInfo->nHeight + pFrameInfo->nHeight / 2) * pFrameInfo->nWidth;
    unsigned char* p_yuv420 = *(unsigned char**)pp_yuv420;
    if (nullptr == p_yuv420)
    {
        p_yuv420 = new unsigned char[bufLen + HEAD_LENGTH];
        p_yuv420[0] = pFrameInfo->nHeight / 255;
        p_yuv420[1] = pFrameInfo->nHeight % 255;
        p_yuv420[2] = pFrameInfo->nWidth / 255;
        p_yuv420[3] = pFrameInfo->nWidth % 255;
        p_yuv420[4] = 0;
    }
    if (T_YV12 == pFrameInfo->nType)
    {
        memcpy(p_yuv420 + HEAD_LENGTH, pBuf, bufLen);
    }
    *(unsigned char**)pp_yuv420 = p_yuv420;
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
��    �ڣ�2022.05.26
��    ������̬�����������Աֻ��Ҫ����һ��
==================================================================*/
bool HikCamera::init()
{
    bool flag = true;
    flag &= NET_DVR_Init();
    flag &= NET_DVR_SetConnectTime(2000, 1);
    flag &= NET_DVR_SetReconnect(10000, true);
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

    userID = NET_DVR_Login_V40(&pLoginInfo, &lpDeviceInfo);

    return userID >= 0;
}

/*==================================================================
�� �� ����HikCamera::getImgInit
������������ȡͼƬ֮ǰ���ĳ�ʼ��
�� �� ֵ���Ƿ��ʼ���ɹ�
��    �ߣ�Dzm
��    �ڣ�2022.05.26
��    �����ú�����Ҫ��Ϊ�������֣������Ⱥ�˳��
          1.���ý���ص�
          2.����ʵʱԤ��������ʵʩ�ص�
          ��������ͷ��֧�����������䣬��������������Ż���mark��
==================================================================*/
bool HikCamera::getImgInit()
{
    //���ý���ص�
    bool flag = true;
    flag &= PlayM4_GetPort(&nPort); /* ��ȡ���ſ�ͨ���� */
    flag &= PlayM4_SetStreamOpenMode(nPort, STREAME_REALTIME); /* ������ģʽ */
    flag &= PlayM4_OpenStream(nPort, nullptr, 0, 1024 * 1024); /* ���� ���һ���������˽⺬�塾mark�� */
    flag &= PlayM4_SetDecCallBackMend(nPort, DecCBFun, pp_yuv420); /* ������Ƶ����ص����� */
    flag &= PlayM4_Play(nPort, nullptr); /* ��ʼ���� */
    if (false == flag)
    {
        PlayM4_FreePort(nPort);
        Sleep(1000);
        return false;  // ���ý���ص�ʧ�ܣ�����
    }

    //����ʵʱԤ��������ʵʩ�ص�
    NET_DVR_PREVIEWINFO struPlayInfo;
    struPlayInfo.hPlayWnd = nullptr; //����Ϊ�գ��豸SDK������ֻȡ��
    struPlayInfo.lChannel = 1; //Channel number �豸ͨ��
    struPlayInfo.dwStreamType = 0;// �������ͣ�0-��������1-��������2-����3��3-����4, 4-����5,5-����6,7-����7,8-����8,9-����9,10-����10
    struPlayInfo.dwLinkMode = 0;// 0-TCP��ʽ,1-UDP��ʽ,2-�ಥ��ʽ,3-RTP��ʽ��4-RTP/RTSP,5-RSTP/HTTP
    struPlayInfo.bBlocked = 0; // 0-������ȡ��, 1-����ȡ��
    LONG handle = NET_DVR_RealPlay_V40(userID, &struPlayInfo, fRealDataCallBack_V30, &nPort);
    if (handle < 0)
    {
        NET_DVR_Logout(userID);
        NET_DVR_Cleanup();
        return false;  // ʵʱ����ʧ�ܣ�����
    }
    clock_t t0, t1;
    t0 = clock();
    while (0 == nHeight * nWidth)  // �ؼ�һ�����ȴ��ص�����DecCBFun����֮��ſ��Խ���֮�����
    {
        t1 = clock();
        if (t1 - t0 > 2000)  // 2��δ�ɹ�����ʱ
        {
            NET_DVR_StopRealPlay(handle);
            NET_DVR_Logout(userID);
            NET_DVR_Cleanup();
            return false;  // �ص���������ʧ�ܣ�����
        }
        unsigned char* p_yuv420 = *pp_yuv420;
        if (nullptr != p_yuv420)
        {
            nHeight = p_yuv420[0] * 255 + p_yuv420[1];
            nWidth = p_yuv420[2] * 255 + p_yuv420[3];
        }
    }
    return true;
}

/*==================================================================
�� �� ����HikCamera::getYUV420Buf
������������ȡͼ��buffer
���������
----------p_buffer��   ͼ��bufferָ�룬���÷�ʽ��Ϊ�����������ǰ����ռ�
----------shallowCopy���Ƿ����ǳ����
�� �� ֵ���Ƿ��ȡͼƬ
��    �ߣ�Dzm
��    �ڣ�2022.05.26
��    ����
==================================================================*/
bool HikCamera::getYUV420Buf(unsigned char*& p_buffer, bool shallowCopy)
{
    unsigned char* p_yuv420 = *pp_yuv420;
    unsigned char* p_yuv420Buffer = p_yuv420 + HEAD_LENGTH;
    if (true == shallowCopy)
    {
        // ǳ������Ч�ʸߣ����Ǵ������ݿ��ܻ�������������ݵ��ٶȣ���ɴ�����������ݲ��ֱ������ɵĸ���
        p_buffer = p_yuv420Buffer;
    }
    else
    {
        // ���������洢�ռ������Ч�ʵͣ������ȶ�
        if (nullptr == p_buffer)
        {
            return false;
        }
        memcpy(p_buffer, p_yuv420Buffer, (nHeight + nHeight / 2) * nWidth);
    }
    return true;
}

/*==================================================================
�� �� ����HikCamera::PTZPreset
��������������ͷ��̨����ΪԤ���
���������
----------cmd��    ���������ͷ�ļ��Ժ궨�����ʽ����
----------index��  Ԥ������
�� �� ֵ���Ƿ���óɹ�
��    �ߣ�Dzm
��    �ڣ�2022.05.25
��    ������������Ԥ��
SET_PRESET 8 ����Ԥ�õ�
CLE_PRESET 9 ���Ԥ�õ�
GOTO_PRESET 39 ת��Ԥ�õ�
==================================================================*/
bool HikCamera::PTZPreset(int cmd, int index)
{
    return NET_DVR_PTZPreset(handle, cmd, index);
}

/*==================================================================
�� �� ����HikCamera::PTZCtrl
��������������ͷ��̨����
���������
----------cmd��    ���������ͷ�ļ��Ժ궨�����ʽ����
----------speed��  �����ٶȣ�ת�����Ŵ�
----------stop��   ֹͣ��־λ��Ϊfalseʱ��ʼ��Ϊtrueʱֹͣ
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

/*==================================================================
�� �� ����HikCamera::makeClearerStart
�����������˶�ͼ����������߳̿���
���������
----------sleepTime��   ÿ��ѭ��֮��ļ��
��    �ߣ�Dzm
��    �ڣ�2022.05.25
��    ����
==================================================================*/
void HikCamera::makeClearerStart(int sleepTime)
{
    b_clearer = true;
    p_t_clearer = (void*)(new std::thread(&HikCamera::makeClearer, this, sleepTime));
    ((std::thread*)p_t_clearer)->detach();
}

/*==================================================================
�� �� ����HikCamera::makeClearerStart
�����������˶�ͼ����������߳̽���
��    �ߣ�Dzm
��    �ڣ�2022.05.24
��    ����
==================================================================*/
void HikCamera::makeClearerEnd()
{
    b_clearer = false;
    if (nullptr != p_t_clearer)
    {
        std::thread* temp = (std::thread*)p_t_clearer;
        delete temp;
        p_t_clearer = nullptr;
    }
}

/*==================================================================
�� �� ����HikCamera::makeClearer
�����������˶�ͼ����������̺߳���
���������
----------sleepTime��   ÿ��ѭ��֮��ļ��
��    �ߣ�Dzm
��    �ڣ�2022.05.25
��    ����
==================================================================*/
void HikCamera::makeClearer(int sleepTime)
{
    while (b_clearer)
    {
        NET_DVR_MakeKeyFrame(userID, 1); // ��������NET_DVR_MakeKeyFrameSub
        Sleep(sleepTime);
    }
}

HikCamera::HikCamera()
{
    nPort = 0;
    b_clearer = true;
    p_t_clearer = nullptr;
    pp_yuv420 = new unsigned char*;
    *(pp_yuv420) = nullptr;
}

HikCamera::~HikCamera()
{
    delete* pp_yuv420;
    delete pp_yuv420;
    makeClearerEnd();
}
