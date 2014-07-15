/**
*@file LibMysql.h
*@brief 数据库类下变量及函数的声明。
*
*该文件下定义的数据库类，包含了与工程中所有用到的与数据库相关操作的功能类对象
*
*@author 作者 zhang nannan&&lu ying ,包含email等
*@version 版本号(0.0)
*@data 2012-8-21
*/
#pragma once


#include "stdafx.h"
#include "winsock.h"
#include "publicheader.h"
#include <string.h>
#include "atlstr.h"
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <sstream>
#include <vector>

using namespace std;


///记录视频前景段信息的结构体
typedef struct _VideoFGTable
{
	int nOldPara;        ///<段号
	int startFrame;      ///<本段起始帧号
	int endFrame;        ///<本段结束帧号
}VideoFGTable;

///记录视频背景段信息的结构体
typedef struct _VideoBGTable
{
	int BGImgID;           ///<背景图像编号
	int startFrame;        ///<背景图像适用范围
	int endFrame;
}VideoBGTable;

///记录图片ROI上下左右坐标的结构体
typedef struct _ImageROI
{
	/*int nTop;
	int nBottom;
	int nLeft;
	int nRight;*/
	int nX;
	int nY;
	int nWidth;
	int nHeight;
}ImageROI;

///原始记录运动目标信息的结构体
typedef struct _OrigTraceTable 
{
	int nOldPara;       ///<旧段号
	int segID;          ///<物体编号
	int nX;
	int nY;
	int nWidth;
	int nHeight;
	int origFrame;      ///<物体所在的原始帧号
	//CString segSize;    ///<物体大小
	int segSize;
}OrigTraceTable;

///新旧段对应的结构体
typedef struct _NewToOldParaTable 
{
	int nNewPara;         ///<新段号
	int nOldPara;         ///<旧段号
}NewToOldParaTable;

///新旧帧对应的结构体
typedef struct _NewToOldFrameTable
{
	int origFrame;       ///<旧帧号
	int newFrame;        ///<新帧号
}NewToOldFrameTable;

///最终记录运动目标信息的结构体
typedef struct _NewTraceTable 
{
	int newFrame;        ///<新段号
	int segID;           ///<物体编号
	int nX;
	int nY;
	int nWidth;
	int nHeight;
	int CentX;          ///<中心点的坐标
	int CentY;
	int origFrame;      ///<原始帧号
	int nTop;
	int nBottom;
	int nLeft;
	int nRight;
	
	//CString segSize;    ///<物体大小
	int segSize;
}NewTraceTable;
typedef struct _NewTrackParams
{
	int newFrame;        ///<新段号
	int segID;
	int CentX;          ///<中心点的坐标
	int CentY;
	int origFrame;      ///<原始帧号
	int nTop;
	int nBottom;
	int nLeft;
	int nRight;
}NewTrackParam;

///记录记录信息的结构体
typedef struct _CaseTable
{
	CString caseName;       ///<记录名称
	CString caseCategory;   ///<记录类别
	CString caseRecordTime; ///<立案时间
	CString caseHappenTime; ///<记录发生时间
	CString caseDescription;///<记录描述
	CString caseNote;       ///<记录备注

}CaseTable;

///记录视频信息的结构体
typedef struct _VideoTable
{
	CString caseName;       ///<记录名称
	CString cameraName;     ///<监控点名称
	CString originPath;     ///<原始视频路径
	CString segPath;        ///<分割视频路径
	CString fusionPath;     ///<融合视频路径
	int     origTotalFrames;///<原始视频的总帧数
	int     segTotalFrames; ///<切割视频的总帧数
	int     fusTotalFrames; ///<融合视频的总帧数
}VideoTable;

///记录图像信息的结构体
typedef struct _ImageTable
{
	CString caseName;     ///<记录名称
	CString imageCategory;///<图片类别
	CString imagePath;    ///<图片路径
}ImageTable;

///记录线索信息的结构体
typedef struct _ClueTable
{
	CString caseName;        ///<记录名称
	CString cameraName;      ///<监控点名称
	CString clueHappenTime;  ///<线索发生时间
	CString clueCategory;    ///<线索类别
	CString latestModifyTime;///<最近修改时间
	CString filePath;        ///<关联文件路径
	CString clueDescription; ///<线索描述
}ClueTable;

///记录查找物体信息的结构体
typedef struct _SearchSegIDParams
{
	int origFrame;         ///<原始帧号
	ImageROI ROI;          ///<那个目标的ROI
}SearchSegIDParams;

///记录手动目标检索中一个目标的信息的结构体
typedef struct  
{
	int nOldPara;
	int OriFrameID;
	CvRect roi;
}objectInfo;


///@brief 定义数据库类
///
///定义的数据库类，包含了工程中所有用到的数据库的操作
///可以实现工程中所用到的数据库表的创建、查找、更新和删除等操作
///
///@invariant
///
class CDataMySql
{
public:
	CDataMySql(void);
	~CDataMySql(void);
public:
	MYSQL m_mysql;
public:

	void print_error(MYSQL* conn,char* message);            ///<跟踪出错信息的函数
	///
	///表的通用操作
	///
	bool CleanTable(CString tableName);                             ///<清空表格
	bool DropTable(CString tableName);                              ///<删除表
	bool IfExistTable(CString tableName,bool* ifExist);             ///<查看某个表是否存在
	bool DeleteCertainCaseInfor(CString caseName);                  ///<删除四个表中所有关于记录的信息
	bool StoreTableData2Text(CString txtPath,CString tableName);    ///<将表中的数据保存到txt文件中
	bool LoadTextData2Table(CString txtPath,CString tableName);     ///<将txt中的数据到入表中
  
	///
	///记录表的相关函数
	///
    bool CreateCaseTable(CString CaseTableName="caseTable");                     ///<创建记录表
	bool InsertDataCaseTable(CaseTable CaseTableData ,CString CaseTableName);    ///< 插入数据到记录表	
	bool IfExistCaseNameInCaseTable(CString caseName,bool* ifExist);             ///<判断记录表中是否已经存在此名字的记录
	bool UpdateCaseDataInCaseTable(CaseTable caseData,CString caseName,CString tableName="caseTable");///<更新某一记录名的记录信息
    bool GetAllCaseNameFromCaseTable(vector<CString>* CaseName_Str);             ///<得到所有记录名
	bool GetCurCaseDataFromCurCaseTable(CaseTable* caseData,CString curCaseTable);///<得到当前记录的信息
	bool GetAllCaseDataFromCaseTable(vector<CaseTable>* allCaseData);             ///<得到所有记录的信息

	///
	///视频表的相关函数
	///
	bool CreateVideoTable(CString VideoTableName="videoTable");                   ///<创建视频表
	///更新视频帧数信息
	bool InsertorigTotalFrames2VideoTable(int origTotalFrames,CString FusVideoName);
	bool InsertsegTotalFrames2VideoTable(int segTotalFrames,CString FusVideoName);
	bool InsertfusTotalFrames2VideoTable(int fusTotalFrames,CString FusVideoName);
	///获取视频帧数信息
	int GetorigTotalFramesFromVideoTable(CString FusVideoName);
	int GetsegTotalFramesFromVideoTable(CString FusVideoName);
	int GetfusTotalFramesFromVideoTable(CString FusVideoName);
	///插入数据操作
	bool InsertData2VideoTable(VideoTable VideoTableData);
	bool InsertSegVideo2VideoTable(CString segVideoPath,CString caseName,CString cameraName,CString originPath);
	bool InsertFusionVideo2VideoTable(CString fusionVideoPath,CString caseName,CString cameraName,CString originPath);
	
	int GetOrigVideoNumFromVideoTable(CString CaseName,CString CameraName);      ///<获得某记录监控点下的视频数目
	int GetCaseOrigVideoNumFromVideoTable(CString CaseName);                     ///<获得某记录下所有视频的数目

	CString GetFusionNameFromVideoTable(CString caseName,CString cameraName,CString videoName);///<得到某记录某监控点的某视频的融合视频路径
	CString GetCaseNameFromVideoTable(CString FusVideoName);                                   ///<得到融合视频的记录名称
	CString GetCameraNameFromVideoTable(CString FusVideoName);                                 ///<得到融合视频的监控点名称

	bool GetVideoNameFromVideoTable(CString CaseName,CString CameraName,CString(*VideoName_str)[3],int videoTotalNum);///<获得某记录某监控点下所有视频的名称
	bool GetFusionNameFromVideoTable(CString caseName,CString CameraName,vector<CString>* fusionName_Str);            ///<获得某记录某监控点下所有融合视频的名称
	bool GetMoniorNameFromVideoTable(CString caseName,vector<CString>* CameraName_Str);                               ///<获得某记录下所有监控点的名称
    bool GetOriginNameFromVideoTable(CString CaseName,CString CameraName,vector<CString>* VideoName_Str);             ///<获得某记录某监控点下所有原始视频的名称

	bool IfExistOrigPathVideoTable(CString caseName,CString cameraName,CString videoName,bool* ifExist);              ///<列表中是否已经有了某个记录某个监控点的视频
	bool DeleteDataFromVideoTable(CString caseName,CString cameraName,CString originPath);                            ///<删除视频表中的一条记录
	bool GetCaseVideoDataFromVideoTable(vector<VideoTable>* caseAllVideoData,CString caseName);                       ///<获得视频表中某记录下所有视频的信息
	///
	///图片表的相关函数
	///
	bool CreateImageTable(CString ImageTableName="imageTable");                                     ///<创建图片表
	bool InsertData2ImageTable(ImageTable imgTableData);                                            ///<图片表中插入一条记录
	bool GetCategoryNameFromImageTable(CString caseName,vector<CString>* CategoryName_str);         ///<获得某记录下所有图片类别名称
	bool GetImgPathFromImageTable(CString caseName,CString category,vector<CString>* ImageName_Str);///<获得某记录某类别下所有图片的路径
	bool IfExistImagePathImageTable(CString caseName,CString category,CString imgName,bool* ifExist);///<判断某记录某类别下是否已经有了某张图片
    bool DeleteDataFromImageTable(CString caseName,CString category,CString imagePath);              ///<从图片表中删除一条记录
	bool IfExistImgDataImageTable(ImageTable imgData,bool* ifExist);                                 ///<判断图片表中是否有了一条相同的记录
	int GetCaseImgNumFromImageTable(CString caseName);                                               ///<获得图片表中某一个记录下的图片总数
	bool GetCaseImgDataFromImageTable(vector<ImageTable> *caseAllImgData,CString caseName);          ///<获得某一记录下所有图片的路径
	///
	///线索表的相关函数
	///	
	bool CreateClueTable(CString ClueTableName="clueTable");                                         ///<创建线索表
	bool InsertData2ClueTable(ClueTable clueData);                                                   ///<将一条记录插入到线索表中
	bool GetClueDataFromClueTable(vector<ClueTable>* caseClueData,CString caseName);                 ///<获得某记录下所有的线索信息
	bool GetClueDataFromClueTable(ClueTable* clueData,bool*isExistClue);                             ///<判断线索表中是否已经存在一条相同的记录
	bool UpdateDescriptionInClueTable(ClueTable clueData);                                           ///<更新线索表中的线索描述
	bool UpdateClueDataInClueTable(ClueTable clueData);                                              ///<更新一条线索记录
	bool DeleteDataFromClueTable(ClueTable clueData);                                                ///<删除线索表中的某条记录
	bool GetClueDescripFromClueTable(CString caseName,CString cameraName,CString cluePath,CString& clueDesrip);///<获得某记录某监控点的某个路径文件的所在记录的线索描述
	int GetClueNumFromClueTable(CString caseName);                                                   ///<得到某个记录线索的个数

	///////////////////////////
	///视频分割融合处理中的相关函数
	//////////////////
	
	///
	///trace表的相关函数
	///
	bool CreateOrigTraceTable(CString OrigTraceTableName);                                           ///<创建原始trace表
	bool InsertData2OrigTraceTable(OrigTraceTable infor1,CString OrigTraceTableName);                ///<向表中插入一条记录
	bool GetInfor1DataFromOrigTraceTable(int row,OrigTraceTable* infor1,CString OrigTraceTableName);  ///<从表的特定行读取数据存入结构体
	bool SearchSegOfSize(CString tableName,int segSize,vector<SearchSegIDParams>* segIdParam);	  ///<从旧trace表中查询尺寸大小的物体的原始帧号和ROI，得到查询结果
	///
	///newtrace表的相关函数
	///
	bool CreateNewTraceTable(CString tableName);                                                        ///<创建new trace表
	bool Unite2NewTraceTable(CString newTraceTable,CString origTraceTable,CString frameTable);	        ///<由原始trace表和新旧帧对应表生成	
	bool FindROIFromNewTraceTable(int NewFrame,vector <CvRect> *m_ROI,CString tableName);                ///<获得新帧中所有运动目标的ROI
	int FindOrigFrameFromNewTraceTable(CString newTraceTable,CvRect rect);	                            ///<从new trace表中根据ROI信息找到原始视频帧数
	int FindOrigFrameFromVideoFGTable(CString VideoFGTable,int origPara);	                            ///<从videofg表中根据段号找到原始视频帧数
	int FindOrigParaFromFGTraceTable(CString FGTraceTable, CvRect rect);	                            ///<从FGTrace表中根据ROI信息找到原始视频段号
	int FindSegIDFromFGTraceTable(CString FGTraceTable,int nOldPara);	                            ///<从new trace表中根据ROI信息找到原始视频帧数
	bool FindTimePositionFromNewTraceTable(int NewFrame,vector <CvPoint> *m_Point,vector<int> *origFrame,CString tableName);
	///
	///NewToOldParaTable表的相关函数
	///
	bool CreateNewToOldParaTable(CString NewToOldParaTableName);                                         ///<创建新旧段对应表
	bool InsertData2NewToOldParaTable(NewToOldParaTable infor2,CString NewToOldParaTableName);           ///<向表中插入一条记录
	int FindNewParaFromNewToOldParaTable(int oldParag,CString NewToOldParaTableName);                   ///<根据旧段号从表中查询新段号
	///
	///NewToOldFrameTable表的相关函数
	///	
	bool CreateNewToOldFrameTable(CString NewToOldFrameTableName);                                      ///<创建新旧帧对应表
    bool InsertData2NewToOldFrameTable(NewToOldFrameTable infor3,CString NewToOldFrameTableName);       ///<向表中插入一条记录
	///
	///运动前景段表的相关函数
	///	
	bool CreateVideoFGTable(CString tableName);                                                         ///<创建运动前景段表
	bool InsertData2VideoFGTable(VideoFGTable infor,CString tableName);                                 ///<向表中插入一条记录
	int FindEndFrameFromVideoFGTable(int paraNum,CString tableName);                                    ///<找到某一前景段的结束帧号
	int FindStartFrameFromVideoFGTable(int paraNum,CString TableName);                                  ///<找到某一前景段的起始帧号

	///
	///运动背景段表的相关函数
	///	
	bool CreateVideoBGTable(CString tableName);                                                           ///<创建运动背景段表
	bool InsertData2VideoBGTable(VideoBGTable infor,CString tableName);                                  ///<向表中插入一条记录
	int FindEndFrameFromVideoBGTable(int bkgID,CString tableName);                                       ///<找到某一前景段的结束帧号
	int FindStartFrameFromVideoBGTable(int bkgID,CString TableName);                                     ///<找到某一前景段的起始帧号

	///
	///CombineSegsTable里面用到的表的相关函数
	///
	bool CreateCombineSegsTable(CString CombineSegsTableName);											///建表
	bool InsertData2CombineSegsTable(OrigTraceTable traceTab, CString CombineSegsTableName);			///插入数据
	bool FindROIFromCombineSegsTable(vector <CvRect> *m_ROI, CString tableName);						///获得所有ROI
	bool GetObjectInfoFromObjectTable(vector<objectInfo> *objDetectedInfos,CString ObjectTableName);       ///<获取所有运动目标的信息

	///
	///VidAbstract用到的表的相关函数
	///	
	bool CreateCListItemTable(CString tableName);                                                        ///<创建一个基于记录名，监控点名和在CListCtrl行号的表
	bool InsertData2CListItemTable(CString tableName,CString caseName,CString cameraName,CString videoName,int clistRow);///<向表中插入一条记录
	bool IfExistCListItemTable(CString tableName,CString caseName,CString cameraName,bool* ifExist);     ///<列表中是否已经有了某个记录某个监控点的视频
	CString GetCurrentOrigVideoName(CString clistTableName,int clistRow);                                ///<得到目前点击的item的原始视频路径
	CString GetCurCameraName(CString clistTableName,int clistRow);                                       ///<得到目前点击的item的记录名称
	CString GetCurCaseName(CString clistTableName,int clistRow);                                         ///<得到目前点击的item的监控点名称
	CString GetCurrentFusionVideoName(CString clistTableName,int clistRow);                              ///<得到目前点击的item的融合视频路径
	int GetCurrentProcessingRow(CString clistTableName,CString caseName,CString cameraName);             ///<得到目前正在处理的item的行号

	///
	///DrawDetect里面用到的表的相关函数
	///
	bool CreateObjectTable(CString tableName);                                                            ///<创建表
	int GetTableDistinctNum(CString tableName);                                                           ///<得到表中目标总数
	bool InsertData2ObjectTable(NewTrackParam infor5,CString ObjectTableName);                            ///<向表中插入一条记录
	void GetCenterFromObjectTable(int objectID, CString ObjectTableName, vector<CPoint>* centersOfObj);   ///<获取某个运动目标的轨迹点集合
	void GetRectFromObjectTable(int frameID, CString ObjectTableName, vector<CRect>* rect,vector<int>* objID);///<获取某一帧的运动物体的ID和Rect
	void GetSingleTraceFromObjectTable(int objID, CString ObjectTableName, vector<CRect>* rect,vector<CPoint>* centersOfObj);///<一次性读取某个运动目标的轨迹点集合和Rect集合
	int  GetObjectIDFromObjectTable(int frameID,CRect rect,CString ObjectTableName);                       ///<获取某一帧特殊位置的物体ID
	
	bool GetFrameidFromObjectTable(int objectID,CString ObjectTableName,vector<int>* frameID);              ///<获取某一运动目标的frameID集合


};