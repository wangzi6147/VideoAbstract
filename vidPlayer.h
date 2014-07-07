/**
*@file  vidPlayer.h
*@brief 视频播放类的定义。
*
*该文件定义了视频播放类的相关功能。
*@data 2014-06-04
*/

#pragma  once
#include "afxwin.h"
#include "publicheader.h"
#include "DisplayFrame.h"
#include <vector>
#include "LibMySql.h"
#include "VideoSeg.h"
using std::vector;

#define PLAY_STATE_PLAY                 0///<播放   
#define PLAY_STATE_PAUSE                1///<暂停
#define PLAY_STATE_FAST_FORWORD         2///<快进
#define PLAY_STATE_FAST_BACKWORD	    3///<快退
#define PLAY_STATE_FRAME_FAST_FORWORD   4///<逐帧快进
#define PLAY_STATE_FRAME_FAST_BACKWORD  5///<逐帧快退
#define PLAY_STATE_STOP                 6///<中止
#define PLAY_STATE_SCREEN_SHOT          7///<截屏
#define PLAY_STATE_PLAY_PIECE           8///<点击目标框后，进行片段播放


/*********************************************
Struct:        pointForCvMouse 
Description:   用于cvMouseHandler函数向外传递点击事件的参数
Author:        Lu Ying
Date:          2012-9-11
*********************************************/
typedef struct _pointForCvMouse
{
	int x;			    ///<左键按下坐标x
	int y;				///<左键按下坐标y
	int x1;				///<左键弹起坐标x
	int y1;				///<左键弹起坐标y
	BOOL lBtnUp;		///<左键弹起标志位
	BOOL clickInCVwnd;  ///<点击视频播放窗口(暂停状态有效)
	BOOL clickInBPwnd;  ///<点击显示图片窗口(结果非空有效)
	int canDraw;        ///<画框(线)标志位(仅在drawDetection有效)
}pointForCvMouse;

/*********************************************
Class:         vidPlayer 
Description:   用于视频播放（包括线程开启、窗口初始化、播放操作等）
Author:        Lu Peng&Chen Daiwu
Date:          2012-8-24
*********************************************/
class vidPlayer
{

public:
	vidPlayer();
	~vidPlayer();

public:
	CString m_filePath;
	int m_currentFrameNO;//当前帧号
	int m_lastFrameNO;
	int m_startFrameNO;//帧头号，在载入文件时应当使current = start
	int m_endFrameNO;//帧未号
	CvSize m_size;//记录视频画面尺寸
	double m_fps;//帧率
	int m_playState;//播放状态
	double m_timeGap;//播放时，帧间时间差
	IplImage *m_curFrame;//当前帧指针
	IplImage *m_curFrameCopy;
	BOOL threadRunOrNot;//进程状态，开or关
	CString m_windowName;//当前进程控制的CV窗口名
	DisplayFrame disPlayImage;//用于内嵌播放窗口的对象
	int m_videoTimeInSecond;  //播放视频的长度
	int m_CurrentTimeInSecond;//以秒为单位的当前时间
	BOOL m_gotCVlclick;//

	


	double d_AbsVideoTimeRatio; //totalFrameCount--总帧数*videoTimeInSecond--视频时间长度（s）

	//CWnd *m_SliderCtrl;
	int CSliderPos;    //滑动条位置
	CWnd *m_pShowTimeWnd;       //显示播放时间的句柄

	bool bEditIsChange; //<Edit Control的标志位
	
protected:
	CWnd *m_pWnd;               //显示视频的窗口句柄
	CWinThread *m_threadControl;//进程控制

public:
	BOOL playInitial(CWnd *pWnd, CString windowName);
	UINT playProcess();
	BOOL play();
	BOOL pause();
	BOOL stop();
	BOOL fastForward();
	BOOL fastBackward();
	BOOL fastForwardByFrame();
	BOOL fastBackwardByFrame();
	IplImage *screenShot();
	BOOL drawROI(vector <CRect> rect, CvScalar rectColor, int thickness);  ///<图像帧画框
	BOOL stopPlay();

	void ShowTime();  //显示当前播放进度时间
	BOOL playPiece(); //播放片段时，在外部调用以setCaptureProperty
    void SetUrl(LPCTSTR lpszNewValue);

	CPoint m_clickPosInCVWnd;///<记录左键单击的点
	CPoint m_lBtnDownPosInCVWnd;///<记录左键按下的点
	CPoint m_lBtnUpPosInCVWnd;///<记录左键弹起的点
	
	vector<CvPoint> TimePosition;
	vector<int> Time;
	CDataMySql* m_MysqlVideoParaSearchHandle;
	CVideo *m_videoPro;
	bool timeshow;
};

UINT RunPlayProcess(LPVOID controlNO);
void cvMouseHandler(int eventType,int x,int y,int flags, void *param);
CString absOriNameTansf(CString nameIn);
