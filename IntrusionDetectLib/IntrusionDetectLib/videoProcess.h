#pragma  once

#include "areaDetect.h"
#include "opencv2\legacy\legacy.hpp"
#include  "opencv2\video\background_segm.hpp"
#include "DirectionDetect.h"
#include "JsonParser.h"

using namespace cv;

class videoProcess
{
public:
	videoProcess(JsonParser parser);
	~videoProcess();
	void processer(char* videopathname);
	bool showcontours(Mat ori_mat);
	void markseg();
	areaDetect det;
	DirectionDetect* directionDetect;

	int process(Mat frame);
	int processStream(unsigned char* buffer, size_t len, videoProcess * pro);
	int yuv2Mat(unsigned char * buf, int wrap, int width, int height, videoProcess * pro);

private:
	Mat frame;
	Mat fgMaskMOG2;
	Mat resultImage;
	Mat BG;
	BackgroundSubtractor * pMOG2;
	bool ifcontinue;
	int frameCount;              //隔多少帧进行方向检测
	vector<IntrOb> befOb;
	vector<IntrOb> curOb;

	JsonParser parser;
	bool preDetect = false;
	int frameCount_;
	IplImage* p_Image;
	int init();
};

