/*
 * DetectWithDiff.cpp
 *
 *  Created on: 2014-12-22
 *      Author: wangzi6147
 */

#include <pris_videotest_JNIClient.h>

int CheckChange(unsigned char * img, int iNx, int iNy) {
//	enum eChk { eDX=8, eDY=8, eMM=3, eDV=256};
//	enum eChk { eDX=4, eDY=4, eMM=2, eDV=64};
	enum eChk {
		eMM = 3, eDX = (1 << eMM), eDY = (1 << eMM), eDV = 256
	};
	static int aY[1 << 15][2] = { 0 }, iIni = 1, iArr = 0; //(1<<15)=32768   640x480/4/4=307200/4/4=19200
	int iM = 0, iY, iX, iI, iJ, iK, iXet = 0, iV0, iV1, iV2, iVv = 0;
	unsigned char *p0 = img, *p1 = p0, *p2, *p3;

	for (iY = 0; iY < iNy; iY += eDY, p0 += iNx << eMM, p1 = p0)
		for (iX = 0; iX < iNx; iX += eDX, p1 += eDX, iM++) {
			p2 = p1;
			for (iJ = 0, iV1 = 0; iJ < eDY; iJ++, p2 += iNx)
				for (iI = 0, p3 = p2; iI < eDX; iI++)
					iV1 += *p3++;
			aY[iM][iArr] = iV1;
			iV0 = aY[iM][!iArr];
			if (iIni)
				continue;
			iV2 = iV1 > iV0 ? iV1 - iV0 : iV0 - iV1;
			iVv += eDV < iV2;
			iV1 = eDV < iV2 ? 255 : 0; //iV2<<6;
			if (255 < iV1)
				iV1 = 255;
			//*msk++ = iV1; //16<iV2;
		}
	if (iIni)
		iIni = 0;
	iArr = !iArr;
	iXet = 16 < iVv;
	return (iVv);
}

JNIEXPORT jboolean JNICALL Java_pris_videotest_JNIClient_detectWithDiff(
		JNIEnv * env, jclass, jbyteArray pixels, jint width, jint height) {
	jbyte * cPixels;
	cPixels = env->GetByteArrayElements(pixels, 0);
	int change = CheckChange((unsigned char*)cPixels, width, height);
	env->ReleaseByteArrayElements(pixels, cPixels, 0);
	return change>width*height*0.0002;
}

