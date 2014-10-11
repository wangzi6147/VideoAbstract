// ChangeIniFileDlg.cpp : implementation file
//

#include "stdafx.h"
#include "videotest2.h"
#include "ChangeIniFileDlg.h"
#include "afxdialogex.h"


// ChangeIniFileDlg dialog

IMPLEMENT_DYNAMIC(ChangeIniFileDlg, CDialogEx)

ChangeIniFileDlg::ChangeIniFileDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(ChangeIniFileDlg::IDD, pParent)
{

}

ChangeIniFileDlg::~ChangeIniFileDlg()
{
}

BOOL ChangeIniFileDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
		
	//get string from my.ini
	int m_CompressionRatio = ::GetPrivateProfileInt(TEXT("InitSampleSetting"), 
                              TEXT("m_nCompressionRatioH"), 
                              0,   //如果没读到会被设置成0
							  TEXT("..\\my.ini"));
	int m_MinTargetArea = ::GetPrivateProfileInt(TEXT("InitStorage"), 
                              TEXT("m_nMinTargetArea"), 
                              0,   //如果没读到会被设置成0
							  TEXT("..\\my.ini"));

	//show to dialog
	/*CString _Str_CompressionRatio, _Str_MinTargetArea;
	_Str_CompressionRatio.Format("%d", m_CompressionRatio);
	_Str_MinTargetArea.Format("%d", m_MinTargetArea);
	m_wndEditCompressionRatio.SetWindowTextA(_T(_Str_CompressionRatio));
	m_wndEditMinTargetArea.SetWindowTextA(_T(_Str_MinTargetArea));
*/
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}
void ChangeIniFileDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_COMPRESSIONRATIO, m_wndEditCompressionRatio);	
	DDX_Control(pDX, IDC_EDIT_MINTARGETAREA, m_wndEditMinTargetArea);
}


BEGIN_MESSAGE_MAP(ChangeIniFileDlg, CDialogEx)
	ON_BN_CLICKED(IDSAVE, &ChangeIniFileDlg::OnBnClickedSave)
END_MESSAGE_MAP()


// ChangeIniFileDlg message handlers


void ChangeIniFileDlg::OnBnClickedSave()
{
	// get TextA from edit in dialog
	//CString m_StrCompressionRatio, m_StrMinTargetArea;
	//m_wndEditCompressionRatio.GetWindowTextA(m_StrCompressionRatio);
	//m_wndEditMinTargetArea.GetWindowTextA(m_StrMinTargetArea);  
	//
	//if(_ttoi(m_StrCompressionRatio) != 0)
	//{
	//	//get int from my.ini
	//	int m_CompressionRatio = ::GetPrivateProfileInt(TEXT("InitSampleSetting"), 
 //                             TEXT("m_nCompressionRatioH"), 
 //                             0,   //如果没读到会被设置成0
	//						  TEXT("..\\my.ini"));
	//	if (_ttoi(m_StrCompressionRatio) != m_CompressionRatio) 
	//	{
	//		WritePrivateProfileString (TEXT("InitSampleSetting"),   
 //                             TEXT("m_nCompressionRatioH"), 
 //                              TEXT(m_StrCompressionRatio), 
	//							TEXT("..\\my.ini")); 
	//	}
	//}

	//if(_ttoi(m_StrMinTargetArea) != 0)
	//{
	//	//get int from my.ini
	//	int m_MinTargetArea = ::GetPrivateProfileInt(TEXT("InitStorage"), 
 //                             TEXT("m_nMinTargetArea"), 
 //                             0,   //如果没读到会被设置成0
	//						  TEXT("..\\my.ini"));
	//
	//	if  (_ttoi(m_StrMinTargetArea) != m_StrMinTargetArea)  
	//	{
	//		WritePrivateProfileString (TEXT("InitStorage"),   
 //                             TEXT("m_nMinTargetArea"), 
 //                              TEXT(m_StrMinTargetArea), 
	//							TEXT("..\\my.ini"));
	//	}
	//}

	CDialogEx::OnOK();
}
