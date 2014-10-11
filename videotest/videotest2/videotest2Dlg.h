
// videotest2Dlg.h : 头文件
/* 去除CWMPControls控件有关头文件
@version 版本号(0.0)
*@data 2014-10-9
*/

#pragma once
#include "publicheader.h"
#include "CvvImage.h"
#include "afxwin.h"  
#include "afxcmn.h"
#include "VideoSeg.h"
#include "Resource.h"
#include "vidPlayer.h"
#include "ObjectViewSUBDlg.h"
#include "LibMySql.h"
#include "VideoAbsSUBDlg.h"
#include "PictureAbsSUBDlg.h"
#include "ButtonBeautify.h"
#include "UIbeautify.h"
#include "WelcomeDlg.h"
#include "DrawDetectionSUBDlg.h"


// Cvideotest2Dlg 对话框
class Cvideotest2Dlg : public CDialogEx
{
public:
	Cvideotest2Dlg(CWnd* pParent = NULL);	// 标准构造函数
	~Cvideotest2Dlg();
	CDataMySql* m_MysqlVideoParaSearchHandle;
	int m_CurSelTab; 
    VideoAbsSUBDlg m_page1;
	PictureAbsSUBDlg m_page2;
    ObjectViewSUBDlg m_page3;
	DrawDetectionSUBDlg m_page4;
    CDialog* pDialog[4];

// 对话框数据
	enum { IDD = IDD_VIDEOTEST2_DIALOG };

// 实现
protected:
	HICON m_hIcon;
	CEdit		m_wndEditFrameNo1;
	CEdit		m_wndEditInputFrameNo1;
	CEdit		m_wndEditInput_nPara;
	int			m_nInputFrameNo1;
	int         SubMenuCheckedNO;
	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

public:
	bool m_ifStartPro;          ///<是否点击了生成摘要
	bool Generated;///<用户是否点击了“生成”按钮
	TableParams m_tableParams;
	vector<objectInfo> objDetectedInfos;	///<存储数据库的物体信息，用来显示缩略图
	BOOL  If_playpiece;//原始视频开始播放片段置标志位;
	CString PathName;	
	CVideo* m_videoPro;
	CProgressCtrl* myPro;
	vidPlayer player1;
	vidPlayer player2;
	vector <CvRect> m_objectRectVector;                  ///<用于存储融合视频中目标ROI的CvRect向量
	vector <OrigTraceTable> m_objectOrigTraceTableVector;        ///<用于存储融合视频中目标ROI的OrigTraceTable向量
	CvRect m_theObjRecClicked;                           ///<记录被鼠标单击的目标ROI框
	int m_clickedObjRecPosInVec;                        ///<记录被鼠标点击的目标ROI框在m_objectRectVector中的下标号
	int m_distanceToRecCenter;                          ///<鼠标点击位置与ROI中心点的距离
	int  nHitItem;                                      ///<当前选中的行的索引
	int preHitItem;                                     ///<上一次选中的行的索引
	int m_pageSum;							///<记录一页多少个缩略图
	int m_pageNum;							///<记录当前缩略图中显示的为第几页
	int OriFrame;                           ///all图片点击返回原视频
	bool UserClick1;///<用于判断用户有无按下鼠标，无操作时为FALSE，有操作时为TRUE
	bool Generated1;///<用户是否点击了“生成”按钮
	afx_msg void OnBnClickedButtonObject();//逐一显示前景对象按钮
	afx_msg void OnAbout();
	afx_msg void OnClickObjectViewDlg();
	afx_msg void OnMenuClickedOpen();

	//afx_msg void OnBnClickedBtnGotoFrame();
	//afx_msg void OnBnClickedBtnGotoPara();
	afx_msg void OnTcnSelchangeTab1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedBtnOpen();
	afx_msg void OnBnClickedBtnView();
	afx_msg void OnBnClickedButtonSettings();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnBnClickedBtnexit();
	afx_msg void OnTimer(UINT nIDEvent);           ///<计时器用于播放器进度条以及时间显示刷新
	CTabCtrl m_tab;
    CRect m_rect;
	CRect rcTabRect;                               ///<子窗口坐标
	POINT old;
	UIbeautify UIBeautifier;                      ///<用于美化界面的对象
	CButtonBeautify Btn_BTN1_OPEN;
	CButtonBeautify Btn_BTN1_VIEW;
	CButtonBeautify Btn_BTN1_ABOUT;
	CButtonBeautify Btn_BTN1_EXIT;
	CButtonBeautify Btn_BTN1_SAVE;
	CButtonBeautify Btn_BTN1_TITLE;
	CBitmap *BitmapBackGroundStrech;              ///<拉伸后图片
	CSliderCtrl m_CSliderPlayer1Ctrl;   ///<窗口一滑动条控件变量
	CSliderCtrl m_CSliderPlayer2Ctrl;   ///<窗口二滑动条控件变量
	ObjectViewSUBDlg ObjectDlg;//逐一显示前景对象的窗口
	WelcomeDlg WelDlg;
	void DetectResultnew();
	void DisplayPageSumImgnew();
	void ShowImage1(IplImage* pSrc, UINT ID);
	void OnAppExit();
	void ShowTime(int m_currentFrameNO,            ///<用于显示播放时间的函数
		int totalFrameCount,
		int videoTimeInSecond,
		CWnd *m_pShowTimeWnd, int flag);
	void ShowCurrentFrameNO(int m_currentFrameNO, CEdit* m_wndEditFrameNo1);

};

void cvMouseHandlerInPic1(int eventType, int x, int y, int flags, void *param);
void cvMouseHandlerInPic2(int eventType, int x, int y, int flags, void *param);
