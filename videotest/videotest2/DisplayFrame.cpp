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
	lpbmih = new BITMAPINFO;
	lpbmih->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);	
	lpbmih->bmiHeader.biWidth = 0;
	lpbmih->bmiHeader.biHeight = 0;
	lpbmih->bmiHeader.biPlanes = 1;
	lpbmih->bmiHeader.biBitCount = 24;
	lpbmih->bmiHeader.biCompression = BI_RGB;
	lpbmih->bmiHeader.biSizeImage = 0;
	lpbmih->bmiHeader.biXPelsPerMeter = 0;
	lpbmih->bmiHeader.biYPelsPerMeter = 0;
	lpbmih->bmiHeader.biClrUsed = 0;
	lpbmih->bmiHeader.biClrImportant = 0;
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

void DisplayFrame::ShowPictureHolderBMP(IplImage *image, HDC hDC, CRect rec, CClientDC &dc)
{	
	lpbmih->bmiHeader.biWidth = image->width;				//初始化位图信息头的图片高和宽
	lpbmih->bmiHeader.biHeight = -image->height;			//负值表示位图正序存放(默认是倒序存放的)

	BYTE *pBits;											//位图数据指针
	HBITMAP hBitmap = CreateDIBSection(hDC, lpbmih, DIB_RGB_COLORS, (void**)&pBits, NULL, 0);		//创建位图句柄
	memcpy(pBits, image->imageData, image->imageSize);		//将IplImage图片数据拷贝到bmp数据区域    
	ShowImg.CreateFromBitmap(hBitmap, NULL, FALSE);			//将位图句柄关联到CPictureHolder对象 	
	ShowImg.Render((CDC*)&dc, &rec, 0);						//在picture控件所在区域(rec)显示图片
	if(!DeleteObject(hBitmap))
		AfxMessageBox("删除位图，释放资源失败！");
}
