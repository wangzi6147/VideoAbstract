#pragma once
#include "ButtonBeautify.h"
#include "UIbeautify.h"

// Dialog1 对话框

class Dialog1 : public CDialogEx
{
	DECLARE_DYNAMIC(Dialog1)

public:
	Dialog1(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~Dialog1();

// 对话框数据
	enum { IDD = IDD_DIALOG1 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

//实现
public:
	bool Tab1_ifStartPro;          ///<是否点击了生成摘要
	BOOL  Tab1_playpiece;//原始视频开始播放片段置标志位;
	//滑动条
	CSliderCtrl Tab1_CSliderPlayer1Ctrl;   ///<窗口一滑动条控件变量
	CSliderCtrl Tab1_CSliderPlayer2Ctrl;   ///<窗口二滑动条控件变量
	vidPlayer Tab1_Player1;
	vidPlayer Tab1_Player2;
	CDataMySql* Tab1_MysqlHandle;
	vector <CvRect> Tab1_objectRectVector;                  ///<用于存储融合视频中目标ROI的CvRect向量
	CvRect Tab1_theObjRecClicked;                           ///<记录被鼠标单击的目标ROI框
	int Tab1_clickedObjRecPosInVec;                        ///<记录被鼠标点击的目标ROI框在m_objectRectVector中的下标号
	int Tab1_distanceToRecCenter;                          ///<鼠标点击位置与ROI中心点的距离
	CString Tab1_PathName;	
	CVideo* Tab1_videoPro;
	CProgressCtrl* Tab1_Pro;
	HICON m_hIcon;
public:
	void ViewAbs();
	void ShowTime(int m_currentFrameNO,            ///<用于显示播放时间的函数
		int totalFrameCount,
		int videoTimeInSecond,
		CWnd *m_pShowTimeWnd, int flag);
	//afx_msg void OnBnClickedButton9();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedBtnTab1OriPause();
	afx_msg void OnBnClickedBtnTab1OriStop();
	afx_msg void OnBnClickedBtnTab1AbsStop();
	afx_msg void OnBnClickedBtnTab1AbsPlay();
	afx_msg void OnBnClickedBtnTab1AbsPause();
	afx_msg void OnBnClickedBtnTab1GenerateAbs();
	//afx_msg void OnBnClickedBtnTab1ViewAbs();
	afx_msg void OnBnClickedBtnTab1OriPlay();
	afx_msg void OnBnClickedCheckTab1();


	CRect m_rect;
	POINT old;  

	UIbeautify UIBeautifier;                      ///<用于美化界面的对象
	CButtonBeautify Btn_GENERATE_ABS;
	CButtonBeautify Btn_BTN1_PLAY;
	CButtonBeautify Btn_BTN1_PAUSE;
	CButtonBeautify Btn_BTN1_STOP;
	CButtonBeautify Btn_BTN2_PLAY;
	CButtonBeautify Btn_BTN2_PAUSE;
	CButtonBeautify Btn_BTN2_STOP;

	CBitmap *BitmapBackGroundStrech;              ///<拉伸后图片
	CRect rcTabRect;                                    ///<子窗口坐标
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
//	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	virtual void OnCancel();
};
