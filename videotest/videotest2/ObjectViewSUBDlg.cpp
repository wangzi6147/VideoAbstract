// ObjectViewSUBDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "videotest2.h"
#include "ObjectViewSUBDlg.h"
#include "afxdialogex.h"
#include "CvvImage.h"
#include "publicheader.h"


// ObjectViewSUBDlg 对话框

IMPLEMENT_DYNAMIC(ObjectViewSUBDlg, CDialog)

ObjectViewSUBDlg::ObjectViewSUBDlg(CWnd* pParent /*=NULL*/)
	: CDialog(ObjectViewSUBDlg::IDD, pParent)
{
	m_pageSum=1;//显示4张图
	m_pageNum = 0;
	UserClick=FALSE;
	Generated=FALSE;
}

ObjectViewSUBDlg::~ObjectViewSUBDlg()
{
	
}

void ObjectViewSUBDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//DDX_Control(pDX, IDC_STATIC_OBJECT1, ObjectPicture_1);
	DDX_Control(pDX, IDC_SLIDER1,m_CSliderPlayerCtrl);
	DDX_Control(pDX, IDC_BTN_OBJECT_ORI_PLAY, BTN_OBJECT_ORI_PLAY);
	DDX_Control(pDX, IDC_BTN_OBJECT_ORI_PAUSE, BTN_OBJECT_ORI_PAUSE);
	DDX_Control(pDX, IDC_BTN_OBJECT_ORI_STOP, BTN_OBJECT_ORI_STOP);

}


BEGIN_MESSAGE_MAP(ObjectViewSUBDlg, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_OBJECT, &ObjectViewSUBDlg::OnBnClickedButtonObject)
	ON_STN_CLICKED(IDC_STATIC_OBJECT1, &ObjectViewSUBDlg::OnStnClickedStaticObject1)
	ON_BN_CLICKED(IDC_OBJECT_LEFT, &ObjectViewSUBDlg::OnBnClickedObjectLeft)
	ON_BN_CLICKED(IDC_OBJECT_RIGHT, &ObjectViewSUBDlg::OnBnClickedObjectRight)
	ON_STN_CLICKED(IDC_STATIC_OBJECT2, &ObjectViewSUBDlg::OnStnClickedStaticObject2)
	ON_STN_CLICKED(IDC_STATIC_OBJECT3, &ObjectViewSUBDlg::OnStnClickedStaticObject3)
	ON_STN_CLICKED(IDC_STATIC_OBJECT4, &ObjectViewSUBDlg::OnStnClickedStaticObject4)
	ON_WM_TIMER()
//	ON_WM_INITMENU()
//	ON_WM_INITMENUPOPUP()
//ON_WM_UNICHAR()
ON_WM_VSCROLL()
ON_BN_CLICKED(IDC_BUTTON1, &ObjectViewSUBDlg::OnBnClickedButton1)
ON_BN_CLICKED(IDC_BUTTON2, &ObjectViewSUBDlg::OnBnClickedButton2)
ON_BN_CLICKED(IDC_BUTTON3, &ObjectViewSUBDlg::OnBnClickedButton3)
//ON_WM_CLOSE()
ON_WM_CLOSE()
//ON_WM_CANCELMODE()
//ON_WM_PAINT()
END_MESSAGE_MAP()


// ObjectViewSUBDlg 消息处理程序

void ObjectViewSUBDlg::ShowImage( IplImage* img, UINT ID ,CString WName)    // ID 是Picture Control控件的ID号
{
	CWnd *m_pWnd = GetDlgItem(ID);
	CDC* pDC = GetDlgItem(ID) ->GetDC();        // 获得显示控件的 DC
	HDC hDC = pDC ->GetSafeHdc();                // 获取 HDC(设备句柄) 来进行绘图操作
	DisplayFrame disPlayImage;//用于内嵌播放窗口的对象

	CRect rect;
	GetDlgItem(ID) ->GetClientRect( &rect );
	int rw = rect.right - rect.left;            // 求出图片控件的宽和高
	int rh = rect.bottom - rect.top;
	int iw = img->width;                        // 读取图片的宽和高
	int ih = img->height;

	//float temp=max(iw/rw,ih/rh);
	//rw = (int)(iw/temp);		//应显示按比例的宽度
	//rh = (int)(ih/temp);	
	int tx = rect.left;
	int ty = rect.top;
	SetRect( rect, tx, ty, tx+rw, ty+rh );

	//disPlayImage.SetOpenCVWindow(m_pWnd,WName,
	//	rect.Width(), rect.Height());
	//disPlayImage.ShowPicture(WName, img);

	CvvImage cimg;
	cimg.CopyOf(img);
	cimg.DrawToHDC(hDC,&rect);
	ReleaseDC( pDC );
}


void ObjectViewSUBDlg::OnBnClickedButtonObject()
{
	// TODO: 在此添加控件通知处理程序代码
	m_pageNum = 0;
	Generated=TRUE;
	DetectResult();
	DisplayPageSumImg();
	//ObjectPlayer.play();
	pScrollBar->SetScrollRange(0,(objDetectedInfos.size()-3>0)?objDetectedInfos.size()-3:0);//滑块移动的位置为0——100；
	m_CSliderPlayerCtrl.SetRange(0, ObjectPlayer.m_endFrameNO);   ///<初始化窗口一的滑动条
	m_CSliderPlayerCtrl.SetPos(0);
}



void ObjectViewSUBDlg::DisplayPageSumImg()
{
	CvCapture *capture = cvCreateFileCapture(oriPlayer.m_filePath);
	if (!capture)
		return;

	int num = 4;//m_pageSum;
	if (objDetectedInfos.size()/m_pageSum > 4)		//若当前页不为最后一页
		num = 4;//m_pageSum;									
	else
		num = objDetectedInfos.size();			//为最后一页，显示剩余的图

	IplImage* image = NULL;
	CString nOldPara1,nOldPara2,nOldPara3,nOldPara4;
	for (int i = 0; i < 4; i++)/////原来为m_pageSum
	{	
		///显示图片
		if(i < num)											
		{
			///取出视频中该目标起始帧号的那一帧
			cvSetCaptureProperty(capture,CV_CAP_PROP_POS_FRAMES,objDetectedInfos.at(m_pageSum * m_pageNum +i).MidFrameID);
			image = cvQueryFrame(capture);
			if(image == NULL)
				continue;
			///在那一帧中画ROI区域标出目标位置
			CvRect rect = objDetectedInfos.at(m_pageNum * m_pageSum + i).roi;
			//cvRectangle(image,cvPoint(rect.x,rect.y),cvPoint(rect.x+rect.width,rect.y+rect.height),CV_RGB(200,200,0),5,8,0);
			cvSetImageROI(image,rect);

			///在缩略图中显示

			switch (i)
			{
			case 0:
				ShowImage(image,IDC_STATIC_OBJECT1,"displayWindow_1");
				nOldPara1.Format("%d",objDetectedInfos.at(m_pageNum * m_pageSum + i).nOldPara);
				GetDlgItem(IDC_STATIC_0)->SetWindowText(nOldPara1);
				break;
			case 1:
				ShowImage(image,IDC_STATIC_OBJECT2,"displayWindow_2");
				nOldPara2.Format("%d",objDetectedInfos.at(m_pageNum * m_pageSum + i).nOldPara);
				GetDlgItem(IDC_STATIC_1)->SetWindowText(nOldPara2);
				break;
			case 2:
				ShowImage(image,IDC_STATIC_OBJECT3,"displayWindow_3");
				nOldPara3.Format("%d",objDetectedInfos.at(m_pageNum * m_pageSum + i).nOldPara);
				GetDlgItem(IDC_STATIC_2)->SetWindowText(nOldPara3);
				break;
			case 3:
				ShowImage(image,IDC_STATIC_OBJECT4,"displayWindow_4");
				nOldPara4.Format("%d",objDetectedInfos.at(m_pageNum * m_pageSum + i).nOldPara);
				GetDlgItem(IDC_STATIC_3)->SetWindowText(nOldPara4);
				break;
			default:
				break;
			}
		}
		else											///若为最后一页，则用closeup.bmp填充
		{
			image = cvLoadImage("closeup.bmp",CV_LOAD_IMAGE_COLOR);
			switch (i)
			{
			case 0:
				ShowImage(image,IDC_STATIC_OBJECT1,"displayWindow_1");
				nOldPara1="";
				GetDlgItem(IDC_STATIC_0)->SetWindowText(nOldPara1);
				break;
			case 1:
				ShowImage(image,IDC_STATIC_OBJECT2,"displayWindow_2");
				nOldPara2="";
				GetDlgItem(IDC_STATIC_1)->SetWindowText(nOldPara2);
				break;
			case 2:
				ShowImage(image,IDC_STATIC_OBJECT3,"displayWindow_3");
				nOldPara3="";
				GetDlgItem(IDC_STATIC_2)->SetWindowText(nOldPara3);
				break;
			case 3:
				ShowImage(image,IDC_STATIC_OBJECT4,"displayWindow_4");
				nOldPara4="";
				GetDlgItem(IDC_STATIC_3)->SetWindowText(nOldPara4);
				break;
			default:
				break;
			}
		}

	}
	cvReleaseCapture(&capture);
}

void ObjectViewSUBDlg::DetectResult()
{
	m_MysqlVideoHandle->GetObjectInfoFromObjectTable(&objDetectedInfos,videoPro->m_tableParams.CombineSegsTableName);
	m_MysqlVideoHandle->GetObjectOriMidFrameFromVideoFgTable(&objDetectedInfos,videoPro->m_tableParams.FGTraceTableName);
}






void ObjectViewSUBDlg::OnStnClickedStaticObject1()
{
	// TODO: 在此添加控件通知处理程序代码
	if (Generated==TRUE)
	{	
		if(m_pageNum * m_pageSum + 0<objDetectedInfos.size())
		{
			OriFrame=objDetectedInfos.at(m_pageNum * m_pageSum + 0).OriFrameID;
			UserClick=TRUE;
		}
	}


}

void ObjectViewSUBDlg::OnStnClickedStaticObject2()
{
	// TODO: 在此添加控件通知处理程序代码
	if (Generated==TRUE)
	{	
		if(m_pageNum * m_pageSum + 1<objDetectedInfos.size())
		{
			OriFrame=objDetectedInfos.at(m_pageNum * m_pageSum + 1).OriFrameID;
			UserClick=TRUE;
		}
	}
}


void ObjectViewSUBDlg::OnStnClickedStaticObject3()
{
	// TODO: 在此添加控件通知处理程序代码
	if (Generated==TRUE)
	{
		if(m_pageNum * m_pageSum + 2<objDetectedInfos.size())
		{
			OriFrame=objDetectedInfos.at(m_pageNum * m_pageSum + 2).OriFrameID;
			UserClick=TRUE;
		}
	}
}


void ObjectViewSUBDlg::OnStnClickedStaticObject4()
{
	// TODO: 在此添加控件通知处理程序代码
	if (Generated==TRUE)
	{	
		if(m_pageNum * m_pageSum + 2<objDetectedInfos.size())
		{
			OriFrame=objDetectedInfos.at(m_pageNum * m_pageSum + 3).OriFrameID;
			UserClick=TRUE;
		}
	}
}


void ObjectViewSUBDlg::OnBnClickedObjectLeft()
{
	// TODO: 在此添加控件通知处理程序代码
	if(m_pageNum > 0)
	{
		m_pageNum--;
		DisplayPageSumImg();
	}
}


void ObjectViewSUBDlg::OnBnClickedObjectRight()
{
	// TODO: 在此添加控件通知处理程序代码
	DetectResult();			//计算m_pageSum要显示的object填充objDetectedInfos
	if (m_pageNum + 3 < ceil(objDetectedInfos.size()/(float)m_pageSum) - 1)			
	{
		m_pageNum++; 
		DisplayPageSumImg();
	}
}




void ObjectViewSUBDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	CDialog::OnTimer(nIDEvent);
	
	switch(nIDEvent)  
	{  
	case 1: //如果id为1的timer结束，则执行关闭窗口操作  
		//int temp;
		//temp=pScrollBar->GetScrollPos();

		//if (objDetectedInfos.size()>4 && m_pageNum!=(objDetectedInfos.size()-4)*temp/100)
		//{
			//pScrollBar->SetScrollPos(4);
		if (user_click_scrollbar==TRUE)
		{
			m_pageNum=pScrollBar->GetScrollPos();
			if (Generated==TRUE)
			{
				DisplayPageSumImg();
			}
			user_click_scrollbar=FALSE;
		}
		//}
	case 2:///<播放窗口1的进度条及时间显示刷新
		{
			if (If_playpiece)
			{
				CSliderCtrl * SliderCtrl=((CSliderCtrl*)GetDlgItem(IDC_SLIDER1));
				SliderCtrl->SetPos(ObjectPlayer.m_currentFrameNO);
				If_playpiece=FALSE;
			}
			else
			{
				CSliderCtrl * SliderCtrl=((CSliderCtrl*)GetDlgItem(IDC_SLIDER1));
				int CurrentPos=SliderCtrl->GetPos();                            ///<获取当前进度条位置
				int NewCurrentFrameNO = (double)CurrentPos*double(ObjectPlayer.m_endFrameNO-ObjectPlayer.m_startFrameNO)
					/(double)SliderCtrl->GetRangeMax();     ///<根据当前进度条位置，重新计算帧号

				if (ObjectPlayer.m_playState!=PLAY_STATE_STOP && ObjectPlayer.m_playState!=PLAY_STATE_FAST_FORWORD &&ObjectPlayer.m_playState!=PLAY_STATE_PLAY_PIECE)///<若播放器处于正常播放或暂停状态
				{
					if (abs(NewCurrentFrameNO-ObjectPlayer.m_currentFrameNO)>5)
					{
						ObjectPlayer.m_currentFrameNO=NewCurrentFrameNO;             ///<根据新计算的帧号，重置播放器
					}
				}
				SliderCtrl->SetPos(ObjectPlayer.m_currentFrameNO);                   ///<根据新计算的帧号，重置滑动条
				CWnd *pWndTimeDis = GetDlgItem(IDC_STATIC_OBJECTWINDOW);
				if (ObjectPlayer.m_playState != PLAY_STATE_STOP)                     ///<非停止状态下，用方式一显示时间
				{
					ObjectPlayer.ShowTime(ObjectPlayer.m_currentFrameNO, ObjectPlayer.m_endFrameNO - ObjectPlayer.m_startFrameNO,
						ObjectPlayer.m_videoTimeInSecond, pWndTimeDis, 1);
				}
				else                                                            ///<停止状态下，用方式二显示时间
				{
					SliderCtrl->SetPos(0);
					ObjectPlayer.ShowTime(ObjectPlayer.m_currentFrameNO, ObjectPlayer.m_endFrameNO - ObjectPlayer.m_startFrameNO,
						ObjectPlayer.m_videoTimeInSecond, pWndTimeDis, 0);
				}
			}

		}
	case 3://监视显示对象的子对话框
		{
			if (UserClick==TRUE)
			{
				If_playpiece=TRUE;
				ObjectPlayer.m_currentFrameNO=OriFrame;
				UserClick=FALSE;
			}
		}
	default:  
		break;  
	} 
}




BOOL ObjectViewSUBDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化
	UIBeautifier.LoadBackgroundPic(IDB_background);///<背景位图加载
	CWnd* pWnd = CWnd::GetDesktopWindow();
	pWnd->GetClientRect(UIBeautifier.rcDeskRect);///获取桌面矩形
	BitmapBackGroundStrech=UIBeautifier.CBitmapResize(UIBeautifier.m_BitmapBackGround,UIBeautifier.rcDeskRect.Width(),UIBeautifier.rcDeskRect.Height());
	
	GetDlgItem(IDC_STATIC_OBJECT1)->MoveWindow(100.0*UIBeautifier.rcDeskRect.Width() / 1214, 30.0*UIBeautifier.rcDeskRect.Height() / 760, 100.0*UIBeautifier.rcDeskRect.Width() / 1214, 125 * UIBeautifier.rcDeskRect.Height() / 760);
	GetDlgItem(IDC_STATIC_OBJECT2)->MoveWindow(100.0*UIBeautifier.rcDeskRect.Width() / 1214, 155.0*UIBeautifier.rcDeskRect.Height() / 760, 100.0*UIBeautifier.rcDeskRect.Width() / 1214, 125 * UIBeautifier.rcDeskRect.Height() / 760);
	GetDlgItem(IDC_STATIC_OBJECT3)->MoveWindow(100.0*UIBeautifier.rcDeskRect.Width() / 1214, 280.0*UIBeautifier.rcDeskRect.Height() / 760, 100.0*UIBeautifier.rcDeskRect.Width() / 1214, 125 * UIBeautifier.rcDeskRect.Height() / 760);
	GetDlgItem(IDC_STATIC_OBJECT4)->MoveWindow(100.0*UIBeautifier.rcDeskRect.Width() / 1214, 405.0*UIBeautifier.rcDeskRect.Height() / 760, 100.0*UIBeautifier.rcDeskRect.Width() / 1214, 125 * UIBeautifier.rcDeskRect.Height() / 760);
	GetDlgItem(IDC_OBJECT_SCROLLBAR)->MoveWindow(210 * UIBeautifier.rcDeskRect.Width() / 1214, 30.0*UIBeautifier.rcDeskRect.Height() / 760, 25.0*UIBeautifier.rcDeskRect.Width() / 1214, 530.0*UIBeautifier.rcDeskRect.Height() / 760);

	GetDlgItem(IDC_STATIC_OBJECT)->MoveWindow(370.0*UIBeautifier.rcDeskRect.Width()/1214,30.0*UIBeautifier.rcDeskRect.Height()/760,668.0*UIBeautifier.rcDeskRect.Width()/1214,501.0*UIBeautifier.rcDeskRect.Height()/760);
	
	GetDlgItem(IDC_STATIC_OBJECTWINDOW)->MoveWindow(480.0*UIBeautifier.rcDeskRect.Width()/1214,540.0*UIBeautifier.rcDeskRect.Height()/760,80.0*UIBeautifier.rcDeskRect.Width()/1214,40.0*UIBeautifier.rcDeskRect.Height()/760);
	GetDlgItem(IDC_SLIDER_OBJECT_ORI)->MoveWindow(560.0*UIBeautifier.rcDeskRect.Width()/1214,545.0*UIBeautifier.rcDeskRect.Height()/760,480.0*UIBeautifier.rcDeskRect.Width()/1214,20.0*UIBeautifier.rcDeskRect.Height()/760);
	UIBeautifier.LoadButtonBitmaps(BTN_OBJECT_ORI_PLAY,IDB_PLAY_U,IDB_PLAY_D,370.0/1214,540.0/760,400.0/1214,570.0/760);
	UIBeautifier.LoadButtonBitmaps(BTN_OBJECT_ORI_PAUSE,IDB_PAUSE_U,IDB_PAUSE_D,410.0/1214,540.0/760,440.0/1214,570.0/760);
	UIBeautifier.LoadButtonBitmaps(BTN_OBJECT_ORI_STOP,IDB_STOP_U,IDB_STOP_D,450.0/1214,540.0/760,480.0/1214,570.0/760);
	
	//初始化播放窗口位置
	ObjectPlayer.PlaywindowRect.left = 370.0*UIBeautifier.rcDeskRect.Width() / 1214;
	ObjectPlayer.PlaywindowRect.right = 370.0*UIBeautifier.rcDeskRect.Width() / 1214 + 668.0*UIBeautifier.rcDeskRect.Width() / 1214;
	ObjectPlayer.PlaywindowRect.top = 30.0*UIBeautifier.rcDeskRect.Height() / 760;
	ObjectPlayer.PlaywindowRect.bottom = 30.0*UIBeautifier.rcDeskRect.Height() / 760 + 501.0*UIBeautifier.rcDeskRect.Height() / 760;

	pScrollBar = (CScrollBar*)GetDlgItem(IDC_SCROLLBAR1);
	SetTimer(1, 200, NULL);
	ObjectPlayer.playInitial(GetDlgItem(IDC_STATIC_OBJECT), "obj_displayWindow");
	SetTimer(2, 50, NULL);
	SetTimer(3, 300, NULL);
	If_playpiece = FALSE;

	DisplayFrame initImage;
	IplImage* temp_image = cvLoadImage(".\\res\\ori-default.png", CV_LOAD_IMAGE_COLOR);
	CRect rect1;
	GetDlgItem(IDC_STATIC_OBJECT)->GetClientRect(&rect1);
	initImage.SetOpenCVWindow(GetDlgItem(IDC_STATIC_OBJECT), "obj_displayWindowtmp", ObjectPlayer.PlaywindowRect.left, ObjectPlayer.PlaywindowRect.top, ObjectPlayer.PlaywindowRect.Width(), ObjectPlayer.PlaywindowRect.Height());
	if (temp_image != NULL) // Check for invalid input
	{
		initImage.ShowPicture("obj_displayWindowtmp", temp_image);
		cvReleaseImage(&temp_image);
	}
	user_click_scrollbar=FALSE;
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}


void ObjectViewSUBDlg::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	int TempPos = pScrollBar->GetScrollPos();
	switch(nSBCode)
	{
	case SB_THUMBPOSITION://拖动滑块
		if(nPos<objDetectedInfos.size()-3)
		{
			pScrollBar->SetScrollPos(nPos);
			user_click_scrollbar=TRUE;
		}
		//pScrollBar->SetScrollPos(nPos);
		break;
	case SB_THUMBTRACK://拖动滑块
		if(nPos<objDetectedInfos.size()-3)
		{

			pScrollBar->SetScrollPos(nPos);
			user_click_scrollbar=TRUE;
		}

		//pScrollBar->SetScrollPos(nPos);
		break;
	case SB_LINEUP://点击上边的箭头
		if(TempPos > 0)
		{
			TempPos--;
		}
		pScrollBar->SetScrollPos(TempPos);
		user_click_scrollbar=TRUE;
		break;
	case SB_LINEDOWN://点击下边的箭头
		if(TempPos<objDetectedInfos.size()-4)
		{
			TempPos++;
		}
		pScrollBar->SetScrollPos(TempPos);
		user_click_scrollbar=TRUE;
		break;
	} 
	//int i=pScrollBar->GetScrollPos();
	CDialog::OnVScroll(nSBCode, nPos, pScrollBar);
}


void ObjectViewSUBDlg::OnBnClickedButton1()
{
	// TODO: 在此添加控件通知处理程序代码
	if(ObjectPlayer.m_filePath!="")
	{
		//m_CSliderPlayer1Ctrl.SetRange(0, ObjectPlayer.m_endFrameNO);///<滑动条初始化

		///<时间显示准备
		CString VideoTime;                                     
		int VedioHour=ObjectPlayer.m_videoTimeInSecond/3600;
		int VedioMinute=ObjectPlayer.m_videoTimeInSecond/60-VedioHour*60;
		int VedioSecond=ObjectPlayer.m_videoTimeInSecond-VedioHour*3600-VedioMinute*60;

		VideoTime.Format(" 0:0:0 / %d:%d:%d ",VedioHour,VedioMinute,VedioSecond);
		GetDlgItem(IDC_STATIC)->SetWindowText(VideoTime);///<将时间显示到控件上

		ObjectPlayer.play();

	}
	else
	{
		AfxMessageBox("请选中文件");
	}

}


void ObjectViewSUBDlg::OnBnClickedButton2()
{
	// TODO: 在此添加控件通知处理程序代码
	ObjectPlayer.pause();
}




void ObjectViewSUBDlg::OnBnClickedButton3()
{
	// TODO: 在此添加控件通知处理程序代码
	ObjectPlayer.stop();
}


void ObjectViewSUBDlg::OnClose()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	CDialog::OnClose();
	//cvReleaseCapture( &capture );
	cvDestroyWindow( "obj_displayWindow" );
	ObjectPlayer.threadRunOrNot=FALSE;
}


//void ObjectViewSUBDlg::OnCancelMode()
//{
//	CDialog::OnCancelMode();
//	cvDestroyWindow( "obj_displayWindow" );
//	ObjectPlayer.threadRunOrNot=FALSE;
//
//	// TODO: 在此处添加消息处理程序代码
//}


void ObjectViewSUBDlg::OnCancel()
{
	// TODO: 在此添加专用代码和/或调用基类
	return;
	//CDialog::OnCancel();
	//cvDestroyWindow( "obj_displayWindow" );
	//ObjectPlayer.threadRunOrNot=FALSE;
}
