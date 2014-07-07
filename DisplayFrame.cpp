/**********************************************************************************
					Copyright:      BUPT PRIS LAB
					File name:      DisplayFrame.cpp
					Author:         Kang Jian
					Date:           2012-3-16

					Description:   	主要为对DisplayFrame类的实现。
									DisplayFrame类主要提供基于MFC与
									OpenCV的图像显示功能。
***********************************************************************************/
#pragma  once
#include "stdafx.h"
#include "DisplayFrame.h"


/*********************************************
Function:      DisplayFrame    
Description:   DisplayFrame类的构造函数
Author:        Kang Jian
Date:          2012-3-19
Calls:         NULL 

Input:         NULL

Return:        NULL 

Others:        NULL
*********************************************/
DisplayFrame::DisplayFrame(void)
{

}

/*********************************************
Function:      ~DisplayFrame    
Description:   DisplayFrame类的析构函数
Author:        Kang Jian
Date:          2012-3-19
Calls:         NULL 

Input:         NULL

Return:        NULL 

Others:        NULL
*********************************************/
DisplayFrame::~DisplayFrame(void)
{

}

/*********************************************
Function:      ShowPicture    
Description:   使用OpenCV中的函数对单张图片进行显示
Author:        Kang Jian
Date:          2012-3-19
Calls:         NULL 

Input:         csWndName - 目标显示窗口的名称，需使用OpenCV的中创建时的名称
			   pImg - 待显示图片的指针

Return:        NULL

Others:        NULL
*********************************************/
void DisplayFrame::ShowPicture(CString csWndName,IplImage *pImg)
{
	char *pWndName = csWndName.GetBuffer();
	cvShowImage(pWndName, pImg);

}

/******************** *************************
Function:      SetOpenCVWindow    
Description:   将使用OpenCV生成的窗口嵌入MFC框架
Author:        Kang Jian
Date:          2012-3-20
Calls:         NULL 

Input:         pWnd - MFC中显示窗口的句柄 
			   csWndName - OpenCV中生成窗口的名称 用于获取句柄
			   nTarWidth - 目标窗口的宽度
			   nTarHeight - 目标窗口的长度

Return:        NULL

Others:        NULL
*********************************************/
void DisplayFrame::SetOpenCVWindow(CWnd *pWnd,CString csWndName,
								   int nTarWidth,int nTarHeight)
{
	char *pWndName = csWndName.GetBuffer();
	cvNamedWindow(pWndName, 0);
	HWND hWnd = (HWND)cvGetWindowHandle(pWndName);//显示控件
	HWND hParent = ::GetParent(hWnd);//父窗口
	::SetParent(hWnd,pWnd->m_hWnd);
	::ShowWindow(hParent,SW_HIDE);//隐藏显示窗口控件
	pWnd->SetWindowPos(NULL, 0, 0, nTarWidth, nTarHeight, 
						SWP_NOMOVE | SWP_NOZORDER);
	cvResizeWindow(pWndName, nTarWidth, nTarHeight);
	
}