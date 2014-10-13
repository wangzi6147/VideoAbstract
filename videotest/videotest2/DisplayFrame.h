/**********************************************************************************
					Copyright:      BUPT PRIS LAB
					File name:      DisplayFrame.h
					Author:         Kang Jian
					Date:           2012-3-15

					Description:   	主要为对DisplayFrame类的定义。
									DisplayFrame类主要提供基于MFC与
									OpenCV的图像显示功能。
***********************************************************************************/

#pragma once
#include "publicheader.h"
#include <afxctl.h>						///<使用CPictureHolder类所要包含的头文件

/*********************************************
ClassName:   DisplayFrame
Author:      Kang Jian
Date:        2012-3-15
Description: 主要针对MFC框架与OpenCV提供图像显示
			 功能。
**********************************************/
class DisplayFrame
{
public:
	LPBITMAPINFO lpbmih;
	CPictureHolder ShowImg;									///<CPictureHolder对象
	DisplayFrame(void);
	~DisplayFrame(void);
public:
	void SetOpenCVWindow(CWnd *pWnd, CString csWndName,
		int nTarLeft, int nTarTop,         //设置OpenCV窗口的位置，
		int nTarHeight, int nTarWidth);        //将OpenCV窗口嵌入MFC内
	void ShowPicture(CString csWndName,IplImage *pImg);        //单张图片显示函数
	void ShowPictureHolderBMP(IplImage *image, HDC hDC,CRect rec, CClientDC &dc);		   ///<利用CPictureHolder类来显示图片
};