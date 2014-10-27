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
	int canDraw;        ///<画框(线)标志位(仅在DrawDetectionSUBDlg有效)
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

	int m_currentFrameNO;//当前帧号
	int m_lastFrameNO;
	int m_startFrameNO;//帧头号，在载入文件时应当使current = start
	int m_endFrameNO;//帧未号
	int m_videoTimeInSecond;  //播放视频的长度
	int m_CurrentTimeInSecond;//以秒为单位的当前时间
	int CSliderPos;    //滑动条位置
	int m_playState;//播放状态
	double m_timeGap;//播放时，帧间时间差
	double m_fps;//帧率
	double d_AbsVideoTimeRatio; //totalFrameCount--总帧数*videoTimeInSecond--视频时间长度（s）
	CvSize m_size;//记录视频画面尺寸
	IplImage *m_curFrame;//当前帧指针
	IplImage *m_curFrameCopy;
	BOOL threadRunOrNot;//进程状态，开or关
	CString m_windowName;//当前进程控制的CV窗口名
	CString m_filePath;
	DisplayFrame disPlayImage;//用于内嵌播放窗口的对象
	BOOL m_gotCVlclick;
	BOOL playInitial(CWnd *pWnd, CString windowName);
	UINT playProcess();
	BOOL play();
	BOOL pause();
	BOOL stop();
	BOOL fastForward();
	BOOL fastBackward();
	BOOL fastForwardByFrame();
	BOOL fastBackwardByFrame();
	BOOL stopPlay();
	BOOL playPiece(); //播放片段时，在外部调用以setCaptureProperty
	bool bEditIsChange; //<Edit Control的标志位
	CWnd *m_pShowTimeWnd;       //显示播放时间的句柄
	IplImage *screenShot();
	CPoint m_clickPosInCVWnd;///<记录左键单击的点
	CPoint m_lBtnDownPosInCVWnd;///<记录左键按下的点
	CPoint m_lBtnUpPosInCVWnd;///<记录左键弹起的点
	CDataMySql* m_MysqlVideoParaSearchHandle;
	CVideo *m_videoPro;
	vector<CvPoint> TimePosition;
	vector<int> Time;
	vector<CvRect> ROI;
	CRect PlaywindowRect;       //播放窗口原始大小
	int m_origPara;
	bool timeshow;
	void ShowTime();  //显示当前播放进度时间
	void ShowTime(int m_currentFrameNO,            ///<用于显示播放时间的函数
		int totalFrameCount,
		int videoTimeInSecond,
		CWnd *m_pShowTimeWnd, int flag);
	void SetUrl(LPCTSTR lpszNewValue);
//*************************************************************************
///@手绘检测部分新增成员
///@0924 ChenYJ
	IplImage *frameCopyForDraw; ///<显示帧
	bool m_IfStartDetect;       ///<手绘检测开始与否标志位
	int  m_drawDetectFlag;      ///<手绘检测状态标志位
	vector<int>   objectIDs;    ///<运动目标ID集合（检索结果）
	vector<CPoint> pointsToShow;///<运动目标轨迹点集合
	vector<CRect>  RectToShow;  ///<运动目标Rect集合
	int m_clickObjectID;        ///<鼠标左键单击的运动目标ID
	inline BOOL playInDrawDetection();   ///<播放状态下的运动目标检测
	inline BOOL pauseInDrawDetection();  ///<暂停状态下的运动目标检测
	BOOL drawDetection(vector <CPoint> twoPoints, IplImage *img, int flag);///<图像帧画轨迹
	BOOL drawROI(vector <CvRect> rect, CvScalar rectColor, int thickness);  ///<图像帧画框
	inline void UpdatePlayData(int flag);///<播放状态下的数据更新
	inline void UpdatePauseData();       ///<暂停状态下的数据更新
	CDataMySql *m_MysqlObjHandle;///<内部数据库句柄，用于查询ObjectTable
	CString objtablename;       ///<ObjectTable数据表名
	vector<CRect> TempRect;     ///<运动目标Rect集合（临时变量）
	vector<int>   TempObjectID; ///<运动目标ID集合（临时变量）
	CPoint m_clickPoint;	    ///<鼠标左键单击点

protected:
	CWnd *m_pWnd;               //显示视频的窗口句柄
	CWinThread *m_threadControl;//进程控制
};

UINT RunPlayProcess(LPVOID controlNO);
void cvMouseHandler(int eventType,int x,int y,int flags, void *param);
CString absOriNameTansf(CString nameIn);
