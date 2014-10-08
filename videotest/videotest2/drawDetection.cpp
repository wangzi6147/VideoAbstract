// drawDetection.cpp : 实现文件
//

#include "stdafx.h"
#include "videotest2.h"
#include "drawDetection.h"
#include "afxdialogex.h"

// drawDetection 对话框

extern pointForCvMouse mousePosInCVwmd;     ///<鼠标响应参量

IMPLEMENT_DYNAMIC(drawDetection, CDialogEx)

drawDetection::drawDetection(CWnd* pParent /*=NULL*/)
	: CDialogEx(drawDetection::IDD, pParent)
{
	m_ifExistProData = true;
	m_curVideoPath   = "";
	objtablename     = "";
	tracker.m_IfContinue = false;
	player.m_IfStartDetect = false;
	numOfObj = -1;
	m_MysqlHandle= new CDataMySql;
	draw_videoPro = new CVideo;


}

drawDetection::~drawDetection()
{
}

void drawDetection::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SLIDER_IN_DRAW_DETECT,m_CSlider_playerCtrl);
	DDX_Control(pDX, IDC_SLIDER_IN_DRAW_DETECT2,m_CSlider_oriplayerCtrl);

	DDX_Control(pDX, IDC_BTN_PLAY_IN_DRAW_DETECT, Btn_BTN1_DRAW_PLAY);
	DDX_Control(pDX, IDC_BTN_PAUSE_IN_DRAW_DETECT, Btn_BTN1_DRAW_PAUSE);
	DDX_Control(pDX, IDC_BTN_STOP_IN_DRAW_DETECT, Btn_BTN1_DRAW_STOP);
	DDX_Control(pDX, IDC_BTN_PLAY_IN_DRAW_DETECT2, Btn_BTN2_DRAW_PLAY);
	DDX_Control(pDX, IDC_BTN_PAUSE_IN_DRAW_DETECT2, Btn_BTN2_DRAW_PAUSE);
	DDX_Control(pDX, IDC_BTN_STOP_IN_DRAW_DETECT2, Btn_BTN2_DRAW_STOP);
}


BEGIN_MESSAGE_MAP(drawDetection, CDialogEx)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BTN_PLAY_IN_DRAW_DETECT, &drawDetection::OnBnClickedBtnPlayInDrawDetect)
	ON_BN_CLICKED(IDC_BTN_PAUSE_IN_DRAW_DETECT, &drawDetection::OnBnClickedBtnPauseInDrawDetect)
	ON_BN_CLICKED(IDC_BTN_STOP_IN_DRAW_DETECT, &drawDetection::OnBnClickedBtnStopInDrawDetect)
	ON_BN_CLICKED(IDC_BTN_START_PROCESS__IN_DRAW_DETECT, &drawDetection::OnBnClickedBtnStartProcess)
	ON_BN_CLICKED(IDC_BTN_STOP_PROCESS, &drawDetection::OnBnClickedBtnStopProcess)
	ON_BN_CLICKED(IDC_CHECK_LINE_IN_DRAW_DETECT, &drawDetection::OnBnClickedCheckLineInDrawDetect)
	ON_BN_CLICKED(IDC_CHECK_RECT_IN_DRAW_DETECT, &drawDetection::OnBnClickedCheckRectInDrawDetect)
	ON_BN_CLICKED(IDC_BTN_CLEAR_IN_DRAW_DETECT, &drawDetection::OnBnClickedBtnClearInDrawDetect)
	ON_BN_CLICKED(IDC_BTN_SEARCH_IN_DRAW_DETECT, &drawDetection::OnBnClickedBtnSearchInDrawDetect)
	ON_BN_CLICKED(IDC_BTN_PLAY_IN_DRAW_DETECT2, &drawDetection::OnBnClickedBtnPlayInDrawDetect2)
	ON_BN_CLICKED(IDC_BTN_PAUSE_IN_DRAW_DETECT2, &drawDetection::OnBnClickedBtnPauseInDrawDetect2)
	ON_BN_CLICKED(IDC_BTN_STOP_IN_DRAW_DETECT2, &drawDetection::OnBnClickedBtnStopInDrawDetect2)
	ON_WM_LBUTTONUP()
	ON_WM_HSCROLL()
END_MESSAGE_MAP()


// drawDetection 消息处理程序


BOOL drawDetection::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	UIBeautifier.LoadBackgroundPic(IDB_background);///<背景位图加载
	CWnd* pWnd = CWnd::GetDesktopWindow();
	pWnd->GetClientRect(UIBeautifier.rcDeskRect);///获取桌面矩形
	BitmapBackGroundStrech = UIBeautifier.CBitmapResize(UIBeautifier.m_BitmapBackGround, UIBeautifier.rcDeskRect.Width(), UIBeautifier.rcDeskRect.Height());


	GetDlgItem(IDC_BTN_START_PROCESS__IN_DRAW_DETECT)->MoveWindow(145.0*UIBeautifier.rcDeskRect.Width() / 1214, 55.0*UIBeautifier.rcDeskRect.Height() / 760, 70.0*UIBeautifier.rcDeskRect.Width() / 1214, 30.0*UIBeautifier.rcDeskRect.Height() / 760);
	GetDlgItem(IDC_BTN_STOP_PROCESS)->MoveWindow(145.0*UIBeautifier.rcDeskRect.Width() / 1214, 95.0*UIBeautifier.rcDeskRect.Height() / 760, 70.0*UIBeautifier.rcDeskRect.Width() / 1214, 30.0*UIBeautifier.rcDeskRect.Height() / 760);
	GetDlgItem(IDC_PROGRESS_IN_DRAW_DETECT)->MoveWindow(135.0*UIBeautifier.rcDeskRect.Width() / 1214, 20.0*UIBeautifier.rcDeskRect.Height() / 760, 270.0*UIBeautifier.rcDeskRect.Width() / 1214, 20.0*UIBeautifier.rcDeskRect.Height() / 760);
	GetDlgItem(IDC_CHECK_LINE_IN_DRAW_DETECT)->MoveWindow(235.0*UIBeautifier.rcDeskRect.Width() / 1214, 55.0*UIBeautifier.rcDeskRect.Height() / 760, 50.0*UIBeautifier.rcDeskRect.Width() / 1214, 30.0*UIBeautifier.rcDeskRect.Height() / 760);
	GetDlgItem(IDC_BTN_SEARCH_IN_DRAW_DETECT)->MoveWindow(305.0*UIBeautifier.rcDeskRect.Width() / 1214, 55.0*UIBeautifier.rcDeskRect.Height() / 760, 70.0*UIBeautifier.rcDeskRect.Width() / 1214, 30.0*UIBeautifier.rcDeskRect.Height() / 760);
	GetDlgItem(IDC_CHECK_RECT_IN_DRAW_DETECT)->MoveWindow(235.0*UIBeautifier.rcDeskRect.Width() / 1214, 95.0*UIBeautifier.rcDeskRect.Height() / 760, 50.0*UIBeautifier.rcDeskRect.Width() / 1214, 30.0*UIBeautifier.rcDeskRect.Height() / 760);
	GetDlgItem(IDC_BTN_CLEAR_IN_DRAW_DETECT)->MoveWindow(305.0*UIBeautifier.rcDeskRect.Width() / 1214, 95.0*UIBeautifier.rcDeskRect.Height() / 760, 70.0*UIBeautifier.rcDeskRect.Width() / 1214, 30.0*UIBeautifier.rcDeskRect.Height() / 760);
	GetDlgItem(IDC_DISPLAY_IN_DRAW_DETECT)->MoveWindow(100.0*UIBeautifier.rcDeskRect.Width() / 1214, 140.0*UIBeautifier.rcDeskRect.Height() / 760, 340.0*UIBeautifier.rcDeskRect.Width() / 1214, 255.0*UIBeautifier.rcDeskRect.Height() / 760);
	GetDlgItem(IDC_STATIC_TIME_IN_DRAW_DETECT)->MoveWindow(250.0*UIBeautifier.rcDeskRect.Width() / 1214,400.0*UIBeautifier.rcDeskRect.Height() / 760, 80.0*UIBeautifier.rcDeskRect.Width() / 1214, 30.0*UIBeautifier.rcDeskRect.Height() / 760);
	GetDlgItem(IDC_SLIDER_IN_DRAW_DETECT)->MoveWindow(330.0*UIBeautifier.rcDeskRect.Width() / 1214, 400.0*UIBeautifier.rcDeskRect.Height() / 760, 110.0*UIBeautifier.rcDeskRect.Width() / 1214, 30.0*UIBeautifier.rcDeskRect.Height() / 760);
	GetDlgItem(IDC_DISPLAY_IN_DRAW_DETECT2)->MoveWindow(540.0*UIBeautifier.rcDeskRect.Width() / 1214, 10.0*UIBeautifier.rcDeskRect.Height() / 760, 513.0*UIBeautifier.rcDeskRect.Width() / 1214, 385.0*UIBeautifier.rcDeskRect.Height() / 760);
	GetDlgItem(IDC_STATIC_TIME_IN_DRAW_DETECT2)->MoveWindow(690.0*UIBeautifier.rcDeskRect.Width() / 1214, 400.0*UIBeautifier.rcDeskRect.Height() / 760, 100.0*UIBeautifier.rcDeskRect.Width() / 1214, 30.0*UIBeautifier.rcDeskRect.Height() / 760);
	GetDlgItem(IDC_SLIDER_IN_DRAW_DETECT2)->MoveWindow(790.0*UIBeautifier.rcDeskRect.Width() / 1214, 400.0*UIBeautifier.rcDeskRect.Height() / 760, 263.0*UIBeautifier.rcDeskRect.Width() / 1214, 30.0*UIBeautifier.rcDeskRect.Height() / 760);

	GetDlgItem(IDC_PIC_CTRL0_IN_DRAW_DETECT)->MoveWindow(7.0*UIBeautifier.rcDeskRect.Width() / 1214, 440.0*UIBeautifier.rcDeskRect.Height() / 760, 158.0*UIBeautifier.rcDeskRect.Width() / 1214, 100.0*UIBeautifier.rcDeskRect.Height() / 760);
	GetDlgItem(IDC_PIC_CTRL1_IN_DRAW_DETECT)->MoveWindow(165.0*UIBeautifier.rcDeskRect.Width() / 1214, 440.0*UIBeautifier.rcDeskRect.Height() / 760, 158.0*UIBeautifier.rcDeskRect.Width() / 1214, 100.0*UIBeautifier.rcDeskRect.Height() / 760);
	GetDlgItem(IDC_PIC_CTRL2_IN_DRAW_DETECT)->MoveWindow(323.0*UIBeautifier.rcDeskRect.Width() / 1214, 440.0*UIBeautifier.rcDeskRect.Height() / 760, 158.0*UIBeautifier.rcDeskRect.Width() / 1214, 100.0*UIBeautifier.rcDeskRect.Height() / 760);
	GetDlgItem(IDC_PIC_CTRL3_IN_DRAW_DETECT)->MoveWindow(481.0*UIBeautifier.rcDeskRect.Width() / 1214, 440.0*UIBeautifier.rcDeskRect.Height() / 760, 158.0*UIBeautifier.rcDeskRect.Width() / 1214, 100.0*UIBeautifier.rcDeskRect.Height() / 760);
	GetDlgItem(IDC_PIC_CTRL4_IN_DRAW_DETECT)->MoveWindow(639.0*UIBeautifier.rcDeskRect.Width() / 1214, 440.0*UIBeautifier.rcDeskRect.Height() / 760, 158.0*UIBeautifier.rcDeskRect.Width() / 1214, 100.0*UIBeautifier.rcDeskRect.Height() / 760);
	GetDlgItem(IDC_PIC_CTRL5_IN_DRAW_DETECT)->MoveWindow(797.0*UIBeautifier.rcDeskRect.Width() / 1214, 440.0*UIBeautifier.rcDeskRect.Height() / 760, 158.0*UIBeautifier.rcDeskRect.Width() / 1214, 100.0*UIBeautifier.rcDeskRect.Height() / 760);
	GetDlgItem(IDC_PIC_CTRL6_IN_DRAW_DETECT)->MoveWindow(955.0*UIBeautifier.rcDeskRect.Width() / 1214, 440.0*UIBeautifier.rcDeskRect.Height() / 760, 158.0*UIBeautifier.rcDeskRect.Width() / 1214, 100.0*UIBeautifier.rcDeskRect.Height() / 760);
	GetDlgItem(IDC_DRAW_DETECT_SCROLLBAR)->MoveWindow(7.0*UIBeautifier.rcDeskRect.Width() / 1214, 540.0*UIBeautifier.rcDeskRect.Height() / 760, 1106.0*UIBeautifier.rcDeskRect.Width() / 1214, 25.0*UIBeautifier.rcDeskRect.Height() / 760);

	UIBeautifier.LoadButtonBitmaps(Btn_BTN1_DRAW_PLAY, IDB_PLAY_U, IDB_PLAY_D, 100.0 / 1214, 400.0 / 760, 130.0 / 1214, 430.0 / 760);
	UIBeautifier.LoadButtonBitmaps(Btn_BTN1_DRAW_PAUSE, IDB_PAUSE_U, IDB_PAUSE_D, 150.0 / 1214, 400.0 / 760, 180.0 / 1214, 430.0 / 760);
	UIBeautifier.LoadButtonBitmaps(Btn_BTN1_DRAW_STOP, IDB_STOP_U, IDB_STOP_D, 200.0 / 1214, 400.0/ 760, 230.0 / 1214,430.0/ 760);
	UIBeautifier.LoadButtonBitmaps(Btn_BTN2_DRAW_PLAY, IDB_PLAY_U, IDB_PLAY_D, 540.0 / 1214, 400.0 / 760, 570.0 / 1214, 430.0 / 760);
	UIBeautifier.LoadButtonBitmaps(Btn_BTN2_DRAW_PAUSE, IDB_PAUSE_U, IDB_PAUSE_D, 590.0 / 1214, 400.0 / 760, 620.0 / 1214, 430.0 / 760);
	UIBeautifier.LoadButtonBitmaps(Btn_BTN2_DRAW_STOP, IDB_STOP_U, IDB_STOP_D, 640.0 / 1214, 400.0 / 760, 670.0 / 1214, 430.0 / 760);
	///左播放器初始化
	//player.m_filePath = "";
	//player.playInitial(GetDlgItem(IDC_DISPLAY_IN_DRAW_DETECT), "displayWindow1");
	//player.m_startFrameNO = player.m_currentFrameNO = 0;
	player.m_endFrameNO = 0;
	player.m_drawDetectFlag = 0;
	player.m_IfStartDetect = false;

	///右播放器初始化
	oriPlayer.m_filePath = "";
	oriPlayer.playInitial(GetDlgItem(IDC_DISPLAY_IN_DRAW_DETECT2), "displayWindow2");
	oriPlayer.m_startFrameNO = oriPlayer.m_currentFrameNO = 0;
	oriPlayer.m_endFrameNO = 0;
	oriPlayer.m_drawDetectFlag = 0;
	oriPlayer.m_IfStartDetect = false;

	///进度条初始化
	VTrackPro = (CProgressCtrl*)GetDlgItem(IDC_PROGRESS_IN_DRAW_DETECT);
	VTrackPro->SetRange(0,200);
	VTrackPro->SetPos(0);
	GetDlgItem(IDC_PROGRESS_IN_DRAW_DETECT)->ShowWindow(false);
	GetDlgItem(IDC_BTN_STOP_PROCESS)->ShowWindow(false);

	///设置定时器
	SetTimer(11,300,NULL);//定时器，id为11，时间间隔为300ms
	SetTimer(1,100,NULL);
	SetTimer(2,100,NULL);
	SetTimer(3,100,NULL);

	Draw_Detect_pScrollBar = (CScrollBar*)GetDlgItem(IDC_DRAW_DETECT_SCROLLBAR);

	///翻页初始化
	m_pageNum = 0;
	m_pageSum = 7;

	user_click_scrollbar=FALSE;

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}


void drawDetection::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent==1)
	{
		///刷新进度条
		CSliderCtrl * SliderCtrl=((CSliderCtrl*)GetDlgItem(IDC_SLIDER_IN_DRAW_DETECT));
		int CurrentPos=SliderCtrl->GetPos();                            ///<获取当前进度条位置
		int NewCurrentFrameNO = (double)CurrentPos*double(player.m_endFrameNO-player.m_startFrameNO)
			/(double)SliderCtrl->GetRangeMax();     ///<根据当前进度条位置，重新计算帧号

		if (player.m_playState!=PLAY_STATE_STOP && player.m_playState!=PLAY_STATE_FAST_FORWORD&&player.m_playState!=PLAY_STATE_PLAY_PIECE)///<若播放器处于正常播放或暂停状态
		{
			if (abs(NewCurrentFrameNO-player.m_currentFrameNO)>5)
			{
				player.m_currentFrameNO=NewCurrentFrameNO;             ///<根据新计算的帧号，重置播放器
			}
		}
		SliderCtrl->SetPos(player.m_currentFrameNO);                   ///<根据新计算的帧号，重置滑动条
		CWnd *pWndTimeDis = GetDlgItem(IDC_STATIC_TIME_IN_DRAW_DETECT);
		if (player.m_playState != PLAY_STATE_STOP)                     ///<非停止状态下，用方式一显示时间
		{
			ShowTime(player.m_currentFrameNO, player.m_endFrameNO - player.m_startFrameNO,
				player.m_videoTimeInSecond, pWndTimeDis, 1);
		}
		else                                                            ///<停止状态下，用方式二显示时间
		{
			SliderCtrl->SetPos(0);
			ShowTime(player.m_currentFrameNO, player.m_endFrameNO - player.m_startFrameNO,
				player.m_videoTimeInSecond, pWndTimeDis, 0);

		}
	}
	else if(nIDEvent==2)
	{
		///刷新进度条
		if (oriPlayer.m_drawDetectFlag != 5)
		{
			CSliderCtrl * SliderCtrl=((CSliderCtrl*)GetDlgItem(IDC_SLIDER_IN_DRAW_DETECT2));
			int n=oriPlayer.m_currentFrameNO;
			int CurrentPos=SliderCtrl->GetPos();
			int NewCurrentFrameNO = (double)CurrentPos*double(oriPlayer.m_endFrameNO-oriPlayer.m_startFrameNO)
				/(double)SliderCtrl->GetRangeMax();//获得了当前拖动到的指定帧

			if (oriPlayer.m_playState!=PLAY_STATE_STOP && oriPlayer.m_playState!=PLAY_STATE_FAST_FORWORD&&oriPlayer.m_playState!=PLAY_STATE_PLAY_PIECE)
			{
				if (abs(NewCurrentFrameNO-oriPlayer.m_currentFrameNO)>5)
				{
					oriPlayer.m_currentFrameNO=NewCurrentFrameNO;
				}
			}

			SliderCtrl->SetPos(oriPlayer.m_currentFrameNO);
			CWnd *pWndTimeDis = GetDlgItem(IDC_STATIC_TIME_IN_DRAW_DETECT2);
			if (oriPlayer.m_playState != PLAY_STATE_STOP)
			{ 
				ShowTime(oriPlayer.m_currentFrameNO, oriPlayer.m_endFrameNO - oriPlayer.m_startFrameNO, 
					oriPlayer.m_videoTimeInSecond, pWndTimeDis, 1);
			}
			else
			{
				SliderCtrl->SetPos(0);
				ShowTime(oriPlayer.m_currentFrameNO, oriPlayer.m_endFrameNO - oriPlayer.m_startFrameNO, 
					oriPlayer.m_videoTimeInSecond, pWndTimeDis, 0);
			}
		}
		else 
		{
			CSliderCtrl * SliderCtrl=((CSliderCtrl*)GetDlgItem(IDC_SLIDER_IN_DRAW_DETECT2));
			SliderCtrl->SetPos(oriPlayer.m_currentFrameNO);
			CWnd *pWndTimeDis = GetDlgItem(IDC_STATIC_TIME_IN_DRAW_DETECT2);
			ShowTime(oriPlayer.m_currentFrameNO,Video_FrameCount, oriPlayer.m_videoTimeInSecond, pWndTimeDis, 1);

		}
	}
	else if(nIDEvent==11)
	{
		if (m_ifExistProData)
			return;
		int speed = tracker.VTrackSpeed()*100;
		VTrackPro->SetPos(speed);
		if (speed>=200 && tracker.m_IfContinue)
		{
			///进度条到达上限时
			tracker.m_IfContinue = false;///<继续处理标志位置否
			MessageBox("完成处理");///<弹窗提示“完成处理”
			m_ifExistProData = true;///<数据库存在与否标志位为true
			///修改界面控件属性
			GetDlgItem(IDC_PROGRESS_IN_DRAW_DETECT)->ShowWindow(false);
			GetDlgItem(IDC_BTN_STOP_PROCESS)->ShowWindow(false);
			GetDlgItem(IDC_CHECK_LINE_IN_DRAW_DETECT)->EnableWindow(true);
			GetDlgItem(IDC_CHECK_RECT_IN_DRAW_DETECT)->EnableWindow(true);
		}
	}
	else if (nIDEvent==3)
	{
		if (user_click_scrollbar==TRUE)
		{
			m_pageNum=Draw_Detect_pScrollBar->GetScrollPos();
			DisplayPageSumImg();
			user_click_scrollbar = FALSE;
		}
	}
}



void drawDetection::ShowTime(int m_currentFrameNO, int totalFrameCount, 
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

		CurVideoTime.Format(" %d::%d::%d / %d::%d::%d ",CurVedioHour,CurVedioMinute,CurVedioSecond,VedioHour,VedioMinute,VedioSecond);
		m_pShowTimeWnd->SetWindowText(CurVideoTime);
	}
	else if (flag == 0)
	{
		CurVideoTime.Format(" %d::%d::%d / %d::%d::%d ",0,0,0,0,0,0);
		m_pShowTimeWnd->SetWindowText(CurVideoTime);
	}
}


void drawDetection::OnBnClickedBtnPlayInDrawDetect()
{
	// TODO: 在此添加控件通知处理程序代码
	///判断视频是否正确读取
	if (player.m_filePath=="")
	{
		AfxMessageBox("请选择要播放的文件！！！");
		return;
	}
	///判断视频是否完成处理
	if (tracker.m_IfContinue)
	{
		MessageBox("正在对视频进行处理，请等待...");
		return;
	}

	m_CSlider_playerCtrl.SetRange(0, player.m_endFrameNO);///<滑动条初始化
	Video_FrameCount=player.m_endFrameNO;
	CString VideoTime;                                     ///<时间显示准备
	int VedioHour=player.m_videoTimeInSecond/3600;
	int VedioMinute=player.m_videoTimeInSecond/60-VedioHour*60;
	int VedioSecond=player.m_videoTimeInSecond-VedioHour*3600-VedioMinute*60;

	VideoTime.Format(" 0::0::0 / %d::%d::%d ",VedioHour,VedioMinute,VedioSecond);
	GetDlgItem(IDC_STATIC_TIME_IN_DRAW_DETECT)->SetWindowText(VideoTime);///<将时间显示到控件上


	///修改界面控件属性
	GetDlgItem(IDC_BTN_PLAY_IN_DRAW_DETECT)->ShowWindow(false);
	GetDlgItem(IDC_BTN_PLAY_IN_DRAW_DETECT)->EnableWindow(false);
	GetDlgItem(IDC_BTN_PAUSE_IN_DRAW_DETECT)->ShowWindow(TRUE);
	GetDlgItem(IDC_BTN_PAUSE_IN_DRAW_DETECT)->EnableWindow(TRUE);
	///修改手绘检测状态
	if ( BST_CHECKED == IsDlgButtonChecked(IDC_CHECK_LINE_IN_DRAW_DETECT)&&
		IsDlgButtonChecked(IDC_CHECK_RECT_IN_DRAW_DETECT) != BST_CHECKED )
	{
		player.m_drawDetectFlag = 1;   ///<画线时，状态=1
		player.objectIDs.clear();	   ///<画框、画线改变命令时，清空objectIDs
	}	
	else if ( BST_CHECKED == IsDlgButtonChecked(IDC_CHECK_RECT_IN_DRAW_DETECT)&&
		IsDlgButtonChecked(IDC_CHECK_LINE_IN_DRAW_DETECT) != BST_CHECKED )
	{
		player.m_drawDetectFlag = 2;   ///<画框时，状态=2
		player.objectIDs.clear();	   ///<画框、画线改变命令时，清空objectIDs
	}

	///保存鼠标位置
	mousePosInCVwmd.x = tempDrewPoint1.x;
	mousePosInCVwmd.y = tempDrewPoint1.y;
	mousePosInCVwmd.x1 = tempDrewPoint2.x;
	mousePosInCVwmd.y1 = tempDrewPoint2.y;
	player.play();///<视频播放    //在此处开启线程
}

///@brief drawDetection类的原始视频窗口的暂停按钮响应函数
/// 
///@param[in|out] NULL
///@pre  NULL
///@return NULL
///@retval NULL
///@post NULL
void drawDetection::OnBnClickedBtnPauseInDrawDetect()
{
	// TODO: 在此添加控件通知处理程序代码
	if (player.m_filePath=="")
	{
		return;
	}
	if (tracker.m_IfContinue)
	{
		MessageBox("正在对视频进行处理，请等待...");
		return;
	}
	GetDlgItem(IDC_BTN_PLAY_IN_DRAW_DETECT)->ShowWindow(TRUE);
	GetDlgItem(IDC_BTN_PLAY_IN_DRAW_DETECT)->EnableWindow(TRUE);
	GetDlgItem(IDC_BTN_PAUSE_IN_DRAW_DETECT)->ShowWindow(false);
	GetDlgItem(IDC_BTN_PAUSE_IN_DRAW_DETECT)->EnableWindow(false);
	tempDrewPoint1 = player.m_clickPosInCVWnd;//左键点击的点
	tempDrewPoint2 = player.m_lBtnUpPosInCVWnd;//左键弹起的点

	if ( BST_CHECKED == IsDlgButtonChecked(IDC_CHECK_LINE_IN_DRAW_DETECT)&&
		IsDlgButtonChecked(IDC_CHECK_RECT_IN_DRAW_DETECT) != BST_CHECKED )
	{
		player.m_drawDetectFlag = 3;///<画线时，状态=3
	}	
	else if ( BST_CHECKED == IsDlgButtonChecked(IDC_CHECK_RECT_IN_DRAW_DETECT)&&
		IsDlgButtonChecked(IDC_CHECK_LINE_IN_DRAW_DETECT) != BST_CHECKED )
	{
		player.m_drawDetectFlag = 4;///<画框时，状态=4
	}

	player.pause();///<视频暂停
}


///@brief drawDetection类的原始视频窗口的中止按钮响应函数
/// 
///@param[in|out] NULL
///@pre  NULL
///@return NULL
///@retval NULL
///@post NULL
void drawDetection::OnBnClickedBtnStopInDrawDetect()
{
	// TODO: 在此添加控件通知处理程序代码
	if (player.m_filePath=="")
	{
		AfxMessageBox("您并未播放任何视频！！！");
		return;
	}
	if (tracker.m_IfContinue)
	{
		MessageBox("正在对视频进行处理，请等待...");
		return;
	}
	GetDlgItem(IDC_BTN_PLAY_IN_DRAW_DETECT)->ShowWindow(TRUE);
	GetDlgItem(IDC_BTN_PLAY_IN_DRAW_DETECT)->EnableWindow(TRUE);
	GetDlgItem(IDC_BTN_PAUSE_IN_DRAW_DETECT)->ShowWindow(false);
	GetDlgItem(IDC_BTN_PAUSE_IN_DRAW_DETECT)->EnableWindow(false);
	player.m_drawDetectFlag = 0;///<状态变量置零
	cvWaitKey(5);	            ///<延时处理
	///清空vector数据
	player.RectToShow.swap(vector<CRect>());
	player.pointsToShow.swap(vector<CPoint>());
	player.objectIDs.swap(vector<int>());
	//numOfObj = -1;//注释之后则不显示检测到的物体，无黄框
	player.stop();///<视频中止
	player.m_IfStartDetect = false;///<视频检测标志位置否
}



void drawDetection::OnBnClickedBtnStartProcess()
{
	// TODO: 在此添加控件通知处理程序代码
	m_curVideoPath=player.m_filePath;
	if(m_curVideoPath=="")
		return;
	CString m_curVideoNameText;
	//m_curVideoNameText=video
	objtablename = draw_videoPro->m_tableParams.ObjectTableName;//m_curVideoNameText.Left(m_curVideoNameText.Find(".")) + "_ObjectTable";
	player.objtablename=objtablename;
	oriPlayer.objtablename=objtablename;
	if(!m_MysqlHandle->IfExistTable(objtablename,&m_ifExistProData))
	{
		AfxMessageBox("数据库查询语句出错");//debug用
		return;
	}
	if(m_ifExistProData)
	{///存在数据表则无需再做处理
		MessageBox("该视频已做处理");
		return ;
	}
	vector<CString> oriVideoName;
	if (player.m_filePath == m_curVideoPath)
	{
		player.m_drawDetectFlag = 0;
		oriPlayer.m_drawDetectFlag = 0;
		player.stopPlay();
		oriPlayer.stopPlay();
		GetDlgItem(IDC_BTN_PLAY_IN_DRAW_DETECT)->EnableWindow(FALSE);
		GetDlgItem(IDC_BTN_PAUSE_IN_DRAW_DETECT)->EnableWindow(FALSE);
		GetDlgItem(IDC_BTN_PLAY_IN_DRAW_DETECT2)->EnableWindow(FALSE);
		GetDlgItem(IDC_BTN_PAUSE_IN_DRAW_DETECT2)->EnableWindow(FALSE);
	}
	char *filename = m_curVideoPath.GetBuffer(m_curVideoPath.GetLength());///获取绝对路径
	tracker.DoProcessing(filename,objtablename);///<开始跟踪检测处理

	GetDlgItem(IDC_PROGRESS_IN_DRAW_DETECT)->ShowWindow(true);
	GetDlgItem(IDC_BTN_STOP_PROCESS)->ShowWindow(true);
	GetDlgItem(IDC_CHECK_LINE_IN_DRAW_DETECT)->EnableWindow(false);
	GetDlgItem(IDC_CHECK_RECT_IN_DRAW_DETECT)->EnableWindow(false);
}





void drawDetection::OnBnClickedBtnStopProcess()
{
	// TODO: 在此添加控件通知处理程序代码
	tracker.StopProcessing();
	GetDlgItem(IDC_PROGRESS_IN_DRAW_DETECT)->ShowWindow(false);
	GetDlgItem(IDC_BTN_STOP_PROCESS)->ShowWindow(false);
	GetDlgItem(IDC_CHECK_LINE_IN_DRAW_DETECT)->EnableWindow(true);
	GetDlgItem(IDC_CHECK_RECT_IN_DRAW_DETECT)->EnableWindow(true);
}




void drawDetection::OnBnClickedCheckLineInDrawDetect()
{
	// TODO: 在此添加控件通知处理程序代码
	m_MysqlHandle->IfExistTable(objtablename,&m_ifExistProData);
	if(!m_ifExistProData)
	{///存在数据表则无需再做处理
		MessageBox("请先对视频做处理");
		GetDlgItem(IDC_CHECK_LINE_IN_DRAW_DETECT)->SendMessage(BM_SETCHECK, BST_UNCHECKED, NULL);  
		player.m_drawDetectFlag = 0;
		return ;
	}
	///若有勾选，保存状态量，修改开始检测标志位
	if ( BST_CHECKED == IsDlgButtonChecked(IDC_CHECK_LINE_IN_DRAW_DETECT)&&
		IsDlgButtonChecked(IDC_CHECK_RECT_IN_DRAW_DETECT) != BST_CHECKED )
	{
		player.m_IfStartDetect  = false;
		player.m_drawDetectFlag = 1;
	}	
	else if ( BST_CHECKED == IsDlgButtonChecked(IDC_CHECK_RECT_IN_DRAW_DETECT)&&
		IsDlgButtonChecked(IDC_CHECK_LINE_IN_DRAW_DETECT) != BST_CHECKED )
	{
		player.m_IfStartDetect  = false;
		player.m_drawDetectFlag = 2;
	}
	///否则，清空vector数据
	else /*if ( BST_CHECKED != IsDlgButtonChecked(IDC_CHECK_RECT_IN_DRAW_DETECT)&&
		IsDlgButtonChecked(IDC_CHECK_LINE_IN_DRAW_DETECT) != BST_CHECKED )*/
	{
		player.m_drawDetectFlag = 0;
		cvWaitKey(5);
		player.RectToShow.swap(vector<CRect>());
		player.pointsToShow.swap(vector<CPoint>());
		player.objectIDs.swap(vector<int>());
		numOfObj = -1;
		player.m_IfStartDetect  = false;
	}
}




void drawDetection::OnBnClickedCheckRectInDrawDetect()
{
	// TODO: 在此添加控件通知处理程序代码
		if(!m_ifExistProData)
	{///存在数据表则无需再做处理
		MessageBox("请先对视频做处理");
		player.m_drawDetectFlag = 0;
		GetDlgItem(IDC_CHECK_RECT_IN_DRAW_DETECT)->SendMessage(BM_SETCHECK, BST_UNCHECKED, NULL);  
		return ;
	}
	///若有勾选，保存状态量，修改开始检测标志位
	if (BST_CHECKED == IsDlgButtonChecked(IDC_CHECK_RECT_IN_DRAW_DETECT)&&
		IsDlgButtonChecked(IDC_CHECK_LINE_IN_DRAW_DETECT) != BST_CHECKED)
	{
		player.m_IfStartDetect  = false;
		player.m_drawDetectFlag = 2;
	}	
	else if (BST_CHECKED == IsDlgButtonChecked(IDC_CHECK_LINE_IN_DRAW_DETECT)&&
		IsDlgButtonChecked(IDC_CHECK_RECT_IN_DRAW_DETECT) != BST_CHECKED)
	{
		player.m_IfStartDetect  = false;
		player.m_drawDetectFlag = 1;
	}
	
	///否则，清空vector数据
	else /*if ( BST_CHECKED != IsDlgButtonChecked(IDC_CHECK_RECT_IN_DRAW_DETECT)&&
		IsDlgButtonChecked(IDC_CHECK_LINE_IN_DRAW_DETECT) != BST_CHECKED )*/
	{
		player.m_drawDetectFlag = 0;
		cvWaitKey(5);
		player.RectToShow.swap(vector<CRect>());
		player.pointsToShow.swap(vector<CPoint>());
		player.objectIDs.swap(vector<int>());
		numOfObj = -1;
		player.m_IfStartDetect  = false;
	}
}






void drawDetection::OnBnClickedBtnClearInDrawDetect()
{
	// TODO: 在此添加控件通知处理程序代码
	if (tracker.m_IfContinue)
	{
		MessageBox("正在对视频进行处理，请等待...");
		return;
	}
	oriPlayer.stopPlay();///<检测结果视频停止播放
	oriPlayer.m_drawDetectFlag = 0;
	oriPlayer.m_IfStartDetect = false;
	player.m_drawDetectFlag = 0;
	player.m_IfStartDetect = false;
	player.m_playState = PLAY_STATE_PLAY;
	numOfObj = -1;
	cvWaitKey(5);
	///情况vector数据
	player.pointsToShow.swap(vector<CPoint>());
	player.RectToShow.swap(vector<CRect>());
	player.objectIDs.swap(vector<int>());
	oriPlayer.pointsToShow.swap(vector<CPoint>()); 
	oriPlayer.RectToShow.swap(vector<CRect>());

	mousePosInCVwmd.x=0;
	mousePosInCVwmd.x1=0;
	mousePosInCVwmd.y=0;
	mousePosInCVwmd.y1=0;
	///界面控件属性修改
	GetDlgItem(IDC_BTN_PLAY_IN_DRAW_DETECT2)->ShowWindow(TRUE);
	GetDlgItem(IDC_BTN_PLAY_IN_DRAW_DETECT2)->EnableWindow(TRUE);
	GetDlgItem(IDC_BTN_PAUSE_IN_DRAW_DETECT2)->ShowWindow(false);
	GetDlgItem(IDC_BTN_PAUSE_IN_DRAW_DETECT2)->EnableWindow(false);
	GetDlgItem(IDC_CHECK_LINE_IN_DRAW_DETECT)->SendMessage(BM_SETCHECK, BST_UNCHECKED, NULL);  
	GetDlgItem(IDC_CHECK_RECT_IN_DRAW_DETECT)->SendMessage(BM_SETCHECK, BST_UNCHECKED, NULL);  
}




void drawDetection::OnBnClickedBtnSearchInDrawDetect()
{
	// TODO: 在此添加控件通知处理程序代码
	m_curVideoPath=player.m_filePath;
	if (m_curVideoPath == "")
		return;
	if (tracker.m_IfContinue)
	{
		MessageBox("正在对视频进行处理，请等待...");
		return;
	}
	if ( BST_CHECKED != IsDlgButtonChecked(IDC_CHECK_LINE_IN_DRAW_DETECT) && IsDlgButtonChecked(IDC_CHECK_RECT_IN_DRAW_DETECT) != BST_CHECKED )
	{
		MessageBox("请选择检测条件");
		return;
	}
	m_MysqlHandle->IfExistTable(objtablename,&m_ifExistProData);
	if (!m_ifExistProData)
	{///存在数据表则无需再做处理
		MessageBox("请先对视频进行处理");
		return;
	}
	if (player.m_clickPosInCVWnd.x <0 || player.m_lBtnUpPosInCVWnd.x <0)
	{///无检测条件则返回
		MessageBox("请画出检测范围");
		return;
	}
	numOfObj = m_MysqlHandle->GetTableDistinctNum(objtablename);///<获取改视频运动物体总数

	if (numOfObj == -1)
	{
		MessageBox("ObjectTable出错");
		return;
	}
	///初始化手绘检测参量
	if (player.m_IfStartDetect && player.pointsToShow.size()!=0)
	{
		return;//说明检测条件没有改变--------------2013.3.7 luying
	}
	player.m_IfStartDetect = false;
	player.objectIDs.swap(vector<int>());
	objDetectedInfos.swap(vector<objectInfo>());
	oriPlayer.RectToShow.swap(vector<CRect>());
	oriPlayer.pointsToShow.swap(vector<CPoint>());
	player.RectToShow.swap(vector<CRect>());
	player.pointsToShow.swap(vector<CPoint>());
	DetectResult();                 ///<获取检测结果
	m_pageNum = 0;
	DisplayPageSumImg();			///<在缩略图中显示检测到的目标信息
	player.m_IfStartDetect = true;
	Draw_Detect_pScrollBar->SetScrollRange(0,(objDetectedInfos.size()-7>0)?objDetectedInfos.size()-7:0);
}


void drawDetection::DisplayPageSumImg()
{
	CvCapture *capture = cvCreateFileCapture(oriPlayer.m_filePath);
	if (!capture)
		return;

	int num = m_pageSum;
	if (objDetectedInfos.size() > m_pageNum+7)		//若当前页不为最后一页
		num = m_pageSum;									//显示七张图
	else
		num = objDetectedInfos.size();			//为最后一页，显示剩余的图

	IplImage* image = NULL;
	for (int i = 0; i < m_pageSum; i++)
	{	
		///显示图片
		if(i < num)											
		{
			///取出视频中该目标起始帧号的那一帧
			cvSetCaptureProperty(capture,CV_CAP_PROP_POS_FRAMES,objDetectedInfos.at(m_pageNum +i).firstFrameID);
			image = cvQueryFrame(capture);
			if(image == NULL)
				continue;
			///在那一帧中画ROI区域标出目标位置
			CRect rect = objDetectedInfos.at(m_pageNum + i).C_roi;
			cvRectangle(image,cvPoint(rect.left,rect.top),cvPoint(rect.right,rect.bottom),CV_RGB(200,200,0),5,8,0);
		}
		else											///若为最后一页，则用closeup.bmp填充
			image = cvLoadImage("closeup.bmp",CV_LOAD_IMAGE_COLOR);

		///在缩略图中显示
		CRect rt;
		CDC* pDC = GetDC();
		HDC hDC = pDC->m_hDC;
		GetDlgItem(IDC_PIC_CTRL0_IN_DRAW_DETECT + i)->GetClientRect(rt);   
		CClientDC dc(GetDlgItem(IDC_PIC_CTRL0_IN_DRAW_DETECT + i));
		m_ShowResult.ShowPictureHolderBMP(image,hDC,rt,dc);	
		ReleaseDC(pDC);
	}
	cvReleaseCapture(&capture);
}

void drawDetection::DetectResult()
{
	///得到过线或过框的所有物体信息
	CPoint drewPoit1, drewPoit2;
	drewPoit1 = player.m_clickPosInCVWnd;
	drewPoit2 = player.m_lBtnUpPosInCVWnd;
	int flag = player.m_drawDetectFlag;
	player.m_drawDetectFlag = 0;

	vector <CPoint> centersOfObj;	///用于装载某一目标的center坐标
	int objCount = 0;
	int count = 0;					///计数器，计到m_pageSum张，就够显示该页的缩略图了，则跳出循环
	if (!objDetectedInfos.empty())
		///若不是第一页的m_pageSum张图，则以objCount为起点取下m_pageSum张目标，用来在缩略图中显示
		objCount = objDetectedInfos.at(objDetectedInfos.size() - 1).objectID + 1;		

	switch (flag)
	{
	case 1:
	case 3:
		{
			double disP1P2 = sqrt(double((drewPoit1.x-drewPoit2.x)*(drewPoit1.x-drewPoit2.x) +
				(drewPoit1.y-drewPoit2.y)*(drewPoit1.y-drewPoit2.y)));///计算所画线段p1p2长度
			int rCloseCount = 0;
			for (; objCount<numOfObj; objCount++)
			{
				///在下面将第objCount个目标的轨迹点读入centersOfObj中
				centersOfObj.clear();
				m_MysqlHandle->GetCenterFromObjectTable(objCount,objtablename,&centersOfObj);
				int numOfCenter = centersOfObj.size();
				rCloseCount = 0;//记录在矩形内出现的中心数
				for(int centerCount=0; centerCount<numOfCenter; centerCount++)
				{
					int centerX = centersOfObj[centerCount].x;
					int centerY = centersOfObj[centerCount].y;
					///下面判断当前目标中心点是否在 画线所对应的矩形内
					if (((centerX<=drewPoit1.x&&centerX>=drewPoit2.x)||(centerX<=drewPoit2.x&&centerX>=drewPoit1.x)) &&
						((centerY<=drewPoit1.y&&centerY>=drewPoit2.y)||(centerY<=drewPoit2.y&&centerY>=drewPoit1.y)))//用户画矩形的方式不同
					{
						double disP1C = sqrt(double((drewPoit1.x-centerX)*(drewPoit1.x-centerX) +
							(drewPoit1.y-centerY)*(drewPoit1.y-centerY)));
						double disP2C = sqrt(double((drewPoit2.x-centerX)*(drewPoit2.x-centerX) +
							(drewPoit2.y-centerY)*(drewPoit2.y-centerY)));//考察中心点距两个鼠标点之间的距离
						if ((disP2C+disP1C-disP1P2) < 5)//这个判断方式？？
						{
							rCloseCount += 1;
							if (rCloseCount > 1)
							{
								///有两个点很接近线段p1p2，判为过线
								///将越过线的目标信息存起来
								objectInfo tempInfo;		///临时变量，用来存储越线目标出现的第一帧帧号、第一帧中的位置及最后一帧帧号
								m_MysqlHandle->GetObjectInfoFromDrawObjectTable(objCount,objtablename,&tempInfo);
								objDetectedInfos.push_back(tempInfo);
								//if (++count == m_pageSum)				///每次取够m_pageSum张，则返回
								//{
								//	centersOfObj.swap(vector<CPoint>());
								//	player.m_drawDetectFlag = flag;
								//	return;
								//}
								break;
							}
						}
					}
				}
			}
		}
		break;
	case 2:			///进框检测
	case 4:
		{
			for (; objCount<numOfObj; objCount++)
			{
				///在下面将第objCount个目标的轨迹点读入centersOfObj中
				centersOfObj.swap(vector<CPoint>());
				m_MysqlHandle->GetCenterFromObjectTable(objCount,objtablename,&centersOfObj);
				int numOfCenter = centersOfObj.size();
				for(int centerCount=0; centerCount<numOfCenter; centerCount++)
				{
					int centerX = centersOfObj[centerCount].x;
					int centerY = centersOfObj[centerCount].y;
					if (((centerX<=drewPoit1.x&&centerX>=drewPoit2.x)||(centerX<=drewPoit2.x&&centerX>=drewPoit1.x)) &&
						((centerY<=drewPoit1.y&&centerY>=drewPoit2.y)||(centerY<=drewPoit2.y&&centerY>=drewPoit1.y)))
					{
						objectInfo tempInfo;					///将越过线的目标信息存起来
						m_MysqlHandle->GetObjectInfoFromDrawObjectTable(objCount,objtablename,&tempInfo);
						objDetectedInfos.push_back(tempInfo);
						//if (++count == m_pageSum)				///每次取够m_pageSum张，则返回
						//{
						//	centersOfObj.clear();
						//	player.m_drawDetectFlag = flag;
						//	return;
						//}
						break;

					}
				}
			}
		}
		break;
	default:
		break;
	}
	centersOfObj.swap(vector<CPoint>());
	player.m_drawDetectFlag = flag;
}

void drawDetection::OnBnClickedBtnPlayInDrawDetect2()
{
	// TODO: 在此添加控件通知处理程序代码
	if (tracker.m_IfContinue)
	{
		MessageBox("正在对视频进行处理，请等待...");
		return;
	}
	if (player.m_IfStartDetect == false)
	{
		MessageBox("未开始检测");
		return;
	}
	m_CSlider_oriplayerCtrl.SetRange(0,Video_FrameCount);///<滑动条初始化
	CString VideoTime;                                     ///<时间显示准备
	int VedioHour=oriPlayer.m_videoTimeInSecond/3600;
	int VedioMinute=oriPlayer.m_videoTimeInSecond/60-VedioHour*60;
	int VedioSecond=oriPlayer.m_videoTimeInSecond-VedioHour*3600-VedioMinute*60;
	VideoTime.Format(" 0::0::0 / %d::%d::%d ",VedioHour,VedioMinute,VedioSecond);
	GetDlgItem(IDC_STATIC_TIME_IN_DRAW_DETECT)->SetWindowText(VideoTime);///<将时间显示到控件上


	GetDlgItem(IDC_BTN_PLAY_IN_DRAW_DETECT2)->ShowWindow(false);
	GetDlgItem(IDC_BTN_PLAY_IN_DRAW_DETECT2)->EnableWindow(false);
	GetDlgItem(IDC_BTN_PAUSE_IN_DRAW_DETECT2)->ShowWindow(TRUE);
	GetDlgItem(IDC_BTN_PAUSE_IN_DRAW_DETECT2)->EnableWindow(TRUE);
	//oriPlayer.m_playState=PLAY_STATE_PLAY;
	oriPlayer.play();
}



void drawDetection::OnBnClickedBtnPauseInDrawDetect2()
{
	// TODO: 在此添加控件通知处理程序代码
	if (tracker.m_IfContinue)
	{
		MessageBox("正在对视频进行处理，请等待...");
		return;
	}
	if (player.m_IfStartDetect == false)
	{
		MessageBox("未开始检测");
		return;
	}
	GetDlgItem(IDC_BTN_PLAY_IN_DRAW_DETECT2)->ShowWindow(TRUE);
	GetDlgItem(IDC_BTN_PLAY_IN_DRAW_DETECT2)->EnableWindow(TRUE);
	GetDlgItem(IDC_BTN_PAUSE_IN_DRAW_DETECT2)->ShowWindow(false);
	GetDlgItem(IDC_BTN_PAUSE_IN_DRAW_DETECT2)->EnableWindow(false);
	oriPlayer.pause();
}




void drawDetection::OnBnClickedBtnStopInDrawDetect2()
{
	// TODO: 在此添加控件通知处理程序代码

	if (tracker.m_IfContinue)
	{
		MessageBox("正在对视频进行处理，请等待...");
		return;
	}
	if (player.m_IfStartDetect == false)
	{
		MessageBox("未开始检测");
		return;
	}
	GetDlgItem(IDC_BTN_PLAY_IN_DRAW_DETECT2)->ShowWindow(TRUE);
	GetDlgItem(IDC_BTN_PLAY_IN_DRAW_DETECT2)->EnableWindow(TRUE);
	GetDlgItem(IDC_BTN_PAUSE_IN_DRAW_DETECT2)->ShowWindow(false);
	GetDlgItem(IDC_BTN_PAUSE_IN_DRAW_DETECT2)->EnableWindow(false);
	oriPlayer.m_drawDetectFlag = 0;
	oriPlayer.stop();
}




void drawDetection::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	int num = m_pageSum;
	//if (objDetectedInfos.size()/m_pageSum > m_pageNum)	   ///若当前页不为最后一页
	//num = m_pageSum;
	//else
	//	num = objDetectedInfos.size()%m_pageSum;		   ///为最后一页，显示剩余的图
	CRect rec;
	for (int i = 0; i < num; i++)						   ///通过验证鼠标点击的坐标为哪张图片的区域来设置choice为选中哪张
	{
		GetDlgItem(IDC_PIC_CTRL0_IN_DRAW_DETECT + i)->GetWindowRect(rec);
		ScreenToClient(rec);					
		if (rec.PtInRect(point))
		{
			///在右侧的播放窗口中播放选中的目标的相关视频
			oriPlayer.m_drawDetectFlag = 0;
			objectInfo tempInfo = objDetectedInfos.at(m_pageNum + i);
			///在此播放视频片段
			oriPlayer.play();
			oriPlayer.m_currentFrameNO=oriPlayer.m_startFrameNO = tempInfo.firstFrameID;//201303013陆鹏改，解决下面列表框有时点击后，右边框不显示画框现象
			oriPlayer.m_endFrameNO = tempInfo.lastFrameID - 2;
			oriPlayer.m_playState = PLAY_STATE_PLAY_PIECE;
			oriPlayer.m_clickObjectID = tempInfo.objectID;
			oriPlayer.m_drawDetectFlag = 5;
			break;
		}
	}
	CDialogEx::OnLButtonUp(nFlags, point);
}



void drawDetection::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	int TempPos = pScrollBar->GetScrollPos();
	switch(nSBCode)
	{
	case SB_THUMBPOSITION://拖动滑块
		if(nPos<objDetectedInfos.size()-6)
		{
			pScrollBar->SetScrollPos(nPos);
			user_click_scrollbar=TRUE;
		}
		//pScrollBar->SetScrollPos(nPos);
		break;
	case SB_THUMBTRACK://拖动滑块
		if(nPos<objDetectedInfos.size()-6)
		{

			pScrollBar->SetScrollPos(nPos);
			user_click_scrollbar=TRUE;
		}

		//pScrollBar->SetScrollPos(nPos);
		break;
	case SB_LINELEFT://点击上边的箭头
		if(TempPos > 0)
		{
			TempPos--;
		}
		pScrollBar->SetScrollPos(TempPos);
		user_click_scrollbar=TRUE;
		break;
	case SB_LINERIGHT://点击下边的箭头
		if(TempPos<objDetectedInfos.size()-7)
		{
			TempPos++;
		}
		pScrollBar->SetScrollPos(TempPos);
		user_click_scrollbar=TRUE;
		break;
	} 
	//int i=pScrollBar->GetScrollPos();
	CDialogEx::OnHScroll(nSBCode, nPos, pScrollBar);
}
