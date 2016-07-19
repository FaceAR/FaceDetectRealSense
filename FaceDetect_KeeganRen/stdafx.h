// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件
//

#pragma once

#include "targetver.h"

#include <stdio.h>
#include <tchar.h>

// TODO:  在此处引用程序需要的其他头文件
#include <iostream>
#include "assert.h"
#include "wchar.h"
#include "windows.h"
#include "conio.h"

#define ASSERT(Condition, Message)

const UINT CPUT_MAX_PATH = 2048;


//////
// TODO: OpenCV
#include <opencv.hpp>

#define _USE_MATH_DEFINES
#include <math.h>
#include <fstream>
#include <string.h>
#include <time.h>