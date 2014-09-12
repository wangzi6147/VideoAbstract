/**
*@file  vidPlayer.cpp
*@brief 视频播放类的定义。
*
*该文件定义了视频播放类的相关功能。
*@data 2014-06-04
*/

#include "stdafx.h"
#include "vidPlayer.h"
#include <afx.h>


pointForCvMouse mousePosInCVwmd;

vidPlayer::vidPlayer()
{
	threadRunOrNot = FALSE;
	m_playState = PLAY_STATE_STOP;
	m_CurrentTimeInSecond=0;
	bEditIsChange = false;
}

vidPlayer::~vidPlayer()
{
}

///@brief vidPlayer类的播放初始化，用来初始化视频参数
/// 
///@param[in] 视频显示窗口句柄，视频显示窗口名称
///@pre    NULL
///@return BOOL
///@retval NULL
///@post   NULL
BOOL vidPlayer::playInitial(CWnd *pWnd, CString windowName)
{
	m_pWnd = pWnd;
	m_windowName = windowName;
	timeshow=FALSE;
	
	CvCapture *capture = cvCreateFileCapture(m_filePath);///<获取视频头指针
	if (capture == NULL)
	{///获取失败则返回
		return FALSE;
	}
	m_size.width = cvGetCaptureProperty(capture, CV_CAP_PROP_FRAME_WIDTH);  ///<获取视频帧宽度
	m_size.height = cvGetCaptureProperty(capture, CV_CAP_PROP_FRAME_HEIGHT);///<获取视频帧高度
	m_fps = cvGetCaptureProperty(capture, CV_CAP_PROP_FPS);///<获取视频帧率
	m_startFrameNO = m_currentFrameNO = 0;///<初始化视频起始帧号和当前帧号
	m_endFrameNO = m_lastFrameNO = cvGetCaptureProperty(capture, CV_CAP_PROP_FRAME_COUNT);///<初始化视频结束帧号和上一次播放帧号
	m_timeGap = 1000.0/m_fps;///<初始化视频时间间隔
	m_videoTimeInSecond = double(m_endFrameNO - m_startFrameNO)/m_fps;///<初始化视频秒数间隔
	cvReleaseCapture(&capture);///<释放视频头指针

	return TRUE;

}


///@brief vidPlayer类的播放线程控制函数
/// 
///@param[in|out] NULL
///@pre    NULL
///@return UINT
///@retval NULL
///@post   NULL
UINT vidPlayer::playProcess()
{
	const char *path = (LPCTSTR)m_filePath;
	CvCapture *capture = cvCreateFileCapture(path);///<获取视频头指针
	if (!capture)
	{///获取失败，输出提示语句
		AfxMessageBox("您要播放的文件已损坏或者已经从原路径移除！！！\n请重新选择文件。");
		return 0;
	}
	cvSetCaptureProperty(capture, CV_CAP_PROP_POS_FRAMES, m_currentFrameNO);///<重置视频当前帧位置
	//frameCopyForDraw = cvCreateImage(m_size,IPL_DEPTH_8U, 3);///<初始化显示图像帧
	m_curFrame = cvCreateImage(m_size,IPL_DEPTH_8U, 3);
	m_curFrameCopy = cvCreateImage(m_size,IPL_DEPTH_8U, 3);
	while (threadRunOrNot)
	{
//		int para = 5;
//		//if (m_windowName == "displayWindow1")//luying 10.29 为解决鼠标点击无法及时响应的问题
//		{
//			cvSetMouseCallback(m_windowName, cvMouseHandler, &para);///<开启鼠标响应
//		}
		//if (m_playState != PLAY_STATE_PAUSE)
		//{///非暂停状态下，对鼠标点击不响应
		//	mousePosInCVwmd.clickInCVwnd = FALSE;
		//	m_gotCVlclick = FALSE;
		//}

		if(m_playState == PLAY_STATE_PLAY)///播放
		{
			while((m_currentFrameNO < m_endFrameNO)&& (m_playState == PLAY_STATE_PLAY))
			{
				if (threadRunOrNot == FALSE)
					break;///<线程状态为FALSE时，跳出循环
				
				if (m_currentFrameNO < m_startFrameNO)
				{///当前帧小于起始帧，则重置起始帧
					cvSetCaptureProperty(capture, CV_CAP_PROP_POS_FRAMES, m_startFrameNO);
					m_currentFrameNO = m_startFrameNO;
					m_lastFrameNO = m_currentFrameNO;
				}
				if ( (abs(m_lastFrameNO-m_currentFrameNO)>=4) || bEditIsChange)
				{///拖动滑动条引起的前一帧与当前帧差距过大，则重置当前帧
					cvSetCaptureProperty(capture, CV_CAP_PROP_POS_FRAMES, m_currentFrameNO);
					m_lastFrameNO=m_currentFrameNO;
					bEditIsChange = false;
				}
				m_curFrame = cvQueryFrame(capture);///<获取当前帧图像

				
				if (!m_curFrame)
					continue;

				if(timeshow==TRUE)//显示时间
				{
					TimePosition.clear();
					Time.clear();
					ROI.clear();
					m_MysqlVideoParaSearchHandle->FindTimePositionFromNewTraceTable(m_currentFrameNO, &TimePosition, &Time, m_videoPro->m_tableParams.NewTraceTable);
					m_MysqlVideoParaSearchHandle->FindROIFromNewTraceTable(m_currentFrameNO, &ROI, m_videoPro->m_tableParams.NewTraceTable);

					CvFont font;
					cvInitFont(&font, CV_FONT_HERSHEY_COMPLEX, 1.0, 1.0, 0, 2, 1); 
					int i=0;
					
					while (i<Time.size())
					{
						CString str_TargetVideoTime;  //
						//d_AbsVideoTimeRatio = (player1.m_endFrameNO- player1.m_startFrameNO)* player1.m_videoTimeInSecond;
						m_origPara = m_MysqlVideoParaSearchHandle->FindOrigParaFromFGTraceTable(m_videoPro->m_tableParams.FGTraceTableName, ROI[i]);
						int TargetTimeInSecond = (double)Time[i]/(double)d_AbsVideoTimeRatio;
						int TargetVedioHour=TargetTimeInSecond/3600;
						int TargetVedioMinute=TargetTimeInSecond/60-TargetVedioHour*60;
						int TargetVedioSecond=TargetTimeInSecond-TargetVedioHour*3600-TargetVedioMinute*60;

						str_TargetVideoTime.Format("%d  %d:%d:%d",m_origPara,TargetVedioHour,TargetVedioMinute,TargetVedioSecond);
						
						cvPutText(m_curFrame,str_TargetVideoTime,TimePosition[i], &font, CV_RGB(255, 255, 255));//TimePosition[i]
						i++;
					}
				}
				disPlayImage.ShowPicture(m_windowName, m_curFrame);
               
				//playInDrawDetection();///<手绘目标检测
				//m_currentFrameNO = m_lastFrameNO; //trun to 0;
				m_lastFrameNO = m_currentFrameNO;
				m_currentFrameNO ++;

				cvWaitKey(1);
				Sleep(m_timeGap);///<延时处理

				int para = 5;
				cvSetMouseCallback(m_windowName, cvMouseHandler, &para);

				m_clickPosInCVWnd.x = mousePosInCVwmd.x;
				m_clickPosInCVWnd.y = mousePosInCVwmd.y;
				m_lBtnUpPosInCVWnd.x = mousePosInCVwmd.x1;
				m_lBtnUpPosInCVWnd.y = mousePosInCVwmd.y1;
				m_gotCVlclick = mousePosInCVwmd.clickInCVwnd;//cv窗口得到点击参数便传出
			}
		}
		else if(m_playState == PLAY_STATE_PLAY_PIECE)///播放片段
		{
			cvSetCaptureProperty(capture,CV_CAP_PROP_POS_FRAMES,m_startFrameNO);///重置起始帧
			m_currentFrameNO = (int)cvGetCaptureProperty(capture,CV_CAP_PROP_POS_FRAMES);
			m_lastFrameNO = m_currentFrameNO;
			m_playState = PLAY_STATE_PLAY;///<视频状态转为播放
		}
		else if(m_playState == PLAY_STATE_PAUSE)///暂停
		{
			if (threadRunOrNot == FALSE)
				break;///<线程状态为FALSE时，跳出循环
			
			if (abs(m_lastFrameNO-m_currentFrameNO)>=4)
			{///拖动滑动条引起的前一帧与当前帧差距过大，则重置当前帧
				cvSetCaptureProperty(capture, CV_CAP_PROP_POS_FRAMES, m_currentFrameNO);
				m_lastFrameNO=m_currentFrameNO;
				m_curFrame = cvQueryFrame(capture);	///<获取当前帧图像
				if (!m_curFrame)
					continue;
				
				disPlayImage.ShowPicture(m_windowName, m_curFrame);///<显示当前帧图像

				cvWaitKey(1);
				Sleep(m_timeGap);///<延时处理
			}

			int para = 5;
			cvSetMouseCallback(m_windowName, cvMouseHandler, &para);

			m_clickPosInCVWnd.x = mousePosInCVwmd.x;
			m_clickPosInCVWnd.y = mousePosInCVwmd.y;
			m_lBtnUpPosInCVWnd.x = mousePosInCVwmd.x1;
			m_lBtnUpPosInCVWnd.y = mousePosInCVwmd.y1;
			m_gotCVlclick = mousePosInCVwmd.clickInCVwnd;//cv窗口得到点击参数便传出

			//pauseInDrawDetection();///<手绘目标检测
			cvWaitKey(1);          //20130313陆鹏改 防刷新太快
			Sleep(m_timeGap);///<延时处理
		}
		else if(m_playState == PLAY_STATE_FAST_FORWORD)///快进
		{
			while((m_currentFrameNO < m_endFrameNO)&&(m_playState == PLAY_STATE_FAST_FORWORD))
			{
				if (threadRunOrNot == FALSE)
					break;///<线程状态为FALSE时，跳出循环
				
				m_curFrame = cvQueryFrame(capture);///<获取当前帧图像
				if (!m_curFrame)
					continue;
				
				disPlayImage.ShowPicture(m_windowName, m_curFrame);///<显示当前帧图像
				m_currentFrameNO ++;///<当前帧号递增一
				cvWaitKey(1);
				Sleep(m_timeGap/4);///<1/4延时处理
			}

		}
		else if (m_playState == PLAY_STATE_FAST_BACKWORD)///快退
		{
			while((m_currentFrameNO >= m_startFrameNO)&&(m_playState == PLAY_STATE_FAST_BACKWORD))
			{
				if (threadRunOrNot == FALSE)
					break;///<线程状态为FALSE时，跳出循环
				
				m_curFrame = cvQueryFrame(capture);///<获取当前帧图像
				if (!m_curFrame)
					continue;
				
				disPlayImage.ShowPicture(m_windowName, m_curFrame);///<显示当前帧图像
				m_currentFrameNO --;///<当前帧号递减一
				cvWaitKey(1);
				Sleep(m_timeGap/4);///<1/4延时处理
			}

		}
		else if(m_playState == PLAY_STATE_FRAME_FAST_FORWORD)///逐帧快进
		{
			if(m_currentFrameNO < m_endFrameNO && m_currentFrameNO > m_startFrameNO)
			{
				if (threadRunOrNot == FALSE)
					break;///<线程状态为FALSE时，跳出循环

				m_curFrame = cvQueryFrame(capture);	///<获取当前帧图像
				if (!m_curFrame)
					continue;
				
				disPlayImage.ShowPicture(m_windowName, m_curFrame);///<显示当前帧图像

				m_lastFrameNO=m_currentFrameNO;///<前一帧号等于当前帧号
				m_currentFrameNO ++;///<当前帧号递增一

				m_playState = PLAY_STATE_PAUSE;///<视频状态转为暂停
			}
		}
		else if (m_playState == PLAY_STATE_FRAME_FAST_BACKWORD)///逐帧快退
		{
			if(m_currentFrameNO < m_endFrameNO && m_currentFrameNO > m_startFrameNO)
			{
				if (threadRunOrNot == FALSE)
					break;///<线程状态为FALSE时，跳出循环
				
				m_currentFrameNO --;///<当前帧号递减一
				cvSetCaptureProperty(capture, CV_CAP_PROP_POS_FRAMES, m_currentFrameNO);///<重置当前帧
				m_curFrame = cvQueryFrame(capture);	///<获取当前帧图像
				if (!m_curFrame)
					continue;
				
				disPlayImage.ShowPicture(m_windowName, m_curFrame);///<显示当前帧图像
				m_playState = PLAY_STATE_PAUSE;///<视频状态转为暂停
			}

		}
		else if (m_playState == PLAY_STATE_STOP)///中止播放
		{
			if (threadRunOrNot == FALSE)
				break;///<线程状态为FALSE时，跳出循环
			
			m_currentFrameNO = m_startFrameNO;///<先将帧号置零，然后等待播放按键响应  //2013陆鹏改 帧号置位0改成startframe,防播放片段情况
			cvSetCaptureProperty(capture, CV_CAP_PROP_POS_FRAMES, m_currentFrameNO);///<重置当前帧为视频首帧
			m_curFrame = cvQueryFrame(capture);	///<获取当前帧图像
			disPlayImage.ShowPicture(m_windowName, m_curFrame);///<显示当前帧图像
			cvWaitKey(1);        //20130313陆鹏改 防刷新太快
			Sleep(m_timeGap/4);///<1/4延时处理
		}
		else if (m_playState == PLAY_STATE_SCREEN_SHOT)///截图
		{
			///进入暂停状态，并等待其他按键响应
			if (threadRunOrNot == FALSE)
				break;///<线程状态为FALSE时，跳出循环
			
			m_playState = PLAY_STATE_PAUSE;///<视频状态转为暂停
		}	
		if(m_currentFrameNO >= m_endFrameNO)
		{m_playState =PLAY_STATE_PAUSE;
		 m_currentFrameNO=m_startFrameNO;
		}
		     
	}
	cvReleaseCapture(&capture);///<释放视频头指针
	
	return 0;

}


///@brief vidPlayer类的视频播放,线程开启函数
BOOL vidPlayer::play()
{
	if(threadRunOrNot == FALSE)
	{
		threadRunOrNot = TRUE;
		CRect rec;
		m_pWnd->GetClientRect(rec);

		disPlayImage.SetOpenCVWindow(m_pWnd, m_windowName, 
			rec.Height()*m_size.width/m_size.height, rec.Height());
		m_threadControl = ::AfxBeginThread(RunPlayProcess,this);
	}
	m_playState = PLAY_STATE_PLAY;

	return TRUE;
}

BOOL vidPlayer::playPiece()
{
	m_playState = PLAY_STATE_PLAY_PIECE;
	return TRUE;
}

BOOL vidPlayer::pause()
{
	m_playState = PLAY_STATE_PAUSE;
	return TRUE;
}

BOOL vidPlayer::fastForward()
{
	m_playState = PLAY_STATE_FAST_FORWORD;
	return TRUE;
}

BOOL vidPlayer::fastBackward()
{
	m_playState = PLAY_STATE_FAST_BACKWORD;
	return TRUE;
}

BOOL vidPlayer::fastForwardByFrame()
{
	m_playState = PLAY_STATE_FRAME_FAST_FORWORD;
	return TRUE;
}

BOOL vidPlayer::fastBackwardByFrame()
{
	m_playState = PLAY_STATE_FRAME_FAST_BACKWORD;
	return TRUE;
}
BOOL vidPlayer::stop()
{
	m_playState = PLAY_STATE_STOP;
	return TRUE;
}

IplImage *vidPlayer::screenShot()
{
	m_playState = PLAY_STATE_SCREEN_SHOT;
	return m_curFrame;
}

BOOL vidPlayer::stopPlay()
{

	if (threadRunOrNot == TRUE)
	{
		TRACE("threadRunOrNot = FALSE\n");
		threadRunOrNot = FALSE;
		WaitForSingleObject(m_threadControl->m_hThread,INFINITE);
		TRACE("before cvDestroyWindow\n");
		cvDestroyWindow(m_windowName);
	}
	return TRUE;
}

UINT RunPlayProcess(LPVOID controlNO)
{
	vidPlayer *OpenPlayHandle = (vidPlayer *)controlNO;

	OpenPlayHandle->playProcess();
	return 0;
}

BOOL vidPlayer::drawROI(vector <CRect> rect, CvScalar rectColor, int thickness)//用于绘制目标框
{
	int rectCount = rect.size();
	cvCopy(m_curFrame, m_curFrameCopy);
	CvPoint point1, point2;
	for (int i = 0; i<rectCount; i++)
	{
		point1.x = rect[i].left; point1.y = rect[i].top;
		point2.x = rect[i].left; point2.y = rect[i].bottom;
		cvLine(m_curFrameCopy, point1, point2, rectColor, thickness);//左边

		point1.x = rect[i].left; point1.y = rect[i].top;
		point2.x = rect[i].right; point2.y = rect[i].top;
		cvLine(m_curFrameCopy, point1, point2, rectColor, thickness);//上边

		point1.x = rect[i].right; point1.y = rect[i].bottom;
		point2.x = rect[i].left; point2.y = rect[i].bottom;
		cvLine(m_curFrameCopy, point1, point2, rectColor, thickness);//下边

		point1.x = rect[i].right; point1.y = rect[i].top;
		point2.x = rect[i].right; point2.y = rect[i].bottom;
		cvLine(m_curFrameCopy, point1, point2, rectColor, thickness);//右边		
	}
	disPlayImage.ShowPicture(m_windowName, m_curFrameCopy);

	return TRUE;
}





void vidPlayer::ShowTime()
{
	CString CurVideoTime;
	int CurVedioHour=m_CurrentTimeInSecond/3600;
	int CurVedioMinute=m_CurrentTimeInSecond/60-CurVedioHour*60;
	int CurVedioSecond=m_CurrentTimeInSecond-CurVedioHour*3600-CurVedioMinute*60;

	int VedioHour=m_videoTimeInSecond/3600;
	int VedioMinute=m_videoTimeInSecond/60-VedioHour*60;
	int VedioSecond=m_videoTimeInSecond-VedioHour*3600-VedioMinute*60;

	CurVideoTime.Format(" %d::%d::%d / %d::%d::%d ",CurVedioHour,CurVedioMinute,CurVedioSecond,VedioHour,VedioMinute,VedioSecond);
	m_pShowTimeWnd->SetWindowText(CurVideoTime);
}

void cvMouseHandler(int eventType,int x,int y,int flags, void *param)
{
	switch (eventType)
	{
	case CV_EVENT_LBUTTONDOWN:///左键按下     
		{
			mousePosInCVwmd.x = x;
			mousePosInCVwmd.y = y;
			mousePosInCVwmd.x1 = x;
			mousePosInCVwmd.y1 = y;
			mousePosInCVwmd.clickInCVwnd = TRUE;
			mousePosInCVwmd.lBtnUp = FALSE;
		}
		break;
	case CV_EVENT_LBUTTONUP:///左键弹起
		{
			if (mousePosInCVwmd.clickInCVwnd == TRUE)
			{
				mousePosInCVwmd.x1 = x;
				mousePosInCVwmd.y1 = y;
				mousePosInCVwmd.lBtnUp = TRUE;
				mousePosInCVwmd.clickInCVwnd = FALSE;
			}
		}
		break;
	case CV_EVENT_MOUSEMOVE:///左键拖动
		{
			if (mousePosInCVwmd.clickInCVwnd == TRUE && mousePosInCVwmd.lBtnUp == FALSE)
			{
				mousePosInCVwmd.x1 = x;
				mousePosInCVwmd.y1 = y;
			}		
		}
		break;
	}
}

CString absOriNameTansf(CString nameIn)
{
	CString nameOut;
	int isAvi = nameIn.Find(".avi");
	if (isAvi != -1)
	{
		int isAviFusion = nameIn.Find("_fusion.avi");
		if (isAviFusion != -1 )
		{
			nameOut = nameIn.Left(isAviFusion);
		}
		else
		{
			nameOut = nameIn;
			nameOut.Append("_fusion.avi");
		}
	}
	return nameOut;
}
