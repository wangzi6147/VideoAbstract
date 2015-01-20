#include "pris_videotest_JNIClient.h"
#include <opencv2/opencv.hpp>
#include "vibe.h"

ViBe_BGS Vibe_Bgs;
cv::Mat gray;

int m_nVideoResizeW = 40;
int m_nVideoResizeH = 30;

JNIEXPORT jboolean JNICALL Java_pris_videotest_JNIClient_initVIBE(JNIEnv * env,
		jclass, jintArray pixels, jint width, jint height) {
	///这里是初始化的步骤
	jint * cPixels;
	cPixels = env->GetIntArrayElements(pixels, 0);
	cv::Mat imgData(height, width, CV_8UC4, (unsigned char*) cPixels);
	env->ReleaseIntArrayElements(pixels, cPixels, 0);
	cv::cvtColor(imgData, gray, CV_RGB2GRAY);
	Vibe_Bgs.init(gray);
	Vibe_Bgs.processFirstFrame(gray);
	return true;
}
JNIEXPORT jintArray JNICALL Java_pris_videotest_JNIClient_detectWithVIBE(
		JNIEnv * env, jclass, jintArray pixels, jint width, jint height) {
	jint * cPixels;
	cPixels = env->GetIntArrayElements(pixels, 0);

	cv::Mat imgData(height, width, CV_8UC4, (unsigned char*) cPixels);
	int result[width * height];
	////////每一帧的输入图像为imgData，得到的检测结果存在result里////////

	cv::cvtColor(imgData, gray, CV_RGB2GRAY);
	Vibe_Bgs.testAndUpdate(gray);
	cv::Mat mask = Vibe_Bgs.getMask();
	resize(mask, mask, cv::Size(m_nVideoResizeW, m_nVideoResizeH), 0, 0, CV_INTER_LINEAR);
	for (int i = 0; i < m_nVideoResizeW * m_nVideoResizeH; i++) {
		result[i] = mask.data[i];
	}
	////////这里写VIBE的内容////////
	env->ReleaseIntArrayElements(pixels, cPixels, 0);
	jintArray intArray = env->NewIntArray(m_nVideoResizeW * m_nVideoResizeH);
	env->SetIntArrayRegion(intArray, 0, m_nVideoResizeW * m_nVideoResizeH, result);
	return intArray;
}
