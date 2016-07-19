#include "stdafx.h"
#include "ConvertPXCImage2CVMat.h"

ConvertPXCImage2CVMat::ConvertPXCImage2CVMat()
{
}


ConvertPXCImage2CVMat::~ConvertPXCImage2CVMat()
{
}

cv::Mat ConvertPXCImage2CVMat::ConvertPXCImageToOpenCVMat(PXCImage *pxcImg, ImageFormat format)
{
	cv::Mat dstImg;
	PXCImage::ImageData data;
	PXCImage::ImageInfo imgInfo = pxcImg->QueryInfo();

	switch (format){
	case STREAM_TYPE_COLOR:
		pxcImg->AcquireAccess(PXCImage::ACCESS_READ, PXCImage::PIXEL_FORMAT_RGB24, &data);
		dstImg = cv::Mat(imgInfo.height, imgInfo.width, CV_8UC3);
		memcpy(dstImg.data, data.planes[0], imgInfo.height*imgInfo.width * 3 * sizeof(pxcBYTE));
		break;
	case STREAM_TYPE_DEPTH:
		pxcImg->AcquireAccess(PXCImage::ACCESS_READ, PXCImage::PIXEL_FORMAT_DEPTH_RAW, &data);
		dstImg = cv::Mat(imgInfo.height, imgInfo.width, CV_16U);
		memcpy(dstImg.data, data.planes[0], imgInfo.height*imgInfo.width * 2 * sizeof(pxcBYTE));
		break;
	}

	pxcImg->ReleaseAccess(&data);
	return dstImg;
}