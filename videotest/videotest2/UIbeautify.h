/**
*@file UIbeautify.h
*@brief 主要为使用界面美化功能的调用功能类的声明。
*
*此文件定义的UIbeautify类，其功能上主要完成两方面内容：背景和按钮的美化。
*为了界面的效果可以适应桌面的大小，需要用到图片的拉伸，此处用到了opencv的图片拉伸操作。
*
*@author 作者 lu peng&&chen dai wu ,包含email等
*@version 版本号(0.0)
*@data 2012-8-21
*/
#pragma  once
#include "afxwin.h"
#include "ButtonBeautify.h"                                ///@brief 按钮美化扩展类                                 
#include "publicheader.h"                                  ///@brief 包括opencv等lib的加载

///@brief 定义的界面美化类
///
///UIbeautify，此类功能上主要完成两方面内容：背景和按钮的美化。
///为了界面的效果可以适应桌面的大小，需要用到图片的拉伸，此处用到了opencv的图片拉伸操作。
///
///@invariant rcDeskRect，取桌面大小后，值不变
///

class UIbeautify
{
public:
	UIbeautify();
	virtual ~UIbeautify();

	///有关美化背景的操作。
	void LoadBackgroundPic(int n_IDC);                    ///<加载背景图片
	void PaintBackground(HWND phwnd);                     ///<画背景界面的一些设置操作

    ///两个实现按钮的美化以及定位的函数。
    ///第二个LoadButtonBitmaps函数多了两个参数,来先设置图片大小,以实现按钮图片重复利用
	void LoadButtonBitmaps(CButtonBeautify &BtName, int n_IDB1, int n_IDB2, double LeftRatio, double TopRatio, double RightRatio, double BottomRatio); ///<实现按钮的美化以及定位
	void LoadButtonBitmaps(CButtonBeautify &BtName, int n_IDB1, int n_IDB2, double LeftRatio, double TopRatio, double RightRatio, double BottomRatio, int width, int height);///<实现按钮的美化以及定位
	void FillBitmapInfo(LPBITMAPINFO bmi, int width, int height, int bpp, int origin); ///<实现新建Bitmap图片时填充Bitmap信息

	///有关图片改变大小的操作。
	///CBitmapResize调用了CBitmapToIplImage和IplImageToCBitmap函数，从CBitmap转成IplImage，
	///然后利用opencv的cvResize改变大小,再保存成CBitmap格式，保存时调用了FillBitmapInfo函数。
	CBitmap  *CBitmapResize(CBitmap& Bmp,int width,int height);///<实现CBitmap类型图片大小变换
	CBitmap  *IplImageToCBitmap(const IplImage *pImage);       ///<实现CBitmap转IplImage
	CBitmap m_BitmapBackGround;           ///<保存背景图片
	IplImage *CBitmapToIplImage(CBitmap& Bmp);                 ///<实现CBitmap转IplImage
	CRect rcDeskRect;                     ///<保存桌面坐标
	LPBITMAPINFO bmi;                     ///<位图信息头         
	BOOL OnEraseBkgnd(HWND phwnd, CDC* pDC, CBitmap& Bmp);///<画背景函数
	
};