// Dialog1.cpp : 实现文件
//

#include "stdafx.h"
#include "videotest2.h"
#include "Dialog1.h"
#include "VideoSeg.h"
#include "afxdialogex.h"
#include "ifRebuildDlg.h"
#include "CvvImage.h"
#include "LibMySql.h"

// Dialog1 对话框

IMPLEMENT_DYNAMIC(Dialog1, CDialogEx)

Dialog1::Dialog1(CWnd* pParent /*=NULL*/)
	: CDialogEx(Dialog1::IDD, pParent)
{
	Tab1_ifStartPro = false;
	Tab1_playpiece = FALSE;
	Tab1_videoPro = new CVideo;
	Tab1_MysqlHandle = new CDataMySql;
}

Dialog1::~Dialog1()
{
	if (Tab1_videoPro != NULL)
	{
		delete Tab1_videoPro;
		Tab1_videoPro = NULL;
	}
	if (Tab1_MysqlHandle != NULL)
	{
		delete Tab1_MysqlHandle;
		Tab1_MysqlHandle = NULL;
	}
}

void Dialog1::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SLIDER1_TAB1, Tab1_CSliderPlayer1Ctrl);
	DDX_Control(pDX, IDC_SLIDER2_TAB1, Tab1_CSliderPlayer2Ctrl);
	//DDX_Control(pDX, IDC_EDIT_FRAME_NO, Tab1_wndEditFrameNo1);
	//DDX_Control(pDX,IDC_BTN_TAB1_GENERATE_ABS,Btn_GENERATE_ABS);
	DDX_Control(pDX,IDC_BTN_TAB1_ORI_PLAY,Btn_BTN1_PLAY);
	DDX_Control(pDX,IDC_BTN_TAB1_ORI_PAUSE,Btn_BTN1_PAUSE);
	DDX_Control(pDX,IDC_BTN_TAB1_ORI_STOP,Btn_BTN1_STOP);
	DDX_Control(pDX,IDC_BTN_TAB1_ABS_PLAY,Btn_BTN2_PLAY);
	DDX_Control(pDX,IDC_BTN_TAB1_ABS_PAUSE,Btn_BTN2_PAUSE);
	DDX_Control(pDX,IDC_BTN_TAB1_ABS_STOP,Btn_BTN2_STOP);
}


BEGIN_MESSAGE_MAP(Dialog1, CDialogEx)
	//ON_BN_CLICKED(IDC_BUTTON9, &Dialog1::OnBnClickedButton9)

//	ON_WM_TIMER()
ON_WM_TIMER()
ON_BN_CLICKED(IDC_BTN_TAB1_ORI_PAUSE, &Dialog1::OnBnClickedBtnTab1OriPause)
ON_BN_CLICKED(IDC_BTN_TAB1_ORI_STOP, &Dialog1::OnBnClickedBtnTab1OriStop)
ON_BN_CLICKED(IDC_BTN_TAB1_ABS_STOP, &Dialog1::OnBnClickedBtnTab1AbsStop)
ON_BN_CLICKED(IDC_BTN_TAB1_ABS_PLAY, &Dialog1::OnBnClickedBtnTab1AbsPlay)
ON_BN_CLICKED(IDC_BTN_TAB1_ABS_PAUSE, &Dialog1::OnBnClickedBtnTab1AbsPause)
ON_BN_CLICKED(IDC_BTN_TAB1_GENERATE_ABS, &Dialog1::OnBnClickedBtnTab1GenerateAbs)
ON_BN_CLICKED(IDC_BTN_TAB1_ORI_PLAY, &Dialog1::OnBnClickedBtnTab1OriPlay)
ON_BN_CLICKED(IDC_CHECK_TAB1, &Dialog1::OnBnClickedCheckTab1)
//ON_WM_PAINT()
ON_WM_QUERYDRAGICON()
END_MESSAGE_MAP()


// Dialog1 消息处理程序



//播放器时间显示功能
void Dialog1::ShowTime(int m_currentFrameNO, int totalFrameCount,
	int videoTimeInSecond, CWnd *m_pShowTimeWnd, int flag)
{
	CString CurVideoTime;
	if (flag == 1)
	{
		int CurrentTimeInSecond = (double)m_currentFrameNO / double(totalFrameCount)*videoTimeInSecond;
		int CurVedioHour = CurrentTimeInSecond / 3600;
		int CurVedioMinute = CurrentTimeInSecond / 60 - CurVedioHour * 60;
		int CurVedioSecond = CurrentTimeInSecond - CurVedioHour * 3600 - CurVedioMinute * 60;

		int VedioHour = videoTimeInSecond / 3600;
		int VedioMinute = videoTimeInSecond / 60 - VedioHour * 60;
		int VedioSecond = videoTimeInSecond - VedioHour * 3600 - VedioMinute * 60;

		CurVideoTime.Format(" %d:%d:%d / %d:%d:%d ", CurVedioHour, CurVedioMinute, CurVedioSecond, VedioHour, VedioMinute, VedioSecond);
		m_pShowTimeWnd->SetWindowText(CurVideoTime);
	}
	else if (flag == 0)
	{
		CurVideoTime.Format(" %d:%d:%d / %d:%d:%d ", 0, 0, 0, 0, 0, 0);
		m_pShowTimeWnd->SetWindowText(CurVideoTime);
	}

}


/*void Dialog1::OnBnClickedButton9()
{
	// TODO:  在此添加控件通知处理程序代码
	CFileDialog dlg(TRUE, NULL, NULL, OFN_HIDEREADONLY, _T("Files (*.mp3 *.wma *.dat *.wmv *.avi *.mov *.mmm *.mid *.mpeg)|*.mp3;*.wma;*.dat;*.wmv;*.avi;*.mov;*.mmm;*.mid;*.mpeg|All Files (*.*)|*.*||"), NULL);
	if (dlg.DoModal() == IDOK)
	{
		if (Tab1_Player1.m_currentFrameNO != 0)
		{
			Tab1_Player1.stopPlay();
		}
		Tab1_PathName = dlg.GetPathName();
		Tab1_PathName.MakeUpper();

		Tab1_Player1.m_filePath = Tab1_PathName;
		Tab1_Player1.playInitial(GetDlgItem(IDC_STATIC_TAB1_ORI), "displayWindow1");//该初始化需要在文件路径确认后完成

		Tab1_CSliderPlayer1Ctrl.SetRange(0, Tab1_Player1.m_endFrameNO);
		Tab1_CSliderPlayer1Ctrl.SetPos(0);

		Tab1_Player1.play();

		CString m_pFilePath = Tab1_PathName;
		CString m_tmpFileName1, m_tmpFileName2;
		GetVideoNameFromAbsolutePath1(&m_pFilePath, &m_tmpFileName1);///<获取文件名(包含后缀)
		GetFileNameBeforeDot(&m_tmpFileName1, &m_tmpFileName2);        ///<获取文件名(不含后缀)
		Tab1_videoPro->m_tableParams.VideoFGTableName.Format("%s_VideoFGTable", m_tmpFileName2);
		Tab1_videoPro->m_tableParams.FGTraceTableName.Format("%s_FGTraceTable", m_tmpFileName2);
		Tab1_videoPro->m_tableParams.NewToOldFrameTableName.Format("%s_New2OldFrameTable", m_tmpFileName2);
		Tab1_videoPro->m_tableParams.NewTraceTable.Format("%s_NewTraceTable", m_tmpFileName2);
		Tab1_videoPro->m_tableParams.CombineSegsTableName.Format("%s_CombineSegsTable", m_tmpFileName2);

	}
}
*/


 







void Dialog1::OnTimer(UINT_PTR nIDEvent)
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值

	//CDialogEx::OnTimer(nIDEvent);
	switch (nIDEvent)
	{
	case 1://player1的滑动条
		{
		if (Tab1_playpiece)
		{
			CSliderCtrl * SliderCtrl = ((CSliderCtrl*)GetDlgItem(IDC_SLIDER1_TAB1));
			SliderCtrl->SetPos(Tab1_Player1.m_currentFrameNO);
		}
		else
		{
			CSliderCtrl * SliderCtrl = ((CSliderCtrl*)GetDlgItem(IDC_SLIDER1_TAB1));
			int CurrentPos = SliderCtrl->GetPos();                            ///<获取当前进度条位置
			int NewCurrentFrameNO = (double)CurrentPos*double(Tab1_Player1.m_endFrameNO - Tab1_Player1.m_startFrameNO)
				/ (double)SliderCtrl->GetRangeMax();     ///<根据当前进度条位置，重新计算帧号

			if (Tab1_Player1.m_playState != PLAY_STATE_STOP && Tab1_Player1.m_playState != PLAY_STATE_FAST_FORWORD &&Tab1_Player1.m_playState != PLAY_STATE_PLAY_PIECE)///<若播放器处于正常播放或暂停状态
			{
				if (abs(NewCurrentFrameNO - Tab1_Player1.m_currentFrameNO)>5)
				{
					Tab1_Player1.m_currentFrameNO = NewCurrentFrameNO;             ///<根据新计算的帧号，重置播放器
				}
			}
			SliderCtrl->SetPos(Tab1_Player1.m_currentFrameNO);                   ///<根据新计算的帧号，重置滑动条
			CWnd *pWndTimeDis = GetDlgItem(IDC_STATIC_TAB1_ORI_TIME);
			if (Tab1_Player1.m_playState != PLAY_STATE_STOP)                     ///<非停止状态下，用方式一显示时间
			{
				ShowTime(Tab1_Player1.m_currentFrameNO, Tab1_Player1.m_endFrameNO - Tab1_Player1.m_startFrameNO,
					Tab1_Player1.m_videoTimeInSecond, pWndTimeDis, 1);
			}
			else                                                            ///<停止状态下，用方式二显示时间
			{
				SliderCtrl->SetPos(0);
				ShowTime(Tab1_Player1.m_currentFrameNO, Tab1_Player1.m_endFrameNO - Tab1_Player1.m_startFrameNO,
					Tab1_Player1.m_videoTimeInSecond, pWndTimeDis, 0);
			}
		}
	    }

	case 2://player2的滑动条
	{
		if (Tab1_playpiece)
		{
			CSliderCtrl * SliderCtrl = ((CSliderCtrl*)GetDlgItem(IDC_SLIDER2_TAB1));
			SliderCtrl->SetPos(Tab1_Player2.m_currentFrameNO);
		}
		else
		{
			CSliderCtrl * SliderCtrl = ((CSliderCtrl*)GetDlgItem(IDC_SLIDER2_TAB1));
			int CurrentPos = SliderCtrl->GetPos();                            ///<获取当前进度条位置
			int NewCurrentFrameNO = (double)CurrentPos*double(Tab1_Player2.m_endFrameNO - Tab1_Player2.m_startFrameNO)
				/ (double)SliderCtrl->GetRangeMax();     ///<根据当前进度条位置，重新计算帧号

			if (Tab1_Player2.m_playState != PLAY_STATE_STOP && Tab1_Player2.m_playState != PLAY_STATE_FAST_FORWORD &&Tab1_Player2.m_playState != PLAY_STATE_PLAY_PIECE)///<若播放器处于正常播放或暂停状态
			{
				if (abs(NewCurrentFrameNO - Tab1_Player2.m_currentFrameNO)>5)
				{
					Tab1_Player2.m_currentFrameNO = NewCurrentFrameNO;             ///<根据新计算的帧号，重置播放器
				}
			}
			SliderCtrl->SetPos(Tab1_Player2.m_currentFrameNO);                   ///<根据新计算的帧号，重置滑动条
			CWnd *pWndTimeDis = GetDlgItem(IDC_STATIC_TAB1_ABS_TIME);
			if (Tab1_Player2.m_playState != PLAY_STATE_STOP)                     ///<非停止状态下，用方式一显示时间
			{
				ShowTime(Tab1_Player2.m_currentFrameNO, Tab1_Player2.m_endFrameNO - Tab1_Player2.m_startFrameNO,
					Tab1_Player2.m_videoTimeInSecond, pWndTimeDis, 1);
			}
			else                                                            ///<停止状态下，用方式二显示时间
			{
				SliderCtrl->SetPos(0);
				ShowTime(Tab1_Player2.m_currentFrameNO, Tab1_Player2.m_endFrameNO - Tab1_Player2.m_startFrameNO,
					Tab1_Player2.m_videoTimeInSecond, pWndTimeDis, 0);
			}
		}
	}
	case 3:///<刷进度条
	{
		if (!Tab1_ifStartPro)
		{
			Tab1_Pro->SetPos(0);
			SetDlgItemText(IDC_STATIC_TAB1_PRO, "");//helloworld
		}
		else if (Tab1_videoPro->m_IfDoneFus)///<如果已经生成摘要则将ClistCtrl中响应的行显示为“已生成摘要”
		{
			Tab1_ifStartPro = false;
			MessageBox("已生成摘要");
		}
		///如果正在生成还未生成完毕,刷新进度条上方的显示处理阶段
		else if (Tab1_videoPro->m_IfContinue)
		{
			int VideoPos = (int)Tab1_videoPro->ProcessingSpeed();
			if (VideoPos == 0)
			{
				//SetDlgItemText(IDC_STATIC_TAB1_PRO, "视频预处理……");
				if(Tab1_videoPro->StepBitmapDefault[0] == false)
				{
					CBitmap *m_bmp=new CBitmap;
					m_bmp->LoadBitmap(IDB_STEP1);
					HBITMAP hBmp;// = HBITMAP(m_bmp);//将CBitmap转换为HBITMAP
					hBmp=(HBITMAP)(*m_bmp);
					((CStatic *)GetDlgItem(IDC_STATIC_TAB1_ABS))->SetBitmap(hBmp);
					Tab1_videoPro->StepBitmapDefault[0] = true;
				}				
			}
			if (VideoPos == 1)
			{
				//SetDlgItemText(IDC_STATIC_TAB1_PRO, "生成切分视频……");
				if(Tab1_videoPro->StepBitmapDefault[1] == false)
				{
					CBitmap *m_bmp=new CBitmap;
					m_bmp->LoadBitmap(IDB_STEP2);
					HBITMAP hBmp;// = HBITMAP(m_bmp);//将CBitmap转换为HBITMAP
					hBmp=(HBITMAP)(*m_bmp);
					((CStatic *)GetDlgItem(IDC_STATIC_TAB1_ABS))->SetBitmap(hBmp);
					Tab1_videoPro->StepBitmapDefault[1] = true;
				}		
			}
			if (VideoPos == 2)
			{
				//SetDlgItemText(IDC_STATIC_TAB1_PRO, "视频背景处理……");
				if(Tab1_videoPro->StepBitmapDefault[2] == false)
				{
					CBitmap *m_bmp=new CBitmap;
					m_bmp->LoadBitmap(IDB_STEP3);
					HBITMAP hBmp;// = HBITMAP(m_bmp);//将CBitmap转换为HBITMAP
					hBmp=(HBITMAP)(*m_bmp);
					((CStatic *)GetDlgItem(IDC_STATIC_TAB1_ABS))->SetBitmap(hBmp);
					Tab1_videoPro->StepBitmapDefault[2] = true;
				}	
			}
			if (VideoPos == 3)
			{
				//SetDlgItemText(IDC_STATIC_TAB1_PRO, "视频前景处理……");
				if(Tab1_videoPro->StepBitmapDefault[3] == false)
				{
					CBitmap *m_bmp=new CBitmap;
					m_bmp->LoadBitmap(IDB_STEP4);
					HBITMAP hBmp;// = HBITMAP(m_bmp);//将CBitmap转换为HBITMAP
					hBmp=(HBITMAP)(*m_bmp);
					((CStatic *)GetDlgItem(IDC_STATIC_TAB1_ABS))->SetBitmap(hBmp);
					Tab1_videoPro->StepBitmapDefault[3] = true;
				}	
			}
			if (VideoPos == 4)
			{
				//SetDlgItemText(IDC_STATIC_TAB1_PRO, "视频融合……");
				if(Tab1_videoPro->StepBitmapDefault[4] == false)
				{
					CBitmap *m_bmp=new CBitmap;
					m_bmp->LoadBitmap(IDB_STEP5);
					HBITMAP hBmp;// = HBITMAP(m_bmp);//将CBitmap转换为HBITMAP
					hBmp=(HBITMAP)(*m_bmp);
					((CStatic *)GetDlgItem(IDC_STATIC_TAB1_ABS))->SetBitmap(hBmp);
					Tab1_videoPro->StepBitmapDefault[4] = true;
				}	
			}
			Tab1_Pro->SetPos((int)(Tab1_videoPro->ProcessingSpeed()*100.0));
		}
		else
		{
			if ((int)Tab1_videoPro->ProcessingSpeed() == -1)
			{
				Tab1_ifStartPro = false;
				MessageBox("视频时长过短，无需融合处理");

			}
		}
	}
	case 4://摘要窗口的左键单击
	{
		if (Tab1_Player2.m_gotCVlclick == TRUE)  //&& Abstractplayer.m_playState == PLAY_STATE_PAUSE
		{
			Tab1_clickedObjRecPosInVec = -1;
			CPoint posInWnd = Tab1_Player2.m_lBtnUpPosInCVWnd;///<播放器对象传出的点击坐标，此时的点击位置已经是原始帧尺寸的坐标
			double clickPosXInImg = double(posInWnd.x);
			double clickPosYInImg = double(posInWnd.y);///<注意此时的点击位置已经是原始帧尺寸的坐标
			Tab1_distanceToRecCenter = 1000000;

			Tab1_objectRectVector.clear();
			///<从数据库中读取ROI信息
			if (!Tab1_MysqlHandle->FindROIFromNewTraceTable(Tab1_Player2.m_currentFrameNO, &Tab1_objectRectVector, Tab1_videoPro->m_tableParams.NewTraceTable))//////"00015_h_NewTraceTable"
			{
				AfxMessageBox("数据库出错");
				return;
			}

			for (int i = 0; i<Tab1_objectRectVector.size(); i++)///<遍历所有ROI目标框
			{
				if (clickPosXInImg < Tab1_objectRectVector[i].x + Tab1_objectRectVector[i].width && clickPosXInImg >Tab1_objectRectVector[i].x///<判断点击是否在当前框内
					&& clickPosYInImg < Tab1_objectRectVector[i].y + Tab1_objectRectVector[i].height && clickPosYInImg > Tab1_objectRectVector[i].y)
				{
					int tempDistance = (clickPosXInImg - (Tab1_objectRectVector[i].x + Tab1_objectRectVector[i].width / 2))*
						(clickPosXInImg - (Tab1_objectRectVector[i].x + Tab1_objectRectVector[i].width / 2)) +
						(clickPosYInImg - (Tab1_objectRectVector[i].y + Tab1_objectRectVector[i].height / 2))*
						(clickPosYInImg - (Tab1_objectRectVector[i].y + Tab1_objectRectVector[i].height / 2));///<计算点击位置与当前ROI中心的距离

					if (tempDistance < Tab1_distanceToRecCenter)
					{
						Tab1_distanceToRecCenter = tempDistance;///<当前距离小于记录的最小距离，则更新这个最小距离
						Tab1_clickedObjRecPosInVec = i;         ///<并记录当前ROI在向量中的下标值
					}
				}
			}
			if (Tab1_clickedObjRecPosInVec == -1)
			{
				return;
			}
			/*if (m_theObjRecClicked != m_objectRectVector[m_clickedObjRecPosInVec])
			{*/
			Tab1_theObjRecClicked = Tab1_objectRectVector[Tab1_clickedObjRecPosInVec];
			int m_theorigFrame = 0;
			if (Tab1_videoPro != NULL)
			{
				int m_origPara = Tab1_MysqlHandle->FindOrigParaFromFGTraceTable(Tab1_videoPro->m_tableParams.FGTraceTableName, Tab1_theObjRecClicked);
				m_theorigFrame = Tab1_MysqlHandle->FindOrigFrameFromVideoFGTable(Tab1_videoPro->m_tableParams.VideoFGTableName, m_origPara);//m_videoPro->m_tableParams.NewTraceTable

				//m_theorigFrame = m_MysqlVideoParaSearchHandle->FindOrigFrameFromNewTraceTable(m_videoPro->m_tableParams.NewTraceTable,m_theObjRecClicked);//m_videoPro->m_tableParams.NewTraceTable
			}

			if (m_theorigFrame == -1)
			{
				MessageBox("数据库有误");
				Tab1_Player2.m_gotCVlclick = FALSE;
				return;
			};
			//else
			//{
			//	Originalplayer.m_startFrameNO = MAX(0,m_theorigFrame-500);///<未能得到帧号，则默认播放前500帧和后1000帧
			//	Originalplayer.m_endFrameNO   = Originalplayer.m_startFrameNO + 1000;
			//}

			//player1.m_filePath = absOriNameTansf(Abstractplayer.m_filePath);				
			Tab1_Player1.m_currentFrameNO = m_theorigFrame;                                 ///<初始化原始视频播放器
			//}
			Tab1_playpiece = TRUE;
			Tab1_Player2.m_gotCVlclick = FALSE;                                       ///<点击比对完毕，将得到点击标志位置否
		}
	}



	default:
		break;
	}
}


BOOL Dialog1::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	
	UIBeautifier.LoadBackgroundPic(IDB_background);///<背景位图加载
	CWnd* pWnd = CWnd::GetDesktopWindow();
	pWnd->GetClientRect(UIBeautifier.rcDeskRect);///获取桌面矩形
	BitmapBackGroundStrech=UIBeautifier.CBitmapResize(UIBeautifier.m_BitmapBackGround,UIBeautifier.rcDeskRect.Width(),UIBeautifier.rcDeskRect.Height());

	GetDlgItem(IDC_STATIC_TAB1_ORI)->MoveWindow(50.0*UIBeautifier.rcDeskRect.Width()/1214,30.0*UIBeautifier.rcDeskRect.Height()/760,350.0*UIBeautifier.rcDeskRect.Width()/1214,262.5*UIBeautifier.rcDeskRect.Height()/760);
	GetDlgItem(IDC_STATIC_TAB1_ABS)->MoveWindow(500.0*UIBeautifier.rcDeskRect.Width()/1214,30.0*UIBeautifier.rcDeskRect.Height()/760,600.0*UIBeautifier.rcDeskRect.Width()/1214,450.0*UIBeautifier.rcDeskRect.Height()/760);
	GetDlgItem(IDC_SLIDER1_TAB1)->MoveWindow(50.0*UIBeautifier.rcDeskRect.Width()/1214,302.5*UIBeautifier.rcDeskRect.Height()/760,350.0*UIBeautifier.rcDeskRect.Width()/1214,20.0*UIBeautifier.rcDeskRect.Height()/760);
	GetDlgItem(IDC_STATIC_TAB1_ORI_TIME)->MoveWindow(300.0*UIBeautifier.rcDeskRect.Width() / 1214, 332.5*UIBeautifier.rcDeskRect.Height() / 760, 80.0*UIBeautifier.rcDeskRect.Width() / 1214, 30.0*UIBeautifier.rcDeskRect.Height() / 760);
	GetDlgItem(IDC_STATIC_TAB1_PRO)->MoveWindow(50.0*UIBeautifier.rcDeskRect.Width()/1214,460.0*UIBeautifier.rcDeskRect.Height()/760,100.0*UIBeautifier.rcDeskRect.Width()/1214,20.0*UIBeautifier.rcDeskRect.Height()/760);
	GetDlgItem(IDC_PROGRESS_TAB1)->MoveWindow(50.0*UIBeautifier.rcDeskRect.Width()/1214,392.5*UIBeautifier.rcDeskRect.Height()/760,350.0*UIBeautifier.rcDeskRect.Width()/1214,20.0*UIBeautifier.rcDeskRect.Height()/760);
	GetDlgItem(IDC_SLIDER2_TAB1)->MoveWindow(500.0*UIBeautifier.rcDeskRect.Width()/1214,490.0*UIBeautifier.rcDeskRect.Height()/760,600.0*UIBeautifier.rcDeskRect.Width()/1214,20.0*UIBeautifier.rcDeskRect.Height()/760);
	GetDlgItem(IDC_STATIC_TAB1_ABS_TIME)->MoveWindow(830.0*UIBeautifier.rcDeskRect.Width()/1214,530.0*UIBeautifier.rcDeskRect.Height()/760,80.0*UIBeautifier.rcDeskRect.Width()/1214,30.0*UIBeautifier.rcDeskRect.Height()/760);
	GetDlgItem(IDC_CHECK_TAB1)->MoveWindow(1010.0*UIBeautifier.rcDeskRect.Width()/1214,520.0*UIBeautifier.rcDeskRect.Height()/760,80.0*UIBeautifier.rcDeskRect.Width()/1214,30.0*UIBeautifier.rcDeskRect.Height()/760);
	//GetDlgItem(IDC_BTN_TAB1_VIEW_ABS)->MoveWindow(200.0*UIBeautifier.rcDeskRect.Width()/1214,545.0*UIBeautifier.rcDeskRect.Height()/760,50.0*UIBeautifier.rcDeskRect.Width()/1214,20.0*UIBeautifier.rcDeskRect.Height()/760);
	GetDlgItem(IDC_BTN_TAB1_GENERATE_ABS)->MoveWindow(125.0*UIBeautifier.rcDeskRect.Width()/1214,500.0*UIBeautifier.rcDeskRect.Height()/760,200.0*UIBeautifier.rcDeskRect.Width()/1214,30.0*UIBeautifier.rcDeskRect.Height()/760);

	//UIBeautifier.LoadButtonBitmaps(Btn_GENERATE_ABS,IDB_BITMAP1,IDB_BITMAP3,1118.0/1214,10.0/760,1200.0/1214,40.0/760);
	UIBeautifier.LoadButtonBitmaps(Btn_BTN1_PLAY,IDB_PLAY_U,IDB_PLAY_D,50.0/1214,332.5/760,80.0/1214,362.5/760);
	UIBeautifier.LoadButtonBitmaps(Btn_BTN1_PAUSE, IDB_PAUSE_U, IDB_PAUSE_D, 100.0 / 1214, 332.5 / 760, 130.0 / 1214, 362.5 / 760);
	UIBeautifier.LoadButtonBitmaps(Btn_BTN1_STOP, IDB_STOP_U, IDB_STOP_D, 150.0 / 1214, 332.5 / 760, 180.0 / 1214, 362.5 / 760);
	UIBeautifier.LoadButtonBitmaps(Btn_BTN2_PLAY,IDB_PLAY_U,IDB_PLAY_D,500.0/1214,520.0/760,530.0/1214,550.0/760);
	UIBeautifier.LoadButtonBitmaps(Btn_BTN2_PAUSE,IDB_PAUSE_U,IDB_PAUSE_D,550.0/1214,520.0/760,580.0/1214,550.0/760);
	UIBeautifier.LoadButtonBitmaps(Btn_BTN2_STOP,IDB_STOP_U,IDB_STOP_D,600.0/1214,520.0/760,630.0/1214,550.0/760);

	// TODO:  在此添加额外的初始化
	SetTimer(1, 100, NULL);///<计数器初始化
	SetTimer(2, 200, NULL);
	SetTimer(4, 300, NULL);

	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	Tab1_Pro = (CProgressCtrl*)GetDlgItem(IDC_PROGRESS_TAB1);
	Tab1_Pro->SetRange(0, 500);
	Tab1_Pro->SetPos(0);

	Tab1_CSliderPlayer1Ctrl.SetRange(0, Tab1_Player1.m_endFrameNO);   ///<初始化窗口一的滑动条
	Tab1_CSliderPlayer1Ctrl.SetPos(0);

	Tab1_CSliderPlayer2Ctrl.SetRange(0, Tab1_Player2.m_endFrameNO);   ///<初始化窗口二的滑动条
	Tab1_CSliderPlayer2Ctrl.SetPos(0);

	DisplayFrame initImage;
	IplImage* temp_image = cvLoadImage(".\\res\\ori-default.png", CV_LOAD_IMAGE_COLOR);
	CRect rect1;
	GetDlgItem(IDC_STATIC_TAB1_ORI)->GetClientRect(&rect1);
	initImage.SetOpenCVWindow(GetDlgItem(IDC_STATIC_TAB1_ORI), "displayWindow1tmp",rect1.Width(), rect1.Height());
	if (temp_image != NULL) // Check for invalid input
	{
		initImage.ShowPicture("displayWindow1tmp", temp_image);
		cvReleaseImage(&temp_image);
	}

	//temp_image = cvLoadImage(".\\res\\abs-default.png", CV_LOAD_IMAGE_COLOR);
	//GetDlgItem(IDC_STATIC_TAB1_ABS)->GetClientRect(&rect1);
	//initImage.SetOpenCVWindow(GetDlgItem(IDC_STATIC_TAB1_ABS), "displayWindow2tmp",rect1.Width(), rect1.Height());
	//if (temp_image != NULL) // Check for invalid input
	//{
	//	initImage.ShowPicture("displayWindow2tmp", temp_image);
	//	cvReleaseImage(&temp_image);
	//}
	CBitmap *m_bmp2=new CBitmap;
	m_bmp2->LoadBitmap(IDB_ABS);
	HBITMAP hBmp2;// = HBITMAP(m_bmp);//将CBitmap转换为HBITMAP
	hBmp2=(HBITMAP)(*m_bmp2);
	((CStatic *)GetDlgItem(IDC_STATIC_TAB1_ABS))->SetBitmap(hBmp2);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常:  OCX 属性页应返回 FALSE
}


void Dialog1::OnBnClickedBtnTab1OriPause()
{
	// TODO:  在此添加控件通知处理程序代码
	Tab1_Player1.pause();
}


void Dialog1::OnBnClickedBtnTab1OriStop()
{
	// TODO:  在此添加控件通知处理程序代码
	Tab1_Player1.stop();
}


void Dialog1::OnBnClickedBtnTab1AbsPlay()
{
	// TODO:  在此添加控件通知处理程序代码
	if (Tab1_ifStartPro){
		AfxMessageBox("生在生成摘要，请稍后操作");
		return;
	}
	if (Tab1_Player2.m_filePath != "")
	{
		Tab1_CSliderPlayer2Ctrl.SetRange(0, Tab1_Player2.m_endFrameNO);///<滑动条初始化

		///<时间显示准备
		CString VideoTime;
		int VedioHour = Tab1_Player2.m_videoTimeInSecond / 3600;
		int VedioMinute = Tab1_Player2.m_videoTimeInSecond / 60 - VedioHour * 60;
		int VedioSecond = Tab1_Player2.m_videoTimeInSecond - VedioHour * 3600 - VedioMinute * 60;

		VideoTime.Format(" 0:0:0 / %d:%d:%d ", VedioHour, VedioMinute, VedioSecond);
		GetDlgItem(IDC_STATIC_TAB1_ABS_TIME)->SetWindowText(VideoTime);///<将时间显示到控件上

		Tab1_Player2.play();

	}
	else
	{
		AfxMessageBox("请选中文件");
	}
}


void Dialog1::OnBnClickedBtnTab1AbsPause()
{
	// TODO:  在此添加控件通知处理程序代码
	Tab1_Player2.pause();
}


void Dialog1::OnBnClickedBtnTab1AbsStop()
{
	// TODO:  在此添加控件通知处理程序代码
	Tab1_Player2.stop();
}


void Dialog1::OnBnClickedBtnTab1GenerateAbs()
{
	// TODO:  在此添加控件通知处理程序代码
	CString a = "\\";
	LPCTSTR m = a;
	CString b = "/";
	LPCTSTR n = b;
	SetTimer(3, 200, NULL);
	if (Tab1_ifStartPro)
	{
		MessageBox("正在生成摘要，请等待");
		return;
	}
	if (Tab1_PathName != "")
	{
		CString m_tmpFileName1, m_tmpFileName2;
		GetVideoNameFromAbsolutePath1(&Tab1_Player1.m_filePath, &m_tmpFileName1);///<获取文件名(包含后缀)
		GetFileNameBeforeDot(&m_tmpFileName1, &m_tmpFileName2);        ///<获取文件名(不含后缀)
		CString path = "../" + m_tmpFileName2 + "/" + m_tmpFileName2 + "_fusion.avi";
		//通过文件名判断摘要是否已经生成
		if (_access(path, 0) == -1)
		{
			Tab1_videoPro->m_IfContinue = true;
			Tab1_videoPro->m_IfDoneFus = false;
			Tab1_ifStartPro = true;
			//player2.stop();///停止播放
			Tab1_Player2.stopPlay();///关闭进程
			Tab1_PathName.Replace(*m, *n);
			Tab1_videoPro->DoProcessing((LPSTR)(LPCTSTR)Tab1_PathName);
			return;
		}
		else{
			Tab1_videoPro->m_IfDoneFus = true;
			ifRebuildDlg Dlg;	//提示是否重新生成
			Dlg.DoModal();
			if (Dlg.ifrebuild){
				Tab1_videoPro->m_IfContinue = true;
				Tab1_videoPro->m_IfDoneFus = false;
				Tab1_ifStartPro = true;
				//player2.stop();///停止播放
				Tab1_Player2.stopPlay();///关闭进程
				Tab1_PathName.Replace(*m, *n);
				Tab1_videoPro->DoProcessing((LPSTR)(LPCTSTR)Tab1_PathName);
			}
			return;
		}

	}
}


void Dialog1::ViewAbs()
{
	// TODO:  在此添加控件通知处理程序代码
	if (Tab1_ifStartPro)
	{
		::MessageBox(NULL, _T("摘要尚未处理完成"), _T("error!"), MB_OK);
		return;
	}

	if (Tab1_Player1.m_currentFrameNO != 0)
	{
		Tab1_Player2.stopPlay();
	}
	CString m_tmpFileName1, m_tmpFileName2;
	GetVideoNameFromAbsolutePath1(&Tab1_Player1.m_filePath, &m_tmpFileName1);///<获取文件名(包含后缀)
	GetFileNameBeforeDot(&m_tmpFileName1, &m_tmpFileName2);        ///<获取文件名(不含后缀)
	CString path = "../" + m_tmpFileName2 + "/" + m_tmpFileName2 + "_fusion.avi";
	Tab1_Player2.m_filePath = path;
	if (_access(path, 0) == -1)
	{
		AfxMessageBox("找不到文件");
		return;
	}


	/*m_pageNum = 0;
	m_pageSum = 2;*/
	/*m_tableParams.CombineSegsTableName.Format("%s_CombineSegsTable", m_tmpFileName2);
	m_tableParams.FGTraceTableName.Format("%s_FGTraceTable", m_tmpFileName2);
	Generated1 = TRUE;
	DisplayPageSumImgnew();*/
	//SetTimer(5, 600, NULL); //新加载的Picture Control的计时器

	Tab1_Player2.playInitial(GetDlgItem(IDC_STATIC_TAB1_ABS), "displayWindow2");//该初始化需要在文件路径确认后完成
	Tab1_CSliderPlayer2Ctrl.SetRange(0, Tab1_Player2.m_endFrameNO);
	Tab1_CSliderPlayer2Ctrl.SetPos(0);

	Tab1_Player2.play();
	Tab1_Player2.m_MysqlVideoParaSearchHandle = Tab1_MysqlHandle;
	Tab1_Player2.m_videoPro = Tab1_videoPro;
	Tab1_Player2.timeshow = FALSE;
}


void Dialog1::OnBnClickedBtnTab1OriPlay()
{
	// TODO:  在此添加控件通知处理程序代码
	if (Tab1_Player1.m_filePath != "")
	{
		Tab1_CSliderPlayer1Ctrl.SetRange(0, Tab1_Player1.m_endFrameNO);///<滑动条初始化

		///<时间显示准备
		CString VideoTime;
		int VedioHour = Tab1_Player1.m_videoTimeInSecond / 3600;
		int VedioMinute = Tab1_Player1.m_videoTimeInSecond / 60 - VedioHour * 60;
		int VedioSecond = Tab1_Player1.m_videoTimeInSecond - VedioHour * 3600 - VedioMinute * 60;

		VideoTime.Format(" 0:0:0 / %d:%d:%d ", VedioHour, VedioMinute, VedioSecond);
		GetDlgItem(IDC_STATIC_TAB1_ORI_TIME)->SetWindowText(VideoTime);///<将时间显示到控件上

		Tab1_Player1.play();

	}
	else
	{
		AfxMessageBox("请选中文件");
	}

}


void Dialog1::OnBnClickedCheckTab1()
{
	// TODO:  在此添加控件通知处理程序代码
	if (((CButton *)GetDlgItem(IDC_CHECK_TAB1))->GetCheck())
	{
		Tab1_Player2.timeshow = TRUE;
		Tab1_Player2.d_AbsVideoTimeRatio = (Tab1_Player1.m_endFrameNO - Tab1_Player1.m_startFrameNO) / Tab1_Player1.m_videoTimeInSecond;//totalFrameCount--总帧数*videoTimeInSecond--视频时间长度（s）
	}
	else
	{
		Tab1_Player2.timeshow = FALSE;
	}
}


//void Dialog1::OnPaint()
//{
//	if (IsIconic())
//	{
//		CPaintDC dc(this); // 用于绘制的设备上下文
//
//		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);
//
//		// 使图标在工作区矩形中居中
//		int cxIcon = GetSystemMetrics(SM_CXICON);
//		int cyIcon = GetSystemMetrics(SM_CYICON);
//		CRect rect;
//		GetClientRect(&rect);
//		int x = (rect.Width() - cxIcon + 1) / 2;
//		int y = (rect.Height() - cyIcon + 1) / 2;
//
//		// 绘制图标
//		dc.DrawIcon(x, y, m_hIcon);
//	}
//	else
//	{
//		this->MoveWindow(UIBeautifier.rcDeskRect.left, UIBeautifier.rcDeskRect.top, UIBeautifier.rcDeskRect.Width(), UIBeautifier.rcDeskRect.Height(), TRUE); ///<给选项卡定位
//		CDialogEx::OnPaint();
//	}
//}


HCURSOR Dialog1::OnQueryDragIcon()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	return static_cast<HCURSOR>(m_hIcon);
}


void Dialog1::OnCancel()
{
	// TODO: 在此添加专用代码和/或调用基类

	return;
}
