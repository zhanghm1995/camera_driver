#include <iostream>
#include <string>
#include <cstring>

#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include "FrameObserver.h"
#include "VimbaCPP/Include/VimbaCPP.h"
#include "ApiController.h"

class Markocamera{
public:
	cv::Mat src_img;//保存原图像
	Markocamera(){};
	~Markocamera(){};
	
	void Opencamera();
	bool CaptureImage();
	void Closecamera();

private:

	VmbErrorType err;
};
