#pragma once
#include "afxwin.h"
#include "vidPlayer.h"
#include "LibMySql.h"
#include "ButtonBeautify.h"
#include "UIbeautify.h"
#include "VTrack.h"

// DrawDetectionSUBDlg 对话框

class DrawDetectionSUBDlg : public CDialogEx
{
	DECLARE_DYNAMIC(DrawDetectionSUBDlg)

public:
	DrawDetectionSUBDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~DrawDetectionSUBDlg();

// 对话框数据
	enum { IDD = IDD_DRAW_DETECT };

	friend class Cvideotest2Dlg;    //友元类

	CSliderCtrl m_CSlider_playerCtrl;   ///<原始播放滑动条控件变量
	CSliderCtrl m_CSlider_oriplayerCtrl;   ///<检测结果窗口二滑动条控件变量    //改动
	CScrollBar* Draw_Detect_pScrollBar;
	CVideo* draw_videoPro;
	CDataMySql* m_MysqlHandle;
	CBitmap *BitmapBackGroundStrech;
	UIbeautify UIBeautifier;
	CButtonBeautify Btn_BTN1_DRAW_PLAY;
	CButtonBeautify Btn_BTN1_DRAW_PAUSE;
	CButtonBeautify Btn_BTN1_DRAW_STOP;
	CButtonBeautify Btn_BTN2_DRAW_PLAY;
	CButtonBeautify Btn_BTN2_DRAW_PAUSE;
	CButtonBeautify Btn_BTN2_DRAW_STOP;

	bool user_click_scrollbar;
	int Video_FrameCount;//记录视频的总帧数，在滑动条的部分运用
	virtual BOOL OnInitDialog();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnBnClickedBtnPlayInDrawDetect();
	afx_msg void OnBnClickedBtnPauseInDrawDetect();
	afx_msg void OnBnClickedBtnStopInDrawDetect();
	afx_msg void OnBnClickedBtnStartProcess();
	afx_msg void OnBnClickedBtnStopProcess();
	afx_msg void OnBnClickedCheckLineInDrawDetect();
	afx_msg void OnBnClickedCheckRectInDrawDetect();
	afx_msg void OnBnClickedBtnClearInDrawDetect();
	afx_msg void OnBnClickedBtnSearchInDrawDetect();
	afx_msg void OnBnClickedBtnPlayInDrawDetect2();
	afx_msg void OnBnClickedBtnPauseInDrawDetect2();
	afx_msg void OnBnClickedBtnStopInDrawDetect2();
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	CProgressCtrl* VTrackPro;
	DECLARE_MESSAGE_MAP()

private:
	vidPlayer player;    ///<待测视频播放类对象
	vidPlayer oriPlayer; ///<结果视频播放类对象
	VTrack    tracker;   ///<视频跟踪类对象
	CString m_curCaseNameText;              ///<当前案件名称
	CString m_curCameraNameText;            ///<当前监控点名称
	CString m_curVideoPath;                 ///<当前视频路径
	CString objtablename;                   ///<当前数据表名
	CPoint tempDrewPoint1;                  ///<当前测试条件点1
	CPoint tempDrewPoint2;                  ///<当前测试条件点2
	vector<CString>    oriVideoName;        ///<当前视频名称集合
	vector<objectInfo> objDetectedInfos;	///<存储数据库中过线或过框的物体信息，用来显示缩略图
	DisplayFrame  m_ShowResult;				///<显示图片的类
	bool    m_ifExistProData;               ///<数据表存在与否标志位
	int     numOfObj;                       ///<视频运动目标总数
	int m_pageNum;							///<记录当前缩略图中显示的为第几页
	int m_pageSum;							///<记录一页多少个缩略图

	void DetectResult();					///<每次得到m_pageSum张越线（框）的目标，存进objDetectedInfos中
	void DisplayPageSumImg();				///<显示缩略图
};
