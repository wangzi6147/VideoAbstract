#pragma once
#include "afxwin.h"
#include "vidPlayer.h"
#include "LibMySql.h"
#include "ButtonBeautify.h"
#include "UIbeautify.h"


// CObjectViewDlg 对话框





class CObjectViewDlg : public CDialog
{
	DECLARE_DYNAMIC(CObjectViewDlg)

public:
	CObjectViewDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CObjectViewDlg();
	void ShowImage( IplImage* img, UINT ID,CString WName);    // ID 是Picture Control控件的ID号

// 对话框数据
	enum { IDD = IDD_OBJECT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:

	afx_msg void OnBnClickedButtonObject();
	afx_msg void OnBnClickedObjectLeft();
	afx_msg void OnBnClickedObjectRight();
	afx_msg void OnStnClickedStaticObject1();
	afx_msg void OnStnClickedStaticObject2();
	afx_msg void OnStnClickedStaticObject3();
	afx_msg void OnStnClickedStaticObject4();
	
	CStatic ObjectPicture_1;
	int m_pageSum;							///<记录一页多少个缩略图
	int m_pageNum;							///<记录当前缩略图中显示的为第几页
	CDataMySql* m_MysqlVideoHandle;
	CVideo *videoPro;
	vidPlayer oriPlayer;
	void DisplayPageSumImg();
	vector<objectInfo> objDetectedInfos;	///<存储数据库的物体信息，用来显示缩略图
	void DetectResult();
	bool UserClick;///<用于判断用户有无按下鼠标，无操作时为FALSE，有操作时为TRUE
	bool Generated;///<用户是否点击了“生成”按钮
	int OriFrame;//用于传出到主对话框，让播放器跳到对应的帧
	afx_msg void OnTimer(UINT_PTR nIDEvent);
//	afx_msg void OnInitMenu(CMenu* pMenu);
//	afx_msg void OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu);
//	afx_msg void OnUniChar(UINT Char, UINT nRepCnt, UINT nFlags);
	virtual BOOL OnInitDialog();

	CScrollBar* pScrollBar;
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	vidPlayer ObjectPlayer;
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedButton3();
	CSliderCtrl m_CSliderPlayerCtrl; 
	bool user_click_scrollbar;
	BOOL  If_playpiece;//原始视频开始播放片段置标志位;
	void ShowTime(int m_currentFrameNO,            ///<用于显示播放时间的函数
		int totalFrameCount,
		int videoTimeInSecond, 
		CWnd *m_pShowTimeWnd, int flag);
//	afx_msg void OnClose();
	afx_msg void OnClose();
//	afx_msg void OnCancelMode();
	virtual void OnCancel();
//	afx_msg void OnPaint();
	HICON m_hIcon;

	CRect m_rect;
	//afx_msg void OnSize(UINT nType, int cx, int cy);
	POINT old;  

	UIbeautify UIBeautifier;                      ///<用于美化界面的对象
	CButtonBeautify  BTN_OBJECT_ORI_PLAY;
	CButtonBeautify  BTN_OBJECT_ORI_PAUSE;
	CButtonBeautify  BTN_OBJECT_ORI_STOP;

	CBitmap *BitmapBackGroundStrech;              ///<拉伸后图片
	CRect rcTabRect;                                    ///<子窗口坐标
};
