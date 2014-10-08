#pragma once
#include "afxwin.h"
#include "vidPlayer.h"
#include "LibMySql.h"
#include "ButtonBeautify.h"
#include "UIbeautify.h"
#include "VTrack.h"

// drawDetection 对话框

class drawDetection : public CDialogEx
{
	DECLARE_DYNAMIC(drawDetection)

public:
	drawDetection(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~drawDetection();

// 对话框数据
	enum { IDD = IDD_DRAW_DETECT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	CProgressCtrl* VTrackPro;
	DECLARE_MESSAGE_MAP()
private:
	vidPlayer player;    ///<待测视频播放类对象
	vidPlayer oriPlayer; ///<结果视频播放类对象
	VTrack    tracker;   ///<视频跟踪类对象
	bool    m_ifExistProData;               ///<数据表存在与否标志位
	int     numOfObj;                       ///<视频运动目标总数
	CString m_curCaseNameText;              ///<当前案件名称
	CString m_curCameraNameText;            ///<当前监控点名称
	CString m_curVideoPath;                 ///<当前视频路径
	CString objtablename;                   ///<当前数据表名
	CPoint tempDrewPoint1;                  ///<当前测试条件点1
	CPoint tempDrewPoint2;                  ///<当前测试条件点2
	vector<CString>    oriVideoName;        ///<当前视频名称集合
	vector<objectInfo> objDetectedInfos;	///<存储数据库中过线或过框的物体信息，用来显示缩略图
	int m_pageNum;							///<记录当前缩略图中显示的为第几页
	int m_pageSum;							///<记录一页多少个缩略图
	DisplayFrame  m_ShowResult;				///<显示图片的类
	void DetectResult();					///<每次得到m_pageSum张越线（框）的目标，存进objDetectedInfos中
	void DisplayPageSumImg();				///<显示缩略图

public:
	friend class Cvideotest2Dlg;    //友元类
	void ShowTime(int m_currentFrameNO,            ///<用于显示播放时间的函数
		int totalFrameCount,
		int videoTimeInSecond, 
		CWnd *m_pShowTimeWnd, int flag);

	CSliderCtrl m_CSlider_playerCtrl;   ///<原始播放滑动条控件变量
	CSliderCtrl m_CSlider_oriplayerCtrl;   ///<检测结果窗口二滑动条控件变量    //改动
	bool user_click_scrollbar;
	int Video_FrameCount;//记录视频的总帧数，在滑动条的部分运用
	CScrollBar* Draw_Detect_pScrollBar;

	CVideo* draw_videoPro;
	CDataMySql* m_MysqlHandle;

	//UIbeautify UIBeautifier;					///<用于美化界面的对象
	//CBitmap *BitmapBackGroundStrech;			///<拉伸后图片
	/////界面美化相关对象、变量声明
	//CButtonBeautify Btn_PLAY_IN_INTELLI_SEARCH;					///<播放
	//CButtonBeautify Btn_PAUSE_IN_INTELLI_SEARCH;				///<暂停
	//CButtonBeautify Btn_STOP_IN_INTELLI_SEARCH;					///<停止
	//CButtonBeautify Btn_FAST_FOWARD_IN_INTELLI_SEARCH;			///<快进
	//CButtonBeautify Btn_FAST_BACKWARD_IN_INTELLI_SEARCH;		///<快退
	//CButtonBeautify Btn_FRAME_FAST_FOWARD_IN_INTELLI_SEARCH;	///<逐帧快进     
	//CButtonBeautify Btn_FRAME_FAST_BACKWARD_IN_INTELLI_SEARCH;	///<逐帧快退 
	//CButtonBeautify Btn_CLOSE;									///<退出按钮
	//CButtonBeautify Btn_START_PROCESS_IN_DRAW_DETECT;			///<开始处理 
	//CButtonBeautify Btn_STOP_PROCESS;							///<终止处理
	//CButtonBeautify Btn_SCREENSHOT_IN_DRAW_DETECT;				///<视频截图
	//CButtonBeautify Btn_CLEAR_IN_DRAW_DETECT;					///<还原播放
	//CButtonBeautify Btn_SEARCH_IN_DRAW_DETECT;					///<开始检测
	//CButtonBeautify Btn_PLAY_IN_INTELLI_SEARCH2;				///<播放
	//CButtonBeautify Btn_PAUSE_IN_INTELLI_SEARCH2;				///<暂停
	//CButtonBeautify Btn_STOP_IN_INTELLI_SEARCH2;				///<停止
	//CButtonBeautify Btn_FAST_FOWARD_IN_INTELLI_SEARCH2;			///<快进
	//CButtonBeautify Btn_FAST_BACKWARD_IN_INTELLI_SEARCH2;		///<快退
	//CButtonBeautify Btn_FRAME_FAST_FOWARD_IN_INTELLI_SEARCH2;   ///<逐帧快进     
	//CButtonBeautify Btn_FRAME_FAST_BACKWARD_IN_INTELLI_SEARCH2; ///<逐帧快退 
	//CButtonBeautify Btn_SCREENSHOT_IN_DRAW_DETECT2;				///<视频截图
	//CButtonBeautify Btn_SAVEAS_CLUE_IN_DRAW_DETECT;				///<为该帧生成线索
	//CButtonBeautify BTN_PAGE_UP_IN_DRAW_DETECT;					///<上一页
	//CButtonBeautify BTN_PAGE_DOWN_IN_DRAW_DETECT;				///<下一页

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

	CBitmap *BitmapBackGroundStrech;

	UIbeautify UIBeautifier;
	CButtonBeautify Btn_BTN1_DRAW_PLAY;
	CButtonBeautify Btn_BTN1_DRAW_PAUSE;
	CButtonBeautify Btn_BTN1_DRAW_STOP;
	CButtonBeautify Btn_BTN2_DRAW_PLAY;
	CButtonBeautify Btn_BTN2_DRAW_PAUSE;
	CButtonBeautify Btn_BTN2_DRAW_STOP;
};
