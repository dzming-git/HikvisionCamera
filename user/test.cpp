#include "hikvision_cpp.h"
int main()
{
	HikCamera camera;
	camera.init();
	camera.login("192.168.1.16", "admin", "scuimage508");
	camera.getImgInit();
	Mat img;
	while (1)
	{
		camera.getImg(img, false);
		imshow("test", img);
		waitKey(1);
	}
	return 0;
}