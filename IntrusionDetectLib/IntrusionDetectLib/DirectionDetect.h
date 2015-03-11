#pragma once

#include "cv.h"
#include "highgui.h"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <iostream>
#include <fstream>
#include <vector>
#include <queue>
#define _USE_MATH_DEFINES
#include <math.h>
#include <windows.h>
//#include "videoProcess.h"
using namespace std;
using namespace cv;

typedef struct _IntrOb{
	Rect detecR;
	int segID;
	int module;
	double orient;     //angle
	CvPoint center;
	Mat sift_src;
}IntrOb;

#define GRADIENT 0x0001
#define THETA 0x0002


class DirectionDetect
{
public:
	//此部分针对数据库
	float computeCos(CvPoint p1,CvPoint p2);//计算两个点的斜率
	float computeGradient(CvPoint p1,CvPoint p2);//计算两个点的斜率
	float computeTheta(CvPoint p1,CvPoint p2);//计算两点方向角
	int computeTrajectory(vector<CvPoint>* trajectory,vector<float>* result,UINT flag,int delta=23);//计算轨迹的方向角或者斜率
	int computeCenterOfTrajectory(vector<CvPoint>* cornerPoint,int width,int height,vector<CvPoint>* centerPoint);//将轨迹的左上角坐标转化为中心坐标
	//还需要数据库控制指针等接口

	//下面针对demo,接口参数为相邻两帧数据
	bool computeOrientByTwoFrame(vector<IntrOb>* befOb, vector<IntrOb>*curOb, int direction);
	void drawOrient(Mat mat, vector<IntrOb>*curOb);
	int computeModule(CvPoint p1,CvPoint p2);


	//2015.3.8
	bool initHistoryData(int deltaFrame);
	bool computeOrient(vector<IntrOb>*curOb, int direction);//用隔一定数量的两帧数据计算方向

protected:
private:
	int aver_k; //average k of orientation平均斜率
	int delta;
	queue<vector<IntrOb>> historyData;
};