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
	directionDetect->initHistoryData(5);   ///初始化计算角度间隔
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

	Vector<Rect> myRect;
	Vector<Rect> myRect_;
	for (; mySeq; mySeq = mySeq->h_next)
	{
		Rect r = ((CvContour*)mySeq)->rect;
		if (r.height*r.width > 1500)///<轮廓点过少，不做处理
			myRect_.push_back(r);
	}

	//合并重合框

	for (int i = 0; i<myRect_.size(); i++)
	{
		Rect r1 = myRect_[i];
		for (int j = i + 1; j < myRect_.size(); j++)
		{
			Rect r2 = myRect_[j];
			int maxx = min(r1.x + r1.width, r2.x + r2.width);
			int maxy = min(r1.y + r1.height, r2.y + r2.height);
			int minx = max(r1.x, r2.x);
			int miny = max(r1.y, r2.y);
			if (minx < maxx + 10 && miny < maxy + 10)
			{
				Rect r, r3;
				r3.x = min(r1.x, r2.x), r3.y = min(r1.y, r2.y), r3.width = max(r1.x + r1.width, r2.x + r2.width) - r3.x, r3.height = max(r1.y + r1.height, r2.y + r2.height) - r3.y;
				for (int m = i; m < j; m++)
				{
					Rect r4 = myRect_[m];
					int maxx = min(r3.x + r3.width, r4.x + r4.width);
					int maxy = min(r3.y + r3.height, r4.y + r4.height);
					int minx = max(r3.x, r4.x);
					int miny = max(r3.y, r4.y);
					if (minx < maxx + 10 && miny < maxy + 10)
					{
						r.x = min(r3.x, r4.x), r.y = min(r3.y, r4.y), r.width = max(r3.x + r3.width, r4.x + r4.width) - r.x, r.height = max(r3.y + r3.height, r4.y + r4.height) - r.y;
						myRect_[m].x = 0, myRect_[m].y = 0, myRect_[m].width = 0, myRect_[m].height = 0;
					}
					else
						r = r3;
				}
				myRect_[i].x = r.x, myRect_[i].y = r.y, myRect_[i].width = r.width, myRect_[i].height = r.height;
				r1 = myRect_[i];
				myRect_[j].x = 0, myRect_[j].y = 0, myRect_[j].width = 0, myRect_[j].height = 0;
			}
		}
	}
	for (int i = 0; i < myRect_.size(); i++)
	{
		if (myRect_[i].width != 0)
			myRect.push_back(myRect_[i]);
	}



	//sift检测原图像
	Vector<Mat> sift_Mat;
	Mat ori_mat_ = ori_mat.clone();
	Mat siftMat;
	for (int i = 0; i < myRect.size(); i++)
	{
		siftMat = ori_mat_(Range(myRect[i].y, myRect[i].y + myRect[i].height), Range(myRect[i].x, myRect[i].x + myRect[i].width));
		sift_Mat.push_back(siftMat);
	}



	IntrOb trans;
	int segID = 1;
	for (int i = 0; i<myRect.size(); i++)
	{
		Rect r = myRect[i];
		trans.detecR = r;
		trans.segID = segID;
		trans.sift_src = sift_Mat[i];
		segID++;
		curOb.push_back(trans);
		rectangle(ori_mat, cv::Point(r.x, r.y), cv::Point(r.x + r.width, r.y + r.height), cv::Scalar(255, 0, 0), 1);
		if (det.detect(r))///区域检测
		{
			ifInArea = true;
			//rectangle(ori_mat, Point(det.rect.x, det.rect.y), Point(det.rect.x + det.rect.width, det.rect.y + det.rect.height), Scalar(255, 0, 0), -1);
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
	//namedWindow("Frame");
	//namedWindow("FG Mask MOG 2");
	return 0;
}

int videoProcess::processStream(unsigned char* buffer, size_t len, id_image_t * ptr){
	int count = decode(buffer, len, this, ptr);
	return count;
}

int videoProcess::yuv2Mat(unsigned char * buf, int wrap, int width, int height){
	unsigned char * ptr = new unsigned char[width*height];
	int a = 0, i;
	for (i = 0; i < height; i++)
	{
		memcpy(ptr + a, buf + i * wrap, width);
		a += width;
	}
	Mat frame(height, width, CV_8UC1, ptr);
	int ifIntrusion = this->process(frame);
	delete[] ptr;
	return ifIntrusion;
}

int videoProcess::process(Mat frame)
{
	pMOG2->operator()(frame, fgMaskMOG2,-0.0001);
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
				ifCorrectDerection = ifCorrectDerection || directionDetect->computeOrient(&curOb, parser.params->direction[ds]);
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
