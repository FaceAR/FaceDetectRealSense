// FaceDetect_KeeganRen.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "GameFaceData.h"
#include "util_render.h"

#include "glview.h"

float velocity = 0.002;
float cubeSize = 0.01;
float angle = 0.0;
GLdouble cameraDistance = 3.0;
float axisSize = 0.25;
float axisThick_ = 0.005;
int window_w = 1000;
int window_h = 1000;

////
cv::Mat dataPoints;
int window1;
int rows, rows2;
const int cols = 3;
float axisThick;
float cubeSize_;
float velocityKey = 0.02;

int * FLAG;
FILE * fp;

void show3dPoint(int argc, char** argv, cv::Mat &dataPoints)
{
	axisThick = axisThick_;
	cubeSize_ = cubeSize / 4.;
	/// OpenGL
	FLAG = (int *)malloc(sizeof(int)* rows);
	for (int i = 0; i<rows; i++){
		FLAG[i] = 0;
	}
	/// OpenGL
	// GLUT initialize
	initFlag();
	initParam();
	Initialize();
	//window1
	glutInit(&argc, argv);
	glutInitWindowPosition(0, 0);
	glutInitWindowSize(window_w, window_h);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);
	window1 = glutCreateWindow("Window1");

	glutDisplayFunc(disp);
	glutMouseFunc(mouse);
	glutMotionFunc(drag);
	glutPassiveMotionFunc(passive);
	glutMouseWheelFunc(MouseWheel);
	glutIdleFunc(myGlutIdle);
	glutKeyboardFunc(glut_keyboard);
	glutIdleFunc(animate);
	glClearColor(0.0, 0.0, 0.0, 0.5);
	glutMainLoop();
}

int main(int argc, char** argv)
{
	std::cout << "Begin" << std::endl;
	FaceDetect_Init();

	UtilRender renderc(L"Color"), renderd(L"Depth");

	while (true)
	{
		std::vector<cv::Rect> facesRects;
		std::vector<cv::Mat_<int> > faceLandmarks2d;
		std::vector<cv::Mat_<int> > faceLandmarks3d;
		FaceDetect_FrameAcquire(/*facesRects, faceLandmarks*/);
		FaceDetect_GetFaceRects(facesRects);
		//FaceDetect_GetFaceLandmarks2D(faceLandmarks2d);
		//FaceDetect_GetFaceLandmarks3D(faceLandmarks3d);
		FaceDetect_GetFaceLandmarks(faceLandmarks2d, faceLandmarks3d);
		//FaceDetect_GetFacePoseAndPulse();

		PXCImage *colorIm = FaceDetect_GetColorImage();
		PXCImage *depthIm = FaceDetect_GetDepthImage();
		ConvertPXCImage2CVMat test;
		if (colorIm) {
			cv::Mat colorMat = test.ConvertPXCImageToOpenCVMat(colorIm, ImageFormat::STREAM_TYPE_COLOR);
			std::cout << "facesRects.size() = " << facesRects.size() << std::endl;
			for (size_t i = 0; i < facesRects.size(); ++i) {
				cv::rectangle(colorMat, facesRects.at(i), cv::Scalar(255, 255, 0), 2);
				std::stringstream ss;
				ss << i;
				std::string id = ss.str();
				id = "ID:" + id;
				cv::putText(colorMat, id, cv::Point(facesRects.at(i).x + facesRects.at(i).width / 2, facesRects.at(i).y - facesRects.at(i).height / 20), CV_FONT_HERSHEY_COMPLEX, 0.4, cv::Scalar(255, 255, 0));
			}
			std::cout << "faceLandmarks2d.size() = " << faceLandmarks2d.size() << std::endl;
			for (size_t i = 0; i < faceLandmarks2d.size(); ++i) {
				cv::Mat_<int> faceLandmark2d = faceLandmarks2d.at(i);
				for (size_t j = 0; j < faceLandmark2d.rows / 2; ++j) {
					std::stringstream ss_point;
					ss_point << j;
					std::string point_id = ss_point.str();
					cv::putText(colorMat, point_id, cv::Point(faceLandmark2d.at<int>(j)-5, faceLandmark2d.at<int>(j + faceLandmark2d.rows / 2) - 5), CV_FONT_HERSHEY_COMPLEX, 0.4, cv::Scalar(0, 0, 255));

					cv::Point2i pointCircle(faceLandmark2d.at<int>(j), faceLandmark2d.at<int>(j + faceLandmark2d.rows / 2));
					cv::circle(colorMat, pointCircle, 1, cv::Scalar(0, 255, 0), 2);
				}
			}
			for (size_t i = 0; i < faceLandmarks3d.size(); ++i) {
				cv::Mat_<int> faceLandmark3d = faceLandmarks3d.at(i);
				cv::Mat_<float> faceLandmark3d_78_3;
				faceLandmark3d_78_3.create(78, 3);
				faceLandmark3d_78_3.setTo(0.0);
				for (size_t j = 0; j < faceLandmark3d.rows / 3; ++j) {
					faceLandmark3d_78_3.at<float>(j, 0) = (float)faceLandmark3d.at<int>(j);
					faceLandmark3d_78_3.at<float>(j, 1) = (float)faceLandmark3d.at<int>(j + faceLandmark3d.rows/3);
					faceLandmark3d_78_3.at<float>(j, 2) = (float)faceLandmark3d.at<int>(j + 2*faceLandmark3d.rows/3);
				}
				//show3dPoint(argc, argv, dataPoints);
			}
			cv::imshow("OpenCV Window Color", colorMat);
			//cv::waitKey(1);
		}
		if (depthIm) {
			ConvertPXCImage2CVMat test;
			cv::Mat depthMat = test.ConvertPXCImageToOpenCVMat(depthIm, ImageFormat::STREAM_TYPE_DEPTH);
			cv::imshow("OpenCV Window Depth", depthMat);
			//cv::waitKey(1);
		}
		renderc.RenderFrame(colorIm);
		renderd.RenderFrame(depthIm);

		FaceDetect_FrameRelease();

		char character_press = cv::waitKey(33);
		// quit the application
		if (character_press == 'q') {
			break;
		}
	}
	FaceDetect_Shutdown();
	std::cout << "End" << std::endl;
	cv::waitKey();
	return 0;
}

