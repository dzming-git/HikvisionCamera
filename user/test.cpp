#include "hikvision_cpp.h"
#include <core\core.hpp>
#include <highgui\highgui.hpp>
#include <imgproc\imgproc.hpp>
#include <iostream>
using namespace cv;
using namespace std;
int main()
{
	string cameraIp = "192.168.1.16";
	HikCamera camera;
	HikCamera::init();
	camera.login(cameraIp.data(), "admin", "scuimage508");
	camera.getImgInit();

	Mat imgYUV420, imgBGR;
	unsigned char* p_buf = new unsigned char[(camera.nHeight + camera.nHeight / 2) * camera.nWidth];
	while (1) 
	{
		if (false == camera.getYUV420Buf(p_buf)) 
		{
			continue;
		}
		imgYUV420 = Mat(camera.nHeight + camera.nHeight / 2, camera.nWidth, CV_8UC1, p_buf);
		cvtColor(imgYUV420, imgBGR, COLOR_YUV2BGR_YV12);
		imshow(cameraIp.data(), imgBGR);
		waitKey(1);
	}
	return 0;
}