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
函 数 名：DecCBFun
功能描述：回调函数，用于解码，并将解码后的图像传出
输入参数：
----------pBuf:              图像缓冲区指针
----------pFrameInfo：       图像的信息，比如宽（nWidth）、高（nHeight）、图像类型（nType）
----------pp_yuv420：        由getImgInit函数中的PlayM4_SetDecCallBackMend作为参数输入，输出图像的双重指针
                             前 HEAD_LENGTH 位为信息位，目前安排：0-3位存储高宽，4位保留，5-end存储yuv420的buffer
作    者：Dzm
日    期：2022.05.26
其    它：海康威视SDK中用到的PlayCtrl模块中的4中PlayM4_SetDecCallBack都没有void*参数
          https:qcsdn.com/q/a/324952.html#download
          上述网站中的PlayCtrl模块中的PlayM4_SetDecCallBack有void* 参数，进行替换即可
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
日    期：2022.05.26
其    它：静态函数，多个成员只需要调用一次
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

    userID = NET_DVR_Login_V40(&pLoginInfo, &lpDeviceInfo);

    return userID >= 0;
}

/*==================================================================
函 数 名：HikCamera::getImgInit
功能描述：获取图片之前做的初始化
返 回 值：是否初始化成功
作    者：Dzm
日    期：2022.05.26
其    它：该函数主要分为两个部分：（有先后顺序）
          1.设置解码回调
          2.启动实时预览，设置实施回调
          部分摄像头不支持主码流传输，后续进行针对性优化【mark】
==================================================================*/
bool HikCamera::getImgInit()
{
    //设置解码回调
    bool flag = true;
    flag &= PlayM4_GetPort(&nPort); /* 获取播放库通道号 */
    flag &= PlayM4_SetStreamOpenMode(nPort, STREAME_REALTIME); /* 设置流模式 */
    flag &= PlayM4_OpenStream(nPort, nullptr, 0, 1024 * 1024); /* 打开流 最后一个参数不了解含义【mark】 */
    flag &= PlayM4_SetDecCallBackMend(nPort, DecCBFun, pp_yuv420); /* 设置视频解码回调函数 */
    flag &= PlayM4_Play(nPort, nullptr); /* 开始播放 */
    if (false == flag)
    {
        PlayM4_FreePort(nPort);
        Sleep(1000);
        return false;  // 设置解码回调失败，返回
    }

    //启动实时预览，设置实施回调
    NET_DVR_PREVIEWINFO struPlayInfo;
    struPlayInfo.hPlayWnd = nullptr; //窗口为空，设备SDK不解码只取流
    struPlayInfo.lChannel = 1; //Channel number 设备通道
    struPlayInfo.dwStreamType = 0;// 码流类型，0-主码流，1-子码流，2-码流3，3-码流4, 4-码流5,5-码流6,7-码流7,8-码流8,9-码流9,10-码流10
    struPlayInfo.dwLinkMode = 0;// 0-TCP方式,1-UDP方式,2-多播方式,3-RTP方式，4-RTP/RTSP,5-RSTP/HTTP
    struPlayInfo.bBlocked = 0; // 0-非阻塞取流, 1-阻塞取流
    LONG handle = NET_DVR_RealPlay_V40(userID, &struPlayInfo, fRealDataCallBack_V30, &nPort);
    if (handle < 0)
    {
        NET_DVR_Logout(userID);
        NET_DVR_Cleanup();
        return false;  // 实时播放失败，返回
    }
    clock_t t0, t1;
    t0 = clock();
    while (0 == nHeight * nWidth)  // 关键一步，等待回调函数DecCBFun运行之后才可以进行之后操作
    {
        t1 = clock();
        if (t1 - t0 > 2000)  // 2秒未成功，超时
        {
            NET_DVR_StopRealPlay(handle);
            NET_DVR_Logout(userID);
            NET_DVR_Cleanup();
            return false;  // 回调函数启动失败，返回
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
函 数 名：HikCamera::getYUV420Buf
功能描述：获取图像buffer
输入参数：
----------p_buffer：   图像buffer指针，引用方式作为输出，必须提前分配空间
----------shallowCopy：是否进行浅拷贝
返 回 值：是否获取图片
作    者：Dzm
日    期：2022.05.26
其    它：
==================================================================*/
bool HikCamera::getYUV420Buf(unsigned char*& p_buffer, bool shallowCopy)
{
    unsigned char* p_yuv420 = *pp_yuv420;
    unsigned char* p_yuv420Buffer = p_yuv420 + HEAD_LENGTH;
    if (true == shallowCopy)
    {
        // 浅拷贝，效率高，但是处理数据可能会跟不上生成数据的速度，造成处理的数据内容部分被新生成的覆盖
        p_buffer = p_yuv420Buffer;
    }
    else
    {
        // 深拷贝，物理存储空间独立，效率低，但是稳定
        if (nullptr == p_buffer)
        {
            return false;
        }
        memcpy(p_buffer, p_yuv420Buffer, (nHeight + nHeight / 2) * nWidth);
    }
    return true;
}

/*==================================================================
函 数 名：HikCamera::PTZPreset
功能描述：摄像头云台重置为预设点
输入参数：
----------cmd：    控制命令，由头文件以宏定义的形式给出
----------index：  预设点序号
返 回 值：是否调用成功
作    者：Dzm
日    期：2022.05.25
其    它：需先启动预览
SET_PRESET 8 设置预置点
CLE_PRESET 9 清除预置点
GOTO_PRESET 39 转到预置点
==================================================================*/
bool HikCamera::PTZPreset(int cmd, int index)
{
    return NET_DVR_PTZPreset(handle, cmd, index);
}

/*==================================================================
函 数 名：HikCamera::PTZCtrl
功能描述：摄像头云台控制
输入参数：
----------cmd：    控制命令，由头文件以宏定义的形式给出
----------speed：  控制速度（转动、放大）
----------stop：   停止标志位，为false时开始，为true时停止
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

/*==================================================================
函 数 名：HikCamera::makeClearerStart
功能描述：运动图像更清晰的线程开启
输入参数：
----------sleepTime：   每次循环之间的间隔
作    者：Dzm
日    期：2022.05.25
其    它：
==================================================================*/
void HikCamera::makeClearerStart(int sleepTime)
{
    b_clearer = true;
    p_t_clearer = (void*)(new std::thread(&HikCamera::makeClearer, this, sleepTime));
    ((std::thread*)p_t_clearer)->detach();
}

/*==================================================================
函 数 名：HikCamera::makeClearerStart
功能描述：运动图像更清晰的线程结束
作    者：Dzm
日    期：2022.05.24
其    它：
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
函 数 名：HikCamera::makeClearer
功能描述：运动图像更清晰的线程函数
输入参数：
----------sleepTime：   每次循环之间的间隔
作    者：Dzm
日    期：2022.05.25
其    它：
==================================================================*/
void HikCamera::makeClearer(int sleepTime)
{
    while (b_clearer)
    {
        NET_DVR_MakeKeyFrame(userID, 1); // 子码流：NET_DVR_MakeKeyFrameSub
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
