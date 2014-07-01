
// videotest2Dlg.h : 头文件
/* 去除CWMPControls控件有关头文件
@version 版本号(0.0)
*@data 2014-6-4
*/

#pragma once
#include "publicheader.h"
#include "CvvImage.h"
#include "afxwin.h"  
#include "afxcmn.h"
#include "VideoSeg.h"
#include "Resource.h"
#include "vidPlayer.h"


// Cvideotest2Dlg 对话框
class Cvideotest2Dlg : public CDialogEx
{
// 构造
public:
	Cvideotest2Dlg(CWnd* pParent = NULL);	// 标准构造函数
	~Cvideotest2Dlg();
	CDataMySql* m_MysqlVideoParaSearchHandle;

// 对话框数据
	enum { IDD = IDD_VIDEOTEST2_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
private:
	CString PathName;
	CVideo* m_videoPro;
	CProgressCtrl* myPro;
public:
	bool m_ifStartPro;          ///<是否点击了生成摘要
	vidPlayer player1;
	vidPlayer player2;
	BOOL  If_playpiece;//原始视频开始播放片段置标志位;
	
	afx_msg void OnBnClickedOpen();
	afx_msg void OnBnClickedBtnGenerateAbs();

	
	//void DrawPicToHDC(IplImage *img, UINT ID);
	//void ShowImage(IplImage* pSrc, UINT ID);
	afx_msg void OnBnClickedButton1();
	afx_msg void OnEnChangeEdit3();
	void OnAppExit();

	afx_msg void OnBnClickedBtnexit();
	afx_msg void OnBnClickedBtnViewAbs();
	afx_msg void OnBnClickedBtn1play();
	afx_msg void OnBnClickedBtn1pause();
	afx_msg void OnBnClickedBtn1stop();
	afx_msg void OnBnClickedBtn2play();
	afx_msg void OnBnClickedBtn2pause();
	afx_msg void OnBnClickedBtn2stop();
	afx_msg void OnNMCustomdrawProgress1(NMHDR *pNMHDR, LRESULT *pResult);

//时间显示
public:
	afx_msg void OnTimer(UINT nIDEvent);           ///<计时器用于播放器进度条以及时间显示刷新
	void ShowTime(int m_currentFrameNO,            ///<用于显示播放时间的函数
				  int totalFrameCount,
				  int videoTimeInSecond, 
				  CWnd *m_pShowTimeWnd, int flag);
	void ShowCurrentFrameNO(int m_currentFrameNO, CEdit* m_wndEditFrameNo1);

public:
	///2个播放窗口的滑动条控制变量声明
	CSliderCtrl m_CSliderPlayer1Ctrl;   ///<窗口一滑动条控件变量
	CSliderCtrl m_CSliderPlayer2Ctrl;   ///<窗口二滑动条控件变量

	
protected:
	CEdit		m_wndEditFrameNo1;
	CEdit		m_wndEditInputFrameNo1;
	CEdit		m_wndEditInput_nPara;
	int			m_nInputFrameNo1;
public:
	afx_msg void OnBnClickedBtnGotoFrame();
	afx_msg void OnBnClickedBtnGotoPara();
	vector <CvRect> m_objectRectVector;                  ///<用于存储融合视频中目标ROI的rect向量
	CvRect m_theObjRecClicked;                           ///<记录被鼠标单击的目标ROI框
	int m_clickedObjRecPosInVec;                        ///<记录被鼠标点击的目标ROI框在m_objectRectVector中的下标号
	int m_distanceToRecCenter;                          ///<鼠标点击位置与ROI中心点的距离
	int  nHitItem;                                      ///<当前选中的行的索引
	int preHitItem;                                     ///<上一次选中的行的索引
	afx_msg void OnBnClickedCheck1();
};
