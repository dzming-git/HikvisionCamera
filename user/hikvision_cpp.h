#ifndef __HIKVISION_H__
#define __HIKVISION_H__


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
    void makeClearer(int sleepTime=100);
};

/**********************云台控制命令 begin*************************/
#define LIGHT_PWRON        2    /* 接通灯光电源 */
#define WIPER_PWRON        3    /* 接通雨刷开关 */
#define FAN_PWRON        4    /* 接通风扇开关 */
#define HEATER_PWRON    5    /* 接通加热器开关 */
#define AUX_PWRON1        6    /* 接通辅助设备开关 */
#define AUX_PWRON2        7    /* 接通辅助设备开关 */
#define SET_PRESET        8    /* 设置预置点 */
#define CLE_PRESET        9    /* 清除预置点 */

#define ZOOM_IN            11    /* 焦距以速度SS变大(倍率变大) */
#define ZOOM_OUT        12    /* 焦距以速度SS变小(倍率变小) */
#define FOCUS_NEAR      13  /* 焦点以速度SS前调 */
#define FOCUS_FAR       14  /* 焦点以速度SS后调 */
#define IRIS_OPEN       15  /* 光圈以速度SS扩大 */
#define IRIS_CLOSE      16  /* 光圈以速度SS缩小 */

#define TILT_UP            21    /* 云台以SS的速度上仰 */
#define TILT_DOWN        22    /* 云台以SS的速度下俯 */
#define PAN_LEFT        23    /* 云台以SS的速度左转 */
#define PAN_RIGHT        24    /* 云台以SS的速度右转 */
#define UP_LEFT            25    /* 云台以SS的速度上仰和左转 */
#define UP_RIGHT        26    /* 云台以SS的速度上仰和右转 */
#define DOWN_LEFT        27    /* 云台以SS的速度下俯和左转 */
#define DOWN_RIGHT        28    /* 云台以SS的速度下俯和右转 */
#define PAN_AUTO        29    /* 云台以SS的速度左右自动扫描 */

#define FILL_PRE_SEQ    30    /* 将预置点加入巡航序列 */
#define SET_SEQ_DWELL    31    /* 设置巡航点停顿时间 */
#define SET_SEQ_SPEED    32    /* 设置巡航速度 */
#define CLE_PRE_SEQ        33    /* 将预置点从巡航序列中删除 */
#define STA_MEM_CRUISE    34    /* 开始记录轨迹 */
#define STO_MEM_CRUISE    35    /* 停止记录轨迹 */
#define RUN_CRUISE        36    /* 开始轨迹 */
#define RUN_SEQ            37    /* 开始巡航 */
#define STOP_SEQ        38    /* 停止巡航 */
#define GOTO_PRESET        39    /* 快球转到预置点 */

#define DEL_SEQ         43  /* 删除巡航路径 */
#define STOP_CRUISE        44    /* 停止轨迹 */
#define DELETE_CRUISE    45    /* 删除单条轨迹 */
#define DELETE_ALL_CRUISE 46/* 删除所有轨迹 */

#define PAN_CIRCLE      50   /* 云台以SS的速度自动圆周扫描 */
#define DRAG_PTZ        51   /* 拖动PTZ */
#define LINEAR_SCAN     52   /* 区域扫描 */ //2014-03-15 
#define CLE_ALL_PRESET  53   /* 预置点全部清除 */ 
#define CLE_ALL_SEQ     54   /* 巡航全部清除 */ 
#define CLE_ALL_CRUISE  55   /* 轨迹全部清除 */ 

#define POPUP_MENU      56   /* 显示操作菜单 */

#define TILT_DOWN_ZOOM_IN    58    /* 云台以SS的速度下俯&&焦距以速度SS变大(倍率变大) */
#define TILT_DOWN_ZOOM_OUT  59  /* 云台以SS的速度下俯&&焦距以速度SS变小(倍率变小) */
#define PAN_LEFT_ZOOM_IN    60  /* 云台以SS的速度左转&&焦距以速度SS变大(倍率变大)*/
#define PAN_LEFT_ZOOM_OUT   61  /* 云台以SS的速度左转&&焦距以速度SS变小(倍率变小)*/
#define PAN_RIGHT_ZOOM_IN    62  /* 云台以SS的速度右转&&焦距以速度SS变大(倍率变大) */
#define PAN_RIGHT_ZOOM_OUT  63  /* 云台以SS的速度右转&&焦距以速度SS变小(倍率变小) */
#define UP_LEFT_ZOOM_IN     64  /* 云台以SS的速度上仰和左转&&焦距以速度SS变大(倍率变大)*/
#define UP_LEFT_ZOOM_OUT    65  /* 云台以SS的速度上仰和左转&&焦距以速度SS变小(倍率变小)*/
#define UP_RIGHT_ZOOM_IN    66  /* 云台以SS的速度上仰和右转&&焦距以速度SS变大(倍率变大)*/
#define UP_RIGHT_ZOOM_OUT   67  /* 云台以SS的速度上仰和右转&&焦距以速度SS变小(倍率变小)*/
#define DOWN_LEFT_ZOOM_IN   68  /* 云台以SS的速度下俯和左转&&焦距以速度SS变大(倍率变大) */
#define DOWN_LEFT_ZOOM_OUT  69  /* 云台以SS的速度下俯和左转&&焦距以速度SS变小(倍率变小) */
#define DOWN_RIGHT_ZOOM_IN    70  /* 云台以SS的速度下俯和右转&&焦距以速度SS变大(倍率变大) */
#define DOWN_RIGHT_ZOOM_OUT    71  /* 云台以SS的速度下俯和右转&&焦距以速度SS变小(倍率变小) */
#define TILT_UP_ZOOM_IN        72    /* 云台以SS的速度上仰&&焦距以速度SS变大(倍率变大) */
#define TILT_UP_ZOOM_OUT    73    /* 云台以SS的速度上仰&&焦距以速度SS变小(倍率变小) */
/**********************云台控制命令 end*************************/

#endif // HIKVISION_H__
