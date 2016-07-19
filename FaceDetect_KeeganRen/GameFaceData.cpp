#include "stdafx.h"
#include "GameFaceData.h"


#ifndef DISABLE_RSSDK

#include "pxcsensemanager.h"

struct SFaceDetectData
{
	PXCSenseManager *SenseManager;
	PXCFaceModule *FaceModule;
	PXCFaceData *FaceData;
	PXCFaceConfiguration *FaceConfiguration;


	bool FrameAcquired;

	PXCImage *colorImage;
	PXCImage::ImageData colorImageData;
	PXCImage::ImageInfo colorImageInfo;

	PXCImage *AcquiredPreviewImage;

	bool DetectInProgress;
};

static SFaceDetectData gFaceDetectGlob = {};

void FaceDetect_Init()
{
	PXCSenseManager *sm = PXCSenseManager::CreateInstance();
	if (!sm) {
		std::cout << "Unabel to create the PXCSenseManager!" << std::endl;
		return;
	}
	pxcStatus status;
	status = sm->EnableFace();

	status = sm->EnableStream(PXCCapture::STREAM_TYPE_COLOR, 1280, 720, 30.0f);
	status = sm->EnableStream(PXCCapture::STREAM_TYPE_DEPTH, 640, 480, 30.0f);
	status = sm->Init();

	///
	PXCFaceModule *fs = sm->QueryFace();
	if (fs == NULL) {
		std::cout << "Unabel to query FaceModule\n" << std::endl;
		return ;
	}

	PXCFaceConfiguration *cfg = fs->CreateActiveConfiguration();
	if (cfg == NULL) {
		std::cout << "Unabel to create FaceConfiguration\n" << std::endl;
		return ;
	}
	////
	cfg->detection.isEnabled = TRUE; // face detect
	////
	cfg->landmarks.isEnabled = TRUE; // landmark detect
	cfg->landmarks.maxTrackedFaces = 10;
	////
	cfg->pose.isEnabled = TRUE;
	cfg->pose.maxTrackedFaces = 10;
	////
	cfg->EnableAllAlerts();          // enable all alerts
	cfg->ApplyChanges();             // apply all change to "fs"
	
	///////////////////////////////////////////////
	////
	PXCFaceData *facedata = fs->CreateOutput();

	/////
	gFaceDetectGlob.SenseManager = sm;
	gFaceDetectGlob.FaceModule = fs;
	gFaceDetectGlob.FaceConfiguration = cfg;
	gFaceDetectGlob.FaceData = facedata;
}

bool FaceDetect_FrameAcquire(/*std::vector<cv::Rect> &facesRect, std::vector<cv::Mat_<int> > &faceLandmarks*/)
{
	assert(!gFaceDetectGlob.FrameAcquired);
	pxcStatus acquireStatus = gFaceDetectGlob.SenseManager->AcquireFrame(false);

	gFaceDetectGlob.DetectInProgress = gFaceDetectGlob.FaceData->Update() != 0;
	gFaceDetectGlob.FrameAcquired = acquireStatus >= pxcStatus::PXC_STATUS_NO_ERROR;

	////// Face Detect
	//pxcI32 nfaces = gFaceDetectGlob.FaceData->QueryNumberOfDetectedFaces();
	//std::cout << "nfaces = " << nfaces << std::endl;
	//for (pxcI32 i = 0; i < nfaces; i++) {
	//	//按序号获取一个人脸的数据实例  
	//	std::cout << "face ID" << i << std::endl;
	//	PXCFaceData::Face *trackedface = gFaceDetectGlob.FaceData->QueryFaceByIndex(i);
	//	
	//	/// face detect
	//	PXCFaceData::DetectionData *detectiondata = trackedface->QueryDetection();
	//	if (detectiondata == NULL) {
	//		wprintf_s(L"Unabel to get detection data\n");
	//		return 0;
	//	}
	//	//将当前人脸的位置数据存在rect中  
	//	PXCRectI32 rect;
	//	detectiondata->QueryBoundingRect(&rect);
	//	//PXCRectI32到opencv中Rect类的转化  
	//	cv::Rect cvrect = cv::Rect(rect.x, rect.y, rect.w, rect.h);
	//	facesRect.push_back(cvrect);

	//	/// landmark detect
	//	PXCFaceData::LandmarksData *landmarksdata = trackedface->QueryLandmarks();
	//	if (landmarksdata == NULL) {
	//		wprintf_s(L"Unabel to get landmarks data\n");
	//		return 0;
	//	}
	//	// allocate the array big enough to hold the landmark points.
	//	pxcI32 npoints = landmarksdata->QueryNumPoints();
	//	std::cout << "npoints = " << npoints << std::endl;
	//	PXCFaceData::LandmarkPoint *points = new PXCFaceData::LandmarkPoint[npoints];
	//	// get the landmark data
	//	landmarksdata->QueryPoints(points);
	//	cv::Mat_<int> facelandmark;
	//	facelandmark.create(2 * npoints, 1);
	//	facelandmark.setTo(0);
	//	for (int i = 0; i < npoints; ++i)
	//	{
	//		int x = (int)points[i].image.x;
	//		int y = (int)points[i].image.y;
	//		facelandmark.at<int>(i) = x;
	//		facelandmark.at<int>(i + npoints) = y;
	//	}
	//	faceLandmarks.push_back(facelandmark);

	//	delete[] points;
	//}

	return gFaceDetectGlob.FrameAcquired;
}

void FaceDetect_Shutdown()
{
	if (gFaceDetectGlob.SenseManager != NULL)
	{
		gFaceDetectGlob.SenseManager->Release();
		gFaceDetectGlob.SenseManager = NULL;
	}
}

void FaceDetect_FrameRelease()
{
	assert(gFaceDetectGlob.FrameAcquired);
	if (gFaceDetectGlob.AcquiredPreviewImage)
	{
		gFaceDetectGlob.AcquiredPreviewImage->Release();
		gFaceDetectGlob.AcquiredPreviewImage = NULL;
	}

	gFaceDetectGlob.SenseManager->ReleaseFrame();
	gFaceDetectGlob.FrameAcquired = false;
}

PXCImage *FaceDetect_GetColorImage()
{
	assert(gFaceDetectGlob.FrameAcquired);
	PXCCapture::Sample *sample = gFaceDetectGlob.SenseManager->QuerySample();
	return sample->color;
}

PXCImage *FaceDetect_GetDepthImage()
{
	assert(gFaceDetectGlob.FrameAcquired);
	PXCCapture::Sample *sample = gFaceDetectGlob.SenseManager->QuerySample();
	return sample->depth;
}

bool FaceDetect_GetFaceRects(std::vector<cv::Rect> &facesRects)
{
	//// Face Detect
	pxcI32 nfaces = gFaceDetectGlob.FaceData->QueryNumberOfDetectedFaces();
	std::cout << "nfaces = " << nfaces << std::endl;
	for (pxcI32 i = 0; i < nfaces; i++) {
		//按序号获取一个人脸的数据实例  
		std::cout << "face ID" << i << std::endl;
		PXCFaceData::Face *trackedface = gFaceDetectGlob.FaceData->QueryFaceByIndex(i);

		/// face detect
		PXCFaceData::DetectionData *detectiondata = trackedface->QueryDetection();
		if (detectiondata == NULL) {
			wprintf_s(L"Unabel to get detection data\n");
			return FALSE;
		}
		//将当前人脸的位置数据存在rect中  
		PXCRectI32 rect;
		detectiondata->QueryBoundingRect(&rect);
		//PXCRectI32到opencv中Rect类的转化  
		cv::Rect cvrect = cv::Rect(rect.x, rect.y, rect.w, rect.h);
		facesRects.push_back(cvrect);
	}
	return TRUE;
}

bool FaceDetect_GetFaceLandmarks2D(std::vector<cv::Mat_<int> > &faceLandmarks2d)
{
	//// Face Detect
	pxcI32 nfaces = gFaceDetectGlob.FaceData->QueryNumberOfDetectedFaces();
	std::cout << "nfaces = " << nfaces << std::endl;
	for (pxcI32 i = 0; i < nfaces; i++) {
		//按序号获取一个人脸的数据实例  
		std::cout << "face ID" << i << std::endl;
		PXCFaceData::Face *trackedface = gFaceDetectGlob.FaceData->QueryFaceByIndex(i);

		PXCFaceData::LandmarksData *landmarksdata = trackedface->QueryLandmarks();
		if (landmarksdata == NULL) {
			wprintf_s(L"Unabel to get landmarks data\n");
			//return FALSE;
			continue;
		}
		// allocate the array big enough to hold the landmark points.
		pxcI32 npoints = landmarksdata->QueryNumPoints();
		std::cout << "npoints = " << npoints << std::endl;
		PXCFaceData::LandmarkPoint *points = new PXCFaceData::LandmarkPoint[npoints];
		// get the landmark data
		landmarksdata->QueryPoints(points);
		cv::Mat_<int> facelandmark2d;
		facelandmark2d.create(2 * npoints, 1);
		facelandmark2d.setTo(0);
		for (int i = 0; i < npoints; ++i)
		{
			int x = (int)points[i].image.x;
			int y = (int)points[i].image.y;
			facelandmark2d.at<int>(i) = x;
			facelandmark2d.at<int>(i + npoints) = y;
		}
		faceLandmarks2d.push_back(facelandmark2d);
		delete[] points;
	}
	return TRUE;
}

bool FaceDetect_GetFaceLandmarks3D(std::vector<cv::Mat_<int> > &faceLandmarks3d)
{
	//// Face Detect
	pxcI32 nfaces = gFaceDetectGlob.FaceData->QueryNumberOfDetectedFaces();
	std::cout << "nfaces = " << nfaces << std::endl;
	for (pxcI32 i = 0; i < nfaces; i++) {
		//按序号获取一个人脸的数据实例  
		std::cout << "face ID" << i << std::endl;
		PXCFaceData::Face *trackedface = gFaceDetectGlob.FaceData->QueryFaceByIndex(i);

		PXCFaceData::LandmarksData *landmarksdata = trackedface->QueryLandmarks();
		if (landmarksdata == NULL) {
			wprintf_s(L"Unabel to get landmarks data\n");
			// return FALSE;
			continue;
		}
		// allocate the array big enough to hold the landmark points.
		pxcI32 npoints = landmarksdata->QueryNumPoints();
		std::cout << "npoints = " << npoints << std::endl;
		PXCFaceData::LandmarkPoint *points = new PXCFaceData::LandmarkPoint[npoints];
		//initialize Array
		for (int i = 0; i < npoints; i++) {
			points[i].world.x = 0.0;
			points[i].world.y = 0.0;
			points[i].world.z = 0.0;
		}
		// get the landmark data
		landmarksdata->QueryPoints(points);
		//convert depth data is to millimeters
		for (int i = 0; i < npoints; i++) {
			points[i].world.x *= 1000.0f;
			points[i].world.y *= 1000.0f;
			points[i].world.z *= 1000.0f;
		}
		cv::Mat_<int> facelandmark3d;
		facelandmark3d.create(3 * npoints, 1);
		facelandmark3d.setTo(0);
		for (int i = 0; i < npoints; ++i)
		{
			int x = (int)points[i].world.x;
			int y = (int)points[i].world.y;
			int z = (int)points[i].world.z;
			facelandmark3d.at<int>(i) = x;
			facelandmark3d.at<int>(i + npoints) = y;
			facelandmark3d.at<int>(i + 2*npoints) = z;
		}
		faceLandmarks3d.push_back(facelandmark3d);
		delete[] points;
	}
	return TRUE;
}

bool FaceDetect_GetFaceLandmarks(std::vector<cv::Mat_<int> > &faceLandmarks2d, std::vector<cv::Mat_<int> > &faceLandmarks3d)
{
	//// Face Detect
	pxcI32 nfaces = gFaceDetectGlob.FaceData->QueryNumberOfDetectedFaces();
	std::cout << "nfaces = " << nfaces << std::endl;
	for (pxcI32 i = 0; i < nfaces; i++) {
		//按序号获取一个人脸的数据实例  
		std::cout << "face ID" << i << std::endl;
		PXCFaceData::Face *trackedface = gFaceDetectGlob.FaceData->QueryFaceByIndex(i);

		PXCFaceData::LandmarksData *landmarksdata = trackedface->QueryLandmarks();
		if (landmarksdata == NULL) {
			wprintf_s(L"Unabel to get landmarks data\n");
			// return FALSE;
			continue;
		}
		// allocate the array big enough to hold the landmark points.
		pxcI32 npoints = landmarksdata->QueryNumPoints();
		std::cout << "npoints = " << npoints << std::endl;
		PXCFaceData::LandmarkPoint *points = new PXCFaceData::LandmarkPoint[npoints];
		// get the landmark data
		//initialize Array
		for (int i = 0; i < npoints; i++) {
			points[i].world.x = 0.0;
			points[i].world.y = 0.0;
			points[i].world.z = 0.0;
		}
		landmarksdata->QueryPoints(points);
		//convert depth data is to millimeters
		for (int i = 0; i < npoints; i++) {
			points[i].world.x *= 1000.0f;
			points[i].world.y *= 1000.0f;
			points[i].world.z *= 1000.0f;
		}

		/// RS_Point -> CV_Mat
		cv::Mat_<int> facelandmark2d, facelandmark3d;
		facelandmark2d.create(2 * npoints, 1);
		facelandmark2d.setTo(0);
		facelandmark3d.create(3 * npoints, 1);
		facelandmark3d.setTo(0);
		for (int i = 0; i < npoints; ++i)
		{
			// 2d point
			int x_image = (int)points[i].image.x;
			int y_image = (int)points[i].image.y;
			facelandmark2d.at<int>(i) = x_image;
			facelandmark2d.at<int>(i + npoints) = y_image;

			// 3d point
			int x_world = (int)points[i].world.x;
			int y_world = (int)points[i].world.y;
			int z_world = (int)points[i].world.z;
			facelandmark3d.at<int>(i) = x_world;
			facelandmark3d.at<int>(i + npoints) = y_world;
			facelandmark3d.at<int>(i + 2 * npoints) = z_world;
		}
		faceLandmarks2d.push_back(facelandmark2d);
		faceLandmarks3d.push_back(facelandmark3d);
		
		delete[] points;
	}
	return TRUE;
}

bool FaceDetect_GetFacePoseAndPulse(std::vector<cv::Vec3d> &HeadPositionCV, std::vector<cv::Vec3d> &EulerAnglesCV, std::vector<cv::Vec4d> &QuaternionCV, std::vector<cv::Matx33d> &RotationMatrixCV)
{
	//// Face Detect
	pxcI32 nfaces = gFaceDetectGlob.FaceData->QueryNumberOfDetectedFaces();
	std::cout << "nfaces = " << nfaces << std::endl;
	for (pxcI32 i = 0; i < nfaces; i++) {
		//按序号获取一个人脸的数据实例  
		std::cout << "face ID" << i << std::endl;
		PXCFaceData::Face *trackedface = gFaceDetectGlob.FaceData->QueryFaceByIndex(i);

		////////////////////////////////////////////////////////////////////////////////
		/// pose
		const PXCFaceData::PoseData* poseData = trackedface->QueryPose();
		/////
		/// 头部坐标参数
		pxcBool poseHeadPositionExist;
		PXCFaceData::HeadPosition headposition;
		/// 脸部姿态的欧拉角
		pxcBool poseAnglesExist;
		PXCFaceData::PoseEulerAngles angles;
		/// 世界坐标系系下的脸部位置信息
		pxcBool poseQuaternionExist;
		PXCFaceData::PoseQuaternion quaternion;
		/// 脸部姿态的旋转矩阵
		pxcBool poseRotationMatrixExist;
		pxcF64 poseRotationMatrix[9];

		/// 
		if (poseData == NULL) 
		{
			poseAnglesExist = 0;
			poseQuaternionExist = 0;
			poseHeadPositionExist = 0;
			poseRotationMatrixExist = 0;
		}
		else 
		{
			poseAnglesExist = poseData->QueryPoseAngles(&angles);
			poseQuaternionExist = poseData->QueryPoseQuaternion(&quaternion);
			poseHeadPositionExist = poseData->QueryHeadPosition(&headposition);
			poseRotationMatrixExist = poseData->QueryRotationMatrix(poseRotationMatrix);
		}
	}
	return TRUE;
}

#endif