#include "pris_videotest_JNIClient.h"
#include <opencv2/opencv.hpp>
#include <segmentation_1208_wf.h>

//cv::Mat gray;

bool cv_process_frame(unsigned char * pFrame, int width, int height) {
	FILE *pFl = fopen("/data/tmp/640x480.yuv", "ab");
	if (pFl){
		fwrite(pFrame, 1, 640 * 480 * 3 / 2, pFl), fclose(pFl);
		return true;
	}else {
		return false;
	}
}

JNIEXPORT jboolean JNICALL Java_pris_videotest_JNIClient_initWithByte(
		JNIEnv * env, jclass, jbyteArray pixels, jint width, jint height) {
	///这里是初始化的步骤
	jbyte * cPixels;
	cPixels = env->GetByteArrayElements(pixels, 0);
	remove("/data/tmp/640x480.yuv");
	cv::Mat imgData(height, width, CV_8UC1, (unsigned char*) cPixels);
	imgData.convertTo(imgData, CV_32FC1);
	CvMat g = imgData;
	Initial(&g);
	env->ReleaseByteArrayElements(pixels, cPixels, 0);
	return true;
}

JNIEXPORT jboolean JNICALL Java_pris_videotest_JNIClient_detectWithByte(
		JNIEnv * env, jclass, jbyteArray pixels, jint width, jint height) {
	jbyte * cPixels;
	cPixels = env->GetByteArrayElements(pixels, 0);

	cv::Mat imgData(height, width, CV_8UC1, (unsigned char*) cPixels);
	int result[width * height];
	////////每一帧的输入图像为imgData，返回01////////
	imgData.convertTo(imgData, CV_32FC1);
	CvMat g = imgData;
	env->ReleaseByteArrayElements(pixels, cPixels, 0);
	if (FrameProcessing(&g))
		//if(cv_process_frame((unsigned char*) cPixels, width, height))
			return true;

	return false;
}

