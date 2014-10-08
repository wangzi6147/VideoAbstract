/**
*@file UIbeautify.cpp
*@brief 主要为使用界面美化功能的调用功能类的定义。
*
*此文件定义的UIbeautify类，其功能上主要完成两方面内容：背景和按钮的美化。
*为了界面的效果可以适应桌面的大小，需要用到图片的拉伸，此处用到了opencv的图片拉伸操作。
*
*@author 作者 lu peng&&chen dai wu ,包含email等
*@version 版本号(0.0)
*@data 2012-8-21
*/

#include "stdafx.h"
#include "UIbeautify.h"


///@brief UIbeautify类的构造函数
///
///@param[in|out] NULL 
///@pre  NULL
///@return NULL
///@retval NULL
///@post NULL

UIbeautify::UIbeautify()
{
     bmi =new BITMAPINFO;
}

///@brief UIbeautify类的析构函数
///
///@param[in|out] NULL 
///@pre  NULL
///@return NULL
///@retval NULL
///@post NULL

UIbeautify::~UIbeautify()
{
	delete bmi;
}

///@brief 加载背景图片
///
///@param[in] n_IDC 图片的ID
///@pre  NULL
///@return NULL
///@retval NULL
///@post NULL
void UIbeautify::LoadBackgroundPic(int n_IDC)
{
	
	m_BitmapBackGround.LoadBitmap(n_IDC);///<加载
	BITMAP bm;
	m_BitmapBackGround.GetObject(sizeof(BITMAP), &bm); ///<获取图像属性信息

}
///@brief 画背景函数
///
///@param[in]  phwnd,桌面句柄；pDC,设备上下文对象的类；BitmapBackGroundStrech，伸缩后的背景图
///@pre  NULL
///@return BOOL
///@retval NULL
///@post TRUE

BOOL UIbeautify::OnEraseBkgnd(HWND phwnd, CDC* pDC, CBitmap& BitmapBackGroundStrech)
{
	
	CRect rcClient;
	GetClientRect(phwnd, &rcClient); 

	CDC dcMem;
	dcMem.CreateCompatibleDC(pDC);

	dcMem.SelectObject(BitmapBackGroundStrech);///<为设备环境选择一个对象，取背景图片

	pDC->BitBlt(0,0,rcClient.Width(),rcClient.Height(),&dcMem,0,0,SRCCOPY);///<获取窗口客户区的坐标（整个桌面）   

	dcMem.DeleteDC();
	return TRUE;

}

///@brief 画背景界面的一些设置操作
///
///@param[in]  phwnd,桌面句柄
///@pre  NULL
///@return NULL
///@retval NULL
///@post NULL
void UIbeautify::PaintBackground(HWND phwnd)
{

	CRect rect;
	CWnd* pWnd = CWnd::GetDesktopWindow();            ///<获取窗口客户区的坐标（整个桌面）   显示区域设为整个桌面
	pWnd->GetClientRect(rect);
	MoveWindow(phwnd, rect.left, rect.top, rect.Width(), rect.Height(), TRUE);

	CRgn m_rgn;                                      ///<获取窗口客户区的坐标（整个桌面）   画圆角矩形
	GetClientRect(phwnd, &rect);
	m_rgn.CreateRoundRectRgn(rect.left,rect.top,rect.right+1,rect.bottom+1,5,5); 
	SetWindowRgn(phwnd, m_rgn,TRUE);

}

///@brief 实现按钮的美化以及定位
///
///@param[in]  BtName 需美化的Button按钮;n_IDB1,按钮加载的原图对应Resource号码;n_IDB2,按钮加载的第2张图对应Resource号码;LeftRatio,TopRatio,RightRatio,BottomRatio,按钮相对于界面的左上右下位置比例
///@pre  NULL
///@return NULL
///@retval NULL
///@post NULL
void UIbeautify::LoadButtonBitmaps(CButtonBeautify &BtName,
					               int n_IDB1,              int n_IDB2,
					               double LeftRatio,         double TopRatio,   double RightRatio,     double BottomRatio)
{

	CBitmap Bitmap1,Bitmap2;

	Bitmap1.LoadBitmap(n_IDB1);

	CBitmap* BitmapNew1=CBitmapResize(Bitmap1,rcDeskRect.Width()*(RightRatio-LeftRatio),
									rcDeskRect.Height()*(BottomRatio-TopRatio));

	Bitmap2.LoadBitmap(n_IDB2);
	CBitmap* BitmapNew2=CBitmapResize(Bitmap2,rcDeskRect.Width()*(RightRatio-LeftRatio),
									rcDeskRect.Height()*(BottomRatio-TopRatio));

	BtName.MoveWindow(CRect(rcDeskRect.Width()*LeftRatio,rcDeskRect.Height()*TopRatio,
									(int)(rcDeskRect.Width()*LeftRatio)+(int)(rcDeskRect.Width()*(RightRatio-LeftRatio)),(int)(rcDeskRect.Height()*TopRatio)+(int)(rcDeskRect.Height()*(BottomRatio-TopRatio)))); 
	BtName.SetBitmaps(*BitmapNew1,*BitmapNew2,*BitmapNew2);

	delete BitmapNew1;
	delete BitmapNew2;

}

///@brief 实现按钮的美化以及定位
///
///@param[in]  BtName 需美化的Button按钮;n_IDB1,按钮加载的原图对应Resource号码;n_IDB2,按钮加载的第2张图对应Resource号码;LeftRatio,TopRatio,RightRatio,BottomRatio,按钮相对于界面的左上右下位置比例
///@pre  NULL
///@return NULL
///@retval NULL
///@post NULL
void UIbeautify::LoadButtonBitmaps(CButtonBeautify &BtName,
								   int n_IDB1,              int n_IDB2,
								   double LeftRatio,         double TopRatio,   double RightRatio,     double BottomRatio,
								   int width,               int height)
{

	CBitmap Bitmap1,Bitmap2;

	Bitmap1.LoadBitmap(n_IDB1);

    CBitmap* Bitmapstrech1=CBitmapResize(Bitmap1,width,height);
	CBitmap* BitmapNew1=CBitmapResize(*Bitmapstrech1,rcDeskRect.Width()*(RightRatio-LeftRatio),
		rcDeskRect.Height()*(BottomRatio-TopRatio));

	Bitmap2.LoadBitmap(n_IDB2);
	CBitmap* Bitmapstrech2=CBitmapResize(Bitmap2,width,height);
	CBitmap* BitmapNew2=CBitmapResize(*Bitmapstrech2,rcDeskRect.Width()*(RightRatio-LeftRatio),
		rcDeskRect.Height()*(BottomRatio-TopRatio));


	BtName.MoveWindow(CRect(rcDeskRect.Width()*LeftRatio,rcDeskRect.Height()*TopRatio,
		(int)(rcDeskRect.Width()*LeftRatio)+(int)(rcDeskRect.Width()*(RightRatio-LeftRatio)),(int)(rcDeskRect.Height()*TopRatio)+(int)(rcDeskRect.Height()*(BottomRatio-TopRatio)))); 
	BtName.SetBitmaps(*BitmapNew1,*BitmapNew2,*BitmapNew2);

	delete Bitmapstrech1;
	delete Bitmapstrech2;
	delete BitmapNew1;
	delete BitmapNew2;

}

///@brief 实现CBitmap类型图片大小变换
///
///@param[in]  Bmp,原CBitmap类型图片，width,height,变后的长和高
///@pre  NULL
///@return CBitmap*
///@retval CBitmap*
///@post NULL
CBitmap* UIbeautify::CBitmapResize(CBitmap& Bmp,int width,int height)
{
	IplImage * IpmageResize=CBitmapToIplImage(Bmp);
	IplImage *dst = 0; ///<获取窗口客户区的坐标（整个桌面）   目标图像指针
	CvSize dst_cvsize; ///<获取窗口客户区的坐标（整个桌面）   目标图像尺寸

	dst_cvsize.width = width; ///<目标图像的宽为源图象宽的scale倍
	dst_cvsize.height = height; ///<目标图像的高为源图象高的scale倍

	dst = cvCreateImage( dst_cvsize, IpmageResize->depth, IpmageResize->nChannels); ///<构造目标图象
	cvResize(IpmageResize, dst, CV_INTER_LINEAR); ///<缩放源图像到目标图像
	CBitmap* BmpNew=IplImageToCBitmap(dst);

	cvReleaseImage(&IpmageResize);
	cvReleaseImage(&dst);

	return BmpNew;

}


///@brief 实现CBitmap转IplImage
///
///@param[in]  Bmp,原CBitmap类型图片
///@pre  NULL
///@return  IplImage *
///@retval IplImage *
///@post NULL
IplImage * UIbeautify::CBitmapToIplImage(CBitmap& Bmp)
{
	HBITMAP hBmp = HBITMAP(Bmp);///<将CBitmap转换为HBITMAP
	BITMAP bmp;
	Bmp.GetBitmap(&bmp);///<获得位图信息

	int depth,nChannels;
	if(bmp.bmBitsPixel == 1)///<得到图像深度和通道数
	{
		depth=IPL_DEPTH_1U;
		nChannels=1;
	}
	else
	{
		depth=IPL_DEPTH_8U;  
		nChannels=bmp.bmBitsPixel/8;
	}

	IplImage* pImg = cvCreateImage(cvSize(bmp.bmWidth,bmp.bmHeight), depth, nChannels); ///<创建图像      
	BYTE *pBuffer = new BYTE[bmp.bmHeight*bmp.bmWidth*nChannels]; ///<创建缓冲区   
	GetBitmapBits(hBmp, bmp.bmHeight*bmp.bmWidth*nChannels, pBuffer); ///<将位图信息复制到缓冲区
	memcpy(pImg->imageData, pBuffer, bmp.bmHeight*bmp.bmWidth*nChannels);///<将缓冲区信息复制给IplImage,于是成功从CBitmap得到了IplImage类型的图片pImg.

	delete [] pBuffer;
	return pImg;

}


///@brief 实现IplImage 转 CBitmap
///
///@param[in] *pImage,原IplImage *类型图片
///@pre  NULL
///@return  CBitmap * 
///@retval CBitmap * 
///@post NULL
CBitmap * UIbeautify:: IplImageToCBitmap(const IplImage *pImage)
{
	if( pImage && pImage->depth == IPL_DEPTH_8U )
	{
		CBitmap *pBitmap=new CBitmap;
		HDC hDC = GetDC(NULL);
		
		int bmp_w = pImage->width, bmp_h = pImage->height;
		FillBitmapInfo(bmi,bmp_w,bmp_h,pImage->depth*pImage->nChannels,pImage->origin);
	    char *pBits=NULL;
		HBITMAP hBitmap=::CreateDIBSection(hDC,bmi,DIB_RGB_COLORS,(void**)&pBits,NULL,0);
		memcpy(pBits,pImage->imageData,pImage->imageSize);
		
		pBitmap->Attach(hBitmap);

        ReleaseDC(NULL,hDC);
		return pBitmap;
	}
	else
		return NULL;
}

///@brief 实现新建Bitmap图片时填充Bitmap信息
///
///@param[in] BITMAPINFO* bmi图片信息, int width宽, int height高, int bpp图片编码bit, int origin原始初值
///@pre  NULL
///@return  NULL
///@retval NULL
///@post NULL
void UIbeautify::FillBitmapInfo(LPBITMAPINFO bmi, int width, int height, int bpp, int origin) 
{ 

	bmi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);	//初始化位图信息头
	bmi->bmiHeader.biWidth = width;
	bmi->bmiHeader.biHeight = origin ? abs(height) : -abs(height);
	bmi->bmiHeader.biPlanes = 1;
	bmi->bmiHeader.biBitCount = (unsigned short)bpp;
	bmi->bmiHeader.biCompression = BI_RGB;
	bmi->bmiHeader.biSizeImage = 0;
	bmi->bmiHeader.biXPelsPerMeter = 0;
	bmi->bmiHeader.biYPelsPerMeter = 0;
	bmi->bmiHeader.biClrUsed = 0;
	bmi->bmiHeader.biClrImportant = 0;

}

