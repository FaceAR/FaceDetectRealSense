#pragma once

#include "pxcsensemanager.h"
#include <opencv.hpp>

#define WIDTH 640
#define HEIGHT 480

enum ImageFormat {
	STREAM_TYPE_COLOR = 0,
	STREAM_TYPE_DEPTH = 1,
	STREAM_TYPE_IR = 2
};

class ConvertPXCImage2CVMat
{
public:
	ConvertPXCImage2CVMat();
	~ConvertPXCImage2CVMat();

	cv::Mat ConvertPXCImageToOpenCVMat(PXCImage *pxcImg, ImageFormat format);

};