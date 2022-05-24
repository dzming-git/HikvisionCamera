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
函 数 名：DecCBFun
功能描述：回调函数，用于解码，并将解码后的图像传出
输入参数：
----------pBuf:       图像缓冲区指针
----------pFrameInfo：图像的信息，比如宽（nWidth）、高（nHeight）、图像类型（nType）
----------pp_img：    由getImgInit函数中的PlayM4_SetDecCallBackMend作为参数输入，输出图像的双重指针
作    者：Dzm
日    期：2022.05.22
其    它：海康威视SDK中用到的PlayCtrl模块中的4中PlayM4_SetDecCallBack都没有void*参数
          https:qcsdn.com/q/a/324952.html#download
          上述网站中的PlayCtrl模块中的PlayM4_SetDecCallBack有void* 参数，进行替换即可
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
函 数 名：fRealDataCallBack_V30
功能描述：回调函数，用于实时视频码流数据获取
输入参数：
----------dwDataType:    获取的数据段类型
----------pBuffer：      数据段缓冲区指针
----------dwBufSize：    数据段大小
----------p_nPort：      视频流通道号指针
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
其    它：静态函数，多个成员只需要调用一次
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
----------sDeviceAddress：网络摄像头ip地址
----------sUserName：登录用户名
----------sPassword：登录密码
----------wPort：端口号，一般为8000
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
    cv::Mat **temp = new cv::Mat*;
    pp_img = (void**)(void*)temp;
    //设置解码回调
    if (false == (\
        PlayM4_GetPort(&nPort) /* 获取播放库通道号 */ && \
        PlayM4_SetStreamOpenMode(nPort, STREAME_REALTIME) /* 设置流模式 */ && \
        PlayM4_OpenStream(nPort, nullptr, 0, 1024 * 1024) /* 打开流 最后一个参数不了解含义【mark】 */ && \
        PlayM4_SetDecCallBackMend(nPort, DecCBFun, pp_img) /* 设置视频解码回调函数 */ && \
        PlayM4_Play(nPort, nullptr) /* 开始播放 */))
    {
        return false;  // 设置解码回调失败，返回
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
        return false;  // 实时播放失败，返回
    }
    clock_t t0, t1;
    t0 = clock();
    while (0 == nHeight * nWidth)  // 关键一步，等待回调函数DecCBFun运行之后才可以进行之后操作
    {
        t1 = clock();
        if (t1 - t0 > 2000)
        {
            NET_DVR_StopRealPlay(handle);
            NET_DVR_Logout(lUserID);
            NET_DVR_Cleanup();
            return false;  // 回调函数启动失败，返回
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
函 数 名：HikCamera::getImgBuf
功能描述：获取图像buffer
输入参数：
----------buffer：     图像buffer，引用方式作为输出
----------shallowCopy：是否进行浅拷贝
返 回 值：是否获取图片
作    者：Dzm
日    期：2022.05.23
其    它：深拷贝自己实现，不能使用copyTo
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
        // 浅拷贝，效率高，但是处理数据可能会跟不上生成数据的速度，造成处理的数据内容部分被新生成的覆盖
        buffer = p_img->data;
    }
    else
    {
        // 深拷贝，物理存储空间独立，效率低，但是稳定
        unsigned char* pImgDataSrc = &(p_img->at<uchar>(0, 0));
        int ucharCount = nHeight * nWidth * 3;
        memcpy(p_decoupledBuffer, pImgDataSrc, ucharCount);
        buffer = p_decoupledBuffer;
    }
    return true;
}

/*==================================================================
函 数 名：HikCamera::PTZCtrl
功能描述：摄像头云台控制
输入参数：
----------cmd：   控制命令，由头文件以宏定义的形式给出
----------speed： 转动速度
----------stop：  停止标志位，为false时开始，为true时停止
返 回 值：是否云台控制调用成功
作    者：Dzm
日    期：2022.05.24
其    它：
ZOOM_IN 11 焦距变大(倍率变大) 
ZOOM_OUT 12 焦距变小(倍率变小) 
FOCUS_NEAR 13 焦点前调 
FOCUS_FAR 14 焦点后调 
IRIS_OPEN 15 光圈扩大 
IRIS_CLOSE 16 光圈缩小 
TILT_UP 21 云台上仰 
TILT_DOWN 22 云台下俯 
PAN_LEFT 23 云台左转 
PAN_RIGHT 24 云台右转 
UP_LEFT 25 云台上仰和左转 
UP_RIGHT 26 云台上仰和右转 
DOWN_LEFT 27 云台下俯和左转 
DOWN_RIGHT 28 云台下俯和右转 
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