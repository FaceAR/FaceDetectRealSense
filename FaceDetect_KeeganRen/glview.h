#ifndef GLVIEW_H_INCLUDED
#define GLVIEW_H_INCLUDED

#include "stdafx.h"
//////
// TODO: freeglut
#include <GL\freeglut.h>

extern int rows, rows2;
extern const int cols;

extern cv::Mat dataPoints;

#pragma region
extern const double MY_MINDISTANCE;
extern const double MY_MAXDISTANCE;
extern const double CAMERA_DISTANCE;

extern int window1;
extern float angle;
extern GLdouble upY, fovy, cameraDistance;
extern float axisSize, axisThick_, axisThick;
extern float cubeSize, cubeSize_;
extern float velocity, velocityKey;
extern int window_w, window_h;

extern int MouseX,MouseY;
extern bool M_left;
extern bool M_right;
extern bool M_middle;
extern int wheel;

extern FILE * fp;
extern int * FLAG;
extern GLdouble wx,wy,wz, wx_s,wy_s,wz_s, wx_s2,wy_s2,wz_s2, wDistance;

extern GLdouble nearZ, farZ;
extern GLdouble eyeX, eyeY, eyeZ;
extern GLdouble eyeR, eyeTheta, eyePhi, onEyeTheta, onEyePhi, theta, phi;
extern int eyeXleft, eyeXright, eyeYup, eyeYdown, eyeZin, eyeZout;
extern GLdouble centerX, centerY, centerZ;
extern GLdouble centerXdif, centerYdif, centerZdif;
extern int centerXleft, centerXright, centerYup, centerYdown;
#pragma endregion


struct MaterialStruct {
 GLfloat ambient[4];
 GLfloat diffuse[4];
 GLfloat specular[4];
 GLfloat shininess;
};

void disp();
void initFlag(void);
void initParam(void);
void initParamMove(void);
void drawText(int offset);
void drawPointsAsCube();
void mouse(int button, int state, int x, int y);
void drag(int x, int y);
void passive(int x, int y);
void MouseWheel(int wheel_number, int direction, int x, int y);
void PICK_UP(int x, int y);
int SELECT_HITS(int hits,GLuint *buf);
float returnDepth(int x, int y);
void  CalculateWorldCo(
				int x, int y, float depth,
				double &wx, double &wy,double &wz);
void glut_keyboard(unsigned char key, int x, int y);
void animate(void);
void myGlutIdle();
void Initialize();
void DrawString(std::string str, int w, int h, int x0, int y0);
void drowSphere(double r, 
                double x, double y, double z, 
                MaterialStruct color);
void drowDodecahedron(double r, 
                double x, double y, double z, 
                MaterialStruct color);
void drowCuboid(double a, double b, double c, 
                double x, double y, double z, 
                MaterialStruct color);
void drowCuboid(double a, double b, double c, 
                double x, double y, double z, 
                MaterialStruct color, 
                double theta, 
                double nx, double ny, double nz);
#endif