/**
*@file  publicheader.h
*@brief 公共头文件声明
*
*该文件主要是加载opencv和mysql的声明
*
*
*@author 作者 lu peng&&chen dai wu ,包含email等
*@version 版本号(0.0)
*@data 2012-8-21
*/

#pragma once
#include <string.h>
#include <afx.h>
#include <afxmt.h>
#include <afxwin.h>
#include <Windows.h>
#include <math.h>
#include <io.h> 

#include "atlstr.h"


///加载opencv和mysql头文件
#include "../PublicHeader/mysql/mysql.h"
#include "../PublicHeader/opencv.hpp"

///设置opencv和mysql用到的lib文件的相对路径
#pragma comment(lib,"../PublicHeader/Lib/opencv_core244d.lib")
#pragma comment(lib,"../PublicHeader/Lib/opencv_contrib244d.lib")
#pragma comment(lib,"../PublicHeader/Lib/opencv_photo244d.lib")
#pragma comment(lib,"../PublicHeader/Lib/opencv_videostab244d.lib")
#pragma comment(lib,"../PublicHeader/Lib/opencv_nonfree244d.lib")
#pragma comment(lib,"../PublicHeader/Lib/opencv_highgui244d.lib")
#pragma comment(lib,"../PublicHeader/Lib/opencv_imgproc244d.lib")
#pragma comment(lib,"../PublicHeader/Lib/opencv_objdetect244d.lib")
#pragma comment(lib,"../PublicHeader/Lib/opencv_video244d.lib")
#pragma comment(lib,"../PublicHeader/Lib/opencv_calib3d244d.lib")
#pragma comment(lib,"../PublicHeader/Lib/opencv_features2d244d.lib")
#pragma comment(lib,"../PublicHeader/Lib/opencv_flann244d.lib")
#pragma comment(lib,"../PublicHeader/Lib/opencv_legacy244d.lib")
#pragma comment(lib,"../PublicHeader/Lib/opencv_ts244d.lib")
#pragma comment(lib,"../PublicHeader/Lib/mysql/libmysql.lib")

