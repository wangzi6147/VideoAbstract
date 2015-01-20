#include <opencv2/opencv.hpp>
#include <stdio.h>
#include<math.h>
#include <segmentation_1208_wf.h>

#define ALPHA 0.03
#define THRD 10
#define COUNTTHRD 400
#define ISCHANGE 1
#define NOTCHANGE 0
#define RATIO 2

using namespace cv;


CvMat *BKGFrame = NULL;
int nFrmNum = 0;
int FLAG = 0;


//int main(void)
//{
//	CvCapture* pCapture = cvCreateFileCapture("1.avi");
//	IplImage *pFrame = NULL;
//
//	//cvNamedWindow("temp", 1);
//	int nFrmNum = 0;
//	int count = 0;
//	IplImage *pFrImg = NULL;
//
//	while (pFrame = cvQueryFrame(pCapture))
//	{
//		nFrmNum++;
//		pFrImg = cvCreateImage(cvSize(pFrame->width, pFrame->height), IPL_DEPTH_8U, 1);
//		cvCvtColor(pFrame, pFrImg, CV_BGR2GRAY);
//		CvMat *FrameMat = cvCreateMat(pFrImg->height, pFrImg->width, CV_32FC1);
//		cvConvert(pFrImg, FrameMat);
//		if (nFrmNum == 1)
//			Initial(FrameMat);
//		else
//		{
//			printf("%d\n", FrameProcessing(FrameMat));
//		}
//
//		//cvWaitKey();
//
//	}
//	return 0;
//}

int Initial(CvMat *VideoFrame)
{
	if (VideoFrame == NULL)
		return -1;
	BKGFrame = cvCreateMat(VideoFrame->rows, VideoFrame->cols, CV_32FC1);
	BKGFrame = cvCloneMat(VideoFrame);
	return 0;
}

int BinaryCount(CvMat *BinaryFrame)
{
	int count = 0;
	int i = 0;
	int j = 0;
	for ( i = 0; i < BinaryFrame->rows; ++i)
	{
		const float *myPoint = (const float*)(BinaryFrame->data.ptr + i*BinaryFrame->step);
		for (j =0; j < BinaryFrame->cols; ++j)
		{
			float t = *myPoint;
			if (t  != 0.0f)
				++count;
			//count += *myPoint++;
			++myPoint;
		}
	}
	return count;
}



int FrameProcessing(CvMat *VideoFrame)
{
	nFrmNum++;
	if (nFrmNum % RATIO == 0)
		return FLAG;
	//CvMat *binaryFrame = cvCreateMat(VideoFrame->rows, VideoFrame->cols, CV_8UC1);
	CvMat *binaryFrame = cvCreateMat(VideoFrame->rows, VideoFrame->cols, CV_32FC1);

	CvMat *TempFrame = cvCloneMat(VideoFrame);

	cvAbsDiff(VideoFrame, BKGFrame, binaryFrame);

	cvErode(binaryFrame, binaryFrame, NULL, 1); //¸¯Ê´  

	cvDilate(binaryFrame, binaryFrame, NULL, 1); //ÅòÕÍ  

	cvThreshold(binaryFrame, binaryFrame, THRD, 255.0, CV_THRESH_BINARY);

	int count = BinaryCount(binaryFrame);

	cvRunningAvg(TempFrame, BKGFrame, ALPHA, 0);

	cvReleaseMat(&binaryFrame);
	
	cvReleaseMat(&TempFrame);

	if (count > COUNTTHRD)
		return (FLAG = ISCHANGE);
	else
		return (FLAG = NOTCHANGE);
}












