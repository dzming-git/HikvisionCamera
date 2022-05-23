#include "hikvision_cpp.h"
#include <opencv2\core\core.hpp>
#include <opencv2\highgui\highgui.hpp>
#include <opencv2\imgproc\imgproc.hpp>
using namespace cv;
using namespace std;
int main()
{
	HikCamera camera;
	camera.init();
	camera.login("192.168.1.16", "admin", "scuimage508");
	camera.getImgInit();
	Mat img;
	unsigned char* p_buf = nullptr;
	while (1)
	{
		camera.getImgBuf(p_buf);
		img = Mat(camera.nHeight, camera.nWidth, CV_8UC3, p_buf);
		imshow("test", img);
		waitKey(1);
	}
	return 0;
}