// ifRebuildDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "videotest2.h"
#include "ifRebuildDlg.h"
#include "afxdialogex.h"


// ifRebuildDlg 对话框

IMPLEMENT_DYNAMIC(ifRebuildDlg, CDialogEx)

ifRebuildDlg::ifRebuildDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(ifRebuildDlg::IDD, pParent)
{

}

ifRebuildDlg::~ifRebuildDlg()
{
}

void ifRebuildDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(ifRebuildDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &ifRebuildDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &ifRebuildDlg::OnBnClickedCancel)
END_MESSAGE_MAP()


// ifRebuildDlg 消息处理程序


void ifRebuildDlg::OnBnClickedOk()
{
	// TODO:  在此添加控件通知处理程序代码
	ifrebuild = true;
	CDialogEx::OnOK();
}


void ifRebuildDlg::OnBnClickedCancel()
{
	// TODO:  在此添加控件通知处理程序代码
	ifrebuild = false;
	CDialogEx::OnCancel();
}
