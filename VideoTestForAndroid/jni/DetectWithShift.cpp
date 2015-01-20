#include "pris_videotest_JNIClient.h"
#include <opencv2/opencv.hpp>
#include <segmentation_1208_wf.h>

cv::Mat gray;

JNIEXPORT jboolean JNICALL Java_pris_videotest_JNIClient_initShift(JNIEnv * env,
		jclass, jintArray pixels, jint width, jint height) {
	///这里是初始化的步骤
	jint * cPixels;
	cPixels = env->GetIntArrayElements(pixels, 0);
	cv::Mat imgData(height, width, CV_8UC4, (unsigned char*) cPixels);
	cv::cvtColor(imgData, gray, CV_RGB2GRAY);
//	cv::Mat gray32;
	gray.convertTo(gray, CV_32FC1);
	CvMat g = gray;
	Initial(&g);
	env->ReleaseIntArrayElements(pixels, cPixels, 0);
	return true;
}
JNIEXPORT jboolean JNICALL Java_pris_videotest_JNIClient_detectWithShift(
		JNIEnv * env, jclass, jintArray pixels, jint width, jint height) {
	jint * cPixels;
	cPixels = env->GetIntArrayElements(pixels, 0);

	cv::Mat imgData(height, width, CV_8UC4, (unsigned char*) cPixels);
	int result[width * height];
	////////每一帧的输入图像为imgData，返回01////////

	cv::cvtColor(imgData, gray, CV_RGB2GRAY);
//	cv::Mat gray32;
	gray.convertTo(gray, CV_32FC1);
	CvMat g = gray;
	env->ReleaseIntArrayElements(pixels, cPixels, 0);
	return FrameProcessing(&g);
	////////这里写内容////////
}
