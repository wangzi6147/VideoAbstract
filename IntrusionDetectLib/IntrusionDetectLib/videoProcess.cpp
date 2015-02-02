#include "videoProcess.h"
#include "areaDetect.h"
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/imgproc/imgproc_c.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/video/background_segm.hpp>
#include <stdio.h>
#include<time.h>
#include "DirectionDetect.h"
#include "264decoder.h"

using namespace cv;
using namespace std;

videoProcess::videoProcess(JsonParser parser){
	ifcontinue = true;
	CvRect detectRect = cvRect(parser.params->areas[0].left, parser.params->areas[0].top, parser.params->areas[0].width, parser.params->areas[0].height);
	det.rect = detectRect;
	this->parser = parser;
	directionDetect=new DirectionDetect();
	init();
}

videoProcess::~videoProcess(){
	stop264decoder(this);
	if (directionDetect!=NULL)
	{
		delete directionDetect;
	}
	if (pMOG2 != NULL)
		delete pMOG2; 
}

//void videoProcess::processer(char* videopathname)
//{
//	namedWindow("Frame");
//	namedWindow("FG Mask MOG 2");
//	pMOG2 = new BackgroundSubtractorMOG2(); 
//	IplImage* p_Image=new IplImage;
//	int frameCount_=0;
//
//
//	VideoCapture capture(videopathname);  
//	if (!capture.isOpened()){
//		printf("Unable to open video file:%s", videopathname);
//		waitKey();
//		return;
//	}
//
//	while (ifcontinue){
//		if (!capture.read(frame)) {
//			printf("Unable to read next frame.");
//			waitKey();
//			return;
//		}
//
//		pMOG2->operator()(frame, fgMaskMOG2);
//		pMOG2->getBackgroundImage(BG);
//
//		erode(fgMaskMOG2, fgMaskMOG2, Mat());
//		dilate(fgMaskMOG2, fgMaskMOG2, Mat());
//
//
//		//show the foreground contour
//		showcontours(fgMaskMOG2);
//
//		///标记segID
//		markseg();
//
//		///方向检测		
//		vector<IntrOb> befOb_;
//		vector<IntrOb> curOb_;
//		if (frameCount_ == 0)
//		{
//			for (int j = 0; j < befOb.size(); j++)
//							befOb_.push_back(befOb[j]);
//			frameCount_++;
//		}
//		else if (frameCount_ == frameCount)
//		{
//			resultImage = fgMaskMOG2;
//			if (befOb.size() != 0 && curOb.size() != 0)
//			{
//				//double aver_k = directionDetect->computeOrientByTwoFrame(&befOb, &curOb);
//				*p_Image = fgMaskMOG2;
//				directionDetect->drawOrient(p_Image, &curOb);
//				resultImage = p_Image;
//			}
//			frameCount_ = 1;
//		}
//		else
//			frameCount_++;
//		
//
//		befOb.clear();
//		for (int j = 0; j < curOb.size(); j++)
//			befOb.push_back(curOb[j]);
//
//		curOb.clear();
//		imshow("Frame", frame);
//		imshow("FG Mask MOG 2", fgMaskMOG2);
//
//		if(cvWaitKey(30)=='q')
//			break;
//	}
//
//	capture.release();
//
//	destroyAllWindows(); 
//	delete p_Image;
//}

bool videoProcess::showcontours(Mat ori_mat)
{
	CvSeq* mycont = NULL;
	CvMemStorage* mystor = NULL;
	mystor = cvCreateMemStorage(0);
	cv::Mat fg = ori_mat.clone();
	IplImage* my_pframe;
	my_pframe = &IplImage(fg);
	cvFindContours(my_pframe, mystor, &mycont, sizeof(CvContour), CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, cvPoint(0, 0));

	CvSeq *mySeq = mycont;
	bool ifInArea = false;
	IntrOb trans;
	int i = 1;
	for (; mySeq; mySeq = mySeq->h_next)
	{
		Rect r;
		r = ((CvContour*)mySeq)->rect;///<获取当前轮廓外接矩形
		if (mySeq->total > 6 && (r.height*r.width > 1500))///<轮廓点过少，不做处理
		{
			trans.detecR = r;
			trans.segID = i;
			i++;
			curOb.push_back(trans); 
			rectangle(ori_mat, cv::Point(r.x, r.y), cv::Point(r.x + r.width, r.y + r.height), cv::Scalar(255, 0, 0), 1);
			if (det.detect(r))///区域检测
			{
				ifInArea = true;
				//rectangle(ori_mat, Point(det.rect.x, det.rect.y), Point(det.rect.x + det.rect.width, det.rect.y + det.rect.height), Scalar(255, 0, 0), -1);
			}	
		}
	}
	return ifInArea;
}

void videoProcess::markseg()
{
	if (befOb.size() == 0)
		befOb = curOb;
	else
	{
		int k = 1;
		for (int m = 0; m<curOb.size(); m++)
		{
			curOb[m].segID = 0;
			curOb[m].orient = 0;
			curOb[m].module = 0;
			for (int n = 0; n<befOb.size(); n++)
			{
				int distance = (curOb[m].detecR.x - befOb[n].detecR.x)*(curOb[m].detecR.x - befOb[n].detecR.x) + (curOb[m].detecR.y - befOb[n].detecR.y)*(curOb[m].detecR.y - befOb[n].detecR.y);
				double compareArea = curOb[m].detecR.area()*1.0 / befOb[n].detecR.area();
				if (distance > 400)
				{
					continue;
				}
				if (compareArea > 2 || compareArea < 0.5)
				{
					continue;
				}
				curOb[m].segID = befOb[n].segID;
				break;
			}
			if (curOb[m].segID == 0)
			{
				curOb[m].segID = befOb.size() + k;
				k++;
			}
		}
	}
}

int videoProcess::init()
{
	pMOG2 = new BackgroundSubtractorMOG2();
	frameCount_ = 0;
	frameCount = 2;
	init264decoder();
	namedWindow("Frame");
	namedWindow("FG Mask MOG 2");
	return 0;
}

int videoProcess::processStream(unsigned char* buffer, size_t len, videoProcess * pro){
	decode(buffer, len, pro);
	return 0;
}

int videoProcess::yuv2Mat(unsigned char * buf, int wrap, int width, int height, videoProcess * pro){
	unsigned char * ptr = new unsigned char[width*height];
	int a = 0, i;
	for (i = 0; i < height; i++)
	{
		memcpy(ptr + a, buf + i * wrap, width);
		a += width;
	}
	Mat frame(height, width, CV_8UC1, ptr);
	pro->process(frame);
	delete[] ptr;
	return 0;
}

int videoProcess::process(Mat frame)
{
	pMOG2->operator()(frame, fgMaskMOG2);
	pMOG2->getBackgroundImage(BG);

	erode(fgMaskMOG2, fgMaskMOG2, Mat());
	dilate(fgMaskMOG2, fgMaskMOG2, Mat());


	//show the foreground contour
	bool ifInArea = showcontours(fgMaskMOG2);
	bool ifCorrectDerection = false;

	///标记segID
	markseg();

	///方向检测		
	vector<IntrOb> befOb_;
	vector<IntrOb> curOb_;
	if (frameCount_ == 0)
	{
		for (int j = 0; j < befOb.size(); j++)
			befOb_.push_back(befOb[j]);
		frameCount_++;
	}
	else if (frameCount_ == frameCount)
	{
		resultImage = fgMaskMOG2;
		if (befOb.size() != 0 && curOb.size() != 0)
		{
			for (int ds = 0; ds < parser.params->direction_size; ds++){
				ifCorrectDerection = ifCorrectDerection || directionDetect->computeOrientByTwoFrame(&befOb, &curOb, parser.params->direction[ds]);
			}
			directionDetect->drawOrient(fgMaskMOG2, &curOb);
		}
		frameCount_ = 1;
	}
	else{
		frameCount_++;
		ifCorrectDerection = preDetect;
	}

	befOb.clear();
	for (int j = 0; j < curOb.size(); j++)
		befOb.push_back(curOb[j]);

	curOb.clear();

	preDetect = ifInArea&&ifCorrectDerection;
	if (preDetect)
		rectangle(fgMaskMOG2, cv::Point(0, 0), cv::Point(100, 100), cv::Scalar(255, 0, 0), 1);

	imshow("Frame", frame);
	imshow("FG Mask MOG 2", fgMaskMOG2);

	if (cvWaitKey(30) == ' ')
	while (cvWaitKey(-1) != ' ')
		continue;
	return preDetect;
}
