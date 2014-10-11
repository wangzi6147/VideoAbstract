#pragma once


// ChangeIniFileDlg dialog

class ChangeIniFileDlg : public CDialogEx
{
	DECLARE_DYNAMIC(ChangeIniFileDlg)

public:
	ChangeIniFileDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~ChangeIniFileDlg();
	afx_msg void OnBnClickedSave();

// Dialog Data
	enum { IDD = IDD_CHANGEINIFILE };

protected:
	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	CEdit		m_wndEditCompressionRatio;
	CEdit		m_wndEditMinTargetArea;

	DECLARE_MESSAGE_MAP()
};
