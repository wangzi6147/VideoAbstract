
#include "stdafx.h"
#ifndef VTRACK_H
#define VTRACK_H
#include <fstream>
#include <sstream>
#include <time.h>
//#include <opencv2/opencv.hpp>
#include <vector>
#include "LibMySql.h"

#include "../PublicHeader/opencv.hpp"

/*
#pragma comment(lib,"../PublicHeader/Lib/opencv_core220d.lib")
#pragma comment(lib,"../PublicHeader/Lib/opencv_highgui220d.lib")
#pragma comment(lib,"../PublicHeader/Lib/opencv_imgproc220d.lib")
#pragma comment(lib,"../PublicHeader/Lib/opencv_objdetect220d.lib")
#pragma comment(lib,"../PublicHeader/Lib/opencv_video220d.lib")
#pragma comment(lib,"../PublicHeader/Lib/opencv_calib3d220d.lib")
#pragma comment(lib,"../PublicHeader/Lib/opencv_features2d220d.lib")
#pragma comment(lib,"../PublicHeader/Lib/opencv_ffmpeg220d.lib")
#pragma comment(lib,"../PublicHeader/Lib/opencv_flann220d.lib")
#pragma comment(lib,"../PublicHeader/Lib/opencv_gpu220d.lib")
#pragma comment(lib,"../PublicHeader/Lib/opencv_legacy220d.lib")
#pragma comment(lib,"../PublicHeader/Lib/opencv_ml220d.lib")
#pragma comment(lib,"../PublicHeader/Lib/opencv_ts220.lib")
*/

//#pragma comment(lib,"../PublicHeader/Lib/opencv_core220d.lib")
//#pragma comment(lib,"../PublicHeader/Lib/opencv_highgui220d.lib")
//#pragma comment(lib,"../PublicHeader/Lib/opencv_imgproc220d.lib")
////#pragma comment(lib,"../PublicHeader/Lib/opencv_objdetect220d.lib")
//#pragma comment(lib,"../PublicHeader/Lib/opencv_video220d.lib")



using namespace cv;
using namespace std;

#define RECORD_VIDEO 0  //是否保存含有运动目标轨迹的视频片段
//该宏用于测试函数耗时
#define CALC_RUN_TIME(fun_line, func_name) \
    {\
        clock_t start,stop;\
        float temp;\
        start = clock();\
        fun_line;\
        stop = clock();\
        temp=((double)(stop-start)*1000.0)/CLOCKS_PER_SEC;\
        char format_str[64];\
        strcpy(format_str, (func_name));\
        strcat(format_str, ": %.4fms\n");\
        printf(format_str, temp);\
    };


typedef struct _WorkBlob{
    vector<Rect> rcs;
    vector<Point> centers;
    clock_t lastUpdateTime;
    MatND histMat;
    vector<int> frameIDs;
    vector<Scalar> rgb;
	Mat blobMat;
	Rect trackedRC;
    //int lastROIValue;
}WorkBlob;

class VBlob
{
public:
    VBlob();
    VBlob(WorkBlob& wb, int id);
	~VBlob();

	//void AddBlobInfo(int frmId, Rect pos, Point center);
	//void AppendBlobInfo(VBlob blob);//【2】这个也不大可能出现了
	//void ConvertWorkBlob(WorkBlob wblob, int Bid);
	Rect GetFrameRect(int frmId);//暂停画面显示
	
    vector<Rect> rcs;
    vector<Point> centers;
    vector<int> frameIDs;
    int id;
    Scalar rgbValue;

};

class VFrame
{
public:
    VFrame();
	~VFrame();

	void AddBlobId(int blobId);
	//int GetBlobId();
	vector<int> GetBlobId();
    int id;
	vector<int> blobIdx;
    int blobCount;

};

class VHandleData
{
public:
	VHandleData();//读或写文件
	~VHandleData();



	void StoreVFrame(int frameCount);

    void AddWorkBlob(WorkBlob wb);

	void WriteData(string filename);
	void ReadData(string filename);
	void WriteDatabase(CString objtablename,double *speed,bool *ifContinue);

//private:
	//bool isBlob;
    int blobCount;
    int frameCount;
    vector<VBlob> blobs;
    vector<VFrame> frames;
};

class VTrack
{
public:
    VTrack(Size minBlobSize=Size(10,10), Size maxBlobSize=Size(200,200), vector<Rect> roi=vector<Rect>(),double paramsAlpha=0.1, int timeout=2);
    ~VTrack(void);

	void DoProcessing(char *filename,/* char *dataname,*/ CString tablename);
	void DoProcessing();
	void StopProcessing();
	double VTrackSpeed();
    void ProcessFrame(Mat* frame);
    void SaveData(string filename,double *speed);
	void SaveDatabase(CString objtablename,double *speed,bool *ifContinue);
    Mat* GetBackgroundIMG();
	VHandleData vHandler;
protected:
	CWinThread *m_threadControl;//进程控制
	
private:
    void UpdateBackground(Mat* frame);
    void InitMemory(Mat* frame);
    vector<Rect> FindBlob(Mat* binIMG);

    Scalar GetRectColor(Rect rc);
	void CleanHistroyBlob();
	void TrackHistroyBlob();
	void UpdateHistoryBlob(vector<Rect>& currentRC);
	void UpdateHistoryBlob2(vector<Rect>& currentRC);

    Mat* backgroundIMG32;//CV_32FC1背景
    Mat* backgroundIMG;//CV_8UC1背景
    Mat* grayFrameIMG;//当前帧的灰度图

    Mat* colorIMG;//当前帧的3通道图
    Mat* diffIMG;//背景与当前帧的差
    Mat* diffHist;//记录前景的历史位置，从而进一步确定运动目标
	Mat* preGrayFrameIMG;
    int frameID;//当前帧号
	int frameCount;//总帧数
	double m_speed;//处理进度（等于frameID/frameCount）
    double paramsAlpha;//背景更新因子，初始为0.1
    Size maxBlobSize;//运动物体的最大尺寸
    Size minBlobSize;//运动物体的最小尺寸
	vector<Rect> ROI;
    vector<WorkBlob> historyBlobs;//记录历史中出现的所有运动物体
    clock_t historyTimeout;

public:
	CString objtablename;//数据库表名在Drawdetection.cpp中被定义为objtablename = m_curVideoNameText.Left(m_curVideoNameText.Find(".")) + "_ObjectTable";
	string filename;//视频
	//string dataname;
	bool m_IfContinue;//视频处理中断/继续标志
	
};

#endif

UINT DetectProcess(LPVOID pParam);

