
// videotest2Dlg.cpp : 实现文件
//

#include "stdafx.h"
#include "videotest2.h"
#include "videotest2Dlg.h"
#include "afxdialogex.h"
#include "afxdlgs.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#define new new( _CLIENT_BLOCK, __FILE__, __LINE__)


CDataMySql *m_MysqlHandle;         ///<数据库外部指针变量
pointForCvMouse mousePosInPic;
//bool m_gotCVlclick = FALSE;
// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现

};
/////////////////////////////////////////////////////////2014.6.16
CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}




// Cvideotest2Dlg 对话框


Cvideotest2Dlg::Cvideotest2Dlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(Cvideotest2Dlg::IDD, pParent)
{
	m_ifStartPro = false;
	m_videoPro = new CVideo;
	m_MysqlVideoParaSearchHandle = new CDataMySql;
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	
}

Cvideotest2Dlg::~Cvideotest2Dlg()
	
{
	  delete m_MysqlVideoParaSearchHandle;			   ///<删除数据库操作指针
	  delete m_videoPro;
}

void Cvideotest2Dlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SLIDER_PLAYER1,m_CSliderPlayer1Ctrl);
	DDX_Control(pDX, IDC_SLIDER_PLAYER2,m_CSliderPlayer2Ctrl);
	DDX_Control(pDX, IDC_EDIT_FRAME_NO, m_wndEditFrameNo1);	
	DDX_Control(pDX, IDC_EDIT_INPUT_FRAME_NO, m_wndEditInputFrameNo1);
	//DDX_Control(pDX, IDC_EDIT_INPUT_nPara_NO, m_wndEditInput_nPara);
	
}

BEGIN_MESSAGE_MAP(Cvideotest2Dlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_TIMER()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(ID_OPEN, &Cvideotest2Dlg::OnBnClickedOpen)

	ON_BN_CLICKED(IDC_BTN_GENERATE_ABS, &Cvideotest2Dlg::OnBnClickedBtnGenerateAbs)

	
	ON_BN_CLICKED(IDC_BTN_exit, &Cvideotest2Dlg::OnBnClickedBtnexit)
	ON_BN_CLICKED(IDC_BTN_VIEW_ABS, &Cvideotest2Dlg::OnBnClickedBtnViewAbs)
	ON_BN_CLICKED(IDC_BTN1_play, &Cvideotest2Dlg::OnBnClickedBtn1play)
	ON_BN_CLICKED(IDC_BTN1_pause, &Cvideotest2Dlg::OnBnClickedBtn1pause)
	ON_BN_CLICKED(IDC_BTN1_stop, &Cvideotest2Dlg::OnBnClickedBtn1stop)
	ON_BN_CLICKED(IDC_BTN2_play, &Cvideotest2Dlg::OnBnClickedBtn2play)
	ON_BN_CLICKED(IDC_BTN2_pause, &Cvideotest2Dlg::OnBnClickedBtn2pause)
	ON_BN_CLICKED(IDC_BTN2_stop, &Cvideotest2Dlg::OnBnClickedBtn2stop)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_PROGRESS1, &Cvideotest2Dlg::OnNMCustomdrawProgress1)
	ON_BN_CLICKED(IDC_BTN_GOTO_FRAME, &Cvideotest2Dlg::OnBnClickedBtnGotoFrame)
	//ON_BN_CLICKED(IDC_BTN_GOTO_PARA, &Cvideotest2Dlg::OnBnClickedBtnGotoPara)
	ON_BN_CLICKED(IDC_CHECK1, &Cvideotest2Dlg::OnBnClickedCheck1)
END_MESSAGE_MAP()


// Cvideotest2Dlg 消息处理程序

BOOL Cvideotest2Dlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	
	m_nInputFrameNo1 = 0;
	player1.m_filePath = "";
	player2.m_filePath = "";
	player1.playInitial(GetDlgItem(IDC_STATIC_ABS), "displayWindow1");//该初始化需要在文件路径确认后完成
	player1.m_currentFrameNO = player1.m_startFrameNO = 0;
	player1.m_endFrameNO=3000;

	m_CSliderPlayer1Ctrl.SetRange(0, player1.m_endFrameNO);
	m_CSliderPlayer1Ctrl.SetPos(0);

	player2.playInitial(GetDlgItem(IDC_STATIC_ABS), "displayWindow2");//该初始化需要在文件路径确认后完成
	player2.m_currentFrameNO = player2.m_startFrameNO = 0;		
	player2.m_endFrameNO=3000;

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码

	///进度条初始化
	myPro = (CProgressCtrl*)GetDlgItem(IDC_PROGRESS1);
	myPro->SetRange(0,500);
	myPro->SetPos(0);

	m_CSliderPlayer1Ctrl.SetRange(0, player1.m_endFrameNO);   ///<初始化窗口一的滑动条
	m_CSliderPlayer1Ctrl.SetPos(0);

	m_CSliderPlayer2Ctrl.SetRange(0, player2.m_endFrameNO);   ///<初始化窗口二的滑动条
	m_CSliderPlayer2Ctrl.SetPos(0);

	SetTimer(1, 100, NULL);///<计数器初始化
	SetTimer(2, 200, NULL);
	SetTimer(3, 500, NULL);
	SetTimer(4, 100, NULL);

	If_playpiece=FALSE;
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}
//void Cvideotest2Dlg::InitialPictureControl()
//{
//
//}

void Cvideotest2Dlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void Cvideotest2Dlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
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
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR Cvideotest2Dlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void Cvideotest2Dlg::OnBnClickedOpen()
{
	// TODO: 在此添加控件通知处理程序代码
	
	//文件类型过滤*/
	CFileDialog dlg(TRUE, NULL, NULL,OFN_HIDEREADONLY,_T("Files (*.mp3 *.wma *.dat *.wmv *.avi *.mov *.mmm *.mid *.mpeg)|*.mp3;*.wma;*.dat;*.wmv;*.avi;*.mov;*.mmm;*.mid;*.mpeg|All Files (*.*)|*.*||"),NULL);
	if(dlg.DoModal()==IDOK)
	{
		if (player1.m_currentFrameNO!=0)
		{
			player1.stopPlay();
		}
		PathName=dlg.GetPathName();
		SetDlgItemText(IDC_EDIT3, PathName);
		PathName.MakeUpper();

		player1.m_filePath = PathName;
	    player1.playInitial(GetDlgItem(IDC_STATIC2), "displayWindow1");//该初始化需要在文件路径确认后完成
	
		m_CSliderPlayer1Ctrl.SetRange(0, player1.m_endFrameNO);
		m_CSliderPlayer1Ctrl.SetPos(0);
		
		player1.play();

		CString m_pFilePath=PathName;
		CString m_tmpFileName1, m_tmpFileName2;
		GetVideoNameFromAbsolutePath1(&m_pFilePath,&m_tmpFileName1);///<获取文件名(包含后缀)
		GetFileNameBeforeDot(&m_tmpFileName1,&m_tmpFileName2);        ///<获取文件名(不含后缀)
		m_videoPro->m_tableParams.VideoFGTableName.Format("%s_VideoFGTable",m_tmpFileName2);
		m_videoPro->m_tableParams.FGTraceTableName.Format("%s_FGTraceTable",m_tmpFileName2);
		m_videoPro->m_tableParams.NewToOldFrameTableName.Format("%s_New2OldFrameTable",m_tmpFileName2);
		m_videoPro->m_tableParams.NewTraceTable.Format("%s_NewTraceTable",m_tmpFileName2);
		m_videoPro->m_tableParams.CombineSegsTableName.Format("%s_CombineSegsTable", m_tmpFileName2);

	}
	
}




void Cvideotest2Dlg::OnBnClickedBtnGenerateAbs()
{
	CString a = "\\";
	LPCTSTR m = a;
	CString b = "/";
	LPCTSTR n = b;
	if (m_ifStartPro)
	{
		MessageBox("正在生成摘要，请等待");
		return;
	}
	if (PathName != "")
	{
		m_videoPro->m_IfContinue = true;
		m_ifStartPro = true;
	
		PathName.Replace(*m, *n);
		m_videoPro->DoProcessing((LPSTR)(LPCTSTR)PathName);
	}
}



///@brief 计时器函数，用于刷新进度条和时间显示
/// 
///@param[in] UINT nIDEvent 计时器序号
///@pre  NULL
///@return NULL
///@retval BOOL
///@post NULL

void Cvideotest2Dlg::OnTimer(UINT nIDEvent)
{
	switch (nIDEvent)
	{
		case 1:///<播放窗口1的进度条及时间显示刷新
	    {
			if (If_playpiece)
			{
				CSliderCtrl * SliderCtrl=((CSliderCtrl*)GetDlgItem(IDC_SLIDER_PLAYER1));
				SliderCtrl->SetPos(player1.m_currentFrameNO);
			}
			else
			{
				CSliderCtrl * SliderCtrl=((CSliderCtrl*)GetDlgItem(IDC_SLIDER_PLAYER1));
				int CurrentPos=SliderCtrl->GetPos();                            ///<获取当前进度条位置
				int NewCurrentFrameNO = (double)CurrentPos*double(player1.m_endFrameNO-player1.m_startFrameNO)
										/(double)SliderCtrl->GetRangeMax();     ///<根据当前进度条位置，重新计算帧号

				if (player1.m_playState!=PLAY_STATE_STOP && player1.m_playState!=PLAY_STATE_FAST_FORWORD &&player1.m_playState!=PLAY_STATE_PLAY_PIECE)///<若播放器处于正常播放或暂停状态
				{
					if (abs(NewCurrentFrameNO-player1.m_currentFrameNO)>5)
					{
						player1.m_currentFrameNO=NewCurrentFrameNO;             ///<根据新计算的帧号，重置播放器
					}
				}
				SliderCtrl->SetPos(player1.m_currentFrameNO);                   ///<根据新计算的帧号，重置滑动条
				CWnd *pWndTimeDis = GetDlgItem(IDC_STATIC_TIME_VID1);
				if (player1.m_playState != PLAY_STATE_STOP)                     ///<非停止状态下，用方式一显示时间
				{
					ShowTime(player1.m_currentFrameNO, player1.m_endFrameNO - player1.m_startFrameNO,
						     player1.m_videoTimeInSecond, pWndTimeDis, 1);
					ShowCurrentFrameNO(player1.m_currentFrameNO, &m_wndEditFrameNo1);
				}
				else                                                            ///<停止状态下，用方式二显示时间
				{
					SliderCtrl->SetPos(0);
					ShowTime(player1.m_currentFrameNO, player1.m_endFrameNO - player1.m_startFrameNO,
					         player1.m_videoTimeInSecond, pWndTimeDis, 0);
					ShowCurrentFrameNO(player1.m_currentFrameNO, &m_wndEditFrameNo1);
				}
			}
		}
		case 2:///<刷进度条
		{
			if (!m_ifStartPro)
			{
				myPro->SetPos(0);
				SetDlgItemText(IDC_STATIC, "");//helloworld
			}
			else if (m_videoPro->m_IfDoneFus)///<如果已经生成摘要则将ClistCtrl中响应的行显示为“已生成摘要”
			{
				m_ifStartPro = false;
				MessageBox("已生成摘要");
			}
			///如果正在生成还未生成完毕,刷新进度条上方的显示处理阶段
			else if (m_videoPro->m_IfContinue)
			{
				int VideoPos = (int)m_videoPro->ProcessingSpeed();
				if (VideoPos == 0)
				{
					SetDlgItemText(IDC_STATIC,  "视频预处理……");
				}
				if (VideoPos == 1)
				{
					SetDlgItemText(IDC_STATIC,  "生成切分视频……");
				}
				if (VideoPos == 2)
				{
					SetDlgItemText(IDC_STATIC, "视频背景处理……");
				}
				if (VideoPos == 3)
				{
					SetDlgItemText(IDC_STATIC, "视频前景处理……");
				}
				if (VideoPos == 4)
				{
					SetDlgItemText(IDC_STATIC, "视频融合……");
				}
				myPro->SetPos((int)(m_videoPro->ProcessingSpeed()*100.0));
			}
			else
			{
				if ((int)m_videoPro->ProcessingSpeed() == -1)
				{
					m_ifStartPro = false;
					MessageBox( "视频时长过短，无需融合处理");
				
				}
			}
		}
		case 3:
		{
				CSliderCtrl * SliderCtrl=((CSliderCtrl*)GetDlgItem(IDC_SLIDER_PLAYER2));
				int CurrentPos=SliderCtrl->GetPos();                            ///<获取当前进度条位置
				int NewCurrentFrameNO = (double)CurrentPos*double(player2.m_endFrameNO-player2.m_startFrameNO)
					/(double)SliderCtrl->GetRangeMax();     ///<根据当前进度条位置，重新计算帧号
				if (player2.m_playState!=PLAY_STATE_STOP && player2.m_playState!=PLAY_STATE_FAST_FORWORD)///<若播放器处于正常播放或暂停状态
				{
					if (abs(NewCurrentFrameNO-player2.m_currentFrameNO)>5)
					{
						player2.m_currentFrameNO=NewCurrentFrameNO;             ///<根据新计算的帧号，重置播放器
					}
				}
				SliderCtrl->SetPos(player2.m_currentFrameNO);                   ///<根据新计算的帧号，重置滑动条
				CWnd *pWndTimeDis = GetDlgItem(IDC_STATIC_TIME_VID2);
				if (player2.m_playState != PLAY_STATE_STOP)                     ///<非停止状态下，用方式一显示时间
				{
					ShowTime(player2.m_currentFrameNO, player2.m_endFrameNO -player2.m_startFrameNO,
						player2.m_videoTimeInSecond, pWndTimeDis, 1);
				}
				else                                                            ///<停止状态下，用方式二显示时间
				{
					SliderCtrl->SetPos(0);
					ShowTime(player2.m_currentFrameNO,player2.m_endFrameNO - player2.m_startFrameNO,
						player2.m_videoTimeInSecond, pWndTimeDis, 0);

				}
		}
		case 4://摘要窗口的左键单击
		{
			if (player2.m_gotCVlclick == TRUE)  //&& Abstractplayer.m_playState == PLAY_STATE_PAUSE
			{
				m_clickedObjRecPosInVec = -1;
				CPoint posInWnd = player2.m_lBtnUpPosInCVWnd;///<播放器对象传出的点击坐标，此时的点击位置已经是原始帧尺寸的坐标
				double clickPosXInImg = double(posInWnd.x);
				double clickPosYInImg = double(posInWnd.y);///<注意此时的点击位置已经是原始帧尺寸的坐标
				m_distanceToRecCenter = 1000000;

				m_objectRectVector.clear();
				///<从数据库中读取ROI信息
				if(!m_MysqlVideoParaSearchHandle->FindROIFromNewTraceTable(player2.m_currentFrameNO,&m_objectRectVector,m_videoPro->m_tableParams.NewTraceTable))//////"00015_h_NewTraceTable"
				{
					AfxMessageBox("数据库出错");
					return;
				}

				for (int i=0; i<m_objectRectVector.size(); i++)///<遍历所有ROI目标框
				{
					if (clickPosXInImg < m_objectRectVector[i].x+m_objectRectVector[i].width && clickPosXInImg > m_objectRectVector[i].x///<判断点击是否在当前框内
						&& clickPosYInImg < m_objectRectVector[i].y+m_objectRectVector[i].height && clickPosYInImg > m_objectRectVector[i].y)
					{
						int tempDistance = (clickPosXInImg - (m_objectRectVector[i].x+m_objectRectVector[i].width/2))*
							(clickPosXInImg - (m_objectRectVector[i].x+m_objectRectVector[i].width/2))+
							(clickPosYInImg - (m_objectRectVector[i].y+m_objectRectVector[i].height/2))*
							(clickPosYInImg - (m_objectRectVector[i].y+m_objectRectVector[i].height/2));///<计算点击位置与当前ROI中心的距离

						if (tempDistance < m_distanceToRecCenter)
						{
							m_distanceToRecCenter = tempDistance;///<当前距离小于记录的最小距离，则更新这个最小距离
							m_clickedObjRecPosInVec = i;         ///<并记录当前ROI在向量中的下标值
						}
					}
				}
				if(m_clickedObjRecPosInVec==-1)
				{
					return;
				}
				/*if (m_theObjRecClicked != m_objectRectVector[m_clickedObjRecPosInVec])
				{*/
					m_theObjRecClicked = m_objectRectVector[m_clickedObjRecPosInVec];
					int m_theorigFrame=0;
					if (m_videoPro!=NULL)
					{
						int m_origPara = m_MysqlVideoParaSearchHandle->FindOrigParaFromFGTraceTable(m_videoPro->m_tableParams.FGTraceTableName,m_theObjRecClicked);
						m_theorigFrame = m_MysqlVideoParaSearchHandle->FindOrigFrameFromVideoFGTable(m_videoPro->m_tableParams.VideoFGTableName,m_origPara);//m_videoPro->m_tableParams.NewTraceTable
					
						//m_theorigFrame = m_MysqlVideoParaSearchHandle->FindOrigFrameFromNewTraceTable(m_videoPro->m_tableParams.NewTraceTable,m_theObjRecClicked);//m_videoPro->m_tableParams.NewTraceTable
					}
					
					if (m_theorigFrame == -1)  
					{
						MessageBox("数据库有误");
						player2.m_gotCVlclick = FALSE;     
						return;
					};
					//else
					//{
					//	Originalplayer.m_startFrameNO = MAX(0,m_theorigFrame-500);///<未能得到帧号，则默认播放前500帧和后1000帧
					//	Originalplayer.m_endFrameNO   = Originalplayer.m_startFrameNO + 1000;
					//}
		
					//player1.m_filePath = absOriNameTansf(Abstractplayer.m_filePath);				
					player1.m_currentFrameNO=m_theorigFrame;                                 ///<初始化原始视频播放器
				//}
				If_playpiece=TRUE;
				player2.m_gotCVlclick = FALSE;                                       ///<点击比对完毕，将得到点击标志位置否
			}
		}
		case 5://摘要图片的左键单击
		{
				CString m_windowName = "displayWindow3"; //当前进程控制的CV窗口名
					
				CPoint m_clickPosInCVWnd;///<记录左键单击的点
				CPoint m_lBtnUpPosInCVWnd;///<记录左键弹起的点
				int para = 5;
				cvSetMouseCallback(m_windowName, cvMouseHandlerInPic, &para);

				m_clickPosInCVWnd.x = mousePosInPic.x;
				m_clickPosInCVWnd.y = mousePosInPic.y;
				m_lBtnUpPosInCVWnd.x = mousePosInPic.x1;
				m_lBtnUpPosInCVWnd.y = mousePosInPic.y1;
				//m_gotCVlclick = mousePosInPic.clickInCVwnd;//cv窗口得到点击参数便传出

				if (mousePosInPic.clickInCVwnd )
				{
				
					m_clickedObjRecPosInVec = -1;
					double clickPosXInImg = double(m_lBtnUpPosInCVWnd.x);
					double clickPosYInImg = double(m_lBtnUpPosInCVWnd.y);///<注意此时的点击位置已经是原始帧尺寸的坐标
					m_distanceToRecCenter = 1000000;

					m_objectRectVector.clear();
					///<从数据库中读取ROI信息
					if (!m_MysqlVideoParaSearchHandle->FindROIFromCombineSegsTable(&m_objectRectVector, m_videoPro->m_tableParams.CombineSegsTableName))//////"00015_h_NewTraceTable"
					{
						AfxMessageBox("数据库出错");
						return;
					}

					for (int i = 0; i < m_objectRectVector.size(); i++)///<遍历所有ROI目标框
					{
						if (clickPosXInImg < m_objectRectVector[i].x + m_objectRectVector[i].width && clickPosXInImg > m_objectRectVector[i].x///<判断点击是否在当前框内
							&& clickPosYInImg < m_objectRectVector[i].y + m_objectRectVector[i].height && clickPosYInImg > m_objectRectVector[i].y)
						{
							int tempDistance = (clickPosXInImg - (m_objectRectVector[i].x + m_objectRectVector[i].width / 2))*
								(clickPosXInImg - (m_objectRectVector[i].x + m_objectRectVector[i].width / 2)) +
								(clickPosYInImg - (m_objectRectVector[i].y + m_objectRectVector[i].height / 2))*
								(clickPosYInImg - (m_objectRectVector[i].y + m_objectRectVector[i].height / 2));///<计算点击位置与当前ROI中心的距离

							if (tempDistance < m_distanceToRecCenter)
							{
								m_distanceToRecCenter = tempDistance;///<当前距离小于记录的最小距离，则更新这个最小距离
								m_clickedObjRecPosInVec = i;         ///<并记录当前ROI在向量中的下标值
							}
						}
					}
					if (m_clickedObjRecPosInVec == -1)
					{
						return;
					}
					/*if (m_theObjRecClicked != m_objectRectVector[m_clickedObjRecPosInVec])
					{*/
					m_theObjRecClicked = m_objectRectVector[m_clickedObjRecPosInVec];
					int m_theorigFrame = 0;
					if (m_videoPro != NULL)
					{
						int m_origPara = m_MysqlVideoParaSearchHandle->FindOrigParaFromFGTraceTable(m_videoPro->m_tableParams.FGTraceTableName, m_theObjRecClicked);
						m_theorigFrame = m_MysqlVideoParaSearchHandle->FindOrigFrameFromVideoFGTable(m_videoPro->m_tableParams.VideoFGTableName, m_origPara);//m_videoPro->m_tableParams.NewTraceTable

						//m_theorigFrame = m_MysqlVideoParaSearchHandle->FindOrigFrameFromNewTraceTable(m_videoPro->m_tableParams.NewTraceTable,m_theObjRecClicked);//m_videoPro->m_tableParams.NewTraceTable
					}

					if (m_theorigFrame == -1)
					{
						MessageBox("数据库有误");
						mousePosInPic.clickInCVwnd = FALSE;
						return;
					};

					player1.m_currentFrameNO = m_theorigFrame;                                 ///<初始化原始视频播放器

					If_playpiece = TRUE;
					mousePosInPic.clickInCVwnd=FALSE;                                       ///<点击比对完毕，将得到点击标志位置否
				}
		}

		default:
			break;
	}
}

void Cvideotest2Dlg::OnAppExit()
{
	// same as double-clicking on main window close box
	//ASSERT(m_pMainWnd != NULL);
	SendMessage(WM_CLOSE);
	//_CrtDumpMemoryLeaks();
}


void Cvideotest2Dlg::OnBnClickedBtnexit()
{
	// TODO: 在此添加控件通知处理程序代码
	OnAppExit();
	
}


void Cvideotest2Dlg::OnBnClickedBtnViewAbs()
{
	// TODO: 在此添加控件通知处理程序代码
	//if(PathName)
	if (m_ifStartPro)
	{
		::MessageBox(NULL,_T("摘要尚未处理完成"),_T("error!"),MB_OK);
		return;
	}
	
	if (player1.m_currentFrameNO!=0)
		{
			player2.stopPlay();
		}
		CString path=PathName;
		path.Replace("avi","AVI");
		path = path+"_fusion.avi";
		player2.m_filePath = path;
		if(_access(path,0)==-1)
		{
			AfxMessageBox("找不到文件");
			return;
		}

		 // Read the file
		IplImage* image= NULL;
		image = cvLoadImage("F://video project//videotest//015_H//All.jpg",1);
		if(image != NULL ) // Check for invalid input
		{
			ShowImage(image,IDC_STATIC_ABS2);
			cvReleaseImage(&image);
		}

		SetTimer(4, 600, NULL); //新加载的Picture Control的计时器

		player2.playInitial(GetDlgItem(IDC_STATIC_ABS), "displayWindow2");//该初始化需要在文件路径确认后完成
		m_CSliderPlayer2Ctrl.SetRange(0, player2.m_endFrameNO);
		m_CSliderPlayer2Ctrl.SetPos(0);

		player2.play();
		player2.m_MysqlVideoParaSearchHandle=m_MysqlVideoParaSearchHandle;
		player2.m_videoPro=m_videoPro;
		player2.timeshow=FALSE;
}

void Cvideotest2Dlg::ShowImage( IplImage* img, UINT ID )    // ID 是Picture Control控件的ID号
{
	CWnd *m_pWnd = GetDlgItem(ID);
    CDC* pDC = GetDlgItem( ID ) ->GetDC();        // 获得显示控件的 DC
    HDC hDC = pDC ->GetSafeHdc();                // 获取 HDC(设备句柄) 来进行绘图操作
	DisplayFrame disPlayImage;//用于内嵌播放窗口的对象
	
    CRect rect;
    GetDlgItem(ID) ->GetClientRect( &rect );
    int rw = rect.right - rect.left;            // 求出图片控件的宽和高
    int rh = rect.bottom - rect.top;
    int iw = img->width;                        // 读取图片的宽和高
    int ih = img->height;
	
	rw = (int)(iw*rh/ih);		//应显示按比例的宽度
	int tx = rect.left;
	int ty = rect.top;
    SetRect( rect, tx, ty, tx+rw, ty+rh );

    //CvvImage cimg;
    //cimg.CopyOf( img );                            // 复制图片
    //cimg.DrawToHDC( hDC, &rect );                // 将图片绘制到显示控件的指定区域内
	disPlayImage.SetOpenCVWindow(m_pWnd, "displayWindow3",
		rect.Width(), rect.Height());
	disPlayImage.ShowPicture("displayWindow3", img);

    ReleaseDC( pDC );
}

void Cvideotest2Dlg::OnBnClickedBtn1play()
{
	// TODO: 在此添加控件通知处理程序代码
	if(player1.m_filePath!="")
	{
		m_CSliderPlayer1Ctrl.SetRange(0, player1.m_endFrameNO);///<滑动条初始化
		
		///<时间显示准备
		CString VideoTime;                                     
		int VedioHour=player1.m_videoTimeInSecond/3600;
		int VedioMinute=player1.m_videoTimeInSecond/60-VedioHour*60;
		int VedioSecond=player1.m_videoTimeInSecond-VedioHour*3600-VedioMinute*60;

		VideoTime.Format(" 0:0:0 / %d:%d:%d ",VedioHour,VedioMinute,VedioSecond);
		GetDlgItem(IDC_STATIC_TIME_VID1)->SetWindowText(VideoTime);///<将时间显示到控件上

		player1.play();
		
	}
	else
	{
		AfxMessageBox("请选中文件");
	}

}


void Cvideotest2Dlg::OnBnClickedBtn1pause()
{
	// TODO: 在此添加控件通知处理程序代码
	player1.pause();
	
}


void Cvideotest2Dlg::OnBnClickedBtn1stop()
{
	// TODO: 在此添加控件通知处理程序代码
	player1.stop();
	
}


void Cvideotest2Dlg::OnBnClickedBtn2play()
{
	// TODO: 在此添加控件通知处理程序代码
	if(player2.m_filePath!="")
	{
		m_CSliderPlayer2Ctrl.SetRange(0, player2.m_endFrameNO);///<滑动条初始化
		
		///<时间显示准备
		CString VideoTime;                                     
		int VedioHour=player2.m_videoTimeInSecond/3600;
		int VedioMinute=player2.m_videoTimeInSecond/60-VedioHour*60;
		int VedioSecond=player2.m_videoTimeInSecond-VedioHour*3600-VedioMinute*60;

		VideoTime.Format(" 0:0:0 / %d:%d:%d ",VedioHour,VedioMinute,VedioSecond);
		GetDlgItem(IDC_STATIC_TIME_VID2)->SetWindowText(VideoTime);///<将时间显示到控件上

		player2.play();
		
	}
	else
	{
		AfxMessageBox("请选中文件");
	}
}


void Cvideotest2Dlg::OnBnClickedBtn2pause()
{
	// TODO: 在此添加控件通知处理程序代码
	player2.pause();
	/*GetDlgItem(IDC_BUTTON2_pause)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUTTON2_play)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUTTON2_play)->ShowWindow(TRUE);*/
}


void Cvideotest2Dlg::OnBnClickedBtn2stop()
{
	// TODO: 在此添加控件通知处理程序代码
	player2.stop();
}


void Cvideotest2Dlg::OnNMCustomdrawProgress1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;
}

///@brief 播放器时间显示功能
/// 
///@param[in] int m_currentFrameNO--当前帧号, int totalFrameCount--总帧数, int videoTimeInSecond--视频时间长度（s）, CWnd *m_pShowTimeWnd--显示控件句柄, int flag--显示类型
///@pre  NULL
///@return NULL
///@retval BOOL
///@post NULL
void Cvideotest2Dlg::ShowTime(int m_currentFrameNO, int totalFrameCount, 
						   int videoTimeInSecond, CWnd *m_pShowTimeWnd, int flag)
{
	CString CurVideoTime;
	if (flag == 1)
	{	
		int CurrentTimeInSecond = (double)m_currentFrameNO/double(totalFrameCount)*videoTimeInSecond;
		int CurVedioHour=CurrentTimeInSecond/3600;
		int CurVedioMinute=CurrentTimeInSecond/60-CurVedioHour*60;
		int CurVedioSecond=CurrentTimeInSecond-CurVedioHour*3600-CurVedioMinute*60;

		int VedioHour= videoTimeInSecond/3600;
		int VedioMinute= videoTimeInSecond/60-VedioHour*60;
		int VedioSecond= videoTimeInSecond-VedioHour*3600-VedioMinute*60;

		CurVideoTime.Format(" %d:%d:%d / %d:%d:%d ",CurVedioHour,CurVedioMinute,CurVedioSecond,VedioHour,VedioMinute,VedioSecond);
		m_pShowTimeWnd->SetWindowText(CurVideoTime);
	}
	else if (flag == 0)
	{
		CurVideoTime.Format(" %d:%d:%d / %d:%d:%d ",0,0,0,0,0,0);
		m_pShowTimeWnd->SetWindowText(CurVideoTime);
	}

}

///@brief 显示CEdit* m_wndEditFrameNo对应播放器所显示的帧数，用来查看当前帧，或输入帧数转到指定帧
///@QiFeng 
void Cvideotest2Dlg::ShowCurrentFrameNO(int m_currentFrameNO, CEdit* m_wndEditFrameNo)
{
	CString _Str_FrameNo;
	_Str_FrameNo.Format("%d", m_currentFrameNO);
	m_wndEditFrameNo->SetWindowTextA(_T(_Str_FrameNo));
}

void Cvideotest2Dlg::OnBnClickedBtnGotoFrame()
{
	// TODO: 在此添加控件通知处理程序代码
	
	//输入框
	CString m_StrInputFrameNo;
	m_wndEditInputFrameNo1.GetWindowTextA(m_StrInputFrameNo);
	m_nInputFrameNo1 = _ttoi(m_StrInputFrameNo);
	//<先判断输入框是否改变了帧数  @QiFeng
	if (m_nInputFrameNo1 != player1.m_currentFrameNO)
	{
			CvCapture *capture = cvCreateFileCapture(PathName);///<获取视频头指针
			if (!capture)
			{///获取失败，输出提示语句
				AfxMessageBox("您要播放的文件已损坏或者已经从原路径移除！！！\n请重新选择文件。");
				//return 0;
			}
			cvSetCaptureProperty(capture, CV_CAP_PROP_POS_FRAMES, m_nInputFrameNo1);
			player1.m_currentFrameNO = m_nInputFrameNo1; 		
			
			
			IplImage *m_curFrame;//当前帧指针
			m_curFrame = cvQueryFrame(capture);///<获取当前帧图像
			if (!m_curFrame)
				return;
			player1.m_playState = PLAY_STATE_PAUSE;
			player1.disPlayImage.ShowPicture( player1.m_windowName,m_curFrame);
			
			
			player1.bEditIsChange = true;
	}
		
}


void Cvideotest2Dlg::OnBnClickedBtnGotoPara()
{
	// TODO: 在此添加控件通知处理程序代码
	//输入框
	CString m_StrInput_nPara;
	m_wndEditInput_nPara.GetWindowTextA(m_StrInput_nPara);
	int m_nPara = _ttoi(m_StrInput_nPara);

	///<数据库读出响应的帧号

	int m_ParaStartFrame = m_MysqlVideoParaSearchHandle->FindStartFrameFromVideoFGTable(m_nPara, m_videoPro->m_tableParams.VideoFGTableName);		
	int m_ParaEndFrame = m_MysqlVideoParaSearchHandle->FindEndFrameFromVideoFGTable(m_nPara, m_videoPro->m_tableParams.VideoFGTableName);		
		
	if (m_ParaStartFrame == -1)  
	{
			MessageBox("帧数读取有误！");
			return;
	}
	else
	{
			player1.m_startFrameNO = MAX(0,m_ParaStartFrame - 50);          ///<未能从数据库中读取帧号，则默认播放前500和后1000帧
			player1.m_endFrameNO   = m_ParaEndFrame + 300;		
	}					
			player1.playPiece();												///<更改原始播放器的播放状态
				 
			If_playpiece=TRUE;
			player1.m_gotCVlclick = FALSE; 

}




void Cvideotest2Dlg::OnBnClickedCheck1()
{
	// TODO: 在此添加控件通知处理程序代码
	if (((CButton *)GetDlgItem(IDC_CHECK1))->GetCheck())
	{
		player2.timeshow=TRUE;
		player2.d_AbsVideoTimeRatio = (player1.m_endFrameNO- player1.m_startFrameNO)/ player1.m_videoTimeInSecond;//totalFrameCount--总帧数*videoTimeInSecond--视频时间长度（s）
	}
	else
	{
		player2.timeshow=FALSE;
	}
	
}

void cvMouseHandlerInPic(int eventType, int x, int y, int flags, void *param)
{
	switch (eventType)
	{
	case CV_EVENT_LBUTTONDOWN:///左键按下     
	{
								  mousePosInPic.x = x;
								  mousePosInPic.y = y;
								  mousePosInPic.x1 = x;
								  mousePosInPic.y1 = y;
								  mousePosInPic.clickInCVwnd = TRUE;
								  mousePosInPic.lBtnUp = FALSE;
	}
		break;
	case CV_EVENT_LBUTTONUP:///左键弹起
	{
								if (mousePosInPic.clickInCVwnd == TRUE)
								{
									mousePosInPic.x1 = x;
									mousePosInPic.y1 = y;
									mousePosInPic.lBtnUp = TRUE;
									mousePosInPic.clickInCVwnd = FALSE;
								}
	}
		break;
	case CV_EVENT_MOUSEMOVE:///左键拖动
	{
								if (mousePosInPic.clickInCVwnd == TRUE && mousePosInPic.lBtnUp == FALSE)
								{
									mousePosInPic.x1 = x;
									mousePosInPic.y1 = y;
								}
	}
		break;
	}
}



