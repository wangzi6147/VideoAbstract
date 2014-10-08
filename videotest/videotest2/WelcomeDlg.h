#pragma once

// WelcomeDlg 对话框

class WelcomeDlg : public CDialogEx
{
	DECLARE_DYNAMIC(WelcomeDlg)

public:
	WelcomeDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~WelcomeDlg();

// 对话框数据
	enum { IDD = IDD_WELCOME };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	HICON m_hIcon;
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
};
