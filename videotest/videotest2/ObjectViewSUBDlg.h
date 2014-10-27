#pragma once
#include "afxwin.h"
#include "vidPlayer.h"
#include "LibMySql.h"
#include "ButtonBeautify.h"
#include "UIbeautify.h"


// ObjectViewSUBDlg 对话框


class ObjectViewSUBDlg : public CDialog
{
	DECLARE_DYNAMIC(ObjectViewSUBDlg)

public:
	ObjectViewSUBDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~ObjectViewSUBDlg();
	void ShowImage( IplImage* img, UINT ID,CString WName);    // ID 是Picture Control控件的ID号

// 对话框数据
	enum { IDD = IDD_OBJECT };

	afx_msg void OnBnClickedButtonObject();
	afx_msg void OnBnClickedObjectLeft();
	afx_msg void OnBnClickedObjectRight();
	afx_msg void OnStnClickedStaticObject1();
	afx_msg void OnStnClickedStaticObject2();
	afx_msg void OnStnClickedStaticObject3();
	afx_msg void OnStnClickedStaticObject4();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedButton3();
	afx_msg void OnClose();

	bool UserClick;///<用于判断用户有无按下鼠标，无操作时为FALSE，有操作时为TRUE
	bool Generated;///<用户是否点击了“生成”按钮
	bool user_click_scrollbar;
	BOOL  If_playpiece;//原始视频开始播放片段置标志位;
	int OriFrame;//用于传出到主对话框，让播放器跳到对应的帧
	int m_pageSum;							///<记录一页多少个缩略图
	int m_pageNum;							///<记录当前缩略图中显示的为第几页
	CStatic ObjectPicture_1;
	CDataMySql* m_MysqlVideoHandle;
	CVideo *videoPro;
	vidPlayer oriPlayer;
	vidPlayer ObjectPlayer;
	vector<objectInfo> objDetectedInfos;	///<存储数据库的物体信息，用来显示缩略图
	CScrollBar* pScrollBar;
	CSliderCtrl m_CSliderPlayerCtrl;
	HICON m_hIcon;
	POINT old;
	CRect m_rect;
	CRect rcTabRect;                               ///<子窗口坐标
	CBitmap *BitmapBackGroundStrech;              ///<拉伸后图片
	UIbeautify UIBeautifier;                      ///<用于美化界面的对象
	CButtonBeautify  BTN_OBJECT_ORI_PLAY;
	CButtonBeautify  BTN_OBJECT_ORI_PAUSE;
	CButtonBeautify  BTN_OBJECT_ORI_STOP;
	
	void DetectResult();
	void DisplayPageSumImg();
	virtual void OnCancel();
	virtual BOOL OnInitDialog();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
};
