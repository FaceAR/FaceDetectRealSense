#ifndef __GAME_FACE_DATA__
#define __GAME_FACE_DATA__

#ifndef DISABLE_RSSDK

#include "pxcimage.h"
#include "pxcfacemodule.h"
#include "pxcfacedata.h"
#include "pxcfaceconfiguration.h"
#include "ConvertPXCImage2CVMat.h"

void FaceDetect_Init();
void FaceDetect_Shutdown();

bool FaceDetect_FrameAcquire(/*std::vector<cv::Rect> &facesRect, std::vector<cv::Mat_<int> > &faceLandmarks*/);
void FaceDetect_FrameRelease();

PXCImage *FaceDetect_GetColorImage();
PXCImage *FaceDetect_GetDepthImage();

bool FaceDetect_GetFaceRects(std::vector<cv::Rect> &facesRects);
bool FaceDetect_GetFaceLandmarks2D(std::vector<cv::Mat_<int> > &faceLandmarks2d);
bool FaceDetect_GetFaceLandmarks3D(std::vector<cv::Mat_<int> > &faceLandmarks3d);
bool FaceDetect_GetFaceLandmarks(std::vector<cv::Mat_<int> > &faceLandmarks2d, std::vector<cv::Mat_<int> > &faceLandmarks3d);
bool FaceDetect_GetFacePose(std::vector<cv::Vec3d> &HeadPositionCV, std::vector<cv::Vec3d> &EulerAnglesCV, std::vector<cv::Vec4d> &QuaternionCV, std::vector<cv::Matx33d> &RotationMatrixCV);
bool FaceDetect_GetFacePulse(std::vector<float> &HeartRate);
bool FaceDetect_GetFaceExpression(std::vector< std::vector<std::pair<std::string, float> > > &ExpressionsPairVec);

#endif


#endif // #ifndef __GAME_FACE_DATA__