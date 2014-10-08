// WelcomeDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "videotest2.h"
#include "WelcomeDlg.h"
#include "afxdialogex.h"
#include <Windows.h>


// WelcomeDlg 对话框

IMPLEMENT_DYNAMIC(WelcomeDlg, CDialogEx)

	WelcomeDlg::WelcomeDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(WelcomeDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

WelcomeDlg::~WelcomeDlg()
{ 
}

void WelcomeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(WelcomeDlg, CDialogEx)
	ON_WM_TIMER()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
END_MESSAGE_MAP()


// WelcomeDlg 消息处理程序


void WelcomeDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	CDialogEx::OnTimer(nIDEvent);
	switch(nIDEvent)  
	{  
	case 1: //如果id为1的timer结束，则执行关闭窗口操作  
		SendMessage(WM_CLOSE);  
		break;  
	default:  
		break;  
	} 
}


BOOL WelcomeDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标
	//SetBackgroundImage(IDB_WELCOME);
	SetTimer(1, 3000, NULL);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}


void WelcomeDlg::OnPaint()
{
	//UIBeautifier.PaintBackground(this->GetSafeHwnd());
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		//使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CPaintDC   dc(this);   
		CRect   rect;   
		GetClientRect(&rect);   
		CDC   dcMem;   
		dcMem.CreateCompatibleDC(&dc);   
		CBitmap   bmpBackground;   
		bmpBackground.LoadBitmap(IDB_WELCOME);   
		//IDB_BITMAP是你自己的图对应的ID   
		BITMAP   bitmap;   
		bmpBackground.GetBitmap(&bitmap);   
		CBitmap   *pbmpOld=dcMem.SelectObject(&bmpBackground);   
		dc.StretchBlt(0,0,rect.Width(),rect.Height(),&dcMem,0,0,bitmap.bmWidth,bitmap.bmHeight,SRCCOPY);  
		CDialogEx::OnPaint();

	}
	
}


HCURSOR WelcomeDlg::OnQueryDragIcon()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	return static_cast<HCURSOR>(m_hIcon);
}
