#pragma once
#include "afxwin.h"
#include "vidPlayer.h"
#include "LibMySql.h"


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
};
