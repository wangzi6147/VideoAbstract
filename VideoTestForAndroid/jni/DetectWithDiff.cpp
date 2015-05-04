/*
 * DetectWithDiff.cpp
 *
 *  Created on: 2014-12-22
 *      Author: wangzi6147
 */

#include <pris_videotest_JNIClient.h>
#include <stdio.h>

bool cv_process_frame(unsigned char * pFrame, int width, int height) {
	FILE *pFl = fopen("/data/tmp/640x480.yuv", "ab");
	if (pFl){
		fwrite(pFrame, 1, 640 * 480 * 3 / 2, pFl), fclose(pFl);
		return true;
	}else {
		return false;
	}
}

/*
 * ����imgΪͼ��data
 * iNxΪ���
 * iNyΪ�߶�
 * ����ֵΪ�Ƿ��⵽Ŀ�꣬������1Ϊ��⵽Ŀ�ꡣ
 */
int CheckChange(unsigned char * img, int iNx, int iNy) {
//	enum eChk { eDX=8, eDY=8, eMM=3, eDV=256};
//	enum eChk { eDX=4, eDY=4, eMM=2, eDV=64};
	enum eChk {
		eMM = 3, eDX = (1 << eMM), eDY = (1 << eMM), eDV = 512
	};
	static int aY[1 << 15][2] = { 0 }, iIni = 1, iArr = 0,iMax=0;//iNx*iNy*0.0002; //(1<<15)=32768   640x480/4/4=307200/4/4=19200
	int iM = 0, iY, iX, iI, iJ, iK, iXet = 0, iV0, iV1, iV2, iVv = 0;
	unsigned char *p0 = img, *p1 = p0, *p2, *p3;
	if(0==iMax) iMax=iNx*iNy*0.0002;
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
	iXet = iMax < iVv;

	if(iXet){
		return 1;
	}else {
		return 0;
	}
}

JNIEXPORT jboolean JNICALL Java_pris_videotest_JNIClient_detectWithDiff(
		JNIEnv * env, jclass, jbyteArray pixels, jint width, jint height) {
	static int ifInit = 1;
	jbyte * cPixels;
	cPixels = env->GetByteArrayElements(pixels, 0);
//	if(ifInit){
//		ifInit = 0;
//		remove("/data/tmp/640x480.yuv");
//	}
	int ifChange = CheckChange((unsigned char*)cPixels, width, height);
//	if(ifChange){
//		cv_process_frame((unsigned char*)cPixels, width, height);
//	}
	env->ReleaseByteArrayElements(pixels, cPixels, 0);
	return ifChange;
}

