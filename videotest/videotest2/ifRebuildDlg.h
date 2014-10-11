#pragma once


// ifRebuildDlg 对话框

class ifRebuildDlg : public CDialogEx
{
	DECLARE_DYNAMIC(ifRebuildDlg)

public:
	boolean ifrebuild;	//是否重新生成
	ifRebuildDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~ifRebuildDlg();

// 对话框数据
	enum { IDD = IDD_IFREBUILD };
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
};
