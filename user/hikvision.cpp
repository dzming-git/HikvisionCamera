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
函 数 名：DecCBFun
功能描述：回调函数，用于解码，并将解码后的图像传出
输入参数：
          pBuf:       PlayCtrl内部输入
          pFrameInfo：PlayCtrl内部输入
          pp_img：     由getImgInit函数中的PlayM4_SetDecCallBackMend作为参数输入，输出图像的双重指针
作    者：Dzm
日    期：2022.05.22
其    它：海康威视SDK中用到的PlayCtrl模块中的4中PlayM4_SetDecCallBack都没有void*参数
          https:qcsdn.com/q/a/324952.html#download
          上述网站中的PlayCtrl模块中的PlayM4_SetDecCallBack有void* 参数，进行替换即可
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
函 数 名：fRealDataCallBack_V30
功能描述：回调函数，用于实时视频码流数据获取
输入参数：
          dwDataType:    获取的数据段类型
          pBuffer：      数据段缓冲区
          dwBufSize：    数据段大小
          p_nPort：      视频流通道号指针
作    者：Dzm
日    期：2022.05.22
其    它：
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
函 数 名：HikCamera::init
功能描述：海康威视网络摄像头初始化
返 回 值：是否初始化成功
作    者：Dzm
日    期：2022.05.22
其    它：
==================================================================*/
bool HikCamera::init()
{
    bool flag = NET_DVR_Init();
    NET_DVR_SetConnectTime(2000, 1);
    NET_DVR_SetReconnect(10000, true);
    return flag;
}
/*==================================================================
函 数 名：HikCamera::login
功能描述：海康威视网络摄像头登录
输入参数：
          sDeviceAddress：网络摄像头ip地址
          sUserName：登录用户名
          sPassword：登录密码
          wPort：端口号，一般为8000
返 回 值：是否登录成功
作    者：Dzm
日    期：2022.05.22
其    它：
==================================================================*/
bool HikCamera::login(const char* sDeviceAddress, const char* sUserName, const char* sPassword, unsigned short wPort)
{
    NET_DVR_USER_LOGIN_INFO pLoginInfo;
    NET_DVR_DEVICEINFO_V40 lpDeviceInfo;

    pLoginInfo.bUseAsynLogin = 0;     //同步登录方式
    strcpy_s(pLoginInfo.sDeviceAddress, sDeviceAddress);
    strcpy_s(pLoginInfo.sUserName, sUserName);
    strcpy_s(pLoginInfo.sPassword, sPassword);
    pLoginInfo.wPort = wPort;

    lUserID = NET_DVR_Login_V40(&pLoginInfo, &lpDeviceInfo);

    return lUserID >= 0;
}
/*==================================================================
函 数 名：HikCamera::getImgInit
功能描述：获取图片之前做的初始化
输入参数：
          sDeviceAddress：网络摄像头ip地址
          sUserName：登录用户名
          sPassword：登录密码
          wPort：端口号，一般为8000
返 回 值：是否初始化成功
作    者：Dzm
日    期：2022.05.22
其    它：该函数主要分为两个部分：（有先后顺序）
          1.设置解码回调
          2.启动实时预览，设置实施回调
          部分摄像头不支持主码流传输，后续进行针对性优化【mark】
==================================================================*/
bool HikCamera::getImgInit()
{
    Mat **temp = new Mat*;
    pp_img = (void**)(void*)temp;
    //设置解码回调
    if (false == (\
        PlayM4_GetPort(&nPort) /* 获取播放库通道号 */ && \
        PlayM4_SetStreamOpenMode(nPort, STREAME_REALTIME) /* 设置流模式 */ && \
        PlayM4_OpenStream(nPort, nullptr, 0, 1024 * 1024) /* 打开流 最后一个参数不了解含义【mark】 */ && \
        PlayM4_SetDecCallBackMend(nPort, DecCBFun, pp_img) /* 设置视频解码回调函数 */ && \
        PlayM4_Play(nPort, nullptr) /* 开始播放 */))
    {
        return false;
    }

    //启动实时预览，设置实施回调
    NET_DVR_PREVIEWINFO* pStruPlayInfo = new NET_DVR_PREVIEWINFO;
    pStruPlayInfo->hPlayWnd = nullptr; //窗口为空，设备SDK不解码只取流
    pStruPlayInfo->lChannel = 1; //Channel number 设备通道
    pStruPlayInfo->dwStreamType = 0;// 码流类型，0-主码流，1-子码流，2-码流3，3-码流4, 4-码流5,5-码流6,7-码流7,8-码流8,9-码流9,10-码流10
    pStruPlayInfo->dwLinkMode = 0;// 0-TCP方式,1-UDP方式,2-多播方式,3-RTP方式，4-RTP/RTSP,5-RSTP/HTTP
    pStruPlayInfo->bBlocked = 0; // 0-非阻塞取流, 1-阻塞取流
    LONG handle = NET_DVR_RealPlay_V40(lUserID, pStruPlayInfo, fRealDataCallBack_V30, &nPort);
    if (handle < 0)
    {
        NET_DVR_Logout(lUserID);
        NET_DVR_Cleanup();
        return false;
    }

    int waitMax = 0;
    int WAITMAX = 100;
    while (nullptr == *pp_img) // 关键一步，等待回调函数DecCBFun运行之后才可以进行之后操作
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
函 数 名：HikCamera::getImg
功能描述：获取图像
输入参数：
----------img：        图像对象，引用方式作为输出
----------shallowCopy：是否进行浅拷贝
返 回 值：是否获取图片
作    者：Dzm
日    期：2022.05.22
其    它：深拷贝自己实现，不能使用copyTo
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
        // 浅拷贝，效率高，但是处理数据可能会跟不上生成数据的速度，造成处理的数据内容部分被新生成的覆盖
        img = *pImg;
    }
    else
    {
        // 深拷贝，物理存储空间独立，效率低，但是稳定
//         (*pUserParaDecCBFun->pp_img)->copyTo(*(Mat*)pCameraInfo->pImg);
        // 上一行做法有时候会导致冲突，所以自己实现
        // Mat的数据存储地址连续，按照uchar（8位）来读取，只需要从首地址往后读取rows*cols*3次即可
        // 按照地址读取实时数据，不影响数据的写入，不会出现冲突
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