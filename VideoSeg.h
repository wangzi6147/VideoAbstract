/**
*@file  VideoSeg.h
*@brief 实现视频分割融合功能，生成视频摘要。
*
*主要模块：视频分析、视频分割（背景提取、前景提取）、视频融合（摘要生成）。
*
*
*@author 作者 lu ying&&zhang nan nan ,包含email等
*@version 版本号(0.0)
*@data 2012-8-21
*/
#pragma  once
#include "stdafx.h"
#include "publicheader.h"
#include "LibMySql.h"
#include "GlobalFunctions.h"
#include "opencv2\legacy\legacy.hpp"
#include  "opencv2\video\background_segm.hpp"


#define SEG_L_RATIO  0.05 ///<大物体像素点个数占整幅图的百分比
#define SEG_M_RATIO  0.01 ///<中物体像素点个数占整幅图的百分比
//#define VideoConnection_Is_On  1  ///<视频拼接函数开启


///@brief 定义视频融合所需的数据表名称
///
///该结构体包含视频融合所需的数据表名称
///
///@invariant
///
typedef struct _TableParams
{
	CString VideoFGTableName;       ///<前景段信息表名
	CString FGTraceTableName;       ///<运动物体轨迹表名
	CString NewToOldFrameTableName; ///<新旧视频帧对照表名
	CString NewTraceTable;          ///<目标轨迹信息表名
}TableParams;

///@brief 定义视频融合类
///
///该类包含视频分析、分割、融合三部分功能。
///
///@invariant
///

using namespace cv;

class CVideo
{
public:
	CVideo();
	~CVideo();
public:
    ///DoSegmentation
	int    DoProcessing(const char* pFilePath);///<开始处理，开启线程
	double ProcessingSpeed();///<获得当前处理进度
	void   StopProcessing(); ///<终止处理程序
	int    VideoOriCount();  ///<返回原始视频总帧数
	int    VideoSegCount();  ///<返回分割视频总帧数
	int    VideoFusCount();  ///<返回融合视频总帧数
public:
	///Segmentation Processing
	void VideoProcessing();///<视频分割融合总控函数

	///Initialization
	int Initialization(const char *pVideoPath);///<视频参数初始化
	int InitSampleSetting();     ///<初始化压缩参数
	void InitStorage();          ///<初始化图像数据，分配空间
	void InitGaussParam();       ///<初始化高斯模型
	void InitTableParam();       ///<初始化数据库表
	void InitAbstractVideo(const char* pVideoPath);///<初始化摘要视频参数

	///Video Segmentation Processing
	void VideoParaProcessing();  ///<视频分段
	void VideoBGProcessing();    ///<视频背景提取
	void VideoFGProcessing();    ///<视频前景切分
	void ParaSegmentation();     ///<视频分段算法
	void FGSegmentation();       ///<视频前景切分算法
	void VideoConnection();      ///<分割视频拼接
	void TimeLineRestructed();   ///<时间轴重构
	
	///Video Fusion Processing
	void VideoFusionProcessing();///<视频融合
	void CreateNewFrame();       ///<创建新视频帧
	void FusionNewFrame();       ///<融合新视频帧

	///Video Character Processing
	string ColorSearch(IplImage *CutImg);   ///<运动目标颜色检索
	string SizeSearch(CvRect CutRect);      ///<运动目标大小检索

	///内部变量，私有成员
private:
	CDataMySql        *m_MysqlSegHandle;      ///<视频融合数据表句柄

	CvCapture         *m_pCapture;            ///<视频文件句柄    
	CvGaussBGModel    *m_pGaussBGModel;       ///<高斯模型
	CvGaussBGStatModelParams *m_pGaussParams; ///<高斯模型参数
	BackgroundSubtractorMOG2 m_pBGSubMOG2;  //新版前背景分离



	CvSeq        *m_pCont;          ///<运动目标轮廓指针
	CvMemStorage *m_pStor;          ///<运动目标轮廓存储空间指针

	IplImage *m_pOriFrame;          ///<未经过处理的原始帧(3通道)
	IplImage *m_pProcessFrame;      ///<待处理视频帧(3通道)
//	IplImage *m_pResizeFrame;       ///<经过压缩后的帧(3通道)
	
	Mat m_ResizeFrameMat;       ///<经过压缩后的帧(3通道)
	Mat m_GrayResizeFrameMat;  ///<经过灰度化和压缩的图片(1通道)
    Mat foregroundMat;

//	IplImage *m_pGrayResizeFrame;   ///<经过灰度化和压缩的图片(1通道)
	IplImage *m_pBGFrame;           ///<背景图片(3通道)
	IplImage *m_pBGFinal[10];       ///<提取到的背景图片(3通道)
	IplImage *m_pNewFrame;          ///<融合后的新视频帧(3通道)

	CvVideoWriter *m_pVideoWriter4seg;   ///<视频分割写指针
	CvVideoWriter *m_pVideoWriter4fusion;///<视频融合写指针

	int   m_nBGFinal[10];       ///<提取背景图的静态段范围
	int   m_nFGFinal[1000];     ///<提取前景图的静态段范围
	int   m_nParaFinal[1000];   ///<待融合的动态段范围

	IplImage *m_pGrayFrame; 
	cv::Mat m_FrameMat;          ///<视频图像数组(3通道)
	cv::Mat m_BGMat;             ///<背景图像数组(3通道)
	CvMat *m_pGrayFrameMat;      ///<视频图像数组(1通道)
	CvMat *m_pGrayBGMat;         ///<背景图像数组(1通道)
	CvMat *m_pGrayFGMat;         ///<前景图像数组(1通道)

	int m_nCurrentFrame;         ///<当前帧序号
	int m_nFps;                  ///<当前视频默认帧率
	int m_nTotalFrame;           ///<当前视频总帧数
	int m_nTotalFrameToDo;       ///<总处理时间            默认处理所有帧
	int m_nSampleTime;           ///<采样秒数              默认5秒
	int m_nSamplingRate;         ///<采样率 采样间隔        
	int m_nCompressionRatioW;    ///<视频帧宽度压缩比         默认16     
	int	m_nCompressionRatioH;    ///<视频帧高度压缩比         默认16
	int m_nProcessingRatio;      ///<视频帧大小处理比例
	
	int m_nBGUpdateFrames;       ///<背景更新总帧数        默认m_nTotalFrameToDo/9
	int m_nBGUpdateSampleFrame;  ///<背景更新采样帧数      默认m_nBGUpdateFrames/20
	double m_rFGSegThreshold;       ///<前景切分自适应阈值

	int m_nVideoW;               ///<视频帧宽度
	int m_nVideoH;               ///<视频帧高度
	int m_nVideoResizeW;         ///<视频帧压缩后宽度
	int m_nVideoResizeH;         ///<视频帧压缩后高度
	int m_nMaxTargetArea;        ///<运动目标最大面积
	int m_nMinTargetArea;        ///<运动目标最小面积

	//char m_strName[500];         ///<名称临时变量
	//char m_tempFile[100];        ///<暂存融合图像的文件夹名称
	CString m_frameName;         ///<融合图像名

	CvRect r;                    ///<矩形临时变量

	int SegFrame_Count;
	int FGPara_Count;           ///<提取前景的总段数
	int BGPara_Count;           ///<提取背景的总段数      
    int m_segIDCount;           ///<每一段的segID的数目
	int m_segIDParam;           ///<segID参数，初始化不能为零
	int m_FusionParam;          ///<视频融合参数
	int m_FusionStart;          ///<视频融合起始与否判断条件

	CString m_newTraceTextPath; ///<保存fgtrace表的txt文件路径
	ofstream m_newTraceOfstream;///<保存fgtrace表的txt文件的流

	///外部接口相关变量 - 即一般为从外部获取	
public:
	bool   m_IfContinue;            ///<继续处理标志位
	double m_ProcessingSpeed;       ///<当前处理进度
	string m_strFilePath;           ///<视频路径

	///视频数据表信息
	TableParams        m_tableParams;       ///<表名信息
	VideoBGTable       m_VideoBGParam;      ///<视频背景信息
	VideoFGTable       m_VideoFGParam;      ///<视频前景信息
	OrigTraceTable     m_traceTab;          ///<trace表信息
	NewToOldParaTable  m_New2OldParaParam;  ///<新旧段信息
	NewToOldFrameTable m_New2OldFrameParam; ///<新旧帧信息

	///视频融合部分信息
	string strSegVideoNewName;   ///<生成的分割视频的路径
	string strFusionVideoNewName;///<生成的融合视频的路径
	bool m_IfDoneSeg;            ///<视频分割完成标志位
	bool m_IfDoneFus;            ///<视频融合完成标志位
	int FusFrame_Count;          ///<视频融合总帧数
};

UINT BeginSegProcess(LPVOID pParam);///<处理线程控制函数