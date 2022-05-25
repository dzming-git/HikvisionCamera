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
    /*==================================================================
    �� �� ����HikCamera::init
    ��������������������������ͷ��ʼ��
    �� �� ֵ���Ƿ��ʼ���ɹ�
    ��    �ߣ�Dzm
    ��    �ڣ�2022.05.22
    ��    ������̬�����������Աֻ��Ҫ����һ��
    ==================================================================*/
    static bool init();

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
    bool login(const char* sDeviceAddress, const char* sUserName, const char* sPassword, unsigned short wPort = 8000);

    /*==================================================================
    �� �� ����HikCamera::getImgInit
    ������������ȡͼƬ֮ǰ���ĳ�ʼ��
    �� �� ֵ���Ƿ��ʼ���ɹ�
    ��    �ߣ�Dzm
    ��    �ڣ�2022.05.22
    ��    ����
    ==================================================================*/
    bool getImgInit();

    /*==================================================================
    �� �� ����HikCamera::getImgBuf
    ������������ȡͼ��buffer
    ���������
    ----------buffer��     ͼ��buffer�����÷�ʽ��Ϊ���
    ----------shallowCopy���Ƿ����ǳ����
    �� �� ֵ���Ƿ��ȡͼƬ
    ��    �ߣ�Dzm
    ��    �ڣ�2022.05.23
    ��    ����
    ==================================================================*/
    bool getImgBuf(unsigned char*& p_buffer, bool shallowCopy = false);

    /*==================================================================
    �� �� ����HikCamera::PTZPreset
    ��������������ͷ��̨����ΪԤ���
    ���������
    ----------cmd��    ���������ͷ�ļ��Ժ궨�����ʽ����
    ----------index��  Ԥ������
    �� �� ֵ���Ƿ���̨���Ƶ��óɹ�
    ��    �ߣ�Dzm
    ��    �ڣ�2022.05.24
    ��    ������������Ԥ��
    SET_PRESET 8 ����Ԥ�õ�
    CLE_PRESET 9 ���Ԥ�õ�
    GOTO_PRESET 39 ת��Ԥ�õ�
    ==================================================================*/
    bool PTZPreset(int cmd, int index);

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
    bool PTZCtrl(int cmd, int speed, bool stop);

    /*==================================================================
    �� �� ����HikCamera::makeClearerStart
    �����������˶�ͼ����������߳̿���
    ���������
    ----------sleepTime��   ÿ��ѭ��֮��ļ��
    ��    �ߣ�Dzm
    ��    �ڣ�2022.05.25
    ��    ����
    ==================================================================*/
    void makeClearerStart(int sleepTime);

    /*==================================================================
    �� �� ����HikCamera::makeClearerEnd
    �����������˶�ͼ����������߳̽���
    ��    �ߣ�Dzm
    ��    �ڣ�2022.05.24
    ��    ����
    ==================================================================*/
    void makeClearerEnd();

private:
    long handle;                        /*���*/
    long userID;                        /*��¼����ͷ���û���ʶ*/
    long nPort;                         /*��Ƶ��ͨ����*/
    void** pp_img;                      /*����ָ�룬����ָ������ͷͼ���Դ��ַ*/
    unsigned char* p_decoupledBuffer;   /*�����֮���ͼ��buffer*/
    void* p_t_clearer;                  /*�˶�ͼ����������߳�ָ��*/
    bool b_clearer;                     /*�˶�ͼ��������Ŀ�����־*/

    /*==================================================================
    �� �� ����HikCamera::makeClearer
    �����������˶�ͼ����������̺߳���
    ���������
    ----------sleepTime��   ÿ��ѭ��֮��ļ��
    ��    �ߣ�Dzm
    ��    �ڣ�2022.05.25
    ��    ����
    ==================================================================*/
    void makeClearer(int sleepTime=100);
};

/**********************��̨�������� begin*************************/
#define LIGHT_PWRON        2    /* ��ͨ�ƹ��Դ */
#define WIPER_PWRON        3    /* ��ͨ��ˢ���� */
#define FAN_PWRON        4    /* ��ͨ���ȿ��� */
#define HEATER_PWRON    5    /* ��ͨ���������� */
#define AUX_PWRON1        6    /* ��ͨ�����豸���� */
#define AUX_PWRON2        7    /* ��ͨ�����豸���� */
#define SET_PRESET        8    /* ����Ԥ�õ� */
#define CLE_PRESET        9    /* ���Ԥ�õ� */

#define ZOOM_IN            11    /* �������ٶ�SS���(���ʱ��) */
#define ZOOM_OUT        12    /* �������ٶ�SS��С(���ʱ�С) */
#define FOCUS_NEAR      13  /* �������ٶ�SSǰ�� */
#define FOCUS_FAR       14  /* �������ٶ�SS��� */
#define IRIS_OPEN       15  /* ��Ȧ���ٶ�SS���� */
#define IRIS_CLOSE      16  /* ��Ȧ���ٶ�SS��С */

#define TILT_UP            21    /* ��̨��SS���ٶ����� */
#define TILT_DOWN        22    /* ��̨��SS���ٶ��¸� */
#define PAN_LEFT        23    /* ��̨��SS���ٶ���ת */
#define PAN_RIGHT        24    /* ��̨��SS���ٶ���ת */
#define UP_LEFT            25    /* ��̨��SS���ٶ���������ת */
#define UP_RIGHT        26    /* ��̨��SS���ٶ���������ת */
#define DOWN_LEFT        27    /* ��̨��SS���ٶ��¸�����ת */
#define DOWN_RIGHT        28    /* ��̨��SS���ٶ��¸�����ת */
#define PAN_AUTO        29    /* ��̨��SS���ٶ������Զ�ɨ�� */

#define FILL_PRE_SEQ    30    /* ��Ԥ�õ����Ѳ������ */
#define SET_SEQ_DWELL    31    /* ����Ѳ����ͣ��ʱ�� */
#define SET_SEQ_SPEED    32    /* ����Ѳ���ٶ� */
#define CLE_PRE_SEQ        33    /* ��Ԥ�õ��Ѳ��������ɾ�� */
#define STA_MEM_CRUISE    34    /* ��ʼ��¼�켣 */
#define STO_MEM_CRUISE    35    /* ֹͣ��¼�켣 */
#define RUN_CRUISE        36    /* ��ʼ�켣 */
#define RUN_SEQ            37    /* ��ʼѲ�� */
#define STOP_SEQ        38    /* ֹͣѲ�� */
#define GOTO_PRESET        39    /* ����ת��Ԥ�õ� */

#define DEL_SEQ         43  /* ɾ��Ѳ��·�� */
#define STOP_CRUISE        44    /* ֹͣ�켣 */
#define DELETE_CRUISE    45    /* ɾ�������켣 */
#define DELETE_ALL_CRUISE 46/* ɾ�����й켣 */

#define PAN_CIRCLE      50   /* ��̨��SS���ٶ��Զ�Բ��ɨ�� */
#define DRAG_PTZ        51   /* �϶�PTZ */
#define LINEAR_SCAN     52   /* ����ɨ�� */ //2014-03-15 
#define CLE_ALL_PRESET  53   /* Ԥ�õ�ȫ����� */ 
#define CLE_ALL_SEQ     54   /* Ѳ��ȫ����� */ 
#define CLE_ALL_CRUISE  55   /* �켣ȫ����� */ 

#define POPUP_MENU      56   /* ��ʾ�����˵� */

#define TILT_DOWN_ZOOM_IN    58    /* ��̨��SS���ٶ��¸�&&�������ٶ�SS���(���ʱ��) */
#define TILT_DOWN_ZOOM_OUT  59  /* ��̨��SS���ٶ��¸�&&�������ٶ�SS��С(���ʱ�С) */
#define PAN_LEFT_ZOOM_IN    60  /* ��̨��SS���ٶ���ת&&�������ٶ�SS���(���ʱ��)*/
#define PAN_LEFT_ZOOM_OUT   61  /* ��̨��SS���ٶ���ת&&�������ٶ�SS��С(���ʱ�С)*/
#define PAN_RIGHT_ZOOM_IN    62  /* ��̨��SS���ٶ���ת&&�������ٶ�SS���(���ʱ��) */
#define PAN_RIGHT_ZOOM_OUT  63  /* ��̨��SS���ٶ���ת&&�������ٶ�SS��С(���ʱ�С) */
#define UP_LEFT_ZOOM_IN     64  /* ��̨��SS���ٶ���������ת&&�������ٶ�SS���(���ʱ��)*/
#define UP_LEFT_ZOOM_OUT    65  /* ��̨��SS���ٶ���������ת&&�������ٶ�SS��С(���ʱ�С)*/
#define UP_RIGHT_ZOOM_IN    66  /* ��̨��SS���ٶ���������ת&&�������ٶ�SS���(���ʱ��)*/
#define UP_RIGHT_ZOOM_OUT   67  /* ��̨��SS���ٶ���������ת&&�������ٶ�SS��С(���ʱ�С)*/
#define DOWN_LEFT_ZOOM_IN   68  /* ��̨��SS���ٶ��¸�����ת&&�������ٶ�SS���(���ʱ��) */
#define DOWN_LEFT_ZOOM_OUT  69  /* ��̨��SS���ٶ��¸�����ת&&�������ٶ�SS��С(���ʱ�С) */
#define DOWN_RIGHT_ZOOM_IN    70  /* ��̨��SS���ٶ��¸�����ת&&�������ٶ�SS���(���ʱ��) */
#define DOWN_RIGHT_ZOOM_OUT    71  /* ��̨��SS���ٶ��¸�����ת&&�������ٶ�SS��С(���ʱ�С) */
#define TILT_UP_ZOOM_IN        72    /* ��̨��SS���ٶ�����&&�������ٶ�SS���(���ʱ��) */
#define TILT_UP_ZOOM_OUT    73    /* ��̨��SS���ٶ�����&&�������ٶ�SS��С(���ʱ�С) */
/**********************��̨�������� end*************************/

#endif // HIKVISION_H__
