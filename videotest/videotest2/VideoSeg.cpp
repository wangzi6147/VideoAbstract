/**
*@file  VideoSeg.cpp
*@brief 实现视频分割融合功能，生成视频摘要。
*
*主要模块：视频分析、视频分割（背景提取、前景提取）、视频融合（摘要生成）。
*
*
*@author 作者 lu ying&&zhang nan nan ,包含email等
*@version 版本号(0.0)
*@data 2014-6-4
*/
#include "stdafx.h"
#include <afxwin.h>
#include <afxmt.h>
#include "VideoSeg.h"
#include <vector>



#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#define new new( _CLIENT_BLOCK, __FILE__, __LINE__)

#define OBJECT_MARGIN 20
///@brief CVideo类的标准构造函数
/// 
///@param[in|out] NULL
///@pre  NULL
///@return NULL
///@retval NULL
///@post NULL

CVideo::CVideo()
{///参数初始化
	m_IfDoneSeg       = false;///<视频分割完成标志位置否
	m_IfDoneFus       = false;///<视频融合完成标志位置否
	SegFrame_Count    = 0;    ///<分割视频总帧数置零
	FusFrame_Count    = 0;    ///<融合视频总帧数置零
	StepBitmapDefault[0] = 0;
	StepBitmapDefault[1] = 0;
	StepBitmapDefault[2] = 0;
	StepBitmapDefault[3] = 0;
	StepBitmapDefault[4] = 0;

	m_ProcessingSpeed = 0.0;  ///<处理进度置零

	///视频or图像指针信息置空
	m_pCapture        = NULL;   
	m_pGaussBGModel   = NULL;
	m_pGaussParams    = NULL;

	m_pCont           = NULL;
	m_pStor           = NULL;

	m_pOriFrame       = NULL;
	m_pProcessFrame   = NULL;
//	m_pResizeFrame    = NULL;
	m_pGrayFrame      = NULL;
//	m_pGrayResizeFrame= NULL;
	m_pBGFrame        = NULL;
	for (int i=0;i<10;i++)
	{
		m_pBGFinal[i] = NULL;
	}
	m_pNewFrame       = NULL;

	m_FrameMat       = NULL;      
	m_BGMat          = NULL;       
	m_pGrayFrameMat   = NULL;          
	m_pGrayBGMat      = NULL;            
	m_pGrayFGMat      = NULL;          

	m_pVideoWriter4seg    = NULL;
	m_pVideoWriter4fusion = NULL;
	//mypCombineSegImage = NULL;
	m_MysqlSegHandle = new CDataMySql;///<新建数据库连接句柄
}

///@brief CVideo类的标准析构函数
/// 
///@param[in|out] NULL
///@pre  NULL
///@return NULL
///@retval NULL
///@post NULL

CVideo::~CVideo()
{///释放内存
	if (m_pProcessFrame!=NULL)
		cvReleaseImage(&m_pProcessFrame);
	if (m_pBGFrame!=NULL)
		cvReleaseImage(&m_pBGFrame);
	for (int i=0;i<10;i++)
	{
		if (m_pBGFinal[i]!=NULL)
			cvReleaseImage(&m_pBGFinal[i]);
	}
//	if (m_pResizeFrame!=NULL)
//		cvReleaseImage(&m_pResizeFrame);
	if (m_pGrayFrame!=NULL)
		cvReleaseImage(&m_pGrayFrame);
//	if (m_pGrayResizeFrame!=NULL)
//		cvReleaseImage(&m_pGrayResizeFrame);
	if (m_pNewFrame!=NULL)
		cvReleaseImage(&m_pNewFrame);
	if (m_pCapture!=NULL)
		cvReleaseCapture(&m_pCapture);
	if (m_pVideoWriter4seg!=NULL)
		cvReleaseVideoWriter(&m_pVideoWriter4seg);
	if (m_pVideoWriter4fusion!=NULL)
		cvReleaseVideoWriter(&m_pVideoWriter4fusion);
	if (m_pStor!=NULL)
		cvReleaseMemStorage(&m_pStor);
	if (m_pGaussBGModel!=NULL)
		cvReleaseBGStatModel((CvBGStatModel**)&m_pGaussBGModel);

//	if (m_pFrameMat!=NULL)
//		cvReleaseMat(&m_pFrameMat);
//	if (m_pBGMat!=NULL)
//		cvReleaseMat(&m_pBGMat);
	if (m_pGrayFrameMat!=NULL)
		cvReleaseMat(&m_pGrayFrameMat);
	if (m_pGrayBGMat!=NULL)
		cvReleaseMat(&m_pGrayBGMat);
	if (m_pGrayFGMat!=NULL)
		cvReleaseMat(&m_pGrayFGMat);
	


	delete m_pGaussParams;	///<删除高斯模型参数指针
	delete m_MysqlSegHandle;///<删除数据库连接句柄

	_CrtDumpMemoryLeaks();
}
///@brief CVideo类的开始处理函数
/// 
///@param[in] 视频路径
///@pre  NULL
///@return NULL
///@retval int
///@post NULL
int CVideo::DoProcessing(const char* pFilePath)
{
	m_strFilePath = pFilePath;///<获取视频路径
	AfxBeginThread(BeginSegProcess,this);///<开启视频处理线程
	return 0;
}




///@brief CVideo类的获得当前处理进度函数
/// 
///@param[in|out] NULL
///@pre  NULL
///@return NULL
///@retval double
///@post NULL
double CVideo::ProcessingSpeed()   
{
	return this->m_ProcessingSpeed;///<返回当前处理进度
}



///@brief CVideo类的中止处理函数
/// 
///@param[in|out] NULL
///@pre  NULL
///@return NULL
///@retval NULL
///@post NULL
void CVideo::StopProcessing()
{
	if (!m_IfDoneFus&&m_IfContinue)///<当融合处理未完成时
	{
		this->m_IfContinue = false;///<继续处理标志位置否

		//m_MysqlSegHandle->DropTable(m_tableParams.VideoFGTableName);///<删除前景段信息表
		//cvWaitKey(1);///<等待时间，确保删表成功
		//m_MysqlSegHandle->DropTable(m_tableParams.FGTraceTableName);///<删除运动物体轨迹表
		//cvWaitKey(1);///<等待时间，确保删表成功
		m_MysqlSegHandle->DropTable(m_tableParams.NewToOldFrameTableName);///<删除新旧视频帧对照表
		cvWaitKey(1);///<等待时间，确保删表成功
		//m_MysqlSegHandle->DropTable(m_tableParams.NewTraceTable);///<删除目标轨迹信息表
		cvWaitKey(5);
	}
}



///@brief CVideo类的获取原始视频帧数函数
/// 
///@param[in|out] NULL
///@pre  NULL
///@return NULL
///@retval int
///@post NULL
int CVideo::VideoOriCount()
{
	if(m_nTotalFrameToDo>0)
		return m_nTotalFrameToDo;///<当原始视频总帧数为正时，返回该值
	else
		return -1;///<否则返回-1
}



///@brief CVideo类的获取分割视频帧数函数
/// 
///@param[in|out] NULL
///@pre  NULL
///@return NULL
///@retval int
///@post NULL
int CVideo::VideoSegCount()
{
	if (m_IfDoneSeg)
		return this->SegFrame_Count;///<当分割视频总帧数为正时，返回该值
	else
		return -1;///<否则返回-1
}



///@brief CVideo类的获取融合视频帧数函数
/// 
///@param[in|out] NULL
///@pre  NULL
///@return NULL
///@retval int
///@post NULL
int CVideo::VideoFusCount()
{
	if (m_IfDoneFus)
		return this->FusFrame_Count;///<当融合视频总帧数为正时，返回该值
	else
		return -1;///<否则返回-1
}



///@brief CVideo类的视频分割融合总控函数
/// 
///@param[in|out] NULL
///@pre  NULL
///@return NULL
///@retval NULL
///@post NULL
void CVideo::VideoProcessing()
{
	if(this->Initialization(m_strFilePath.c_str()) != -1)
	{///当初始化函数返回值为正时，进行以下操作
		this->VideoParaProcessing();  ///<视频分段
#ifdef VideoConnection_Is_On
		this->VideoConnection();      ///<分割视频拼接
#endif
		this->VideoBGProcessing();    ///<视频背景提取
		this->VideoFGProcessing();    ///<视频前景切分
		this->VideoFusionProcessing();///<视频融合
	}
	else 
	{///当初始化函数返回值为-1时，继续处理标志位置否
		m_IfContinue = false;
	}
}
///@brief CVideo类的视频参数初始化函数
/// 
///@param[in] 视频路径
///@pre  NULL
///@return NULL
///@retval int
///@post NULL
int  CVideo::Initialization(const char *pVideoPath)
{
	m_pCapture = cvCreateFileCapture(pVideoPath);///<获取视频头指针
	if (m_pCapture == NULL)
		return -1;///<获取失败返回-1
    
	///视频信息
	m_nTotalFrame   = (int)cvGetCaptureProperty(m_pCapture, CV_CAP_PROP_FRAME_COUNT);///<获取视频总帧数
	m_nFps          = (int)cvGetCaptureProperty(m_pCapture, CV_CAP_PROP_FPS);///<获取视频帧率
	m_pStor         = cvCreateMemStorage(0);///<创建运动目标轮廓存储空间

	cvSetCaptureProperty(m_pCapture, CV_CAP_PROP_POS_FRAMES, 1);///<重置视频当前帧为首帧
	m_pOriFrame = cvQueryFrame(m_pCapture);///<获取当前视频帧图像
	if (m_pOriFrame == NULL) 
		return -1;///<当前帧图像获取失败，返回-1

	///视频帧信息
	m_nVideoH = m_pOriFrame->height;///<获取帧图像高度
	m_nVideoW = m_pOriFrame->width; ///<获取帧图像宽度

	///初始化其他参数
	if(InitSampleSetting() == -1)///<初始化压缩参数
		return -1;              ///<初始化压缩参数失败，返回-1
	
	InitStorage();	  ///<初始化图像数据，分配空间
//	InitGaussParam();   ///<初始化高斯模型
	InitTableParam();   ///<初始化数据库表
	InitAbstractVideo(pVideoPath);///<初始化摘要视频参数

	m_newTraceTextPath.Format("%s_newTrace.txt",pVideoPath);
	m_newTraceOfstream.open(m_newTraceTextPath,ios::out); 

	
	return 1;///<初始化函数执行成功，返回1
}
///@brief CVideo类的初始化压缩参数函数
/// 
///@param[in|out] NULL
///@pre  NULL
///@return NULL
///@retval NULL
///@post NULL
int CVideo::InitSampleSetting()
{
	double ratio = 1.0;
	double ratioH = (double)m_nVideoH/800.0;
	double ratioW = (double)m_nVideoW/800.0;// 使结果视频在800*800以内。目前选00015_h为测试视频，原视频640*480，结果视频为640*480
	ratio = MAX(ratioH,ratioW);

	m_nProcessingRatio   = (int)ceil(ratio);///<初始化视频帧大小处理比例
	
	//读取配置文件my.ini @by QiFeng
	int m_CompressionRatio = ::GetPrivateProfileInt(TEXT("InitSampleSetting"), 
                              TEXT("m_nCompressionRatioH"), 
                              0,   //如果没读到会被设置成0
							  TEXT("..\\my.ini"));
	if (m_CompressionRatio != 0)
	{
		m_nCompressionRatioH = m_CompressionRatio;///<初始化高度压缩比
		m_nCompressionRatioW = m_CompressionRatio;///<初始化宽度压缩比
	}
	else
	{
		m_nCompressionRatioH = 16;///<初始化高度压缩比
		m_nCompressionRatioW = 16;///<初始化宽度压缩比
	}
	
	m_nSamplingRate      = m_nFps/2;///<初始化采样帧数间隔
	m_nSampleTime        = 5;///<初始化采样时间间隔
	m_nTotalFrameToDo    = m_nTotalFrame - m_nFps;///<初始化总测试帧数
	if (m_nTotalFrameToDo < 3000)
	{///处理视频总帧数>5000,（4min视频可处理）否则视频果断无需处理,原为20000，@QiFeng
		m_nTotalFrameToDo = -1;
		m_ProcessingSpeed = -1;
		return m_ProcessingSpeed;
	}
	else
	{
		m_ProcessingSpeed = 0;///<初始化处理进度
	}
	m_nTotalFrameToDo    = 45000;

	m_nBGUpdateFrames    = MAX(m_nTotalFrameToDo/9,m_nFps*300);   ///<初始化背景更新总帧数
	m_nBGUpdateSampleFrame = m_nBGUpdateFrames/20;///<初始化背景更新采样帧数
	//m_nBGUpdateFrames    = 4000;
	//m_nBGUpdateSampleFrame = 400;
	
	return m_ProcessingSpeed;
} 
///@brief CVideo类的初始化图像数据，分配空间函数
/// 
///@param[in|out] NULL
///@pre  NULL
///@return NULL
///@retval NULL
///@post NULL
void CVideo::InitStorage()
{
	m_nVideoH /= m_nProcessingRatio;///<初始化视频帧处理高度
	m_nVideoW /= m_nProcessingRatio;///<初始化视频帧处理宽度
	m_nVideoResizeH   = m_nVideoH / m_nCompressionRatioH;///<初始化视频帧压碎后高度
	m_nVideoResizeW   = m_nVideoW / m_nCompressionRatioW;///<初始化视频帧压碎后高度
	m_nMaxTargetArea  = m_nVideoH * m_nVideoW / 1;  ///<初始化运动目标最大面积
	//读取配置文件my.ini @by QiFeng
	int m_MinTargetArea = ::GetPrivateProfileInt(TEXT("InitStorage"), 
                              TEXT("m_nMinTargetArea"), 
                              0,   //如果没读到会被设置成0
							  TEXT("..\\my.ini"));
	if(m_MinTargetArea != 0)
	{
		m_nMinTargetArea = m_MinTargetArea;  ///<初始化运动目标最小面积
	}
	else
	{
		m_nMinTargetArea  = m_nVideoH * m_nVideoW / 400;///<初始化运动目标最小面积
	}

	m_pProcessFrame   = cvCreateImage(cvSize(m_nVideoW, m_nVideoH), IPL_DEPTH_8U, 3);///<初始化处理图像帧，分配空间
	m_pGrayFrame      = cvCreateImage(cvSize(m_nVideoW, m_nVideoH), IPL_DEPTH_8U, 1);///<初始化灰度图像帧，分配空间
	m_pBGFrame        = cvCreateImage(cvSize(m_nVideoW, m_nVideoH), IPL_DEPTH_8U, 3);///<初始化背景图像帧，分配空间
	for (int i=0;i<10;i++)
	{
		m_pBGFinal[i]  = cvCreateImage(cvSize(m_nVideoW, m_nVideoH), IPL_DEPTH_8U, 3);///<初始化提取背景图像帧，分配空间
	}

	cvResize  (m_pOriFrame, m_pProcessFrame, CV_INTER_LINEAR);
	
	cv::Mat m_OriFrameMat = m_pOriFrame;
	cv::Mat m_GrayFrameMat = m_pGrayFrame;
	
	//降维到m_nVideoResizeW*m_nVideoResizeH
	resize(m_OriFrameMat,m_ResizeFrameMat,Size(m_nVideoResizeW,m_nVideoResizeH),0,0,CV_INTER_LINEAR);
	cvtColor(m_OriFrameMat, m_GrayFrameMat, CV_BGR2GRAY,1); ///<灰度化
	resize(m_GrayFrameMat,m_GrayResizeFrameMat,Size(m_nVideoResizeW,m_nVideoResizeH),0,0,CV_INTER_LINEAR);
	cvCopy    (m_pProcessFrame, m_pBGFrame);
	
//	m_pFrameMat      = cvCreateMat(m_nVideoH, m_nVideoW, CV_32FC3);///<初始化视频图像数组(3通道)
//	m_pBGMat         = cvCreateMat(m_nVideoH, m_nVideoW, CV_32FC3);///<初始化背景图像数组(3通道)
	m_pGrayFrameMat  = cvCreateMat(m_nVideoH, m_nVideoW, CV_32FC1);///<初始化视频图像数组(1通道)
	m_pGrayBGMat     = cvCreateMat(m_nVideoH, m_nVideoW, CV_32FC1);///<初始化背景图像数组(1通道)
	m_pGrayFGMat     = cvCreateMat(m_nVideoH, m_nVideoW, CV_32FC1);///<初始化前景图像数组(1通道)
//	cvConvert(m_pProcessFrame, m_pFrameMat);  
	m_FrameMat = m_pProcessFrame;
	m_BGMat = m_pProcessFrame;

	
//	m_pGrayFrameMat = m_pGrayFrame;
//	m_GrayBGMat = m_pGrayFrame;
	
	//m_pFrameMat(m_pProcessFrame);
	//cvConvert(m_pProcessFrame, m_pBGMat);
	cvConvert(m_pGrayFrame,    m_pGrayFrameMat);
	cvConvert(m_pGrayFrame,    m_pGrayBGMat);
	cvConvert(m_pGrayFrame,    m_pGrayFGMat);
}

///@brief CVideo类的初始化数据库表函数
/// 
///@param[in|out] NULL
///@pre  NULL
///@return NULL
///@retval NULL
///@post NULL
void CVideo::InitTableParam()
{
	///对视频路径做处理
	CString m_tmpFileName0 = m_strFilePath.c_str();
    CString m_tmpFileName1, m_tmpFileName2;
	GetVideoNameFromAbsolutePath(&m_tmpFileName0,&m_tmpFileName1);///<获取文件名(包含后缀)
	GetFileNameBeforeDot(&m_tmpFileName1,&m_tmpFileName2);        ///<获取文件名(不含后缀)
	
	///初始化各表表名
	m_tableParams.VideoFGTableName.Format("%s_VideoFGTable",m_tmpFileName2);
	m_tableParams.FGTraceTableName.Format("%s_FGTraceTable",m_tmpFileName2);
	m_tableParams.NewToOldFrameTableName.Format("%s_New2OldFrameTable",m_tmpFileName2);
	m_tableParams.NewTraceTable.Format("%s_NewTraceTable",m_tmpFileName2);
	m_tableParams.CombineSegsTableName.Format("%s_CombineSegsTable", m_tmpFileName2);

	///创建前景段信息表
	if(!m_MysqlSegHandle->DropTable(m_tableParams.VideoFGTableName))
	{
		AfxMessageBox("删除fgtable表错误");
		return;
	}	
	if(!m_MysqlSegHandle->CreateVideoFGTable(m_tableParams.VideoFGTableName))
	{
		AfxMessageBox("建表fgtable错误");
		return;
	}
	m_MysqlSegHandle->CleanTable(m_tableParams.VideoFGTableName);
	///创建运动物体轨迹表
	if(!m_MysqlSegHandle->DropTable(m_tableParams.FGTraceTableName))
	{
		AfxMessageBox("删除trace表错误");
		return;
	}	
	if(!m_MysqlSegHandle->CreateOrigTraceTable(m_tableParams.FGTraceTableName))
	{
		AfxMessageBox("建trace表错误");
		return;
	}
	m_MysqlSegHandle->CleanTable(m_tableParams.FGTraceTableName);
	///创建新旧视频帧对照表
	if(!m_MysqlSegHandle->DropTable(m_tableParams.NewToOldFrameTableName))
	{
		AfxMessageBox("删除NewToOldFrameTable表错误");
		return;
	}
	if(!m_MysqlSegHandle->CreateNewToOldFrameTable(m_tableParams.NewToOldFrameTableName))
	{
		AfxMessageBox("建表NewToOldFrameTable错误");
		return;
	}
	m_MysqlSegHandle->CleanTable(m_tableParams.NewToOldFrameTableName);
	///创建目标轨迹信息表
	if(!m_MysqlSegHandle->DropTable(m_tableParams.NewTraceTable))
	{
		AfxMessageBox("删除新trace表错误");
		return;
	}
	if(!m_MysqlSegHandle->CreateNewTraceTable(m_tableParams.NewTraceTable))
	{	
		AfxMessageBox("新trace表建表失败");
		return;
	}
	m_MysqlSegHandle->CleanTable(m_tableParams.NewTraceTable);
	
	///创建合成前景图信息表
	if (!m_MysqlSegHandle->DropTable(m_tableParams.CombineSegsTableName))
	{
		AfxMessageBox("删除Combine表错误");
		return;
	}
	if (!m_MysqlSegHandle->CreateCombineSegsTable(m_tableParams.CombineSegsTableName))
	{
		AfxMessageBox("建Combine表错误");
		return;
	}
	///创建暂存融合图像的文件夹
	CFileFind m_find;
	if (!m_find.FindFile("..\\"+m_tmpFileName2))///<FindFile在当前目录及其子目录下查询
	{
		CString CreateFileCommand = "md ..\\";
		CreateFileCommand += m_tmpFileName2;
		system(CreateFileCommand);///<DOS命令创建文件夹
	}
}
///@brief CVideo类的初始化摘要视频参数函数
/// 
///@param[in|out] NULL
///@pre  NULL
///@return NULL
///@retval NULL
///@post NULL
void CVideo::InitAbstractVideo(const char* pVideoPath)
{
	///对视频路径做处理
	CString m_tmpFileName0 = m_strFilePath.c_str();
    CString m_tmpFileName1, m_tmpFileName2;
	GetVideoNameFromAbsolutePath(&m_tmpFileName0,&m_tmpFileName1);///<获取文件名(包含后缀)
	GetFileNameBeforeDot(&m_tmpFileName1,&m_tmpFileName2);        ///<获取文件名(不含后缀)
	/*strSegVideoNewName    = pVideoPath;
	strFusionVideoNewName = pVideoPath;*/
	///初始化分割视频路径，新建分割视频写指针
	strSegVideoNewName = "..\\" + m_tmpFileName2 + "\\" + m_tmpFileName2 + "_newcut.avi";
	m_pVideoWriter4seg    = cvCreateVideoWriter(strSegVideoNewName.c_str(), CV_FOURCC('D','I','V','X'), m_nFps/2,cvSize(m_nVideoW, m_nVideoH));
	///初始化融合视频路径，新建融合视频写指针
	strFusionVideoNewName = "..\\" + m_tmpFileName2 + "\\" + m_tmpFileName2 + "_fusion.avi";
	m_pVideoWriter4fusion = cvCreateVideoWriter(strFusionVideoNewName.c_str(), CV_FOURCC('D','I','V','X'), m_nFps/2,cvSize(m_nVideoW, m_nVideoH));
}



///@brief CVideo类的视频分段函数
/// 
///@param[in|out] NULL
///@pre  NULL
///@return NULL
///@retval NULL
///@post NULL
void CVideo::VideoParaProcessing()
{
	if(m_IfContinue == FALSE)///<继续处理标志位为否，返回
		return;

	bool m_tableExist = false;///<数据表存在与否标志位
	///判断数据表是否正确存在
	if(!m_MysqlSegHandle->IfExistTable(m_tableParams.VideoFGTableName,&m_tableExist))
	{
		AfxMessageBox("数据库出错");
		return ;
	}
	if(!m_tableExist)
	{
		AfxMessageBox("前段表不存在");
		return;
	}

	if (!m_pCapture)///<视频头指针不存在，则返回
		return;
	cvSetCaptureProperty(m_pCapture, CV_CAP_PROP_POS_FRAMES,1);///<视频头指针存在，重置当前帧为首帧

	m_nCurrentFrame = (int)cvGetCaptureProperty(m_pCapture, CV_CAP_PROP_POS_FRAMES);///<获取当前帧号
	///初始化视频分段参数
	m_VideoBGParam.BGImgID    = 0;
	m_VideoFGParam.nOldPara   = 0;
	m_VideoBGParam.startFrame = m_nCurrentFrame;
	m_VideoBGParam.endFrame   = m_nCurrentFrame;
	m_VideoFGParam.startFrame = m_nCurrentFrame;
	m_VideoFGParam.endFrame   = m_nCurrentFrame;

	m_rFGSegThreshold = 0.01;///<初始化为0.01，前景至少占全图的1%
	
	///开始视频分段
	while(m_IfContinue)
	{
		m_pOriFrame = cvQueryFrame(m_pCapture);///<获取当前帧图像
		if (!m_pOriFrame)///<获取帧图像失败，退出循环
			break;	
		m_nCurrentFrame = (int)cvGetCaptureProperty(m_pCapture, CV_CAP_PROP_POS_FRAMES);///<获取当前帧号
		m_ProcessingSpeed = (double)m_nCurrentFrame/(double)m_nTotalFrameToDo;///<计算处理进度

        if(m_nCurrentFrame>m_VideoFGParam.endFrame)
		{
			if ((int)m_nCurrentFrame%m_nSamplingRate == 1)
			{///采样抽取帧图像做处理
				cvSmooth  (m_pOriFrame, m_pOriFrame, CV_GAUSSIAN,3,0,0);     ///<高斯滤波
				cv::Mat m_OriFrameMat = m_pOriFrame;

				resize(m_OriFrameMat,m_ResizeFrameMat,Size(m_nVideoResizeW,m_nVideoResizeH),0,0,CV_INTER_LINEAR);///<压缩 640*480 m_pResizeFrame=30*40
				cvtColor(m_ResizeFrameMat, m_GrayResizeFrameMat, CV_BGR2GRAY,1); ///<灰度化
			
				m_pBGSubMOG2.operator()(m_GrayResizeFrameMat, foregroundMat, 0.001);						
				m_GrayResizeFrameMat = foregroundMat; 
				
				this->ParaSegmentation();///<视频分段算法
			}
			if (m_nCurrentFrame >= m_nTotalFrameToDo)///<当前帧为总处理帧号，视频分段处理完成，退出循环
				break;
		}
	}
	///结束视频分段
	BGPara_Count = m_VideoBGParam.BGImgID;   ///<背景段数，即背景图片数
	FGPara_Count = m_VideoFGParam.nOldPara;  ///<前景段数，即分割片段数
}
///@brief CVideo类的视频背景提取函数
/// 
///@param[in|out] NULL
///@pre  NULL
///@return NULL
///@retval NULL
///@post NULL
void CVideo::VideoBGProcessing()
{
	if (!m_pCapture)///<视频头指针不存在，则返回
		return;
    cvSetCaptureProperty(m_pCapture, CV_CAP_PROP_POS_FRAMES,0);///<视频头指针存在，重置当前帧为首帧
	m_pOriFrame = cvQueryFrame(m_pCapture);///<获取当前帧图像
	if (!m_pOriFrame)///<获取帧图像失败，返回
		return;
	m_nCurrentFrame = (int)cvGetCaptureProperty(m_pCapture, CV_CAP_PROP_POS_FRAMES);///<获取当前帧号
	///初始化视频背景提取参数
	m_VideoBGParam.BGImgID    = 0;
	m_VideoBGParam.startFrame = m_nCurrentFrame;
	m_VideoBGParam.endFrame   = m_nCurrentFrame;

	float bgkNum = 0.0;
	float alpha = 0.0;
	float beta = 0.0;
	
	///开始视频背景提取
	bool m_IfInitBGFinal = true;///<背景图片初始化标志位
	for(;m_VideoBGParam.BGImgID < BGPara_Count && m_IfContinue;m_VideoBGParam.BGImgID++)
	{
		m_VideoBGParam.startFrame = m_nBGFinal[m_VideoBGParam.BGImgID];                ///<视频静态段起始帧号
		m_VideoBGParam.endFrame   = m_VideoBGParam.startFrame + m_nBGUpdateSampleFrame;///<视频静态段结束帧号
		//if(m_VideoBGParam.startFrame>500)
		if(m_VideoBGParam.startFrame>(m_nBGUpdateFrames/4))
		{
			cvSetCaptureProperty(m_pCapture, CV_CAP_PROP_POS_FRAMES, m_VideoBGParam.startFrame);
		}
		else
		{
			cvSetCaptureProperty(m_pCapture, CV_CAP_PROP_POS_FRAMES,0);///<视频头指针存在，重置当前帧为首帧
		}
		while (m_IfContinue)
		{
			m_pOriFrame = cvQueryFrame(m_pCapture);///<获取当前帧图像
			if (!m_pOriFrame)///<获取帧图像失败，退出内层循环
				break;
			m_nCurrentFrame = (int)cvGetCaptureProperty(m_pCapture, CV_CAP_PROP_POS_FRAMES);///<获取当前帧号
			if(m_nCurrentFrame < m_VideoBGParam.startFrame)
				continue;
			else if(m_nCurrentFrame>=m_VideoBGParam.startFrame && m_nCurrentFrame<=m_VideoBGParam.endFrame)
			{///<当前帧号处于该静态段范围，采样处理
				//if ((int)m_nCurrentFrame % m_nSamplingRate == 1)
				{
					m_ProcessingSpeed = 1.0+(double)m_nCurrentFrame/(double)m_nTotalFrameToDo;///<计算处理进度
					cvResize(m_pOriFrame, m_pProcessFrame);///<压缩
					m_FrameMat = m_pProcessFrame; //<类型转换：Iplimage转为Mat
					//cvRunningAvg(m_pFrameMat, m_pBGMat, 0.03, 0);///<更新移动平均
					m_BGMat = m_FrameMat.clone();  //复制图像Mat
					*m_pBGFrame = m_BGMat;       //<类型转换：Mat转为Iplimage
					if (m_IfInitBGFinal)
					{
						cvCopy(m_pBGFrame,m_pBGFinal[m_VideoBGParam.BGImgID]);///<背景图片初始化完成，复制图像存入背景图片数组
						m_IfInitBGFinal = false;///<背景图片初始化标志位置否
						bgkNum = 0.0;
					}
					bgkNum += 1.0;
					alpha = (bgkNum-1.0)/bgkNum;
					beta = 1.0-alpha;
					cvAddWeighted(m_pBGFinal[m_VideoBGParam.BGImgID],alpha,m_pBGFrame,beta,0,m_pBGFinal[m_VideoBGParam.BGImgID]);///<图像混色
				}
			}
			else if((m_nCurrentFrame > m_VideoBGParam.endFrame))
			{///当前帧号超出该静态段范围
				m_IfInitBGFinal = true; ///<背景图片初始化标志位重置为true
				break;                  ///<退出内层循环
			}		
		}	
	}
	///结束视频背景提取
}
///@brief CVideo类的视频前景切分函数
/// 
///@param[in|out] NULL
///@pre  NULL
///@return NULL
///@retval NULL
///@post NULL
void CVideo::VideoFGProcessing()
{
	if (!m_pCapture)///<视频头指针不存在，则返回
		return;
	cvSetCaptureProperty(m_pCapture, CV_CAP_PROP_POS_FRAMES,1);///<视频头指针存在，重置当前帧为首帧
	m_pOriFrame = cvQueryFrame(m_pCapture);///<获取当前帧图像
	if (!m_pOriFrame)///<获取帧图像失败，返回
		return;
	///初始化视频前景切分参数
	m_nSamplingRate          = MIN(m_nFps/4,5);
	m_traceTab.segID         = 0;
	m_VideoFGParam.nOldPara  = 0;
	m_VideoBGParam.BGImgID   = 0;
	m_New2OldParaParam.nOldPara = 0;
	m_New2OldParaParam.nNewPara = 0;
	m_segIDCount             = 0;
	m_segIDParam             = 1; ///<初始化不能为零
	m_FusionParam            = 0;
	BackgroundSubtractorMOG2 myGaussModel;
	char *myFileName = new char[40];
	int myCount = 0;
	///开始视频前景切分
	m_nCurrentFrame = (int)cvGetCaptureProperty(m_pCapture, CV_CAP_PROP_POS_FRAMES);///<获取当前帧号
	for (;m_VideoFGParam.nOldPara<FGPara_Count && m_IfContinue;m_VideoFGParam.nOldPara++)
	{
		m_VideoFGParam.startFrame = m_nFGFinal[m_VideoFGParam.nOldPara];             ///<视频动态段起始帧号
		m_VideoFGParam.endFrame   = m_VideoFGParam.startFrame + m_nFps*m_nSampleTime;///<视频动态段起始帧号
		m_segIDCount = 0;
		while(m_IfContinue)
		{
			m_pOriFrame = cvQueryFrame(m_pCapture);///<获取当前帧图像
			if (!m_pOriFrame)///<获取帧图像失败，退出内层循环
				break;
			m_nCurrentFrame = (int)cvGetCaptureProperty(m_pCapture, CV_CAP_PROP_POS_FRAMES);///<获取当前帧号
			m_ProcessingSpeed = 2.0+(double)m_nCurrentFrame/(double)m_nTotalFrameToDo;///<计算处理进度
			if (m_nCurrentFrame < m_VideoFGParam.startFrame)
				continue;
			///<当前帧号处于该动态段范围，采样处理
			if (m_nCurrentFrame/m_nBGUpdateFrames == m_VideoBGParam.BGImgID)
			{
				///背景更新
				if (m_VideoBGParam.BGImgID >= BGPara_Count)
					m_VideoBGParam.BGImgID = BGPara_Count-1;
				
				cvCvtColor(m_pBGFinal[m_VideoBGParam.BGImgID], m_pGrayFrame, CV_BGR2GRAY);
				cvConvert(m_pGrayFrame, m_pGrayBGMat);
				//m_GrayBGMat = m_pGrayFrame;
				if (m_VideoBGParam.BGImgID < BGPara_Count-1)
					m_VideoBGParam.BGImgID++;
			}
			
		//if ((int)m_nCurrentFrame%m_nSamplingRate == 1)
			if ((int)m_nCurrentFrame%3 == 1)
			{
				///前景分割
				cvResize(m_pOriFrame, m_pProcessFrame, CV_INTER_LINEAR);///<缩小
				cvCvtColor (m_pProcessFrame, m_pGrayFrame, CV_BGR2GRAY);///<灰度化
			
				Mat myForegroundMat;
				Mat myFrameMat(m_pGrayFrame,0);

				myGaussModel(myFrameMat, myForegroundMat, 0.01);
				erode(myForegroundMat, myForegroundMat, Mat());
				dilate(myForegroundMat, myForegroundMat, Mat());
				CvMat myCvMat = myForegroundMat;
				cvThreshold(&myCvMat, m_pGrayFrame, 10, 255.0, CV_THRESH_BINARY);///<前景图像二值化
				
				sprintf(myFileName, "%s%d%s", "D:\\myPhoto\\myForeGround", myCount, ".jpg");
				//cvSaveImage(myFileName, m_pGrayFrame);
				myCount++;
				FGSegmentation();///<视频前景切分算法
			}
			if (m_nCurrentFrame>m_VideoFGParam.endFrame)
				break;///<当前帧号超出该动态段范围，退出内层循环
		}
		if (m_segIDCount)
		{///该动态段范围存在运动物体
			TimeLineRestructed();///<重构时间轴
		}
		else
		{///该动态段范围不存在运动物体
			m_nParaFinal[m_VideoFGParam.nOldPara] = -1;///<分段表对应位置改为-1
		}
	}
	m_newTraceOfstream.close();///<关闭流

	///将保存在txt文件中的数据导入fgtrace表中
	if(m_IfContinue && !m_MysqlSegHandle->LoadTextData2Table(m_newTraceTextPath,m_tableParams.FGTraceTableName))
	{
		AfxMessageBox("数据载入出错");
		return;
	}

	///结束视频前景切分
	m_segIDCount = m_traceTab.segID;///<视频前景提取物体总数
}
///@brief CVideo类的视频分段算法
/// 
///@param[in|out] NULL
///@pre  NULL
///@return NULL
///@retval NULL
///@post NULL
void CVideo::ParaSegmentation()
{
	int i,j,k;
	k = 0;
	for (i = 0;i < m_nVideoResizeH; i++)
	{
		for (j = 0;j < m_nVideoResizeW; j++)
		{
			if (m_GrayResizeFrameMat.data[i*m_nVideoResizeW + j] != 0)
			{
				k++;///<计算二值前景图像非0像素点个数
			}
		}
	}
	double k_ratio = (double)k/(double)(m_nVideoResizeW*m_nVideoResizeH);
	if (k_ratio <=0.01)
		return;
	if (k_ratio/m_rFGSegThreshold > 1.5 || k_ratio/m_rFGSegThreshold < 0.79)
		m_rFGSegThreshold = k_ratio;
	
	///检测到运动视频段
	if (k_ratio >= m_rFGSegThreshold)
	{
		m_VideoFGParam.startFrame = m_nCurrentFrame;///<获取动态段起始帧号
		if (m_VideoFGParam.startFrame/m_nBGUpdateFrames == m_VideoBGParam.BGImgID)
		{///判断当前帧号所处的背景图像ID
			if (m_VideoFGParam.startFrame - m_VideoFGParam.endFrame > m_nBGUpdateSampleFrame)
			{///开始找静态段用于视频背景提取
				m_VideoBGParam.startFrame = (m_VideoFGParam.startFrame+m_VideoFGParam.endFrame)/2-m_nBGUpdateSampleFrame/2;///<获取静态段起始帧号
				m_VideoBGParam.endFrame   = (m_VideoFGParam.startFrame+m_VideoFGParam.endFrame)/2+m_nBGUpdateSampleFrame/2;///<获取静态段结束帧号
				m_nBGFinal[m_VideoBGParam.BGImgID] = m_VideoBGParam.startFrame;///<保存静态段位置信息
				m_VideoBGParam.BGImgID++;///<背景图像ID增加一
				cvSetCaptureProperty(m_pCapture, CV_CAP_PROP_POS_FRAMES,m_VideoBGParam.startFrame);///<视频头指针存在，重置当前帧为首帧
				//m_pOriFrame = cvQueryFrame(m_pCapture);///<获取当前帧图像
				//if (!m_pOriFrame)///<获取帧图像失败，返回
					//return;
				//m_nCurrentFrame = (int)cvGetCaptureProperty(m_pCapture, CV_CAP_PROP_POS_FRAMES);///<获取当前帧号
			}
		}
		m_VideoFGParam.endFrame = m_VideoFGParam.startFrame + m_nFps*m_nSampleTime;///<获取动态段结束帧号
		if (m_VideoFGParam.endFrame>= m_nTotalFrameToDo)///<当前帧为总处理帧号，处理完成
		{
			m_VideoFGParam.nOldPara--;///<视频前景段总数减一
			return;///<返回
		}
		else
		{
			if (m_VideoFGParam.endFrame>((m_VideoBGParam.BGImgID+1)*m_nBGUpdateFrames))
			{///视频前景段结束帧号超出对应背景图像ID范围
				m_VideoBGParam.startFrame = (m_VideoBGParam.BGImgID)*m_nBGUpdateFrames+m_nBGUpdateFrames/2-m_nBGUpdateSampleFrame/2;///<获取默认静态段起始帧号
				m_VideoBGParam.endFrame   = (m_VideoBGParam.BGImgID)*m_nBGUpdateFrames+m_nBGUpdateFrames/2+m_nBGUpdateSampleFrame/2;///<获取默认静态段结束帧号
				m_nBGFinal[m_VideoBGParam.BGImgID] = m_VideoBGParam.startFrame;///<保存静态段位置信息
				cvSetCaptureProperty(m_pCapture, CV_CAP_PROP_POS_FRAMES,m_VideoBGParam.startFrame);///<视频头指针存在，重置当前帧为首帧
				m_pOriFrame = cvQueryFrame(m_pCapture);///<获取当前帧图像
				if (!m_pOriFrame)///<获取帧图像失败，返回
					return;
				m_nCurrentFrame = (int)cvGetCaptureProperty(m_pCapture, CV_CAP_PROP_POS_FRAMES);///<获取当前帧号

				m_VideoBGParam.BGImgID++;///<背景图像ID增加一
			}
		}
		m_nFGFinal[m_VideoFGParam.nOldPara] = m_VideoFGParam.startFrame;///<保存动态段位置信息

	








		if(m_IfContinue && !m_MysqlSegHandle->InsertData2VideoFGTable(m_VideoFGParam,m_tableParams.VideoFGTableName))
		{///数据写入前景段信息表
			AfxMessageBox("前景段表插入出错");
			return ;
		}
		m_VideoFGParam.nOldPara++;///<动态段总数增加一
		return;///<返回
	}
}
///@brief CVideo类的视频前景切分算法
/// 
///@param[in|out] NULL
///@pre  NULL
///@return NULL
///@retval NULL
///@post NULL
void CVideo::FGSegmentation()
{
	float ratio = 0.0;
	m_pCont = NULL;
	cvFindContours(m_pGrayFrame, m_pStor, &m_pCont, sizeof(CvContour), CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, cvPoint(0, 0));///<获取二值前景图像运动物体轮廓信息
	CvSeq *mySeq = m_pCont;
	vector<CvSeq *> myLucky;
	for (; mySeq; mySeq = mySeq->h_next)
	{
		r = ((CvContour*)mySeq)->rect;///<获取当前轮廓外接矩形
		if ( mySeq->total > 6 &&(r.height*r.width > 750 ))///<轮廓点过少，不做处理
		{
			myLucky.push_back(mySeq);
		}

	}
	for (int i = 0; i<myLucky.size() ; i++)
	{
		if (myLucky[i] != NULL)
		{
			CvRect myRect = ((CvContour*)myLucky[i])->rect;///<获取当前轮廓外接矩形
			for (int j = 0; j <myLucky.size() ; j++)
			{
				if (i != j&& myLucky[j] != NULL)
				{
					CvRect myTemp = ((CvContour*)myLucky[j])->rect;///<获取当前轮廓外接矩形
					bool temp1 = myRect.x < myTemp.x + myTemp.width + 5 && myTemp.x < myRect.x + myRect.width + 5;
					bool temp2 = myRect.y < myTemp.y + myTemp.height + 5 && myTemp.y < myRect.y + myRect.height + 5;
					if (temp1&&temp2)
					{
						((CvContour*)myLucky[i])->rect.x = (myRect.x < myTemp.x ? myRect.x : myTemp.x);
						((CvContour*)myLucky[i])->rect.y = (myRect.y < myTemp.y ? myRect.y : myTemp.y);
						int maxX = (myRect.x + myRect.width> myTemp.x + myTemp.width ? myRect.x + myRect.width : myTemp.x + myTemp.width);
						int maxY = (myRect.y + myRect.height > myTemp.y + myTemp.height ? myRect.y + myRect.height : myTemp.y + myTemp.height);
						((CvContour*)myLucky[i])->rect.width = maxX - ((CvContour*)myLucky[i])->rect.x;
						((CvContour*)myLucky[i])->rect.height = maxY - ((CvContour*)myLucky[i])->rect.y;
						CvRect myTemp1 = ((CvContour*)myLucky[i])->rect;
						myLucky[j] = NULL;
					}
				}
			}
		}
	}
	for (int i = 0; i<myLucky.size() ; i++)
	{	//if (r.height*r.width>m_nMinTargetArea && r.height*r.width<m_nMaxTargetArea)
		
		if (myLucky[i] != NULL )
		{
			r = ((CvContour*)myLucky[i])->rect;///<获取当前轮廓外接矩形
			//if ( r.y > 61  && r.y + r.height < 418)
			//{
				cvRectangleR(m_pOriFrame, r, CV_RGB(0, 255, 0));
				m_traceTab.nX = r.x;
				m_traceTab.nY = MAX(r.y - OBJECT_MARGIN, 0);
				m_traceTab.nHeight = r.height + OBJECT_MARGIN;
				m_traceTab.nWidth = r.width + OBJECT_MARGIN;  //in case of losing part
				m_traceTab.origFrame = (int)m_nCurrentFrame;
				m_traceTab.nOldPara = m_VideoFGParam.nOldPara;
				ratio = (float)r.width*r.height / m_nVideoH / m_nVideoW;
				if (ratio>SEG_L_RATIO)
				{
					m_traceTab.segSize = 0;//"L";
				}
				else if (ratio>SEG_M_RATIO)
				{
					m_traceTab.segSize = 1;//"M";
				}
				else
				{
					m_traceTab.segSize = 2;//"S";
				}
				if (m_IfContinue &&!m_MysqlSegHandle->InsertData2OrigTraceTable(m_traceTab, m_tableParams.FGTraceTableName))
				{///数据写入运动物体轨迹表
					AfxMessageBox("trace表插入出错");
					return;
				}
				///将fgtrace写入流
				m_newTraceOfstream << m_traceTab.nOldPara << "," << m_traceTab.segID << "," << m_traceTab.nX << "," << m_traceTab.nY << "," << m_traceTab.nWidth << "," << m_traceTab.nHeight << "," << m_traceTab.origFrame << "," << m_traceTab.segSize << endl;
				m_traceTab.segID++;///<运动物体编号自增一
				m_segIDCount++;    ///<运动物体总数自增一
			//}
		}
	}
		
	
		
}	   
///@brief CVideo类的分割视频拼接函数
/// 
///@param[in|out] NULL
///@pre  NULL
///@return NULL
///@retval NULL
///@post NULL
void CVideo::VideoConnection()
{
	if (!m_pCapture)///<视频头指针不存在，则返回
		return;
	cvSetCaptureProperty(m_pCapture, CV_CAP_PROP_POS_FRAMES, 1);///<视频头指针存在，重置当前帧为首帧
	///初始化分割视频拼接参数
	SegFrame_Count = 0;
	int i;
	///开始分割视频拼接参数
	for(i = 0;i<FGPara_Count && m_IfContinue;i++)
	{
		m_VideoFGParam.startFrame = m_nFGFinal[i];///<获取动态段起始帧号
		m_VideoFGParam.endFrame   = m_VideoFGParam.startFrame + m_nFps*m_nSampleTime;///<获取动态段结束帧号
		if(m_VideoFGParam.startFrame>500)
		{
			cvSetCaptureProperty(m_pCapture, CV_CAP_PROP_POS_FRAMES, m_VideoFGParam.startFrame-500);///<视频头指针存在，重置当前帧为首帧
		}
		else
		{
			//cvSetCaptureProperty(m_pCapture, CV_CAP_PROP_POS_FRAMES, 1);///<视频头指针存在，重置当前帧为首帧
		}
		m_pOriFrame = cvQueryFrame(m_pCapture);///<获取当前帧图像
		if (!m_pOriFrame)///<获取帧图像失败，退出循环
			break;
		m_nCurrentFrame = (int)cvGetCaptureProperty(m_pCapture, CV_CAP_PROP_POS_FRAMES);///<获取当前帧号
       
		while (m_IfContinue)
		{
			m_pOriFrame = cvQueryFrame(m_pCapture);///<获取当前帧图像
			if (!m_pOriFrame)///<获取帧图像失败，退出循环
				break;
			m_nCurrentFrame = (int)cvGetCaptureProperty(m_pCapture, CV_CAP_PROP_POS_FRAMES);///<获取当前帧号
			m_ProcessingSpeed = 1.0+(double)m_nCurrentFrame/(double)m_nTotalFrameToDo;///<计算处理进度
			if (m_nCurrentFrame >= m_VideoFGParam.startFrame && m_nCurrentFrame<= m_VideoFGParam.endFrame)
			{///<当前帧号处于该动态段范围，采样处理
				if ((int)m_nCurrentFrame%m_nSamplingRate == 1)
				{
					cvResize(m_pOriFrame, m_pProcessFrame);
					cvWriteFrame(m_pVideoWriter4seg, m_pProcessFrame);///<写入分割视频
					SegFrame_Count++;
				}
			}
			if(m_nCurrentFrame>m_nTotalFrameToDo||m_nCurrentFrame>m_VideoFGParam.endFrame)
				break;
		}
	}
	///结束分割视频拼接参数
	if(i>=FGPara_Count)
	{
		if (m_pVideoWriter4seg!=NULL)
		{
			cvReleaseVideoWriter(&m_pVideoWriter4seg);
		}
		m_IfDoneSeg = true;///<非中断情况，视频分割完成标志位改为true
	}
}
///@brief CVideo类的时间轴重构函数
/// 
///@param[in|out] NULL
///@pre  NULL
///@return NULL
///@retval NULL
///@post NULL
void CVideo::TimeLineRestructed()
{
	///时间轴重构参数初始化
	if (m_segIDCount<5)
	{
		m_nParaFinal[m_VideoFGParam.nOldPara] = -1;///<动态段运动物体过少，判断为噪点
		return;///<返回，不作处理
	}
	if (m_segIDParam == 1)
	{
		m_segIDParam = m_segIDCount;///<初始化segID参数
	}
	if (abs((double)m_segIDCount/(double)m_segIDParam)-1.0 < 0.1)
	{
		///相邻动态段物体数目值近似，能为同一目标在一段时间内的位移运动，应视为整体
		m_segIDParam = (m_segIDCount+m_segIDParam)/2;///<计算动态段物体数目均值
		m_New2OldParaParam.nOldPara = m_VideoFGParam.nOldPara;
		m_nParaFinal[m_VideoFGParam.nOldPara] = m_New2OldParaParam.nNewPara;///<保存新旧视频段对照信息
		m_FusionParam++;///<视频融合参数自增一
		if (m_FusionParam >=4)
		//if (m_FusionParam >=8)
		{///融合为同一段视频的段总数超出范围
			m_segIDParam = 1;///<重置segID参数
			m_New2OldParaParam.nNewPara++;///<融合新段号自增一
			m_FusionParam = 0;///<重置视频融合参数
			return;///<返回
		}
	}
	else
	{
		///相邻动态段物体数目相差过大，表示运动目标变化多
		m_segIDParam = m_segIDCount;///<segID参数等于当前段物体总数
		m_New2OldParaParam.nOldPara = m_VideoFGParam.nOldPara;///<融合新段号等于当前旧段号
		if (m_FusionParam >=4)
		//if (m_FusionParam >=6)
		{///融合为同一段视频的段总数超出范围
			m_FusionParam = 0;///<重置视频融合参数
			m_New2OldParaParam.nNewPara++;///<融合新段号自增一
		}
		else
		{///融合为同一段视频的段总数未超出范围
			m_FusionParam++;///<视频融合参数自增一
		}
		m_nParaFinal[m_VideoFGParam.nOldPara] = m_New2OldParaParam.nNewPara;///<保存新旧视频段对照信息
	}
}
///@brief CVideo类的视频融合函数
/// 
///@param[in|out] NULL
///@pre  NULL
///@return NULL
///@retval NULL
///@post NULL
void CVideo::VideoFusionProcessing()
{
	if(m_IfContinue == FALSE)///<继续处理标志位为否，返回
		return;
	bool m_tableExist = false;///<数据表存在与否标志位
	///判断数据表是否正确存在
	if(!m_MysqlSegHandle->IfExistTable(m_tableParams.FGTraceTableName,&m_tableExist))
	{
		AfxMessageBox("数据库出错");
		return ;
	}
	if(!m_tableExist)
	{
		AfxMessageBox("原始trace表不存在");
		return;
	}
	m_tableExist = false;
	if(!m_MysqlSegHandle->IfExistTable(m_tableParams.NewToOldFrameTableName,&m_tableExist))
	{
		AfxMessageBox("数据库出错");
		return ;
	}
	if(!m_tableExist)
	{
		AfxMessageBox("新旧帧对应表不存在");
		return;
	}
	
	if (!m_pCapture)///<视频头指针不存在，则返回
		return;
	cvSetCaptureProperty(m_pCapture, CV_CAP_PROP_POS_FRAMES, 1);///<视频头指针存在，重置当前帧为首帧
	///初始化视频融合参数
	m_segIDParam   = 0;
	m_FusionParam  = 0;
	m_FusionStart  = 0;
	m_New2OldFrameParam.origFrame = 0;
	m_New2OldFrameParam.newFrame  = 0;
	m_VideoFGParam.nOldPara       = 0;
	///初始化待融合图像帧名称
	CString m_tmpFileName0 = m_strFilePath.c_str();
	CString m_tmpFileName1, m_tmpFileName2;
	GetVideoNameFromAbsolutePath(&m_tmpFileName0,&m_tmpFileName1);///<获取文件名(包含后缀)
	GetFileNameBeforeDot(&m_tmpFileName1,&m_tmpFileName2);        ///<获取文件名(不含后缀)
	m_frameName = "..\\"+ m_tmpFileName2 +"\\new_%d.jpg";

	/*m_pCombineSegsImage = cvCreateImage(cvSize(m_nVideoW, m_nVideoH), IPL_DEPTH_8U, 3);///<新建图像，存所有目标
	cvCopy(m_pBGFinal[0], m_pCombineSegsImage);///<初始化新视频帧图像为当前背景图片*/
			
			

	const int m_nPara4Fusion = 3;
	CombineSegCount = 1;
	ifCombinefst = true;
	CombinePicNum.push_back(0);
	CombinePicNum.push_back(0);
	/*for (int i = 0; i < 20; i++)
	{
		m_pCombineSegImage[i] = cvCreateImage(cvSize(m_nVideoW, m_nVideoH), IPL_DEPTH_8U, 3);
		cvCopy(m_pBGFinal[0], m_pCombineSegImage[i]);
	}*/
	for (int i = 0; i < 2; i++)
	{
		IplImage* myTempImage = cvCreateImage(cvSize(m_nVideoW, m_nVideoH), IPL_DEPTH_8U, 3);
		//if (m_pBGFinal[0]==NULL)
		//	cvCopy(m_pBGFinal[-1], myTempImage);
		//else
		//	cvCopy(m_pBGFinal[0], myTempImage);
		//cvCopy(m_pBGFinal[-1], m_pBGFinal[0]);
		if (m_VideoBGParam.BGImgID>=BGPara_Count)
				m_VideoBGParam.BGImgID = BGPara_Count-1;
		cvCopy(m_pBGFinal[m_VideoBGParam.BGImgID], myTempImage);
		mypCombineSegImage.push_back(myTempImage);
		
	}
	///开始视频融合
	while(m_VideoFGParam.nOldPara<FGPara_Count && m_IfContinue)
	{
		if (m_nParaFinal[m_VideoFGParam.nOldPara] == -1)
		{///<动态段表物体数目为-1时，不作处理
			m_VideoFGParam.nOldPara++;
			continue;
		}
		
		m_FusionStart = m_FusionParam;               ///<记录融合起始参数 
		m_New2OldFrameParam.newFrame = m_FusionStart;///<记录融合起始帧号
		m_pNewFrame   = cvCreateImage(cvSize(m_nVideoW, m_nVideoH), IPL_DEPTH_8U, 3);///<新建新视频帧图像，分配空间
		for (;m_nParaFinal[m_VideoFGParam.nOldPara]%m_nPara4Fusion == 0;m_VideoFGParam.nOldPara++)
		{///取第一部分融合
			ifinsert = true;
			m_VideoFGParam.startFrame = m_nFGFinal[m_VideoFGParam.nOldPara];           ///<获取本次融合起始帧号
			m_VideoFGParam.endFrame = m_VideoFGParam.startFrame + m_nFps*m_nSampleTime;///<获取本次融合结束帧号
			this->CreateNewFrame();///<创建新视频帧
			m_ProcessingSpeed = 3.0+(double)(m_VideoFGParam.nOldPara+1)/(double)FGPara_Count;///<计算处理进度
		}
		cvReleaseImage(&m_pNewFrame);///<释放新视频帧图像空间



		for (int Fus_sample = 1;Fus_sample<m_nPara4Fusion;Fus_sample++)
		{
			m_New2OldFrameParam.newFrame = m_FusionStart;///<重置融合起始帧号
			
			for(;m_nParaFinal[m_VideoFGParam.nOldPara]%m_nPara4Fusion == Fus_sample;m_VideoFGParam.nOldPara++)
			{///取第二部分融合
				ifinsert = true;
				m_VideoFGParam.startFrame = m_nFGFinal[m_VideoFGParam.nOldPara];           ///<获取本次融合起始帧号
				m_VideoFGParam.endFrame = m_VideoFGParam.startFrame + m_nFps*m_nSampleTime;///<获取本次融合结束帧号
				this->FusionNewFrame();///<融合新视频帧
				m_ProcessingSpeed = 3.0+(double)(m_VideoFGParam.nOldPara+1)/(double)FGPara_Count;///<计算处理进度
			}
		}
		if (m_FusionParam - m_FusionStart<=1)
			break;

		char m_strName[500];         ///<名称临时变量
		for (int k=m_FusionStart;k<m_FusionParam && m_IfContinue;k++)
		{
			sprintf_s(m_strName,m_frameName,k);
			m_pNewFrame = cvLoadImage(m_strName);            ///<获取融合后新视频帧图像
			cvWriteFrame(m_pVideoWriter4fusion, m_pNewFrame);///<写入融合视频(也即摘要视频)
			cvReleaseImage(&m_pNewFrame);///<释放新视频帧图像空间
		}
	}
	if (m_VideoFGParam.nOldPara >= FGPara_Count)
	{

		///目标轨迹信息表和前景段信息表合并为NewTraceTable
		if(m_IfContinue &&!m_MysqlSegHandle->Unite2NewTraceTable(m_tableParams.NewTraceTable,m_tableParams.FGTraceTableName,m_tableParams.NewToOldFrameTableName))
		{
			AfxMessageBox("新trace表生成失败");
			return;
		}
		cvWaitKey(1);
		//m_MysqlSegHandle->DropTable(m_tableParams.FGTraceTableName);      ///<删除目标轨迹信息表
		//cvWaitKey(1);///<等待时间，确保删表成功
		m_MysqlSegHandle->DropTable(m_tableParams.NewToOldFrameTableName);///<删除新旧视频帧对照表
		cvWaitKey(1);///<等待时间，确保删表成功
		//m_MysqlSegHandle->DropTable(m_tableParams.VideoFGTableName);      ///<删除前景段信息表
		//cvWaitKey(1);///<等待时间，确保删表成功
		if (m_pVideoWriter4fusion!=NULL)
		{
			cvReleaseVideoWriter(&m_pVideoWriter4fusion);
		}
		m_IfDoneFus = true;            ///<视频融合完成标志位为true
		//cvWaitKey(1);
		FusFrame_Count = m_FusionParam;///<记录视频融合总帧数
	}


	//cvSaveImage("..\\" + m_tmpFileName2 + "\\All_0.jpg", m_pCombineSegImage[0]);////<保存新视频帧图像
	//cvSaveImage("..\\" + m_tmpFileName2 + "\\All_1.jpg", m_pCombineSegImage[1]);


	///结束视频融合
	for (int i = 0; i < mypCombineSegImage.size()-1; i++)
	{
		CString Allpicname;
		Allpicname.Format(m_tmpFileName2 + "\\All%d", i);
		IplImage* myTemp = mypCombineSegImage[i];
		cvSaveImage("..\\" +Allpicname + ".jpg", myTemp);///<保存新视频帧图像
		//cvReleaseImage(&mypCombineSegImage[i]);///<释放新视频帧图像空间
	}
	CombinePicNum.clear();
	mypCombineSegImage.clear();
}
///@brief CVideo类的创建新视频帧函数
/// 
///@param[in|out] NULL
///@pre  NULL
///@return NULL
///@retval NULL
///@post NULL
void CVideo::CreateNewFrame()
{
	if (m_segIDParam >= m_segIDCount)///<已创建相关的新视频帧，不做处理
		return;
	///初始化创建新视频帧参数
	bool m_IfFindStart = true;
	bool m_IfSameFrame = true;
	char m_strName[500];///<名称临时变量

	while (m_IfContinue)
	{
		///获取当前融合段起始帧号和结束帧号
		if(m_IfContinue &&!m_MysqlSegHandle->GetInfor1DataFromOrigTraceTable(m_segIDParam,&m_traceTab,m_tableParams.FGTraceTableName))
		{
			AfxMessageBox("数据库出错");
			return;
		}
		if (m_traceTab.origFrame>=m_VideoFGParam.startFrame)
			break;
		else
		{
			m_segIDParam++;
			if (m_segIDParam >= m_segIDCount)
				return;
		}
	}
	///开始创建新视频帧
	OrigTraceTable     m_traceTabbefore;
	while (m_IfContinue)
	{
		//0702 needs improve
		m_pOriFrame = cvQueryFrame(m_pCapture);///<获取当前帧图像
		if (!m_pOriFrame)///<获取帧图像失败，退出循环
			return;
		m_nCurrentFrame = (int)cvGetCaptureProperty(m_pCapture, CV_CAP_PROP_POS_FRAMES);///<获取当前帧号
		if (m_nCurrentFrame>=m_traceTab.origFrame && m_nCurrentFrame<=m_VideoFGParam.endFrame)
		{///当前帧号处于该动态段范围，有如下处理 //
			m_IfSameFrame = true;
			cvResize(m_pOriFrame, m_pProcessFrame);///<缩小
			///获取背景图片ID
			m_VideoBGParam.BGImgID = (int)(m_VideoFGParam.startFrame+m_VideoFGParam.endFrame)/(2*m_nBGUpdateFrames);
			if (m_VideoBGParam.BGImgID>=BGPara_Count)
				m_VideoBGParam.BGImgID = BGPara_Count-1;
			cvCopy(m_pBGFinal[m_VideoBGParam.BGImgID], m_pNewFrame);///<初始化新视频帧图像为当前背景图片
			///开始找第一条数据
			if(m_IfFindStart)
			{
				if((int)m_nCurrentFrame < m_traceTab.origFrame)
					continue;
				else if ((int)m_nCurrentFrame==m_traceTab.origFrame)
				{
					///当前帧为待融合的原始帧
					m_IfFindStart = false;
					///读取运动目标信息
					r.x = m_traceTab.nX;
					r.y = m_traceTab.nY;
					r.width = m_traceTab.nWidth;
					r.height = m_traceTab.nHeight;
					///将前景目标图像嵌入对应位置的新视频帧图像中
					cvSetImageROI(m_pNewFrame,     r);
					cvSetImageROI(m_pProcessFrame, r);
					cvAddWeighted(m_pProcessFrame, 0.5, m_pNewFrame, 0.5, 0, m_pNewFrame);
					cvResetImageROI(m_pProcessFrame);
					cvResetImageROI(m_pNewFrame);
					
					int temp = m_MysqlSegHandle->FindSegIDFromFGTraceTable(m_tableParams.FGTraceTableName,m_VideoFGParam.nOldPara);
					
					
					for (int i = 0; i < CombineSegCount; i++)
					{
						if (ifinsert)
						{
							if (m_segIDParam >= (temp + m_nSampleTime))
							{
								if (ifCombinefst)
								{
									cvSetImageROI(mypCombineSegImage[i], r);
									cvSetImageROI(m_pProcessFrame, r);
									cvAddWeighted(m_pProcessFrame, 0.8, mypCombineSegImage[i], 0.2, 0, mypCombineSegImage[i]);
									ifCombinefst = false;
									m_traceTab.PicID = i;
									if (m_IfContinue &&!m_MysqlSegHandle->InsertData2CombineSegsTable(m_traceTab, m_tableParams.CombineSegsTableName))
									{
										AfxMessageBox("合成前景图信息表插入错误");
										return;
									}
									cvResetImageROI(m_pProcessFrame);
									cvResetImageROI(mypCombineSegImage[i]);
									ifinsert = false;
								}
								if (m_segIDParam == (temp + m_nSampleTime))
								{
									cvSetImageROI(mypCombineSegImage[CombineSegCount], r);
									cvSetImageROI(m_pProcessFrame, r);
									cvAddWeighted(m_pProcessFrame, 0.8, mypCombineSegImage[CombineSegCount], 0.2, 0, mypCombineSegImage[CombineSegCount]);
									cvResetImageROI(m_pProcessFrame);
									cvResetImageROI(mypCombineSegImage[CombineSegCount]);
									if (!m_MysqlSegHandle->GetInfor1DataFromOrigTraceTable(m_segIDParam, &m_traceTabbefore, m_tableParams.FGTraceTableName))
									{
										AfxMessageBox("新旧帧对应表插入错误");
										return;
									}
								}
								if (m_MysqlSegHandle->GetObjectInfoFromObjectTable(&m_objDetectedInfos, m_tableParams.CombineSegsTableName) && !ifCombinefst&&ifinsert)
								{
									bool test_m_CombineSegsNow;
									bool test_m_CombineSegsBefore = true;
									for (int newsegID = CombinePicNum[i]; newsegID < CombinePicNum[i + 1]; newsegID++)
									{
										test_m_CombineSegsNow = r.x >= (m_objDetectedInfos.at(newsegID).roi.x + m_objDetectedInfos.at(newsegID).roi.width) || (r.x + r.width) <= m_objDetectedInfos.at(newsegID).roi.x || r.y >= (m_objDetectedInfos.at(newsegID).roi.y + m_objDetectedInfos.at(newsegID).roi.height) || (r.y + r.height) <= m_objDetectedInfos.at(newsegID).roi.y;
										test_m_CombineSegsNow = test_m_CombineSegsNow&&test_m_CombineSegsBefore;
										test_m_CombineSegsBefore = test_m_CombineSegsNow;
									}
									if (test_m_CombineSegsNow&&ifinsert)
									{
										///将前景目标图像嵌入对应位置的新视频帧图像中
										cvSetImageROI(mypCombineSegImage[i], r);
										cvSetImageROI(m_pProcessFrame, r);
										cvAddWeighted(m_pProcessFrame, 0.8, mypCombineSegImage[i], 0.2, 0, mypCombineSegImage[i]);
										ifCombinefst = false;
										m_traceTab.PicID = i;
										if (m_IfContinue &&!m_MysqlSegHandle->InsertData2CombineSegsTable(m_traceTab, m_tableParams.CombineSegsTableName))
										{
											AfxMessageBox("合成前景图信息表插入错误");
											return;
										}
										cvResetImageROI(m_pProcessFrame);
										cvResetImageROI(mypCombineSegImage[i]);
										ifinsert = false;
										CombinePicNum[i + 1] = m_objDetectedInfos.size() + 1;
									}
								}
							}
						}
					}

					if (m_New2OldFrameParam.origFrame!=m_traceTab.origFrame)
					{
						m_New2OldFrameParam.origFrame = m_traceTab.origFrame;
						///保存数据到新旧视频帧对照表
						if(m_IfContinue &&!m_MysqlSegHandle->InsertData2NewToOldFrameTable(m_New2OldFrameParam,m_tableParams.NewToOldFrameTableName))
						{
							AfxMessageBox("新旧帧对应表插入数据出错");
							return;
						}
					}
					m_segIDParam++;///<该段已融合目标数自增一
					if (m_segIDParam >= m_segIDCount)
						return;///<该段已融合目标数超出该段总目标数，返回
				}
				else
				{
					///当前帧无法匹配数据表数据
					m_segIDParam++;///<该段已融合目标数自增一
					if (m_segIDParam >= m_segIDCount)
						return;///<该段已融合目标数超出该段总目标数，返回
				}
			}///结束找第一条数据

			///前景目标处于原始图像同一帧的处理
 			while (m_IfSameFrame)
			{
				///获取数据表中当前待融合帧信息
				if(m_IfContinue &&!m_MysqlSegHandle->GetInfor1DataFromOrigTraceTable(m_segIDParam,&m_traceTab,m_tableParams.FGTraceTableName))
				{
					AfxMessageBox("读取原始trace表出错");
					return;
				}
				if (m_traceTab.origFrame > m_VideoFGParam.endFrame)
					break;///<帧号超出该段范围，返回
				if ((int)m_nCurrentFrame == m_traceTab.origFrame)
				{
					///当前帧为待融合的原始帧，读取运动目标信息
					r.x = m_traceTab.nX;
					r.y = m_traceTab.nY;
					r.width = m_traceTab.nWidth;
					r.height = m_traceTab.nHeight;
					///将前景目标图像嵌入对应位置的新视频帧图像中
					cvSetImageROI(m_pNewFrame,     r);
					cvSetImageROI(m_pProcessFrame, r);
					cvAddWeighted(m_pProcessFrame, 0.5, m_pNewFrame, 0.5, 0, m_pNewFrame);
					cvResetImageROI(m_pProcessFrame);
					cvResetImageROI(m_pNewFrame);
					int temp = m_MysqlSegHandle->FindSegIDFromFGTraceTable(m_tableParams.FGTraceTableName,m_VideoFGParam.nOldPara);
					

					
					for (int i = 0; i < CombineSegCount; i++)
					{
						if (ifinsert)
						{
							if (m_segIDParam >= (temp + m_nSampleTime))
							{
								if (ifCombinefst)
								{
									cvSetImageROI(mypCombineSegImage[i], r);
									cvSetImageROI(m_pProcessFrame, r);
									cvAddWeighted(m_pProcessFrame, 0.8, mypCombineSegImage[i], 0.2, 0, mypCombineSegImage[i]);
									ifCombinefst = false;
									m_traceTab.PicID = i;
									if (m_IfContinue &&!m_MysqlSegHandle->InsertData2CombineSegsTable(m_traceTab, m_tableParams.CombineSegsTableName))
									{

										AfxMessageBox("合成前景图信息表插入错误");
										return;
									}
									cvResetImageROI(m_pProcessFrame);
									cvResetImageROI(mypCombineSegImage[i]);
									ifinsert = false;
								}
								if (m_segIDParam == (temp + m_nSampleTime))
								{
									cvSetImageROI(mypCombineSegImage[CombineSegCount], r);
									cvSetImageROI(m_pProcessFrame, r);
									cvAddWeighted(m_pProcessFrame, 0.8, mypCombineSegImage[CombineSegCount], 0.2, 0, mypCombineSegImage[CombineSegCount]);
									cvResetImageROI(m_pProcessFrame);
									cvResetImageROI(mypCombineSegImage[CombineSegCount]);
									if (m_segIDParam == 373)
										IplImage *Image = mypCombineSegImage[4];
									if (!m_MysqlSegHandle->GetInfor1DataFromOrigTraceTable(m_segIDParam, &m_traceTabbefore, m_tableParams.FGTraceTableName))
									{
										AfxMessageBox("新旧帧对应表插入错误");
										return;
									}
								}
								if (m_MysqlSegHandle->GetObjectInfoFromObjectTable(&m_objDetectedInfos, m_tableParams.CombineSegsTableName) && !ifCombinefst&&ifinsert)
								{
									bool test_m_CombineSegsNow;
									bool test_m_CombineSegsBefore = true;
									for (int newsegID = CombinePicNum[i]; newsegID < CombinePicNum[i + 1]; newsegID++)
									{
										test_m_CombineSegsNow = r.x >= (m_objDetectedInfos.at(newsegID).roi.x + m_objDetectedInfos.at(newsegID).roi.width) || (r.x + r.width) <= m_objDetectedInfos.at(newsegID).roi.x || r.y >= (m_objDetectedInfos.at(newsegID).roi.y + m_objDetectedInfos.at(newsegID).roi.height) || (r.y + r.height) <= m_objDetectedInfos.at(newsegID).roi.y;
										test_m_CombineSegsNow = test_m_CombineSegsNow&&test_m_CombineSegsBefore;
										test_m_CombineSegsBefore = test_m_CombineSegsNow;
									}
									if (test_m_CombineSegsNow&&ifinsert)
									{
										///将前景目标图像嵌入对应位置的新视频帧图像中
										cvSetImageROI(mypCombineSegImage[i], r);
										cvSetImageROI(m_pProcessFrame, r);
										cvAddWeighted(m_pProcessFrame, 0.8, mypCombineSegImage[i], 0.2, 0, mypCombineSegImage[i]);
										ifCombinefst = false;
										m_traceTab.PicID = i;
										if (m_IfContinue &&!m_MysqlSegHandle->InsertData2CombineSegsTable(m_traceTab, m_tableParams.CombineSegsTableName))
										{
											AfxMessageBox("合成前景图信息表插入错误");
											return;
										}
										cvResetImageROI(m_pProcessFrame);
										cvResetImageROI(mypCombineSegImage[i]);
										ifinsert = false;
										CombinePicNum[i + 1] = m_objDetectedInfos.size()+1;
									}
								}
							}
						}
					}


					//if( m_segIDParam == (temp +m_nSampleTime))
					//{
					//			/////将前景目标图像嵌入对应位置的新视频帧图像中2
					//	cvSetImageROI(m_pCombineSegsImage,     r);
					//	cvSetImageROI(m_pProcessFrame, r);
					//	cvAddWeighted(m_pProcessFrame, 0.8, m_pCombineSegsImage, 0.2, 0, m_pCombineSegsImage);
					//	if (m_IfContinue &&!m_MysqlSegHandle->InsertData2CombineSegsTable(m_traceTab, m_tableParams.CombineSegsTableName))
					//	{
					//		AfxMessageBox("合成前景图信息表插入错误");
					//		return;
					//	}
					//	cvResetImageROI(m_pProcessFrame);
					//	cvResetImageROI(m_pCombineSegsImage);
					//}
					if (m_New2OldFrameParam.origFrame!=m_traceTab.origFrame)
					{
						m_New2OldFrameParam.origFrame = m_traceTab.origFrame;
						if(m_IfContinue &&!m_MysqlSegHandle->InsertData2NewToOldFrameTable(m_New2OldFrameParam,m_tableParams.NewToOldFrameTableName))
						{
							AfxMessageBox("新旧帧对应表插入错误");
							return;
						}
					}
					m_segIDParam++;
					if (m_segIDParam >= m_segIDCount)
						return;///<该段已融合目标数超出该段总目标数，返回
				}
				else
				{
					m_IfSameFrame = false;
				}
			}
			
			///前景目标处于原始图像不同帧的处理
			if (m_IfSameFrame == false)
			{
				sprintf_s(m_strName,m_frameName,m_New2OldFrameParam.newFrame);
				cvSaveImage(m_strName, m_pNewFrame);///<保存新视频帧
				m_New2OldFrameParam.newFrame++;     ///<新视频帧号自增一
				m_FusionParam++;                    ///<视频融合参数自增一
			}
		}
		if (m_nCurrentFrame > m_VideoFGParam.endFrame)
			break;///<当前帧号超出该动态段范围，退出循环
	}
	if (ifinsert)
	{
		m_traceTabbefore.PicID = CombineSegCount;
		if (m_IfContinue &&!m_MysqlSegHandle->InsertData2CombineSegsTable(m_traceTabbefore, m_tableParams.CombineSegsTableName))
		{
			AfxMessageBox("合成前景图信息表插入错误");
			return;
		}
		CombineSegCount++;
		IplImage* myTempImage = cvCreateImage(cvSize(m_nVideoW, m_nVideoH), IPL_DEPTH_8U, 3);
		cvCopy(m_pBGFinal[m_VideoBGParam.BGImgID], myTempImage);
		mypCombineSegImage.push_back(myTempImage);
		CombinePicNum.push_back(m_objDetectedInfos.size()+1);
	}
	if (!ifinsert)
	{
		cvCopy(m_pBGFinal[m_VideoBGParam.BGImgID], mypCombineSegImage[CombineSegCount]);
	}
	///结束创建新视频帧
	m_FusionParam = MAX(m_FusionParam, m_New2OldFrameParam.newFrame);///<保存该段新视频总帧数
}
///@brief CVideo类的融合新视频帧函数
/// 
///@param[in|out] NULL
///@pre  NULL
///@return NULL
///@retval NULL
///@post NULL
void CVideo::FusionNewFrame()
{
	if (m_segIDParam >= m_segIDCount)///<已创建相关的新视频帧，不做处理
		return;
	///初始化融合新视频帧参数
	bool m_IfFindStart = true;
	bool m_IfSameFrame = true;
	char m_strName[500];///<名称临时变量

	while (m_IfContinue)
	{///获取当前融合段起始帧号和结束帧号
		if(m_IfContinue &&!m_MysqlSegHandle->GetInfor1DataFromOrigTraceTable(m_segIDParam,&m_traceTab,m_tableParams.FGTraceTableName))//旧段首帧
		{
			AfxMessageBox("读取原始trace表出错");
			return;
		}
		if (m_traceTab.origFrame>=m_VideoFGParam.startFrame)
			break;
		else
		{
			m_segIDParam++;
			if (m_segIDParam >= m_segIDCount)
				return;
		}
	}
	///开始融合新视频帧
	OrigTraceTable     m_traceTabbefore;
	while (m_IfContinue)
	{
		m_pOriFrame = cvQueryFrame(m_pCapture);///<获取当前帧图像
		if (!m_pOriFrame)///<获取帧图像失败，退出循环
			return;
		m_nCurrentFrame = (int)cvGetCaptureProperty(m_pCapture, CV_CAP_PROP_POS_FRAMES);///<获取当前帧号
		if (m_nCurrentFrame>=m_traceTab.origFrame && m_nCurrentFrame<=m_VideoFGParam.endFrame)
		{///当前帧号处于该动态段范围，有如下处理
			m_IfSameFrame = true;
			cvResize(m_pOriFrame, m_pProcessFrame);///<缩小
			if (m_New2OldFrameParam.newFrame < m_FusionParam)
			{///新视频帧已存在，则读取新视频帧
				sprintf_s(m_strName,m_frameName,m_New2OldFrameParam.newFrame);
				m_pNewFrame = cvLoadImage(m_strName);
			}
			else
			{
				///新视频帧不存在，则创建新视频帧
				m_VideoBGParam.BGImgID = (int)(m_VideoFGParam.startFrame+m_VideoFGParam.endFrame)/(2*m_nBGUpdateFrames);
				if (m_VideoBGParam.BGImgID>=BGPara_Count)
					m_VideoBGParam.BGImgID = BGPara_Count-1;

				m_pNewFrame = cvCreateImage(cvSize(m_nVideoW, m_nVideoH), IPL_DEPTH_8U, 3);
				cvCopy(m_pBGFinal[m_VideoBGParam.BGImgID], m_pNewFrame);
			}

			///开始找第一条数据
			if(m_IfFindStart)
			{
				if((int)m_nCurrentFrame < m_traceTab.origFrame)
					continue;
				else if ((int)m_nCurrentFrame==m_traceTab.origFrame)
				{
					///当前帧为待融合的原始帧
					m_IfFindStart = false;
					///读取运动目标信息
					r.x = m_traceTab.nX;
					r.y = m_traceTab.nY;
					r.width = m_traceTab.nWidth;
					r.height = m_traceTab.nHeight;
					///将前景目标图像嵌入对应位置的新视频帧图像中
					cvSetImageROI(m_pNewFrame,     r);
					cvSetImageROI(m_pProcessFrame, r);
					cvAddWeighted(m_pProcessFrame, 0.5, m_pNewFrame, 0.5, 0, m_pNewFrame);
					cvResetImageROI(m_pProcessFrame);
					cvResetImageROI(m_pNewFrame);

					int temp = m_MysqlSegHandle->FindSegIDFromFGTraceTable(m_tableParams.FGTraceTableName,m_VideoFGParam.nOldPara);
					

					
					for (int i = 0; i < CombineSegCount; i++)
					{
						if (ifinsert)
						{
							if (m_segIDParam >= (temp + m_nSampleTime))
							{
								if (ifCombinefst)
								{
									cvSetImageROI(mypCombineSegImage[i], r);
									cvSetImageROI(m_pProcessFrame, r);
									cvAddWeighted(m_pProcessFrame, 0.8, mypCombineSegImage[i], 0.2, 0, mypCombineSegImage[i]);
									ifCombinefst = false;
									m_traceTab.PicID = i;
									if (m_IfContinue &&!m_MysqlSegHandle->InsertData2CombineSegsTable(m_traceTab, m_tableParams.CombineSegsTableName))
									{
										AfxMessageBox("合成前景图信息表插入错误");
										return;
									}
									cvResetImageROI(m_pProcessFrame);
									cvResetImageROI(mypCombineSegImage[i]);
									ifinsert = false;
								}
								if (m_segIDParam == (temp + m_nSampleTime))
								{
									cvSetImageROI(mypCombineSegImage[CombineSegCount], r);
									cvSetImageROI(m_pProcessFrame, r);
									cvAddWeighted(m_pProcessFrame, 0.8, mypCombineSegImage[CombineSegCount], 0.2, 0, mypCombineSegImage[CombineSegCount]);
									cvResetImageROI(m_pProcessFrame);
									cvResetImageROI(mypCombineSegImage[CombineSegCount]);
									if (!m_MysqlSegHandle->GetInfor1DataFromOrigTraceTable(m_segIDParam, &m_traceTabbefore, m_tableParams.FGTraceTableName))
									{
										AfxMessageBox("新旧帧对应表插入错误");
										return;
									}
								}
								if (m_MysqlSegHandle->GetObjectInfoFromObjectTable(&m_objDetectedInfos, m_tableParams.CombineSegsTableName) && !ifCombinefst&&ifinsert)
								{
									bool test_m_CombineSegsNow;
									bool test_m_CombineSegsBefore = true;
									for (int newsegID = CombinePicNum[i]; newsegID < CombinePicNum[i + 1]; newsegID++)
									{
										test_m_CombineSegsNow = r.x >= (m_objDetectedInfos.at(newsegID).roi.x + m_objDetectedInfos.at(newsegID).roi.width) || (r.x + r.width) <= m_objDetectedInfos.at(newsegID).roi.x || r.y >= (m_objDetectedInfos.at(newsegID).roi.y + m_objDetectedInfos.at(newsegID).roi.height) || (r.y + r.height) <= m_objDetectedInfos.at(newsegID).roi.y;
										test_m_CombineSegsNow = test_m_CombineSegsNow&&test_m_CombineSegsBefore;
										test_m_CombineSegsBefore = test_m_CombineSegsNow;
									}
									if (test_m_CombineSegsNow&&ifinsert)
									{
										///将前景目标图像嵌入对应位置的新视频帧图像中
										cvSetImageROI(mypCombineSegImage[i], r);
										cvSetImageROI(m_pProcessFrame, r);
										cvAddWeighted(m_pProcessFrame, 0.8, mypCombineSegImage[i], 0.2, 0, mypCombineSegImage[i]);
										ifCombinefst = false;
										m_traceTab.PicID = i;
										if (m_IfContinue &&!m_MysqlSegHandle->InsertData2CombineSegsTable(m_traceTab, m_tableParams.CombineSegsTableName))
										{
											AfxMessageBox("合成前景图信息表插入错误");
											return;
										}
										cvResetImageROI(m_pProcessFrame);
										cvResetImageROI(mypCombineSegImage[i]);
										ifinsert = false;
										CombinePicNum[i + 1] = m_objDetectedInfos.size()+1;
									}
								}
							}
						}
					}

					//if( m_segIDParam == (temp +m_nSampleTime))
					//{
					//			/////将前景目标图像嵌入对应位置的新视频帧图像中
					//	mypCombineSegImage[0] = cvCreateImage(cvSize(m_nVideoW, m_nVideoH), IPL_DEPTH_8U, 3);
					//	cvCopy(m_pBGFinal[0], mypCombineSegImage[0]);
					//	cvSetImageROI(mypCombineSegImage[0],     r);
					//	cvSetImageROI(m_pProcessFrame, r);
					//	cvAddWeighted(m_pProcessFrame, 0.8, mypCombineSegImage[0], 0.2, 0, mypCombineSegImage[0]);
					//	if (m_IfContinue &&!m_MysqlSegHandle->InsertData2CombineSegsTable(m_traceTab, m_tableParams.CombineSegsTableName))
					//	{
					//		AfxMessageBox("合成前景图信息表插入错误");
					//		return;
					//	}
					//	cvResetImageROI(m_pProcessFrame);
					//	cvResetImageROI(mypCombineSegImage[0]);
					//}

					if (m_New2OldFrameParam.origFrame!=m_traceTab.origFrame)
					{
						m_New2OldFrameParam.origFrame = m_traceTab.origFrame;
						if(!m_MysqlSegHandle->InsertData2NewToOldFrameTable(m_New2OldFrameParam,m_tableParams.NewToOldFrameTableName))
						{
							AfxMessageBox("新旧帧对应表插入错误");
							return;
						}
					}
					m_segIDParam++;
					if (m_segIDParam >= m_segIDCount)
						return;///<该段已融合目标数超出该段总目标数，返回
				}
				else
				{
					m_segIDParam++;
					if (m_segIDParam >= m_segIDCount)
						return;
				}
			}///结束找第一条数据

			///前景目标处于原始图像同一帧的处理
			while (m_IfSameFrame)
			{
				///获取数据表中当前待融合帧信息
				if(!m_MysqlSegHandle->GetInfor1DataFromOrigTraceTable(m_segIDParam,&m_traceTab,m_tableParams.FGTraceTableName))
				{
					AfxMessageBox("读取原始trace表出错");
					return;
				}
				if (m_traceTab.origFrame > m_VideoFGParam.endFrame)
					break;///<帧号超出该段范围，返回
				if ((int)m_nCurrentFrame == m_traceTab.origFrame)
				{
					///当前帧为待融合的原始帧，读取运动目标信息
					/*r.x = m_traceTab.nLeft;
					r.y = m_traceTab.nTop;
					r.width = m_traceTab.nRight - m_traceTab.nLeft;
					r.height = m_traceTab.nBottom - m_traceTab.nTop;*/
					r.x = m_traceTab.nX;
					r.y = m_traceTab.nY;
					r.width = m_traceTab.nWidth;
					r.height = m_traceTab.nHeight;
					///将前景目标图像嵌入对应位置的新视频帧图像中
					cvSetImageROI(m_pNewFrame,     r);
					cvSetImageROI(m_pProcessFrame, r);
					cvAddWeighted(m_pProcessFrame, 0.5, m_pNewFrame, 0.5, 0, m_pNewFrame);
					cvResetImageROI(m_pProcessFrame);
					cvResetImageROI(m_pNewFrame);

					int temp = m_MysqlSegHandle->FindSegIDFromFGTraceTable(m_tableParams.FGTraceTableName,m_VideoFGParam.nOldPara);
					

					
					
					
					
					for (int i = 0; i < CombineSegCount; i++)
					{
						if (ifinsert)
						{
							if (m_segIDParam >= (temp + m_nSampleTime))
							{
								if (ifCombinefst)
								{
									cvSetImageROI(mypCombineSegImage[i], r);
									cvSetImageROI(m_pProcessFrame, r);
									cvAddWeighted(m_pProcessFrame, 0.8, mypCombineSegImage[i], 0.2, 0, mypCombineSegImage[i]);
									ifCombinefst = false;
									m_traceTab.PicID = i;
									if (m_IfContinue &&!m_MysqlSegHandle->InsertData2CombineSegsTable(m_traceTab, m_tableParams.CombineSegsTableName))
									{
										AfxMessageBox("合成前景图信息表插入错误");
										return;
									}
									cvResetImageROI(m_pProcessFrame);
									cvResetImageROI(mypCombineSegImage[i]);
									ifinsert = false;
								}
								if (m_segIDParam == (temp + m_nSampleTime))
								{
									cvSetImageROI(mypCombineSegImage[CombineSegCount], r);
									cvSetImageROI(m_pProcessFrame, r);
									cvAddWeighted(m_pProcessFrame, 0.8, mypCombineSegImage[CombineSegCount], 0.2, 0, mypCombineSegImage[CombineSegCount]);
									cvResetImageROI(m_pProcessFrame);
									cvResetImageROI(mypCombineSegImage[CombineSegCount]);
									if (!m_MysqlSegHandle->GetInfor1DataFromOrigTraceTable(m_segIDParam, &m_traceTabbefore, m_tableParams.FGTraceTableName))
									{
										AfxMessageBox("新旧帧对应表插入错误");
										return;
									}
								}
								if (m_MysqlSegHandle->GetObjectInfoFromObjectTable(&m_objDetectedInfos, m_tableParams.CombineSegsTableName) && !ifCombinefst&&ifinsert)
								{
									bool test_m_CombineSegsNow;
									bool test_m_CombineSegsBefore = true;
									for (int newsegID = CombinePicNum[i]; newsegID < CombinePicNum[i+1]; newsegID++)
									{
										test_m_CombineSegsNow = r.x >= (m_objDetectedInfos.at(newsegID).roi.x + m_objDetectedInfos.at(newsegID).roi.width) || (r.x + r.width) <= m_objDetectedInfos.at(newsegID).roi.x || r.y >= (m_objDetectedInfos.at(newsegID).roi.y + m_objDetectedInfos.at(newsegID).roi.height) || (r.y + r.height) <= m_objDetectedInfos.at(newsegID).roi.y;
										test_m_CombineSegsNow = test_m_CombineSegsNow&&test_m_CombineSegsBefore;
										test_m_CombineSegsBefore = test_m_CombineSegsNow;
									}
									if (test_m_CombineSegsNow&&ifinsert)
									{
										///将前景目标图像嵌入对应位置的新视频帧图像中
										cvSetImageROI(mypCombineSegImage[i], r);
										cvSetImageROI(m_pProcessFrame, r);
										cvAddWeighted(m_pProcessFrame, 0.8, mypCombineSegImage[i], 0.2, 0, mypCombineSegImage[i]);
										ifCombinefst = false;
										m_traceTab.PicID = i;
										if (m_IfContinue &&!m_MysqlSegHandle->InsertData2CombineSegsTable(m_traceTab, m_tableParams.CombineSegsTableName))
										{
											AfxMessageBox("合成前景图信息表插入错误");
											return;
										}
										cvResetImageROI(m_pProcessFrame);
										cvResetImageROI(mypCombineSegImage[i]);
										ifinsert = false;
										CombinePicNum[i + 1] = m_objDetectedInfos.size()+1;
									}
								}
							}
						}
					}
					
					
				

					//if( m_segIDParam == (temp +m_nSampleTime))
					//{
					//			/////将前景目标图像嵌入对应位置的新视频帧图像中
					//	cvSetImageROI(m_pCombineSegsImage,     r);
					//	cvSetImageROI(m_pProcessFrame, r);
					//	cvAddWeighted(m_pProcessFrame, 0.8, m_pCombineSegsImage, 0.2, 0, m_pCombineSegsImage);
					//	if (m_IfContinue &&!m_MysqlSegHandle->InsertData2CombineSegsTable(m_traceTab, m_tableParams.CombineSegsTableName))
					//	{
					//		AfxMessageBox("合成前景图信息表插入错误");
					//		return;
					//	}
					//	cvResetImageROI(m_pProcessFrame);
					//	cvResetImageROI(m_pCombineSegsImage);
					//}

					if (m_New2OldFrameParam.origFrame!=m_traceTab.origFrame)
					{
						m_New2OldFrameParam.origFrame = m_traceTab.origFrame;
						if(m_IfContinue &&!m_MysqlSegHandle->InsertData2NewToOldFrameTable(m_New2OldFrameParam,m_tableParams.NewToOldFrameTableName))
						{
							AfxMessageBox("新旧帧对应表插入错误");
							return;
						}
					}
					m_segIDParam++;
					if (m_segIDParam >= m_segIDCount)
						return;///<该段已融合目标数超出该段总目标数，返回
				}
				else
				{
					m_IfSameFrame = false;
				}
			}
			///前景目标处于原始图像不同帧的处理
			if (m_IfSameFrame == false)
			{
				sprintf_s(m_strName,m_frameName,m_New2OldFrameParam.newFrame);
				cvSaveImage(m_strName, m_pNewFrame);///<保存新视频帧图像
				m_New2OldFrameParam.newFrame++;
			}
			if (m_pNewFrame)
				cvReleaseImage(&m_pNewFrame);
		}
		if (m_nCurrentFrame > m_VideoFGParam.endFrame)
			break;///<当前帧号超出该动态段范围，退出循环
	}	
	if (ifinsert)
	{		
		m_traceTabbefore.PicID = CombineSegCount;
		if (m_IfContinue &&!m_MysqlSegHandle->InsertData2CombineSegsTable(m_traceTabbefore, m_tableParams.CombineSegsTableName))
		{
			AfxMessageBox("合成前景图信息表插入错误");
			return;
		}		
		CombineSegCount++;
		IplImage* myTempImage = cvCreateImage(cvSize(m_nVideoW, m_nVideoH), IPL_DEPTH_8U, 3);
		cvCopy(m_pBGFinal[m_VideoBGParam.BGImgID], myTempImage);
		mypCombineSegImage.push_back(myTempImage);
		CombinePicNum.push_back(m_objDetectedInfos.size()+1);
	}
	if (!ifinsert)
	{
		cvCopy(m_pBGFinal[m_VideoBGParam.BGImgID], mypCombineSegImage[CombineSegCount]);
	}
	///结束融合新视频帧
	m_FusionParam = MAX(m_FusionParam, m_New2OldFrameParam.newFrame);///<保存该段新视频总帧数
}


///@brief 处理线程控制函数
/// 
///@param[in] 线程控制参量
///@pre  NULL
///@return NULL
///@retval UINT
///@post NULL
UINT BeginSegProcess(LPVOID pParam)
{
	CVideo *pLibHandle = (CVideo*)pParam;///<线程参量转换
	pLibHandle->VideoProcessing();///<打开线程控制
	return 0;
}