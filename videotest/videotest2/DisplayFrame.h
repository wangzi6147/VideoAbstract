/**********************************************************************************
					Copyright:      BUPT PRIS LAB
					File name:      DisplayFrame.h
					Author:         Kang Jian
					Date:           2012-3-15

					Description:   	��ҪΪ��DisplayFrame��Ķ��塣
									DisplayFrame����Ҫ�ṩ����MFC��
									OpenCV��ͼ����ʾ���ܡ�
***********************************************************************************/

#pragma once
#include "publicheader.h"


/*********************************************
ClassName:   DisplayFrame
Author:      Kang Jian
Date:        2012-3-15
Description: ��Ҫ���MFC�����OpenCV�ṩͼ����ʾ
			 ���ܡ�
**********************************************/
class DisplayFrame
{
public:
	DisplayFrame(void);
	~DisplayFrame(void);

public:
	void SetOpenCVWindow(CWnd *pWnd,CString csWndName,         //����OpenCV���ڵ�λ�ã�
						 int nTarHeight,int nTarWidth);        //��OpenCV����Ƕ��MFC��
	void ShowPicture(CString csWndName,IplImage *pImg);        //����ͼƬ��ʾ����
};