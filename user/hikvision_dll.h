#pragma once
#ifndef _HIKVISION_DLL_H_
#define _HIKVISION_DLL_H_
#ifdef LIBDLL
#define LIBDLL _declspec(dllimport)
#else
#define LIBDLL _declspec(dllexport)
#endif

class HikCamera
{
public:
    HikCamera();
    ~HikCamera();

public:
    int nHeight;        /*图像高*/
    int nWidth;         /*图像宽*/
    /*==================================================================
    函 数 名：HikCamera::init
    功能描述：海康威视网络摄像头初始化
    返 回 值：是否初始化成功
    作    者：Dzm
    日    期：2022.05.22
    其    它：静态函数，多个成员只需要调用一次
    ==================================================================*/
    static bool init();

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
    bool login(const char* sDeviceAddress, const char* sUserName, const char* sPassword, unsigned short wPort = 8000);

    /*==================================================================
    函 数 名：HikCamera::getImgInit
    功能描述：获取图片之前做的初始化
    返 回 值：是否初始化成功
    作    者：Dzm
    日    期：2022.05.22
    其    它：
    ==================================================================*/
    bool getImgInit();

    /*==================================================================
    函 数 名：HikCamera::getImgBuf
    功能描述：获取图像buffer
    输入参数：
    ----------buffer：     图像buffer，引用方式作为输出
    ----------shallowCopy：是否进行浅拷贝
    返 回 值：是否获取图片
    作    者：Dzm
    日    期：2022.05.23
    其    它：
    ==================================================================*/
    bool getImgBuf(unsigned char*& p_buffer, bool shallowCopy = false);

    /*==================================================================
    函 数 名：HikCamera::PTZPreset
    功能描述：摄像头云台重置为预设点
    输入参数：
    ----------cmd：    控制命令，由头文件以宏定义的形式给出
    ----------index：  预设点序号
    返 回 值：是否云台控制调用成功
    作    者：Dzm
    日    期：2022.05.24
    其    它：需先启动预览
    SET_PRESET 8 设置预置点
    CLE_PRESET 9 清除预置点
    GOTO_PRESET 39 转到预置点
    ==================================================================*/
    bool PTZPreset(int cmd, int index);

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
    bool PTZCtrl(int cmd, int speed, bool stop);

    /*==================================================================
    函 数 名：HikCamera::makeClearerStart
    功能描述：运动图像更清晰的线程开启
    输入参数：
    ----------sleepTime：   每次循环之间的间隔
    作    者：Dzm
    日    期：2022.05.25
    其    它：
    ==================================================================*/
    void makeClearerStart(int sleepTime);

    /*==================================================================
    函 数 名：HikCamera::makeClearerEnd
    功能描述：运动图像更清晰的线程结束
    作    者：Dzm
    日    期：2022.05.24
    其    它：
    ==================================================================*/
    void makeClearerEnd();

private:
    long handle;                        /*句柄*/
    long userID;                        /*登录摄像头的用户标识*/
    long nPort;                         /*视频流通道号*/
    void** pp_img;                      /*二重指针，最终指向摄像头图像的源地址*/
    unsigned char* p_decoupledBuffer;   /*解耦合之后的图像buffer*/
    void* p_t_clearer;                  /*运动图像更清晰的线程指针*/
    bool b_clearer;                     /*运动图像更清晰的开启标志*/

    /*==================================================================
    函 数 名：HikCamera::makeClearer
    功能描述：运动图像更清晰的线程函数
    输入参数：
    ----------sleepTime：   每次循环之间的间隔
    作    者：Dzm
    日    期：2022.05.25
    其    它：
    ==================================================================*/
    void makeClearer(int sleepTime = 100);
};

#endif // _HIKVISION_DLL_H_

