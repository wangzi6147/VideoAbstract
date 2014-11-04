#include <pris_videotest_JNIClient.h>
#include <hello_android.h>

JNIEXPORT jint JNICALL Java_pris_videotest_JNIClient_helloAndroid(JNIEnv * env,
		jclass, jintArray pixels, jint width, jint height) {
	int * a = env->GetIntArrayElements(pixels, false);
	int i = hello_android(a, width, height);
	return i;
}

