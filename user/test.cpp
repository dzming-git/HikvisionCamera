#include "hikvision_cpp.h"
#include <opencv2\core\core.hpp>
#include <opencv2\highgui\highgui.hpp>
#include <opencv2\imgproc\imgproc.hpp>
#include <iostream>
using namespace cv;
using namespace std;
int main()
{
	HikCamera camera1;
	HikCamera camera2;
	cout << HikCamera::init();
	cout << camera1.login("192.168.1.16", "admin", "scuimage508");
	cout << camera1.getImgInit();
	cout << camera2.login("192.168.1.7", "admin", "scuimage508");
	cout << camera2.getImgInit();
	Mat img1, img2;
	unsigned char* p_buf1 = nullptr;
	unsigned char* p_buf2 = nullptr;
	while (1)
	{
		camera1.getImgBuf(p_buf1);
		img1 = Mat(camera1.nHeight, camera1.nWidth, CV_8UC3, p_buf1);
		imshow("test1", img1);
		waitKey(1);
		camera2.getImgBuf(p_buf2);
		img2 = Mat(camera2.nHeight, camera2.nWidth, CV_8UC3, p_buf2);
		imshow("test2", img2);
		waitKey(1);
	}
	return 0;
}