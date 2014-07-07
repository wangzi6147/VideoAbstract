/**
*@file LibMySql.cpp
*@brief 数据库类的实现
*
*定义数据库类下各个操作的行为
*可以实现工程中所用到的数据库表的创建、查找、更新和删除操作
*
*@author 作者 zhang nannan&lu ying ,包含email等
*@version 版本号(0.0)
*@data 2012-8-21
*/

#include "stdafx.h"
#include <afxwin.h>
#include <afxmt.h>///<多线程必须加此头文件，不然有无法解析的外部符号
#include "LibMySql.h"

///@brief 命名空间std
///
///命名空间std封装的是标准程序库的名称
using namespace std;

///@brief CDataMySql类的构造函数，进行数据库的初始化和工程与数据库的关联
///
///@param[in|out] NULL 
///@pre  NULL
///@return NULL
///@retval NULL
///@post NULL
CDataMySql::CDataMySql(void)
{
	const char user[] = "root";         ///<username        
	const char pswd[] = "123456";        ///<password
	const char host[] = "localhost";    ///<or"127.0.0.1"
	const char table[] = "mydb";        ///<database
	unsigned int port = 3306;           ///<server port        
	mysql_init(&m_mysql);               ///<数据库初始化
	if(mysql_real_connect(&m_mysql,host,user,pswd,table,port,NULL,0)==NULL)///<与数据库相连
	{
		print_error(&m_mysql,"error message");
		mysql_close(&m_mysql);
		return;
	}
	mysql_query(&m_mysql,"SET NAMES GBK");                        ///<显示为中文
}
///@brief vidPlayDlg类的析构函数
///
///@param[in|out] NULL 
///@pre  NULL
///@return NULL
///@retval NULL
///@post NULL
CDataMySql::~CDataMySql(void)
{
}
///@brief CDataMySql类的建立记录表函数
/// 
///如果数据库中没有记录表则创建一个记录表，列及其属性为(caseName VARCHAR(255) primary key,caseCategory VARCHAR(255) not Null,caseRecordTime VARCHAR(255) not Null,caseHappenTime VARCHAR(255) not Null,caseDescription TEXT not Null,caseNote TEXT not Null)
///@param[in|out] CaseTableName 要创建的表名|NULL
///@pre  NULL
///@return NULL
///@retval BOOL 返回操作是否成功
///@post NULL
bool CDataMySql::CreateCaseTable(CString CaseTableName)
{
	CString m_strSql;///<mysql查询语句
	m_strSql.Format("create table if not exists %s(caseName VARCHAR(255) primary key,caseCategory VARCHAR(255) not Null,caseRecordTime VARCHAR(255) not Null,caseHappenTime VARCHAR(255) ,caseDescription TEXT not Null,caseNote TEXT not Null)",CaseTableName);
	if(mysql_real_query(&m_mysql,(char*)(LPCTSTR)m_strSql,(UINT)m_strSql.GetLength())!=0)
	{ 
		print_error(&m_mysql,"error message");
		return false;
	}
	else
	{
		return true;
	}
}
///@brief CDataMySql类的建立视频表函数
/// 
///如果数据库中没有视频表则创建一个视频表，列及其属性为(caseName VARCHAR(255) not Null,cameraName VARCHAR(255) not Null,originPath VARCHAR(255) not Null,segPath VARCHAR(255),fusionPath VARCHAR(255),\
        origTotalFrames int,segTotalFrames int,fusTotalFrames int)
///@param[in|out] VideoTableName 要创建的表名|NULL
///@pre  NULL
///@return NULL
///@retval BOOL 返回操作是否成功
///@post NULL
bool CDataMySql::CreateVideoTable(CString VideoTableName)
{
	CString m_strSql;///<mysql查询语句
	m_strSql.Format("create table if not exists %s(caseName VARCHAR(255) not Null,cameraName VARCHAR(255) not Null,originPath VARCHAR(255) not Null,segPath VARCHAR(255),fusionPath VARCHAR(255),\
					origTotalFrames int,segTotalFrames int,fusTotalFrames int)",VideoTableName);
	if(mysql_real_query(&m_mysql,(char*)(LPCTSTR)m_strSql,(UINT)m_strSql.GetLength())!=0)
	{ 
		print_error(&m_mysql,"error message");
		return false;
	}
	else
	{
		return true;
	}
}
///@brief CDataMySql类的建立图片表函数
/// 
///如果数据库中没有图片表则创建一个图片表，列及其属性为(caseName VARCHAR(255) not Null,imageCategory VARCHAR(255) not Null,imagePath VARCHAR(255) not Null)
///@param[in|out] ImageTableName in 要创建的表名|NULL
///@pre  NULL
///@return NULL
///@retval BOOL 返回操作是否成功
///@post NULL
bool CDataMySql::CreateImageTable(CString ImageTableName)
{
	CString m_strSql;///<mysql查询语句
	m_strSql.Format("create table if not exists %s(caseName VARCHAR(255) not Null,imageCategory VARCHAR(255) not Null,imagePath VARCHAR(255) not Null)",ImageTableName);
	if(mysql_real_query(&m_mysql,(char*)(LPCTSTR)m_strSql,(UINT)m_strSql.GetLength())!=0)
	{ 
		print_error(&m_mysql,"error message");
		return false;
	}
	else
	{
		return true;
	}
}
///@brief CDataMySql类的建立线索表函数
/// 
///如果数据库中没有线索表则创建一个线索表，列及其属性为(caseName VARCHAR(255) not Null,cameraName VARCHAR(255),clueHappenTime VARCHAR(255),clueCategory VARCHAR(255),latestModifyTime VARCHAR(255),filePath VARCHAR(255),clueDescription TEXT)
///@param[in|out] ClueTableName 要创建的表名|NULL
///@pre  NULL
///@return NULL
///@retval BOOL 返回操作是否成功
///@post NULL
bool CDataMySql::CreateClueTable(CString ClueTableName)
{
	CString m_strSql;///<mysql查询语句
	m_strSql.Format("create table if not exists %s(caseName VARCHAR(255) not Null,cameraName VARCHAR(255),clueHappenTime VARCHAR(255),clueCategory VARCHAR(255),latestModifyTime VARCHAR(255),filePath VARCHAR(255),clueDescription TEXT)",ClueTableName);
	if(mysql_real_query(&m_mysql,(char*)(LPCTSTR)m_strSql,(UINT)m_strSql.GetLength())!=0)
	{ 
		print_error(&m_mysql,"error message");
		return false;
	}
	else
	{
		return true;
	}
}
///@brief CDataMySql类的建立原始trace表函数
/// 
///创建一个名为参数的原始trace表，列及其属性为(nOldPara int not Null,segID int primary key,nTop int not Null,nBottom int not Null,nLeft int not Null,nRight int not Null,origFrame int not Null,segSize varchar(20) not Null)
///@param[in|out] OrigTraceTableName 要创建的表名|NULL
///@pre  NULL
///@return NULL
///@retval BOOL 返回操作是否成功
///@post NULL
bool CDataMySql::CreateOrigTraceTable(CString OrigTraceTableName)
{
	CString m_strSql;///<mysql查询语句
	//m_strSql.Format("create table %s(nOldPara int not Null,segID int primary key,nTop int not Null,nBottom int not Null,nLeft int not Null,nRight int not Null,origFrame int not Null,segSize varchar(20) not Null)",OrigTraceTableName);
	m_strSql.Format("create table %s(nOldPara smallint(3) not Null,segID int primary key,nX smallint(3) not Null,nY smallint(3) not Null,nWidth smallint(3) not Null,nHeight smallint(3) not Null,origFrame int not Null,segSize enum('0','1','2') not Null)",OrigTraceTableName);
	if(mysql_real_query(&m_mysql,(char*)(LPCTSTR)m_strSql,(UINT)m_strSql.GetLength())!=0)
	{ 
		print_error(&m_mysql,"error message");
		return false;
	}
	else
	{
		return true;
	}
}
///@brief CDataMySql类的建立新旧段号对应表函数
/// 
///创建一个名为参数的新旧段号对应表，列及其属性为(nOldPara int primary key,nNewPara int not Null)
///@param[in|out] NewToOldParaTableName 要创建的表名|NULL
///@pre  NULL
///@return NULL
///@retval BOOL 返回操作是否成功
///@post NULL

bool CDataMySql::CreateNewToOldParaTable(CString NewToOldParaTableName)
{
	CString m_strSql;///<mysql查询语句
	m_strSql.Format("create table %s(nOldPara int primary key ,nNewPara int not Null)",NewToOldParaTableName);
	if(mysql_real_query(&m_mysql,(char*)(LPCTSTR)m_strSql,(UINT)m_strSql.GetLength())!=0)
	{ 
		print_error(&m_mysql,"error message");
		return false;
	}
	else
	{
		return true;
	}
}
///@brief CDataMySql类的建立新旧帧号对应表函数
/// 
///创建一个名为参数的新旧帧号对应表，列及其属性为(origFrame int primary key,newFrame int not Null)
///@param[in|out] NewToOldFrameTableName 要创建的表名|NULL
///@pre  NULL
///@return NULL
///@retval BOOL 返回操作是否成功
///@post NULL
bool CDataMySql::CreateNewToOldFrameTable(CString NewToOldFrameTableName)
{
	CString m_strSql;///<mysql查询语句
	m_strSql.Format("create table %s(origFrame int primary key,newFrame int not Null)",NewToOldFrameTableName);
	if(mysql_real_query(&m_mysql,(char*)(LPCTSTR)m_strSql,(UINT)m_strSql.GetLength())!=0)
	{ 
		print_error(&m_mysql,"error message");
		return false;
	}
	else
	{
		return true;
	}
}

///@brief CDataMySql类的建立合成前景图信息表函数
/// 
///创建一个名为参数的合成前景图信息表，列及其属性为(nOldPara int,segID int not null, nX smallint(3) not null,\
		nY smallint(3) not null,nWidth smallint(3) not null,nHeight smallint(3) not null)
///@param[in|out] CombineSegsTableName 要创建的表名|NULL
///@pre  NULL
///@return NULL
///@retval BOOL 返回操作是否成功
///@post NULL
bool CDataMySql::CreateCombineSegsTable(CString CombineSegsTableName)
{
	CString m_strSql;///<mysql查询语句
	m_strSql.Format("create table %s(nOldPara int,segID int not null, nX smallint(3) not null,\
		nY smallint(3) not null,nWidth smallint(3) not null,nHeight smallint(3) not null)", CombineSegsTableName);
	if (mysql_real_query(&m_mysql, (char*)(LPCTSTR)m_strSql, (UINT)m_strSql.GetLength()) != 0)
	{
		print_error(&m_mysql, "error message");
		return false;
	}
	else
	{
		return true;
	}
}

///@brief CDataMySql类的插入数据到合成前景图信息表函数
/// 
///将一条记录插入到表中
///@param[in|out] traceTab 前景信息; CombineSegsTableName 表名|NULL
///@pre  NULL
///@return NULL
///@retval BOOL 返回操作是否成功
///@post NULL
bool CDataMySql::InsertData2CombineSegsTable(OrigTraceTable traceTab, CString CombineSegsTableName)
{
	CString m_strSql;///<mysql查询语句
	m_strSql.Format("insert into %s(nOldPara, segID, nX, nY, nWidth, nHeight)\
		values(\'%d\',\'%d\',\'%d\',\'%d\',\'%d\',\'%d\')",\
		CombineSegsTableName, traceTab.nOldPara, traceTab.segID, traceTab.nX, traceTab.nY, traceTab.nWidth, traceTab.nHeight);
	if (mysql_real_query(&m_mysql, (char*)(LPCTSTR)m_strSql, (UINT)m_strSql.GetLength()) != 0)
	{
		print_error(&m_mysql, "error message");
		return false;
	}
	else
	{
		return true;
	}
}
///@brief CDataMySql类的插入数据到记录表函数
/// 
///将一条记录插入到记录表中
///@param[in|out] CaseTableData 要插入的记录;CaseTableName 记录表的名字|NULL
///@pre  NULL
///@return NULL
///@retval BOOL 返回操作是否成功
///@post NULL
bool CDataMySql::InsertDataCaseTable(CaseTable CaseTableData ,CString CaseTableName)
{
	CString m_strSql;///<mysql查询语句
	m_strSql.Format("insert into %s(caseName,caseCategory,caseRecordTime,caseHappenTime,caseDescription,caseNote)values(\'%s\',\'%s\',\'%s\',\'%s\',\'%s\',\'%s\')" ,CaseTableName,CaseTableData.caseName,CaseTableData.caseCategory,CaseTableData.caseRecordTime,CaseTableData.caseHappenTime,CaseTableData.caseDescription,CaseTableData.caseNote);
	if(mysql_real_query(&m_mysql,(char*)(LPCTSTR)m_strSql,(UINT)m_strSql.GetLength())!=0)
	{ 
		print_error(&m_mysql,"error message");
		return false;
	}
	else
	{
		return true;
	}
}
///@brief CDataMySql类的插入数据到视频表函数
/// 
///将一条记录插入到视频表中
///@param[in|out] VideoTableData 要插入的记录|NULL
///@pre  NULL
///@return NULL
///@retval BOOL 返回操作是否成功
///@post NULL
bool CDataMySql::InsertData2VideoTable(VideoTable VideoTableData)
{
	CString m_strSql;///<mysql查询语句
	m_strSql.Format("insert into videoTable(caseName,cameraName,originPath,segPath,fusionPath) values(\'%s\',\'%s\',\'%s\',\'%s\',\'%s\')" ,VideoTableData.caseName,VideoTableData.cameraName,VideoTableData.originPath,VideoTableData.segPath,VideoTableData.fusionPath);
	if(mysql_real_query(&m_mysql,(char*)(LPCTSTR)m_strSql,(UINT)m_strSql.GetLength())!=0)
	{ 
		print_error(&m_mysql,"error message");
		return false;
	}
	else
	{
		return true;
	}
}
///@brief CDataMySql类的插入数据到图片表函数
/// 
///将一条记录插入到图片表中
///@param[in|out] imgTableData 要插入的记录|NULL
///@pre NULL
///@return NULL
///@retval BOOL 返回操作是否成功
///@post NULL
bool CDataMySql::InsertData2ImageTable(ImageTable imgTableData)
{
	CString m_strSql;///<mysql查询语句
	m_strSql.Format("insert into imageTable(caseName,imageCategory,imagePath) values(\'%s\',\'%s\',\'%s\')" ,imgTableData.caseName,imgTableData.imageCategory,imgTableData.imagePath);
	if(mysql_real_query(&m_mysql,(char*)(LPCTSTR)m_strSql,(UINT)m_strSql.GetLength())!=0)
	{ 
		print_error(&m_mysql,"error message");
		return false;
	}
	else
	{
		return true;
	}
}
///@brief CDataMySql类的插入数据到原始trace表函数
/// 
///将一条记录插入到原始trace表中
///@param[in|out] infor1 要插入的记录;OrigTraceTableName 要插入数据的表名|NULL
///@pre NULL
///@return NULL
///@retval BOOL 返回操作是否成功
///@post NULL
bool CDataMySql::InsertData2OrigTraceTable(OrigTraceTable infor1,CString OrigTraceTableName)
{
	CString m_strSql;///<mysql查询语句
	m_strSql.Format("insert into %s(nOldPara,segID,nX,nY,nWidth,nHeight,origFrame,segSize)\
					values(\'%d\',\'%d\',\'%d\',\'%d\',\'%d\',\'%d\',\'%d\',\'%d\')",\
					OrigTraceTableName,infor1.nOldPara,infor1.segID,infor1.nX,infor1.nY,infor1.nWidth,infor1.nHeight,infor1.origFrame,infor1.segSize); //sql插入语句
	if(mysql_real_query(&m_mysql,(char*)(LPCTSTR)m_strSql,(UINT)m_strSql.GetLength())!=0)
	{ 
		print_error(&m_mysql,"error message");
		return false;
	}
	else
	{
		return true;
	}
}

///@brief CDataMySql类的插入数据到新旧段对应表函数
/// 
///将一条记录插入到新旧段对应表中
///@param[in|out] infor2 要插入的记录;NewToOldParaTableName 要插入数据的表名|NULL
///@pre NULL
///@return NULL
///@retval BOOL 返回操作是否成功
///@post NULL
bool CDataMySql::InsertData2NewToOldParaTable(NewToOldParaTable infor2,CString NewToOldParaTableName)
{
	CString m_strSql;///<mysql查询语句
	m_strSql.Format("insert into %s(nNewPara,nOldPara)values(\'%d\',\'%d\')",NewToOldParaTableName,infor2.nNewPara,infor2.nOldPara);
	if(mysql_real_query(&m_mysql,(char*)(LPCTSTR)m_strSql,(UINT)m_strSql.GetLength())!=0)
	{ 
		print_error(&m_mysql,"error message");
		return false;
	}
	else
	{
		return true;
	}
}

///@brief CDataMySql类的插入数据到新旧帧对应表函数
/// 
///将一条记录插入到新旧帧对应表中
///@param[in|out] infor3 要插入的记录;NewToOldFrameTableName 要插入数据的表名|NULL
///@pre NULL
///@return NULL
///@retval BOOL 返回操作是否成功
///@post NULL
bool CDataMySql::InsertData2NewToOldFrameTable(NewToOldFrameTable infor3,CString NewToOldFrameTableName)
{
	CString m_strSql;///<mysql查询语句
	m_strSql.Format("insert into %s(origFrame,newFrame)values(\'%d\',\'%d\')",NewToOldFrameTableName,infor3.origFrame,infor3.newFrame);
	if(mysql_real_query(&m_mysql,(char*)(LPCTSTR)m_strSql,(UINT)m_strSql.GetLength())!=0)
	{ 
		print_error(&m_mysql,"error message");
		return false;
	}
	else
	{
		return true;
	}
}

///@brief CDataMySql类的在视频表中插入原始视频帧数函数
/// 
///更新视频表中某个融合视频对应的记录中原始视频的总帧数
///@param[in|out] origTotalFrames 原始视频的总帧数;FusVideoName 融合视频的名字|NULL
///@pre NULL
///@return NULL
///@retval BOOL 返回操作是否成功
///@post NULL
bool CDataMySql::InsertorigTotalFrames2VideoTable(int origTotalFrames,CString FusVideoName)
{
	CString m_strSql;///<mysql查询语句
	m_strSql.Format("update videoTable set origTotalFrames=\'%d\' where fusionPath=\'%s\'",origTotalFrames,FusVideoName);
	if(mysql_real_query(&m_mysql,(char*)(LPCTSTR)m_strSql,(UINT)m_strSql.GetLength())!=0)
	{ 
		print_error(&m_mysql,"error message");
		return false;
	}
	else
	{
		return true;
	}
}
///@brief CDataMySql类的在视频表中插入分割视频帧数函数
/// 
///更新视频表中某个融合视频对应的记录中分割视频的总帧数
///@param[in|out] segTotalFrames 分割视频的总帧数;FusVideoName 融合视频的名字|NULL
///@pre NULL
///@return NULL
///@retval BOOL 返回操作是否成功
///@post NULL
bool CDataMySql::InsertsegTotalFrames2VideoTable(int segTotalFrames,CString FusVideoName)
{
	CString m_strSql;///<mysql查询语句
	m_strSql.Format("update videoTable set segTotalFrames=\'%d\' where fusionPath=\'%s\'",segTotalFrames,FusVideoName);
	if(mysql_real_query(&m_mysql,(char*)(LPCTSTR)m_strSql,(UINT)m_strSql.GetLength())!=0)
	{ 
		print_error(&m_mysql,"error message");
		return false;
	}
	else
	{
		return true;
	}
}
///@brief CDataMySql类的在视频表中插入融合视频帧数函数
/// 
///更新视频表中某个融合视频对应的记录中融合视频的总帧数
///@param[in|out] fusTotalFrames 融合视频的总帧数;FusVideoName 融合视频的名字|NULL
///@pre NULL
///@return NULL
///@retval BOOL 返回操作是否成功
///@post NULL
bool CDataMySql::InsertfusTotalFrames2VideoTable(int fusTotalFrames,CString FusVideoName)
{
	CString m_strSql;///<mysql查询语句
	m_strSql.Format("update videoTable set fusTotalFrames=\'%d\' where fusionPath=\'%s\'",fusTotalFrames,FusVideoName);
	if(mysql_real_query(&m_mysql,(char*)(LPCTSTR)m_strSql,(UINT)m_strSql.GetLength())!=0)
	{ 
		print_error(&m_mysql,"error message");
		return false;
	}
	else
	{
		return true;
	}
}
///@brief CDataMySql类的在视频表中插入融合视频帧数函数
/// 
///更新视频表中某个融合视频对应的记录中融合视频的总帧数
///@param[in|out] fusTotalFrames 融合视频的总帧数;FusVideoName 融合视频的名字|NULL
///@pre NULL
///@return NULL
///@retval BOOL 返回操作是否成功
///@post NULL
bool CDataMySql::InsertSegVideo2VideoTable(CString segVideoPath,CString caseName,CString cameraName,CString originPath)
{
	CString m_strSql;///<mysql查询语句
	m_strSql.Format("update videoTable set segPath=\'%s\' where caseName=\'%s\' and cameraName=\'%s\' and originPath=\'%s\'" ,segVideoPath,caseName,cameraName,originPath);
	if(mysql_real_query(&m_mysql,(char*)(LPCTSTR)m_strSql,(UINT)m_strSql.GetLength())!=0)
	{ 
		print_error(&m_mysql,"error message");
		return false;
	}
	else
	{
		return true;
	}
}
///@brief CDataMySql类的在视频表中插入融合视频路径函数
/// 
///更新视频表中某记录某监控点某个视频对应的记录中融合视频的路径
///@param[in|out] fusionVideoPath 融合视频的总帧数;caseName 记录名字;cameraName 监控点名字;originPath 原始视频路径|NULL
///@pre NULL
///@return NULL
///@retval BOOL 返回操作是否成功
///@post NULL
bool CDataMySql::InsertFusionVideo2VideoTable(CString fusionVideoPath,CString caseName,CString cameraName,CString originPath)
{
	CString m_strSql;///<mysql查询语句
	m_strSql.Format("update videoTable set fusionPath=\'%s\' where caseName=\'%s\' and cameraName=\'%s\' and originPath=\'%s\'" ,fusionVideoPath,caseName,cameraName,originPath);
	if(mysql_real_query(&m_mysql,(char*)(LPCTSTR)m_strSql,(UINT)m_strSql.GetLength())!=0)
	{ 
		print_error(&m_mysql,"error message");
		return false;
	}
	else
	{
		return true;
	}
}
///@brief CDataMySql类的查找视频表中原始视频总帧数的函数
/// 
///由融合视频路径在记录表中查找该融合视频对应的原始视频的总帧数
///@param[in|out] FusVideoName 融合视频路径|NULL
///@pre NULL
///@return NULL
///@retval int 返回查找到的融合视频的总帧数，如果操作失败参数为-1
///@post NULL
int CDataMySql::GetorigTotalFramesFromVideoTable(CString FusVideoName)
{
	CString m_strSql;///<mysql查询语句
	MYSQL_RES *m_result;
	MYSQL_ROW m_sqlRow;
	int params = -1;
	m_strSql.Format("select origTotalFrames from videoTable where fusionPath=\'%s\'",FusVideoName);
	if(mysql_real_query(&m_mysql,(char*)(LPCTSTR)m_strSql,(UINT)m_strSql.GetLength())!=0)
	{ 
		print_error(&m_mysql,"error message");
	}
	else
	{
		m_result=mysql_store_result(&m_mysql);///<保存查询到的数据到m_result
		if (m_result)
		{
			m_sqlRow=mysql_fetch_row(m_result);///<获得所有结果字符串
			params = atoi(m_sqlRow[0]);
		}
		if(m_result!=NULL) mysql_free_result(m_result);///<释放结果资源
	}
	return params;
}
///@brief CDataMySql类的查找视频表中分割视频总帧数的函数
/// 
///由融合视频路径在记录表中查找该融合视频对应的分割视频的总帧数
///@param[in|out] FusVideoName 融合视频路径|NULL
///@pre NULL
///@return NULL
///@retval int 返回查找到的分割视频的总帧数，如果操作失败参数为-1
///@post NULL
int CDataMySql::GetsegTotalFramesFromVideoTable(CString FusVideoName)
{
	CString m_strSql;///<mysql查询语句
	MYSQL_RES *m_result;
	MYSQL_ROW m_sqlRow;
	int params = -1;
	m_strSql.Format("select segTotalFrames from videoTable where fusionPath=\'%s\'",FusVideoName);
	if(mysql_real_query(&m_mysql,(char*)(LPCTSTR)m_strSql,(UINT)m_strSql.GetLength())!=0)
	{ 
		print_error(&m_mysql,"error message");
	}
	else
	{
		m_result=mysql_store_result(&m_mysql);///<保存查询到的数据到m_result
		if (m_result)
		{
			m_sqlRow=mysql_fetch_row(m_result);///<获得所有结果字符串
			params = atoi(m_sqlRow[0]);
		}
		if(m_result!=NULL) mysql_free_result(m_result);///<释放结果资源
	}
	return params;
}
///@brief CDataMySql类的查找视频表中融合视频总帧数的函数
/// 
///由融合视频路径在记录表中查找该融合视频的总帧数
///@param[in|out] FusVideoName 融合视频路径|NULL
///@pre NULL
///@return NULL
///@retval int 返回查找到的融合视频的总帧数，如果操作失败参数为-1
///@post NULL
int CDataMySql::GetfusTotalFramesFromVideoTable(CString FusVideoName)
{
	CString m_strSql;///<mysql查询语句
	MYSQL_RES *m_result;
	MYSQL_ROW m_sqlRow;
	int params = -1;
	m_strSql.Format("select fusTotalFrames from videoTable where fusionPath=\'%s\'",FusVideoName);
	if(mysql_real_query(&m_mysql,(char*)(LPCTSTR)m_strSql,(UINT)m_strSql.GetLength())!=0)
	{ 
		print_error(&m_mysql,"error message");
	}
	else
	{
		m_result=mysql_store_result(&m_mysql);///<保存查询到的数据到m_result
		if (m_result)
		{
			m_sqlRow=mysql_fetch_row(m_result);///<获得所有结果字符串
			params = atoi(m_sqlRow[0]);
		}
		if(m_result!=NULL) mysql_free_result(m_result);///<释放结果资源
	}
	return params;
}


///@brief CDataMySql类的查询新段号的函数
/// 
///根据旧段号在新旧段对应表中查找新段号
///@param[in|out] oldParag 旧段号;NewToOldParaTableName 新旧段对应表的表名|NULL
///@pre NULL
///@return NULL
///@retval int 返回查到的新段号，如果操作失败返回-1
///@post NULL
int CDataMySql::FindNewParaFromNewToOldParaTable(int oldParag,CString NewToOldParaTableName)
{
	CString m_strSql;///<mysql查询语句
	MYSQL_RES *m_result;
	MYSQL_ROW m_sqlRow;
	int params = -1;
	m_strSql.Format("select nNewPara from %s where nOldPara = \'%d\'",NewToOldParaTableName,oldParag);
	if(mysql_real_query(&m_mysql,(char*)(LPCTSTR)m_strSql,(UINT)m_strSql.GetLength())!=0)
	{ 
		print_error(&m_mysql,"error message");
	}
	else
	{
		m_result=mysql_store_result(&m_mysql);///<保存查询到的数据到m_result
		if (m_result)
		{
			m_sqlRow=mysql_fetch_row(m_result);///<获得所有结果字符串
			params = atoi(m_sqlRow[0]);
		}
		if(m_result!=NULL) mysql_free_result(m_result);///<释放结果资源
	}
	return params;
}

///@brief CDataMySql类的获得原始trace表中行记录的函数
/// 
///根据行号在制定表中获得该行的一条记录并赋值给结构体的每一项属性
///@param[in|out] row 行号;OrigTraceTableName 原始trace表名|infor1 获得存储记录的OrigTraceTable结构体
///@pre NULL
///@return NULL
///@retval BOOL 返回操作是否成功
///@post NULL
bool CDataMySql::GetInfor1DataFromOrigTraceTable(int segID,OrigTraceTable* infor1,CString OrigTraceTableName)
{
	CString m_strSql;///<mysql查询语句
	MYSQL_RES *m_result;
	MYSQL_ROW m_sqlRow;
	m_strSql.Format("select nOldPara,segID,nX,nY,nWidth,nHeight,origFrame,segSize from %s where segID=\'%d\'",OrigTraceTableName,segID);
	
	if(mysql_real_query(&m_mysql,(char*)(LPCTSTR)m_strSql,(UINT)m_strSql.GetLength())!=0)
	{ 
		print_error(&m_mysql,"error message");
		return false;
	}
	else
	{
		m_result=mysql_store_result(&m_mysql);///<保存查询到的数据到m_result
		if (m_result)
	   {
			m_sqlRow=mysql_fetch_row(m_result);
			infor1->nOldPara = atoi(m_sqlRow[0]);
			infor1->segID = atoi(m_sqlRow[1]);
			infor1->nX = atoi(m_sqlRow[2]);
			infor1->nY = atoi(m_sqlRow[3]);
			infor1->nWidth = atoi(m_sqlRow[4]);
			infor1->nHeight = atoi(m_sqlRow[5]);
			infor1->origFrame = atoi(m_sqlRow[6]);
			infor1->segSize = atoi(m_sqlRow[7]);
			mysql_free_result(m_result);///<释放结果资源	  
		}
		return true; 
	}
		
}

///@brief CDataMySql类的得到表中目标总数的函数
/// 
///得到数据库中某表的不同运动目标总数
///@param[in|out] tableName 表名|NULL
///@pre NULL
///@return NULL
///@retval int 返回不同的运动目标总数，如果操作失败返回-1
///@post NULL
int CDataMySql::GetTableDistinctNum(CString tableName)
{
	CString m_strSql;///<mysql查询语句
	MYSQL_RES *m_result;
	MYSQL_ROW m_sqlRow;
	int params = -1;
	m_strSql.Format("select count(distinct objectID) from %s",tableName);
	if(mysql_real_query(&m_mysql,(char*)(LPCTSTR)m_strSql,(UINT)m_strSql.GetLength())!=0)
	{ 
		print_error(&m_mysql,"error message");
	}
	else
	{
		m_result=mysql_store_result(&m_mysql);///<保存查询到的数据到m_result
		if (m_result)
		{
			m_sqlRow=mysql_fetch_row(m_result);///<获得所有结果字符串
			params = atoi(m_sqlRow[0]);
		}
		if(m_result!=NULL) mysql_free_result(m_result);///<释放结果资源
	}
	return params;
}
///@brief CDataMySql类的清空表的函数
/// 
///清空数据库中某个表
///@param[in|out] tableName 表名|NULL
///@pre NULL
///@return NULL
///@retval BOOL 返回操作是否成功
///@post NULL
bool CDataMySql::CleanTable(CString tableName)
{
	CString m_strSql;///<mysql查询语句
	m_strSql.Format("truncate table %s",tableName); 
	if(mysql_real_query(&m_mysql,(char*)(LPCTSTR)m_strSql,(UINT)m_strSql.GetLength())!=0)
	{ 
		print_error(&m_mysql,"error message");
		return false;
	}
	else
	{
		return true;
	}
}
///@brief CDataMySql类的删除表的函数
/// 
///如果数据库中存在某个表则将其删除
///@param[in|out] tableName 表名|NULL
///@pre NULL
///@return NULL
///@retval BOOL 返回操作是否成功
///@post NULL
bool CDataMySql::DropTable(CString tableName)
{
	CString m_strSql;///<mysql查询语句
	m_strSql.Format("drop table if exists mydb.%s",tableName); 
	if(mysql_real_query(&m_mysql,(char*)(LPCTSTR)m_strSql,(UINT)m_strSql.GetLength())!=0)
	{ 
		print_error(&m_mysql,"error message");
		return false;
	}
	else
	{
		return true;
	}
}

///@brief CDataMySql类的创建前景段表的函数
/// 
///创建某个名字的前景段表，列及其属性为(nOldPara int primary key,startFrame int not null,endFrame int not null)
///@param[in|out] tableName 要创建的表名|NULL
///@pre NULL
///@return NULL
///@retval BOOL 返回操作是否成功
///@post NULL
bool CDataMySql::CreateVideoFGTable(CString tableName)
{
	CString m_strSql;
	m_strSql.Format("create table %s(nOldPara int primary key,startFrame int not null,endFrame int not null)",tableName);
	if(mysql_real_query(&m_mysql,(char*)(LPCTSTR)m_strSql,(UINT)m_strSql.GetLength())!=0)
	{ 
		print_error(&m_mysql,"error message");
		return false;
	}
	else
	{
		return true;
	}
}
///@brief CDataMySql类的插入数据到前景段表函数
/// 
///将一条记录插入到前景段表中
///@param[in|out] tableName 要插入数据的表名;infor 要插入的记录|NULL
///@pre  NULL
///@return NULL
///@retval BOOL 返回操作是否成功
///@post NULL
bool CDataMySql::InsertData2VideoFGTable(VideoFGTable infor,CString tableName)
{
	CString m_strSql;
	m_strSql.Format("insert into %s(nOldPara,startFrame,endFrame)values(\'%d\',\'%d\',\'%d\')",tableName,infor.nOldPara,infor.startFrame,infor.endFrame);
	if(mysql_real_query(&m_mysql,(char*)(LPCTSTR)m_strSql,(UINT)m_strSql.GetLength())!=0)
	{ 
		print_error(&m_mysql,"error message");
		return false;
	}
	else
	{
		return true;
	}
}
int CDataMySql::FindEndFrameFromVideoFGTable(int paraNum,CString TableName)
{
	CString m_strSql;
	MYSQL_RES *m_result;
	MYSQL_ROW m_sqlRow;
	int params = -1;
	m_strSql.Format("select endFrame from %s where nOldPara = \'%d\'",TableName,paraNum);
	if(mysql_real_query(&m_mysql,(char*)(LPCTSTR)m_strSql,(UINT)m_strSql.GetLength())!=0)
	{ 
		print_error(&m_mysql,"error message");
	}
	else
	{
		m_result=mysql_store_result(&m_mysql);///<保存查询到的数据到m_result
		if (m_result)
		{
			m_sqlRow=mysql_fetch_row(m_result);///<获得所有结果字符串
			params = atoi(m_sqlRow[0]);
		}
		if(m_result!=NULL) mysql_free_result(m_result);///<释放结果资源
	}
	return params;
}
int CDataMySql::FindStartFrameFromVideoFGTable(int paraNum,CString TableName)
{
	CString m_strSql;
	MYSQL_RES *m_result;
	MYSQL_ROW m_sqlRow;
	int params = -1;
	m_strSql.Format("select startFrame from %s where nOldPara = \'%d\'",TableName,paraNum);
	if(mysql_real_query(&m_mysql,(char*)(LPCTSTR)m_strSql,(UINT)m_strSql.GetLength())!=0)
	{ 
		print_error(&m_mysql,"error message");
	}
	else
	{
		m_result=mysql_store_result(&m_mysql);///<保存查询到的数据到m_result
		if (m_result)
		{
			m_sqlRow=mysql_fetch_row(m_result);///<获得所有结果字符串
			params = atoi(m_sqlRow[0]);
		}
		if(m_result!=NULL) mysql_free_result(m_result);///<释放结果资源
	}
	return params;
}
///@brief CDataMySql类的背景段表的函数
/// 
///创建某个名字的背景段表，列及其属性为(BGImgID int primary key,startFrame int not null,endFrame int not null)
///@param[in|out] tableName 要创建的表名|NULL
///@pre NULL
///@return NULL
///@retval BOOL 返回操作是否成功
///@post NULL
bool CDataMySql::CreateVideoBGTable(CString tableName)
{
	CString m_strSql;
	m_strSql.Format("create table %s(BGImgID int primary key,startFrame int not null,endFrame int not null)",tableName);
	if(mysql_real_query(&m_mysql,(char*)(LPCTSTR)m_strSql,(UINT)m_strSql.GetLength())!=0)
	{ 
		print_error(&m_mysql,"error message");
		return false;
	}
	else
	{
		return true;
	}
}
///@brief CDataMySql类的插入数据到背景段表函数
/// 
///将一条记录插入到背景段表中
///@param[in|out] tableName 要插入数据的表名;infor 要插入的记录|NULL
///@pre  NULL
///@return NULL
///@retval BOOL 返回操作是否成功
///@post NULL
bool CDataMySql::InsertData2VideoBGTable(VideoBGTable infor,CString tableName)
{
	CString m_strSql;
	m_strSql.Format("insert into %s(BGImgID,startFrame,endFrame)values(\'%d\',\'%d\',\'%d\')",tableName,infor.BGImgID,infor.startFrame,infor.endFrame);
	if(mysql_real_query(&m_mysql,(char*)(LPCTSTR)m_strSql,(UINT)m_strSql.GetLength())!=0)
	{ 
		print_error(&m_mysql,"error message");
		return false;
	}
	else
	{
		return true;
	}
}
///@brief CDataMySql类的查找背景段结束帧函数
/// 
///根据背景段号在背景段表中查找该段的结束帧号
///@param[in|out] bkgID 背景段编号;tableName 要查找的表名|NULL
///@pre  NULL
///@return NULL
///@retval int 查找到的背景段结束帧号，如果操作失败返回-1
///@post NULL
int CDataMySql::FindEndFrameFromVideoBGTable(int bkgID,CString TableName)
{
	CString m_strSql;
	MYSQL_RES *m_result;
	MYSQL_ROW m_sqlRow;
	m_strSql.Format("select endFrame from %s where BGImgID = \'%d\'",TableName,bkgID);
	int params = -1;
	if(mysql_real_query(&m_mysql,(char*)(LPCTSTR)m_strSql,(UINT)m_strSql.GetLength())!=0)
	{ 
		print_error(&m_mysql,"error message");
	}
	else
	{
		m_result=mysql_store_result(&m_mysql);///<保存查询到的数据到m_result
		if (m_result)
		{
			m_sqlRow=mysql_fetch_row(m_result);///<获得所有结果字符串
			params = atoi(m_sqlRow[0]);
		}
		if(m_result!=NULL) mysql_free_result(m_result);///<释放结果资源
	}
	return params;
}
///@brief CDataMySql类的查找背景段起始帧函数
/// 
///根据背景段号在背景段表中查找该段的起始帧号
///@param[in|out] bkgID 背景段编号;tableName 要查找的表名|NULL
///@pre  NULL
///@return NULL
///@retval int 查找到的背景段起始帧号，如果操作失败返回-1
///@post NULL
int CDataMySql::FindStartFrameFromVideoBGTable(int bkgID,CString TableName)
{
	CString m_strSql;
	MYSQL_RES *m_result;
	MYSQL_ROW m_sqlRow;
	int params = -1;
	m_strSql.Format("select startFrame from %s where BGImgID = \'%d\'",TableName,bkgID);
	if(mysql_real_query(&m_mysql,(char*)(LPCTSTR)m_strSql,(UINT)m_strSql.GetLength())!=0)
	{ 
		print_error(&m_mysql,"error message");
	}
	else
	{
		m_result=mysql_store_result(&m_mysql);///<保存查询到的数据到m_result
		if (m_result)
		{
			m_sqlRow=mysql_fetch_row(m_result);///<获得所有结果字符串
			params = atoi(m_sqlRow[0]);
		}
		if(m_result!=NULL) mysql_free_result(m_result);///<释放结果资源
	}
	return params;
}

///@brief CDataMySql类的得到某记录某监控点下视频总数的函数
/// 
///在视频表中得到某记录某监控点下视频总数
///@param[in|out] CaseName 记录名称;CameraName 监控点名称|NULL
///@pre  NULL
///@return NULL
///@retval int 返回视频数目，如果操作失败返回-1
///@post NULL
int CDataMySql::GetOrigVideoNumFromVideoTable(CString CaseName,CString CameraName)    
{
	CString m_strSql;
	MYSQL_RES *m_result;
	MYSQL_ROW m_sqlRow;
	m_strSql.Format("select count(*) from VideoTable where caseName = \'%s\' and CameraName = \'%s\'",CaseName,CameraName);
	int params = -1;
	if(mysql_real_query(&m_mysql,(char*)(LPCTSTR)m_strSql,(UINT)m_strSql.GetLength())!=0)
	{ 
		print_error(&m_mysql,"error message");
	}
	else
	{
		m_result=mysql_store_result(&m_mysql);///<保存查询到的数据到m_result
		if (m_result)
		{
			m_sqlRow=mysql_fetch_row(m_result);///<获得所有结果字符串
			params = atoi(m_sqlRow[0]);
		}
		if(m_result!=NULL) mysql_free_result(m_result);///<释放结果资源
	}
	return params;
}
///@brief CDataMySql类的得到某记录下原始视频总数的函数
/// 
///在视频表中得到某记录下原始视频总数
///@param[in|out] CaseName 记录名称|NULL
///@pre  NULL
///@return NULL
///@retval int 返回视频数目，如果操作失败返回-1
///@post NULL
int CDataMySql::GetCaseOrigVideoNumFromVideoTable(CString CaseName)
{
	CString m_strSql;
	MYSQL_RES *m_result;
	MYSQL_ROW m_sqlRow;
	m_strSql.Format("select count(*) from VideoTable where caseName = \'%s\'",CaseName);
	int params = -1;
	if(mysql_real_query(&m_mysql,(char*)(LPCTSTR)m_strSql,(UINT)m_strSql.GetLength())!=0)
	{ 
		print_error(&m_mysql,"error message");
	}
	else
	{
		m_result=mysql_store_result(&m_mysql);///<保存查询到的数据到m_result
		if (m_result)
		{
			m_sqlRow=mysql_fetch_row(m_result);///<获得所有结果字符串
			params = atoi(m_sqlRow[0]);
		}
		if(m_result!=NULL) mysql_free_result(m_result);///<释放结果资源
	}
	return params;

}
///@brief CDataMySql类的获得融合视频的函数
/// 
///在视频表中得到某记录下某监控点下原始视频对应的融合视频的路径
///@param[in|out] caseName 记录名称;cameraName 监控点名称;videoName 原始视频名称|NULL
///@pre  NULL
///@return NULL
///@retval CString 返回融合视频名称，如果操作失败返回“-1”
///@post NULL
CString CDataMySql::GetFusionNameFromVideoTable(CString caseName,CString cameraName,CString videoName)
{
	CString m_strSql;
	MYSQL_RES *m_result;
	MYSQL_ROW m_sqlRow;
	CString params = "-1";
	m_strSql.Format("select fusionPath from VideoTable where caseName = \'%s\' and cameraName = \'%s\' and originPath = \'%s\'",caseName,cameraName,videoName) ; 
	if(mysql_real_query(&m_mysql,(char*)(LPCTSTR)m_strSql,(UINT)m_strSql.GetLength())!=0)
	{ 
		print_error(&m_mysql,"error message");
	}
	else
	{
		m_result=mysql_store_result(&m_mysql);///<保存查询到的数据到m_result
		if (m_result)
		{
			m_sqlRow=mysql_fetch_row(m_result);///<获得所有结果字符串
			params = m_sqlRow[0];
		}
		if(m_result!=NULL) mysql_free_result(m_result);///<释放结果资源
	}
	return params;
}

///@brief CDataMySql类的获得新帧中所有运动目标的ROI的函数
/// 
///在新trace表中根据新帧号获得所有该帧中运动目标的ROI
///@param[in|out] newFrame 新帧号;tableName 要查找的新trace表|m_ROI 保存查找到的该帧中的物体的ROI
///@pre  NULL
///@return NULL
///@retval BOOL 返回操作是否成功
///@post NULL
bool CDataMySql::FindROIFromNewTraceTable(int NewFrame,vector <CvRect> *m_ROI,CString tableName)
{
	CString m_strSql;
	MYSQL_RES *m_result;
	MYSQL_ROW m_sqlRow;
	m_strSql.Format("select nX,nY,nWidth,nHeight from %s where newFrame = \'%d\'",tableName,NewFrame) ;
	if(mysql_real_query(&m_mysql,(char*)(LPCTSTR)m_strSql,(UINT)m_strSql.GetLength())!=0)
	{ 
		m_result = NULL;///<出错
		print_error(&m_mysql,"error message");
		return false;
	}
	else
	{
		m_result=mysql_store_result(&m_mysql);///<保存查询到的数据到m_result
		for (int i=0;i<m_result->row_count;i++)
		{
			m_sqlRow=mysql_fetch_row(m_result);///<获得所有结果字符串
			CvRect r;
			r.x = atoi(m_sqlRow[0]);
			r.y = atoi(m_sqlRow[1]);
			r.width = atoi(m_sqlRow[2]);
			r.height = atoi(m_sqlRow[3]);
			m_ROI->push_back(r);
		}
		if(m_result!=NULL)mysql_free_result(m_result);///<释放结果资源
		return true;
	}
}

///@brief CDataMySql类的获得合成前景图中所有运动目标的ROI的函数
/// 
///在Combine表中获得所有运动目标的ROI
///@param[in|out] newFrame 新帧号;tableName 要查找的新trace表|m_ROI 保存查找到的该帧中的物体的ROI
///@pre  NULL
///@return NULL
///@retval BOOL 返回操作是否成功
///@post NULL
bool CDataMySql::FindROIFromCombineSegsTable( vector <CvRect> *m_ROI, CString tableName)
{
	CString m_strSql;
	MYSQL_RES *m_result;
	MYSQL_ROW m_sqlRow;
	m_strSql.Format("select nX,nY,nWidth,nHeight from %s ", tableName);
	if (mysql_real_query(&m_mysql, (char*)(LPCTSTR)m_strSql, (UINT)m_strSql.GetLength()) != 0)
	{
		m_result = NULL;///<出错
		print_error(&m_mysql, "error message");
		return false;
	}
	else
	{
		m_result = mysql_store_result(&m_mysql);///<保存查询到的数据到m_result
		for (int i = 0; i<m_result->row_count; i++)
		{
			m_sqlRow = mysql_fetch_row(m_result);///<获得所有结果字符串
			CvRect r;
			r.x = atoi(m_sqlRow[0]);
			r.y = atoi(m_sqlRow[1]);
			r.width = atoi(m_sqlRow[2]);
			r.height = atoi(m_sqlRow[3]);
			m_ROI->push_back(r);
		}
		if (m_result != NULL)mysql_free_result(m_result);///<释放结果资源
		return true;
	}
}


///@brief CDataMySql类的新trace表的函数
/// 
///创建某个名字的新trace段表，列及其属性为(newFrame int not null,segID int primary key,nTop int not null,nBottom int not null,nLeft int not null,nRight int not null,CentX int not null,CentY int not null,origFrame int not null,segSize varchar(20) not null)
///@param[in|out] tableName 要创建的表名|NULL
///@pre NULL
///@return NULL
///@retval BOOL 返回操作是否成功
///@post NULL
bool CDataMySql::CreateNewTraceTable(CString tableName)
{
	CString m_strSql;
	m_strSql.Format("create table %s(newFrame int not null,segID int primary key,nX smallint(3) not null,nY smallint(3) not null,nWidth smallint(3) not null,nHeight smallint(3) not null,origFrame int not null,segSize enum('0','1','2') not null)",tableName);
	if(mysql_real_query(&m_mysql,(char*)(LPCTSTR)m_strSql,(UINT)m_strSql.GetLength())!=0)
	{ 
		print_error(&m_mysql,"error message");
		return false;
	}
	else
	{
		return true;
	}
}
///@brief CDataMySql类的融合生成新trace表的函数
/// 
///根据原始trace表和新旧帧对应表的旧帧号相等连接两表，生成新trace表
///@param[in|out] newTraceTable 新trace表名;origTraceTable 原始trace表名;frameTable 新旧帧对应表|NULL
///@pre  NULL
///@return NULL
///@retval BOOL 返回操作是否成功
///@post NULL
bool CDataMySql::Unite2NewTraceTable(CString newTraceTable,CString origTraceTable,CString frameTable)
{
	CString m_strSql;
	m_strSql.Format("insert into %s(newFrame,segID,nX,nY,nWidth,nHeight,origFrame,segSize) \
					select newFrame,segID,nX,nY,nWidth,nHeight,%s.origFrame,segSize from %s,%s \
					where %s.origFrame=%s.origFrame",newTraceTable,origTraceTable,origTraceTable,frameTable,origTraceTable,frameTable);
	if(mysql_real_query(&m_mysql,(char*)(LPCTSTR)m_strSql,(UINT)m_strSql.GetLength())!=0)
	{ 
		print_error(&m_mysql,"error message");
		return false;
	}
	else
	{
		return true;
	}
}
///@brief CDataMySql类的根据物体大小查找的函数
/// 
///根据物体的大小在表中查找符合条件的记录，被将数据存入结构体中
///@param[in|out] tableName 要查找的表名;segSize 物体大小|segIdParam 查找的数据存入vector容器中
///@pre  NULL
///@return NULL
///@retval BOOL 返回操作是否成功
///@post NULL
bool CDataMySql::SearchSegOfSize(CString tableName,int segSize,vector<SearchSegIDParams>* segIdParam)
{
 CString m_strSql;
 MYSQL_RES *m_result;
 MYSQL_ROW m_sqlRow;
 m_strSql.Format("select origFrame,nX,nY,nWidth,nHeight from %s where segSize=\'%d\'",tableName,segSize);  
 if(mysql_real_query(&m_mysql,(char*)(LPCTSTR)m_strSql,(UINT)m_strSql.GetLength())!=0)
 { 
	 print_error(&m_mysql,"error message");
	 return false;
 }
 else
 {
	 m_result=mysql_store_result(&m_mysql);///<保存查询到的数据到m_result
	 if (m_result)
	 {
		 SearchSegIDParams tempSearchData;
		 m_sqlRow=mysql_fetch_row(m_result);///<获得所有结果字符串
		while (m_sqlRow)
		{
			CString  temp= m_sqlRow[0];
			 if(!temp.IsEmpty())
			 {
				 tempSearchData.origFrame   = atoi(m_sqlRow[0]);
				 tempSearchData.ROI.nX    = atoi(m_sqlRow[1]);
				 tempSearchData.ROI.nY = atoi(m_sqlRow[2]);
				 tempSearchData.ROI.nWidth   = atoi(m_sqlRow[3]);
				 tempSearchData.ROI.nHeight  = atoi(m_sqlRow[4]);
				 (*segIdParam).push_back(tempSearchData);
			 }
			 m_sqlRow=mysql_fetch_row(m_result);///<获得所有结果字符串
		}
		 mysql_free_result(m_result);///<释放结果资源
	 }
 }
 return true;
}
///@brief CDataMySql类的判断数据库中某个表是否存在的函数
/// 
///判断数据库中某个表是否存在
///@param[in|out] tableName 要查找的表名|ifExist 表是否存在的标志
///@pre  NULL
///@return NULL
///@retval BOOL 返回操作是否成功
///@post NULL
bool  CDataMySql::IfExistTable(CString tableName,bool* ifExist)
{
	CString m_strSql;
	MYSQL_RES *m_result;
	MYSQL_ROW m_sqlRow;
	//m_strSql.Format("show tables from mydb like '%s'",tableName);
	//增加数据库名限制，修改by luying 2013.3.4  
	m_strSql.Format("select TABLE_NAME from INFORMATION_SCHEMA.TABLES where TABLE_SCHEMA = 'mydb' and TABLE_NAME='%s'",tableName);
	if(mysql_real_query(&m_mysql,(char*)(LPCTSTR)m_strSql,(UINT)m_strSql.GetLength())!=0)
	{ 
		print_error(&m_mysql,"error message");
		//*ifExist = false;
		return false;
	}
	else
	{
		m_result=mysql_store_result(&m_mysql);///<保存查询到的数据到m_result
		if (m_result)
		{
			m_sqlRow=mysql_fetch_row(m_result);///<获得所有结果字符串
			if(m_sqlRow==NULL)
			{
				*ifExist = false;
			}
			else
			{
				*ifExist = true;
			}
			mysql_free_result(m_result);///<释放结果资源
		}
		else
		{
			*ifExist = false;
		}
		return true;
	}
}

///@brief CDataMySql类的根据ROI信息找到原始视频帧数的函数
/// 
///从new trace表中根据ROI信息找到原始视频帧数
///@param[in|out] newTraceTable 要查找的表名;rect 要查找的ROI信息|NULL
///@pre  NULL
///@return NULL
///@retval int 返回在原始视频中的帧数,如果操作失败返回-1
///@post NULL
int CDataMySql::FindOrigFrameFromNewTraceTable(CString newTraceTable,CvRect rect)
{

	CString m_strSql;
	m_strSql.Format("select origFrame from %s where nX =\'%d\' and nWidth = \'%d\'",newTraceTable,rect.x,rect.width);
	MYSQL_RES *m_result;
	MYSQL_ROW m_sqlRow;
	int params = -1;
	if(mysql_real_query(&m_mysql,(char*)(LPCTSTR)m_strSql,(UINT)m_strSql.GetLength())!=0)
	{ 
		print_error(&m_mysql,"error message");
	}
	else
	{
		m_result=mysql_store_result(&m_mysql);///<保存查询到的数据到m_result
		if (m_result)
		{
			m_sqlRow=mysql_fetch_row(m_result);///<获得所有结果字符串
			params = atoi(m_sqlRow[0]);
		}
		if(m_result!=NULL) mysql_free_result(m_result);///<释放结果资源
	}
	return params;
}
///@brief CDataMySql类的建立clist表的函数
/// 
///建立一个基于记录名，监控点名和在CListCtrl行号的表，列名及其属性为(caseName VARCHAR(255) not null,cameraName VARCHAR(255) not null,videoName VARCHAR(255) not null,rowNum int primary key)
///@param[in|out] tableName 要创建的表名|NULL
///@pre  NULL
///@return NULL
///@retval BOOL 操作是否成功
///@post NULL
bool CDataMySql::CreateCListItemTable(CString tableName)
{
	CString m_strSql;
	m_strSql.Format("create table %s(caseName VARCHAR(255) not null,cameraName VARCHAR(255) not null,videoName VARCHAR(255) not null,rowNum int primary key)",tableName);
	if(mysql_real_query(&m_mysql,(char*)(LPCTSTR)m_strSql,(UINT)m_strSql.GetLength())!=0)
	{ 
		print_error(&m_mysql,"error message");
		return false;
	}
	else
	{
		return true;
	}
}

///@brief CDataMySql类的判断表中是否已经有了某个视频的函数
/// 
///列表中是否已经有了某个记录某个监控点的视频
///@param[in|out] tableName 要查询的表名;caseName 记录名;cameraName 监控点名|ifExist 视频是否存在
///@pre  NULL
///@return NULL
///@retval BOOL 操作是否成功
///@post NULL
bool CDataMySql::IfExistCListItemTable(CString tableName,CString caseName,CString cameraName,bool* ifExist)
{
	CString m_strSql;
	MYSQL_RES *m_result;
	MYSQL_ROW m_sqlRow;
	m_strSql.Format("select count(*) from %s where caseName=\'%s\' and cameraName=\'%s\'",tableName,caseName,cameraName);
	int params = -1;
	if(mysql_real_query(&m_mysql,(char*)(LPCTSTR)m_strSql,(UINT)m_strSql.GetLength())!=0)
	{ 
		print_error(&m_mysql,"error message");
		return false;
	}
	else
	{
		m_result=mysql_store_result(&m_mysql);///<保存查询到的数据到m_result
		if (m_result)
		{
			m_sqlRow=mysql_fetch_row(m_result);///<获得所有结果字符串
			params = atoi(m_sqlRow[0]);
		}
		if(m_result!=NULL) mysql_free_result(m_result);///<释放结果资源
	}
	if(params==0)
	{
		*ifExist = false;
	}
	else
	{
		*ifExist = true;
	}
	return true;

}
///@brief CDataMySql类的插入数据到Clist表函数
/// 
///将一条记录插入到Clist表中
///@param[in|out] tableName 要插入数据的表名;caseName 要插入的记录名;cameraName 要插入记录的监控点名;videoName 要插入记录的视频路径;clistRow 要插入记录的行号|NULL
///@pre  NULL
///@return NULL
///@retval BOOL 返回操作是否成功
///@post NULL
bool CDataMySql::InsertData2CListItemTable(CString tableName,CString caseName,CString cameraName,CString videoName,int clistRow)
{
	CString m_strSql;
	m_strSql.Format("insert into %s(caseName,cameraName,videoName,rowNum)values(\'%s\',\'%s\',\'%s\',\'%d\')",tableName,caseName,cameraName,videoName,clistRow);
	if(mysql_real_query(&m_mysql,(char*)(LPCTSTR)m_strSql,(UINT)m_strSql.GetLength())!=0)
	{ 
		print_error(&m_mysql,"error message");
		return false;
	}
	else
	{
		return true;
	}
}
///@brief CDataMySql类的得到clist表中某行号的视频名称的函数
/// 
///根据rowNum在clist表中查找原始视频路径
///@param[in|out] clistTableName 要查询的表名;clistRow 属性列rowNum的值|NULL
///@pre  NULL
///@return NULL
///@retval CString 返回查询到的原始视频路径，如果操作失败返回“-1”
///@post NULL
CString CDataMySql::GetCurrentOrigVideoName(CString clistTableName,int clistRow)
{
	CString m_strSql;
	MYSQL_RES *m_result;
	MYSQL_ROW m_sqlRow;
	m_strSql.Format("select videoName from %s where rowNum=\'%d\'",clistTableName,clistRow);//originPath from videoTable,%s where videoTable.caseName=%s.caseName and videoTable.cameraName=%s.cameraName and %s.rowNum=\'%d\'",clistTableName,clistTableName,clistTableName,clistTableName,clistRow);
	CString params = "-1";
	if(mysql_real_query(&m_mysql,(char*)(LPCTSTR)m_strSql,(UINT)m_strSql.GetLength())!=0)
	{ 
		print_error(&m_mysql,"error message");
	}
	else
	{
		m_result=mysql_store_result(&m_mysql);///<保存查询到的数据到m_result
		if (m_result)
		{
			m_sqlRow=mysql_fetch_row(m_result);///<获得所有结果字符串
			params = m_sqlRow[0];
		}
		if(m_result!=NULL) mysql_free_result(m_result);///<释放结果资源
	}
	return params;
}
///@brief CDataMySql类的得到clist表中某行号的融合视频名称的函数
/// 
///根据rowNum在clist表和视频表中中查找融合视频路径
///@param[in|out] clistTableName 要查询的表名;clistRow 属性列rowNum的值|NULL
///@pre  NULL
///@return NULL
///@retval CString 返回查询到的融合视频路径，如果操作失败返回“-1”
///@post NULL
CString CDataMySql::GetCurrentFusionVideoName(CString clistTableName,int clistRow)
{
	CString m_strSql;
	MYSQL_RES *m_result;
	MYSQL_ROW m_sqlRow;
	m_strSql.Format("select fusionPath from videoTable,%s where videoTable.caseName=%s.caseName and videoTable.cameraName=%s.cameraName and %s.rowNum=\'%d\'",clistTableName,clistTableName,clistTableName,clistTableName,clistRow);
	CString params = "-1";
	if(mysql_real_query(&m_mysql,(char*)(LPCTSTR)m_strSql,(UINT)m_strSql.GetLength())!=0)
	{ 
		print_error(&m_mysql,"error message");
	}
	else
	{
		m_result=mysql_store_result(&m_mysql);///<保存查询到的数据到m_result
		if (m_result)
		{
			m_sqlRow=mysql_fetch_row(m_result);///<获得所有结果字符串
			params = m_sqlRow[0];
		}
		if(m_result!=NULL) mysql_free_result(m_result);///<释放结果资源
	}
	return params;
}
///@brief CDataMySql类的得到目前正在处理视频所在的行号
/// 
///根据记录名和监控点名得到clist表中某记录某监控点的rowNum的函数
///@param[in|out] clistTableName 要查询的表名;caseName 记录名;cameraName 监控点名|NULL
///@pre  NULL
///@return NULL
///@retval int 返回查询到的rowNum，如果操作失败返回-1
///@post NULL
int CDataMySql::GetCurrentProcessingRow(CString clistTableName,CString caseName,CString cameraName)
{
	CString m_strSql;
	MYSQL_RES *m_result;
	MYSQL_ROW m_sqlRow;
	m_strSql.Format("select rowNum from %s where caseName = \'%s\' and cameraName = \'%s\'",clistTableName,caseName,cameraName);
	int params = -1;
	if(mysql_real_query(&m_mysql,(char*)(LPCTSTR)m_strSql,(UINT)m_strSql.GetLength())!=0)
	{ 
		print_error(&m_mysql,"error message");
	}
	else
	{
		m_result=mysql_store_result(&m_mysql);///<保存查询到的数据到m_result
		if (m_result)
		{
			m_sqlRow=mysql_fetch_row(m_result);///<获得所有结果字符串
			params = atoi(m_sqlRow[0]);
		}
		if(m_result!=NULL) mysql_free_result(m_result);///<释放结果资源
	}
	return params;
}
///@brief CDataMySql类的得到clist表中某行号的记录名称的函数
/// 
///根据rowNum在clist表中查找该记录的记录名称
///@param[in|out] clistTableName 要查询的表名;clistRow 属性列rowNum的值|NULL
///@pre  NULL
///@return NULL
///@retval CString 返回记录名称，如果操作失败返回“-1”
///@post NULL
CString CDataMySql::GetCurCaseName(CString clistTableName,int clistRow)
{
	CString m_strSql;
	MYSQL_RES *m_result;
	MYSQL_ROW m_sqlRow;
	m_strSql.Format("select caseName from %s where rowNum=\'%d\'",clistTableName,clistRow);
	CString params = "-1";
	if(mysql_real_query(&m_mysql,(char*)(LPCTSTR)m_strSql,(UINT)m_strSql.GetLength())!=0)
	{ 
		print_error(&m_mysql,"error message");
	}
	else
	{
		m_result=mysql_store_result(&m_mysql);///<保存查询到的数据到m_result
		if (m_result)
		{
			m_sqlRow=mysql_fetch_row(m_result);///<获得所有结果字符串
			params = m_sqlRow[0];
		}
		if(m_result!=NULL) mysql_free_result(m_result);///<释放结果资源
	}
	return params;
}
///@brief CDataMySql类的得到clist表中某行号的监控点名称的函数
/// 
///根据rowNum在clist表中查找该记录的监控点名称
///@param[in|out] clistTableName 要查询的表名;clistRow 属性列rowNum的值|NULL
///@pre  NULL
///@return NULL
///@retval CString 返回监控点名称，如果操作失败返回“-1”
///@post NULL
CString CDataMySql::GetCurCameraName(CString clistTableName,int clistRow)
{
	CString m_strSql;
	MYSQL_RES *m_result;
	MYSQL_ROW m_sqlRow;
	m_strSql.Format("select cameraName from %s where rowNum=\'%d\'",clistTableName,clistRow);
	CString params = "-1";
	if(mysql_real_query(&m_mysql,(char*)(LPCTSTR)m_strSql,(UINT)m_strSql.GetLength())!=0)
	{ 
		print_error(&m_mysql,"error message");
	}
	else
	{
		m_result=mysql_store_result(&m_mysql);///<保存查询到的数据到m_result
		if (m_result)
		{
			m_sqlRow=mysql_fetch_row(m_result);///<获得所有结果字符串
			params = m_sqlRow[0];
		}
		if(m_result!=NULL) mysql_free_result(m_result);///<释放结果资源
	}
	return params;
}

///@brief CDataMySql类的或得某记录监控点下所有视频的函数
/// 
///获得视频表中某记录某监控点下包括原始视频、分割视频和融合视频在内的所有视频的名字
///@param[in|out] CaseName 记录名;CameraName 监控点名;videoTotalNum 某记录某监控点下原始视频的数目|VideoName_str 所得到的所有视频的vector容器
///@pre  NULL
///@return NULL
///@retval BOOL 返回操作是否成功
///@post NULL
bool CDataMySql::GetVideoNameFromVideoTable(CString CaseName,CString CameraName,CString(*VideoName_str)[3],int videoTotalNum)
{
	CString m_strSql;
	MYSQL_RES *m_result;
	MYSQL_ROW m_sqlRow;
	m_strSql.Format("select originPath,segPath,fusionPath from VideoTable where caseName = \'%s\' and cameraName = \'%s\'",CaseName,CameraName) ; 
	if(mysql_real_query(&m_mysql,(char*)(LPCTSTR)m_strSql,(UINT)m_strSql.GetLength())!=0)
	{ 
		print_error(&m_mysql,"error message");
		return false;
	}
	else
	{
		m_result=mysql_store_result(&m_mysql);///<保存查询到的数据到m_result
		if(m_result)
		{
			for (int Num = 0 ; Num<videoTotalNum; Num++)
			{
				m_sqlRow=mysql_fetch_row(m_result);
				*(*(VideoName_str+Num)+0) = m_sqlRow[0];
				if(!m_sqlRow[1])
				{
				}
				else
				{
					*(*(VideoName_str+Num)+1) = m_sqlRow[1];
				}
				if(!m_sqlRow[2])
				{
				}
				else
				{
					*(*(VideoName_str+Num)+2) = m_sqlRow[2];
				}

			}
			mysql_free_result(m_result);///<释放结果资源
		}	 
	}
	return true ;
}
///@brief CDataMySql类的得到某融合视频的记录名的函数
/// 
///根据融合视频名称在视频表中获得该融合视频对应的记录名称
///@param[in|out] FusVideoName 融合视频名称|NULL
///@pre  NULL
///@return NULL
///@retval CString 返回记录名，如果操作失败返回“-1”
///@post NULL
CString CDataMySql::GetCaseNameFromVideoTable(CString FusVideoName)
{
	CString m_strSql;
	MYSQL_RES *m_result;
	MYSQL_ROW m_sqlRow;
	m_strSql.Format("select caseName from VideoTable where fusionPath = \'%s\'",FusVideoName);
	CString params = "-1";
	if(mysql_real_query(&m_mysql,(char*)(LPCTSTR)m_strSql,(UINT)m_strSql.GetLength())!=0)
	{ 
		print_error(&m_mysql,"error message");
	}
	else
	{
		m_result=mysql_store_result(&m_mysql);///<保存查询到的数据到m_result
		if (m_result)
		{
			m_sqlRow=mysql_fetch_row(m_result);///<获得所有结果字符串
			params = m_sqlRow[0];
		}
		if(m_result!=NULL) mysql_free_result(m_result);///<释放结果资源
	}
	return params;
}
///@brief CDataMySql类的得到某融合视频的监控点名的函数
/// 
///根据融合视频名称在视频表中获得该融合视频对应的监控点名称
///@param[in|out] FusVideoName 融合视频名称|NULL
///@pre  NULL
///@return NULL
///@retval CString 返回监控点名，如果操作失败返回“-1”
///@post NULL
CString CDataMySql::GetCameraNameFromVideoTable(CString FusVideoName)
{
	CString m_strSql;
	MYSQL_RES *m_result;
	MYSQL_ROW m_sqlRow;
	m_strSql.Format("select cameraName from VideoTable where fusionPath = \'%s\'",FusVideoName);
	CString params = "-1";
	if(mysql_real_query(&m_mysql,(char*)(LPCTSTR)m_strSql,(UINT)m_strSql.GetLength())!=0)
	{ 
		print_error(&m_mysql,"error message");
	}
	else
	{
		m_result=mysql_store_result(&m_mysql);///<保存查询到的数据到m_result
		if (m_result)
		{
			m_sqlRow=mysql_fetch_row(m_result);///<获得所有结果字符串
			params = m_sqlRow[0];
		}
		if(m_result!=NULL) mysql_free_result(m_result);///<释放结果资源
	}
	return params;
}
///@brief CDataMySql类的判断记录表中是否已经存在某个记录的函数
/// 
///判断记录表中是否已经存在某个记录
///@param[in|out] caseName 记录名称|ifExist 是否存在某个记录true为已经存在
///@pre  NULL
///@return NULL
///@retval BOOL 返回数据库操作是否成功
///@post NULL
bool CDataMySql::IfExistCaseNameInCaseTable(CString caseName,bool* ifExist)
{
	CString m_strSql;
	m_strSql.Format("select count(*) from caseTable where caseName=\'%s\'",caseName);
	MYSQL_RES *m_result;
	MYSQL_ROW m_sqlRow;
	int params = 0;
	if(mysql_real_query(&m_mysql,(char*)(LPCTSTR)m_strSql,(UINT)m_strSql.GetLength())!=0)
	{ 
		print_error(&m_mysql,"error message");
		return false;
	}
	else
	{
		m_result=mysql_store_result(&m_mysql);///<保存查询到的数据到m_result
		if (m_result)
		{
			m_sqlRow=mysql_fetch_row(m_result);///<获得所有结果字符串
			params = atoi(m_sqlRow[0]);
		}
		if(m_result!=NULL) mysql_free_result(m_result);///<释放结果资源		
	}
	if(params==0)
	{
		*ifExist = false;
	}
	else
	{
		*ifExist = true;
	}
	return true;
}
///@brief CDataMySql类的更新记录信息的函数
/// 
///更新记录表中某个记录对应的记录
///@param[in|out] caseData 更新的记录;caseName 要更新的记录名;tableName 要更新的表名|NULL
///@pre  NULL
///@return NULL
///@retval BOOL 返回数据库操作是否成功
///@post NULL
bool CDataMySql::UpdateCaseDataInCaseTable(CaseTable caseData,CString caseName,CString tableName)
{
	CString m_strSql;///<mysql查询语句
	m_strSql.Format("update %s set caseName=\'%s\',caseCategory=\'%s\',caseRecordTime=\'%s\',\
					caseHappenTime=\'%s\',caseDescription=\'%s\',caseNote=\'%s\' where caseName=\'%s\'" ,
					tableName,caseData.caseName,caseData.caseCategory,caseData.caseRecordTime,caseData.caseHappenTime,
					caseData.caseDescription,caseData.caseNote,caseName);
	if(mysql_real_query(&m_mysql,(char*)(LPCTSTR)m_strSql,(UINT)m_strSql.GetLength())!=0)
	{ 
		print_error(&m_mysql,"error message");
		return false;
	}
	else
	{
		return true;
	}
}
///@brief CDataMySql类的得到某个记录监控点下融合视频路径的函数
/// 
///根据记录名、监控点名得在视频表中得到该记录该监控点下的所有融合视频的名称
///@param[in|out] caseName 记录名;CameraName 监控点名|fusionName_Str 得到保存融合视频名称的vector
///@pre  NULL
///@return NULL
///@retval BOOL 返回数据库操作是否成功
///@post NULL
bool CDataMySql::GetFusionNameFromVideoTable(CString caseName,CString CameraName,vector<CString>* fusionName_Str)
{
	CString m_strSql;
	MYSQL_RES *m_result;
	MYSQL_ROW m_sqlRow=NULL;
	(*fusionName_Str).clear();
	m_strSql.Format("select fusionPath from VideoTable where caseName = \'%s\' and cameraName = \'%s\'",caseName,CameraName) ; 
	if(mysql_real_query(&m_mysql,(char*)(LPCTSTR)m_strSql,(UINT)m_strSql.GetLength())!=0)
	{ 
		print_error(&m_mysql,"error message");
		return false;
	}
	else
	{
		m_result=mysql_store_result(&m_mysql);///<保存查询到的数据到m_result
		if(m_result)
		{
			m_sqlRow=mysql_fetch_row(m_result);///<获得所有结果字符串
			while (m_sqlRow)
			{
				CString  temp= m_sqlRow[0];
				if(!temp.IsEmpty())
				{
					(*fusionName_Str).push_back(m_sqlRow[0]);   
				}
				m_sqlRow = mysql_fetch_row(m_result);///<获得所有结果字符串
			}
			mysql_free_result(m_result);///<释放结果资源
		}
	}
	return true;
}
///@brief CDataMySql类的插入记录到线索表的函数
/// 
///插入一条记录到线索表中
///@param[in|out] clueData 要插入的记录|NULL
///@pre  NULL
///@return NULL
///@retval BOOL 返回数据库操作是否成功
///@post NULL
bool CDataMySql::InsertData2ClueTable(ClueTable clueData)
{
	CString m_strSql;///<mysql查询语句
	m_strSql.Format("insert into clueTable(caseName,cameraName,clueHappenTime,\
					clueCategory,latestModifyTime,filePath,clueDescription) \
					values(\'%s\',\'%s\',\'%s\',\'%s\',\'%s\',\'%s\',\'%s\')" ,
		clueData.caseName,clueData.cameraName,clueData.clueHappenTime,clueData.clueCategory,clueData.latestModifyTime,clueData.filePath,clueData.clueDescription);
	if(mysql_real_query(&m_mysql,(char*)(LPCTSTR)m_strSql,(UINT)m_strSql.GetLength())!=0)
	{ 
		print_error(&m_mysql,"error message");
		return false;
	}
	else
	{
		return true;
	}
}
///@brief CDataMySql类的获得某记录线索数目的函数
/// 
///插入一条记录到线索表中
///@param[in|out] caseName 记录名|NULL
///@pre  NULL
///@return NULL
///@retval int 返回线索数目，如果操作失败则返回-1
///@post NULL
int CDataMySql::GetClueNumFromClueTable(CString caseName)
{
	CString m_strSql;///<mysql查询语句
	MYSQL_RES *m_result;
	MYSQL_ROW m_sqlRow;
	int params = -1;
	m_strSql.Format("select count(*) from clueTable where caseName = \'%s\'",caseName);
	if(mysql_real_query(&m_mysql,(char*)(LPCTSTR)m_strSql,(UINT)m_strSql.GetLength())!=0)
	{ 
		print_error(&m_mysql,"error message");
	}
	else
	{
		m_result=mysql_store_result(&m_mysql);///<保存查询到的数据到m_result
		if (m_result)
		{
			m_sqlRow=mysql_fetch_row(m_result);///<获得所有结果字符串
			params = atoi(m_sqlRow[0]);
		}
		if(m_result!=NULL) mysql_free_result(m_result);///<释放结果资源
	}
	return params;
}
///@brief CDataMySql类的判断某条线索是否存在的函数
/// 
///在线索表中根据记录、监控点、线索类别和线索文件路径判断线索是否存在，如果存在则获得线索的其他信息
///@param[in|out] clueData 用于查找和保存结构的ClueTable结构体|isExistClue 线索是否存在
///@pre  NULL
///@return NULL
///@retval BOOL 返回数据库操作是否成功
///@post NULL
bool CDataMySql::GetClueDataFromClueTable(ClueTable* clueData,bool*isExistClue)
{
	CString m_strSql;
	MYSQL_RES *m_result;
	MYSQL_ROW m_sqlRow;
	m_strSql.Format("select clueHappenTime,latestModifyTime,clueDescription\
					from clueTable where caseName = \'%s\' and cameraName=\'%s\' and clueCategory=\'%s\'and filePath=\'%s\'",\
					clueData->caseName,clueData->cameraName,clueData->clueCategory,clueData->filePath) ; 
	if(mysql_real_query(&m_mysql,(char*)(LPCTSTR)m_strSql,(UINT)m_strSql.GetLength())!=0)
	{ 
		print_error(&m_mysql,"error message");
		return false;
	}
	else
	{
		m_result=mysql_store_result(&m_mysql);///<保存查询到的数据到m_result
		if(m_result)
		{
			m_sqlRow = mysql_fetch_row(m_result);///<获得所有结果字符串
			if(m_sqlRow!=NULL)
			{
				*isExistClue = true;
				clueData->clueHappenTime = m_sqlRow[0];
				clueData->latestModifyTime = m_sqlRow[1];
				clueData->clueDescription = m_sqlRow[2];
			}
			else
			{
				*isExistClue = false;
			}
			mysql_free_result(m_result);///<释放结果资源
		}
		return true;
	}
}
///@brief CDataMySql类的更新某条线索的描述的函数
/// 
///更新线索表中某条线索记录的线索描述
///@param[in|out] clueData 线索记录|NULL
///@pre  NULL
///@return NULL
///@retval BOOL 返回数据库操作是否成功
///@post NULL
bool CDataMySql::UpdateDescriptionInClueTable(ClueTable clueData)
{
	CString m_strSql;///<mysql查询语句
	m_strSql.Format("update clueTable set clueDescription=\'%s\' where caseName=\'%s\' and cameraName=\'%s\' and clueHappenTime=\'%s\'\
					and clueCategory=\'%s\' and latestModifyTime=\'%s\' and filePath=\'%s\'", 
					clueData.clueDescription,clueData.caseName,clueData.cameraName,clueData.clueHappenTime,clueData.clueCategory,clueData.latestModifyTime,clueData.filePath);
	if(mysql_real_query(&m_mysql,(char*)(LPCTSTR)m_strSql,(UINT)m_strSql.GetLength())!=0)
	{ 
		print_error(&m_mysql,"error message");
		return false;
	}
	else
	{
		return true;
	}
}
///@brief CDataMySql类的获得某个记录下所有监控点的名称的函数
/// 
///在视频表中获得某个记录下所有监控点的名称
///@param[in|out] caseName 记录名称|CameraName_Str 所获得的保存所有监控点的vector
///@pre  NULL
///@return NULL
///@retval BOOL 返回数据库操作是否成功
///@post NULL
bool CDataMySql::GetMoniorNameFromVideoTable(CString caseName,vector<CString>* CameraName_Str)
{
	CString m_strSql;
	MYSQL_RES *m_result;
	MYSQL_ROW m_sqlRow;
	(*CameraName_Str).clear();
	m_strSql.Format("select distinct cameraName from VideoTable where caseName = \'%s\'",caseName) ; 
	if(mysql_real_query(&m_mysql,(char*)(LPCTSTR)m_strSql,(UINT)m_strSql.GetLength())!=0)
	{ 
		print_error(&m_mysql,"error message");
		return false;
	}
	else
	{
		m_result=mysql_store_result(&m_mysql);///<保存查询到的数据到m_result
		if(m_result)
		{
			m_sqlRow=mysql_fetch_row(m_result);///<获得所有结果字符串
			while (m_sqlRow)
			{
				CString  temp= m_sqlRow[0];
				if(!temp.IsEmpty())
				{
					(*CameraName_Str).push_back(m_sqlRow[0]);   
				}
				m_sqlRow = mysql_fetch_row(m_result);///<获得所有结果字符串
			}
			mysql_free_result(m_result);///<释放结果资源
		}
	}
	return true;
}
///@brief CDataMySql类的获得所有记录名的函数
/// 
///在记录表中获得所有记录名
///@param[in|out] NULL|CaseName_Str 所获得的保存所有记录名的vector
///@pre  NULL
///@return NULL
///@retval BOOL 返回数据库操作是否成功
///@post NULL
bool CDataMySql::GetAllCaseNameFromCaseTable(vector<CString>* CaseName_Str)
{
	CString m_strSql;
	MYSQL_RES *m_result;
	MYSQL_ROW m_sqlRow;
	(*CaseName_Str).clear();
	m_strSql.Format("select caseName from CaseTable") ; 
	if(mysql_real_query(&m_mysql,(char*)(LPCTSTR)m_strSql,(UINT)m_strSql.GetLength())!=0)
	{ 
		print_error(&m_mysql,"error message");
		return false;
	}
	else
	{
		m_result=mysql_store_result(&m_mysql);///<保存查询到的数据到m_result
		if(m_result)
		{
			m_sqlRow=mysql_fetch_row(m_result);///<获得所有结果字符串
			while (m_sqlRow)
			{
				CString  temp= m_sqlRow[0];
				if(!temp.IsEmpty())
				{
					(*CaseName_Str).push_back(m_sqlRow[0]);   
				}
				m_sqlRow = mysql_fetch_row(m_result);///<获得所有结果字符串
			}
			mysql_free_result(m_result);///<释放结果资源
		}
	}
	return true;
}
///@brief CDataMySql类的获得某记录监控点下所有原始视频的函数
/// 
///在记录表中获得某记录监控点下所有原始视频路径
///@param[in|out] CaseName 记录名;CameraName 监控点名|VideoName_Str 所获得的保存所有原始视频路径的vector
///@pre  NULL
///@return NULL
///@retval BOOL 返回数据库操作是否成功
///@post NULL
bool CDataMySql::GetOriginNameFromVideoTable(CString CaseName,CString CameraName,vector<CString>* VideoName_Str)
{
	CString m_strSql;
	MYSQL_RES *m_result;
	MYSQL_ROW m_sqlRow;
	m_strSql.Format("select originPath from VideoTable where caseName = \'%s\' and cameraName = \'%s\'",CaseName,CameraName) ; 
	if(mysql_real_query(&m_mysql,(char*)(LPCTSTR)m_strSql,(UINT)m_strSql.GetLength())!=0)
	{ 
		print_error(&m_mysql,"error message");
		return false;
	}
	else
	{
		m_result=mysql_store_result(&m_mysql);///<保存查询到的数据到m_result
		if(m_result)
		{
			m_sqlRow=mysql_fetch_row(m_result);///<获得所有结果字符串
			while (m_sqlRow)
			{
				CString  temp= m_sqlRow[0];
				if(!temp.IsEmpty())
				{
					(*VideoName_Str).push_back(m_sqlRow[0]);   
				}
				m_sqlRow = mysql_fetch_row(m_result);///<获得所有结果字符串
			}
			mysql_free_result(m_result);///<释放结果资源
		}
	}
	return true ;
}
///@brief CDataMySql类的获得某一记录所有图片分类名的函数
/// 
///在图片表中获得某一记录所有图片分类的名称
///@param[in|out] CaseName 记录名|VideoName_Str 所获得的保存所有图片类别名的vector
///@pre  NULL
///@return NULL
///@retval BOOL 返回数据库操作是否成功
///@post NULL
bool CDataMySql::GetCategoryNameFromImageTable(CString caseName,vector<CString>* CategoryName_str)    
{
	CString m_strSql;
	m_strSql.Format("select distinct imageCategory from imageTable where caseName=\'%s\'",caseName);
	MYSQL_RES *m_result;
	MYSQL_ROW m_sqlRow;
	if(mysql_real_query(&m_mysql,(char*)(LPCTSTR)m_strSql,(UINT)m_strSql.GetLength())!=0)
	{ 
		print_error(&m_mysql,"error message");
		return false;
	}
	else
	{
		m_result=mysql_store_result(&m_mysql);///<保存查询到的数据到m_result
		if(m_result)
		{
			m_sqlRow=mysql_fetch_row(m_result);///<获得所有结果字符串
			while (m_sqlRow)
			{
				CString  temp= m_sqlRow[0];
				if(!temp.IsEmpty())
				{
					(*CategoryName_str).push_back(m_sqlRow[0]);   
				}
				m_sqlRow = mysql_fetch_row(m_result);///<获得所有结果字符串
			}
			mysql_free_result(m_result);///<释放结果资源
		}
	}
	return true ;
}
///@brief CDataMySql类的获得某一记录图片类别下所有图片路径的函数
/// 
///在图片表中获得某一记录图片类别下所有图片路径
///@param[in|out] CaseName 记录名;category 图片类别|ImageName_Str 所获得的保存所有图片路径的vector
///@pre  NULL
///@return NULL
///@retval BOOL 返回数据库操作是否成功
///@post NULL
bool CDataMySql::GetImgPathFromImageTable(CString caseName,CString category,vector<CString>* ImageName_Str)
{
	CString m_strSql;
	MYSQL_RES *m_result;
	MYSQL_ROW m_sqlRow;
	m_strSql.Format("select imagePath from imageTable where caseName = \'%s\' and imageCategory = \'%s\'",caseName,category) ; 
	if(mysql_real_query(&m_mysql,(char*)(LPCTSTR)m_strSql,(UINT)m_strSql.GetLength())!=0)
	{ 
		print_error(&m_mysql,"error message");
		return false;
	}
	else
	{
		m_result=mysql_store_result(&m_mysql);///<保存查询到的数据到m_result
		if(m_result)
		{
			m_sqlRow=mysql_fetch_row(m_result);///<获得所有结果字符串
			while (m_sqlRow)
			{
				CString  temp= m_sqlRow[0];
				if(!temp.IsEmpty())
				{
					(*ImageName_Str).push_back(m_sqlRow[0]);   
				}
				m_sqlRow = mysql_fetch_row(m_result);///<获得所有结果字符串
			}
			mysql_free_result(m_result);///<释放结果资源
		}
	}
	return true ;
}
///@brief CDataMySql类的删除线索表某条记录的函数
/// 
///在线索表中删除线索表某条记录
///@param[in|out] clueData 要删除的线索|NULL
///@pre  NULL
///@return NULL
///@retval BOOL 返回数据库操作是否成功
///@post NULL
bool CDataMySql::DeleteDataFromClueTable(ClueTable clueData)
{
	CString m_strSql;
	m_strSql.Format("delete from clueTable where caseName=\'%s\' and cameraName=\'%s\' and clueHappenTime=\'%s\'\
					and clueCategory=\'%s\' and latestModifyTime=\'%s\' and filePath=\'%s\' and clueDescription=\'%s\'",
					clueData.caseName,clueData.cameraName,clueData.clueHappenTime,clueData.clueCategory,clueData.latestModifyTime,clueData.filePath,clueData.clueDescription);
	if(mysql_real_query(&m_mysql,(char*)(LPCTSTR)m_strSql,(UINT)m_strSql.GetLength())!=0)
	{ 
		print_error(&m_mysql,"error message");
		return false;
	}
	else
	{
		return true;
	}
}

///@brief CDataMySql类的视频表中是否已经有了某个视频的函数
/// 
///视频表中是否已经有了某个记录某个监控点某个视频
///@param[in|out] caseName 记录名称;cameraName 监控点名称;videoName 原始视频名称|ifExist 是否已经存在
///@pre  NULL
///@return NULL
///@retval BOOL 返回数据库操作是否成功
///@post NULL
bool CDataMySql::IfExistOrigPathVideoTable(CString caseName,CString cameraName,CString videoName,bool* ifExist)
{
	CString m_strSql;
	MYSQL_RES *m_result;
	MYSQL_ROW m_sqlRow;
	m_strSql.Format("select count(*) from videoTable where caseName=\'%s\' and cameraName=\'%s\' and originPath=\'%s\'",caseName,cameraName,videoName);
	int params = -1;
	if(mysql_real_query(&m_mysql,(char*)(LPCTSTR)m_strSql,(UINT)m_strSql.GetLength())!=0)
	{ 
		print_error(&m_mysql,"error message");
		return false;
	}
	else
	{
		m_result=mysql_store_result(&m_mysql);///<保存查询到的数据到m_result
		if (m_result)
		{
			m_sqlRow=mysql_fetch_row(m_result);///<获得所有结果字符串
			params = atoi(m_sqlRow[0]);
		}
		if(m_result!=NULL) mysql_free_result(m_result);///<释放结果资源
	}
	if(params==0)
	{
		*ifExist = false;
	}
	else
	{
		*ifExist = true;
	}
	return true;

}

/****************************DrawDetect里面用到的表********************************************/
///@brief CDataMySql类的运动物体表的函数
/// 
///创建某个名字的运动物体表，列及其属性为(objectID int not null,frameID int not null,centerX int not null,centerY int not null,\
nTop int not null,nBottom int not null,nLeft int not null,nRight int not null)
///@param[in|out] tableName 要创建的表名|NULL
///@pre NULL
///@return NULL
///@retval BOOL 返回操作是否成功
///@post NULL
bool CDataMySql::CreateObjectTable(CString tableName)
{
	CString m_strSql;
	m_strSql.Format("create table %s(objectID int not null,frameID int not null,centerX int not null,centerY int not null,\
					nTop int not null,nBottom int not null,nLeft int not null,nRight int not null)",tableName);
	if(mysql_real_query(&m_mysql,(char*)(LPCTSTR)m_strSql,(UINT)m_strSql.GetLength())!=0)
	{ 
		print_error(&m_mysql,"error message");
		return false;
	}
	else
	{
		return true;
	}
}
///@brief CDataMySql类的插入数据到运动物体表函数
/// 
///将一条记录插入到运动物体表中
///@param[in|out] ObjectTableName 要插入数据的表名;infor5 要插入的记录|NULL
///@pre  NULL
///@return NULL
///@retval BOOL 返回操作是否成功
///@post NULL
bool CDataMySql::InsertData2ObjectTable(NewTrackParam infor5,CString ObjectTableName)
{
	CString m_strSql;///<mysql查询语句
	m_strSql.Format("insert into %s(objectID,frameID,centerX,centerY,nTop,nBottom,nLeft,nRight)\
					values(\'%d\',\'%d\',\'%d\',\'%d\',\'%d\',\'%d\',\'%d\',\'%d\')",\
					ObjectTableName,infor5.segID,infor5.origFrame,infor5.CentX,infor5.CentY,infor5.nTop,infor5.nBottom,infor5.nLeft,infor5.nRight); //sql插入语句
	if(mysql_real_query(&m_mysql,(char*)(LPCTSTR)m_strSql,(UINT)m_strSql.GetLength())!=0)
	{ 
		print_error(&m_mysql,"error message");
		return false;
	}
	else
	{
		return true;
	}
}
///@brief CDataMySql类的查找运动物体信息的函数
/// 
///
///@param[in|out] objectID 物体编号;ObjectTableName 要插入数据的表名|NULL
///@pre  NULL
///@return NULL
///@retval BOOL 返回操作是否成功
///@post NULL
void CDataMySql::GetCenterFromObjectTable(int objectID, CString ObjectTableName, vector<CPoint>* centersOfObj)
{
	CString m_strSql;
	MYSQL_RES *m_result;
	MYSQL_ROW m_sqlRow;
	m_strSql.Format("select centerX,centerY from %s where objectID=\'%d\'",ObjectTableName,objectID);
	if(mysql_real_query(&m_mysql,(char*)(LPCTSTR)m_strSql,(UINT)m_strSql.GetLength())!=0)
	{ 
		print_error(&m_mysql,"error message");
	}
	else
	{
		m_result=mysql_store_result(&m_mysql);///<保存查询到的数据到m_result
		if (m_result)
		{
			m_sqlRow=mysql_fetch_row(m_result);///<获得所有结果字符串
			while (m_sqlRow)
			{
				centersOfObj->push_back(CPoint(atoi(m_sqlRow[0]),atoi(m_sqlRow[1])));
				m_sqlRow=mysql_fetch_row(m_result);
			}
		}
		if(m_result!=NULL) mysql_free_result(m_result);///<释放结果资源
	}

}
///@brief CDataMySql类的获取某一帧的运动物体的ID和Rect函数
/// 
///
///@param[in|out] frameID 帧号;ObjectTableName 表名|rect 物体的rect信息集合; objID ID集合
///@pre  NULL
///@return NULL
///@retval NULL
///@post NULL
void CDataMySql::GetRectFromObjectTable(int frameID, CString ObjectTableName, vector<CRect>* rect,vector<int>* objID)
{
	CString m_strSql;
	MYSQL_RES *m_result;
	MYSQL_ROW m_sqlRow;
	m_strSql.Format("select centerX,centerY,nLeft,nTop,nRight,nBottom,objectID from %s where frameID=\'%d\'",ObjectTableName,frameID);
	if(mysql_real_query(&m_mysql,(char*)(LPCTSTR)m_strSql,(UINT)m_strSql.GetLength())!=0)
	{ 
		print_error(&m_mysql,"error message");
	}
	else
	{
		if (m_result=mysql_store_result(&m_mysql))///<保存查询到的数据到m_result
		{
			while (m_sqlRow=mysql_fetch_row(m_result))
			{
				rect->push_back(CRect(atoi(m_sqlRow[0]),atoi(m_sqlRow[1]),0,0));
				rect->push_back(CRect(atoi(m_sqlRow[2]),atoi(m_sqlRow[3]),atoi(m_sqlRow[4]),atoi(m_sqlRow[5])));
				objID->push_back(atoi(m_sqlRow[6]));
			}
		}
		if(m_result!=NULL) mysql_free_result(m_result);///<释放结果资源
	}
}
///@brief CDataMySql类的一次性读取某个运动目标的轨迹点集合和Rect集合函数
/// 
///
///@param[in|out] objID 运动目标编号;ObjectTableName 表名|rect 物体的rect信息集合; centersOfObj 运动目标的轨迹点集合
///@pre  NULL
///@return NULL
///@retval NULL
///@post NULL
void CDataMySql::GetSingleTraceFromObjectTable(int objID, CString ObjectTableName, vector<CRect>* rect,vector<CPoint>* centersOfObj)
{
	CString m_strSql;
	MYSQL_RES *m_result;
	MYSQL_ROW m_sqlRow;
	m_strSql.Format("select nLeft,nTop,nRight,nBottom,centerX,centerY from %s where objectID=\'%d\'",ObjectTableName,objID);
	if(mysql_real_query(&m_mysql,(char*)(LPCTSTR)m_strSql,(UINT)m_strSql.GetLength())!=0)
	{ 
		print_error(&m_mysql,"error message");
	}
	else
	{
		m_result=mysql_store_result(&m_mysql);///<保存查询到的数据到m_result
		if (m_result)
		{
			m_sqlRow=mysql_fetch_row(m_result);///<获得所有结果字符串
			while (m_sqlRow)
			{
				rect->push_back(CRect(atoi(m_sqlRow[0]),atoi(m_sqlRow[1]),atoi(m_sqlRow[2]),atoi(m_sqlRow[3])));
				centersOfObj->push_back(CPoint(atoi(m_sqlRow[4]),atoi(m_sqlRow[5])));
				m_sqlRow=mysql_fetch_row(m_result);
			}
		}
		if(m_result!=NULL) mysql_free_result(m_result);///<释放结果资源
	}
}
///@brief CDataMySql类的获取某一帧特殊位置的物体ID函数
/// 
///
///@param[in|out] frameID 帧号;rect 某一rect位置;ObjectTableName 表名|NULL
///@pre  NULL
///@return NULL
///@retval NULL
///@post NULL
int  CDataMySql::GetObjectIDFromObjectTable(int frameID,CRect rect,CString ObjectTableName)
{
	CString m_strSql;
	MYSQL_RES *m_result;
	MYSQL_ROW m_sqlRow;
	int param = -1;
	m_strSql.Format("select objectID from %s where nTop=\'%d\' and nBottom=\'%d\' and nLeft=\'%d\' and nRight=\'%d\'",ObjectTableName,rect.top,rect.bottom,rect.left,rect.right);

	if(mysql_real_query(&m_mysql,(char*)(LPCTSTR)m_strSql,(UINT)m_strSql.GetLength())!=0)
	{ 
		print_error(&m_mysql,"error message");
		return param;
	}
	else
	{
		m_result=mysql_store_result(&m_mysql);///<保存查询到的数据到m_result
		if (m_result)
		{
			m_sqlRow=mysql_fetch_row(m_result);///<获得所有结果字符串
			if (m_sqlRow)
			{
				param = atoi(m_sqlRow[0]);
			}
			
		}
		if(m_result!=NULL) mysql_free_result(m_result);///<释放结果资源
		return param;
	}
}
///@brief CDataMySql类的获取某一运动目标的frameID集合函数
/// 
///
///@param[in|out] objectID 运动目标编号;ObjectTableName 表名|frameID 运动目标所在的帧号集合
///@pre  NULL
///@return NULL
///@retval BOOL 数据库操作是否成功
///@post NULL
bool CDataMySql::GetFrameidFromObjectTable(int objectID,CString ObjectTableName,vector<int>* frameID)
{
	CString m_strSql;
	MYSQL_RES *m_result;
	MYSQL_ROW m_sqlRow;
	m_strSql.Format("select frameID from %s where objectID =\'%d\'",ObjectTableName,objectID);
	if(mysql_real_query(&m_mysql,(char*)(LPCTSTR)m_strSql,(UINT)m_strSql.GetLength())!=0)
	{
		print_error(&m_mysql,"error message");
		return false;
	}
	else
	{
		m_result=mysql_store_result(&m_mysql);///<保存查询到的数据到m_result
		if (m_result)
		{
			m_sqlRow=mysql_fetch_row(m_result);///<获得所有结果字符串
			while(m_sqlRow)
			{
				frameID->push_back(atoi(m_sqlRow[0]));
				m_sqlRow=mysql_fetch_row(m_result);
			}
		}
		if(m_result!=NULL) mysql_free_result(m_result);///<释放结果资源
	}
	return true;
}
///@brief CDataMySql类的判断是否已经有了某个图片的函数
/// 
///判断图片表中是否已经有了某个记录某个类别的某张图片
///@param[in|out] caseName 记录名;category 图片分类;imgName 图片路径|ifExist 是否存在
///@pre  NULL
///@return NULL
///@retval BOOL 数据库操作是否成功
///@post NULL
bool CDataMySql::GetObjectInfoFromObjectTable(int objectID,CString ObjectTableName,objectInfo* tempInfo)
{
	CString m_strSql;
	MYSQL_RES *m_result;
	MYSQL_ROW m_sqlRow;
	m_strSql.Format("select objectID,frameID,nTop,nBottom,nLeft,nRight from %s where objectID=\'%d\'",ObjectTableName,objectID);
	if(mysql_real_query(&m_mysql,(char*)(LPCTSTR)m_strSql,(UINT)m_strSql.GetLength())!=0)
	{ 
		print_error(&m_mysql,"error message");
		return false;
	}
	else
	{
		m_result=mysql_store_result(&m_mysql);///<保存查询到的数据到m_result
		if (m_result)
		{
			m_sqlRow=mysql_fetch_row(m_result);///<获得所有结果字符串
			if(m_sqlRow)
			{
				tempInfo->objectID		= atoi(m_sqlRow[0]);
				tempInfo->firstFrameID  = atoi(m_sqlRow[1]);
				tempInfo->lastFrameID	= atoi(m_sqlRow[1]);
				tempInfo->roi.top		= atoi(m_sqlRow[2]);
				tempInfo->roi.bottom	= atoi(m_sqlRow[3]);
				tempInfo->roi.left		= atoi(m_sqlRow[4]);
				tempInfo->roi.right		= atoi(m_sqlRow[5]);
			}
			while (m_sqlRow)
			{
				if (tempInfo->lastFrameID < atoi(m_sqlRow[1]))
				{
					tempInfo->lastFrameID = atoi(m_sqlRow[1]);
				}
				m_sqlRow=mysql_fetch_row(m_result);
			}
		}
		if(m_result!=NULL) mysql_free_result(m_result);///<释放结果资源
		return true;
	}
	

}

///@brief CDataMySql类的判断是否已经有了某个图片的函数
/// 
///判断图片表中是否已经有了某个记录某个类别的某张图片
///@param[in|out] caseName 记录名;category 图片分类;imgName 图片路径|ifExist 是否存在
///@pre  NULL
///@return NULL
///@retval BOOL 数据库操作是否成功
///@post NULL
bool CDataMySql::IfExistImagePathImageTable(CString caseName,CString category,CString imgName,bool* ifExist)
{
	CString m_strSql;
	MYSQL_RES *m_result;
	MYSQL_ROW m_sqlRow;
	m_strSql.Format("select count(*) from imageTable where caseName=\'%s\' and imageCategory=\'%s\' and imagePath=\'%s\'",caseName,category,imgName);
	int params = -1;
	if(mysql_real_query(&m_mysql,(char*)(LPCTSTR)m_strSql,(UINT)m_strSql.GetLength())!=0)
	{ 
		print_error(&m_mysql,"error message");
		return false;
	}
	else
	{
		m_result=mysql_store_result(&m_mysql);///<保存查询到的数据到m_result
		if (m_result)
		{
			m_sqlRow=mysql_fetch_row(m_result);///<获得所有结果字符串
			params = atoi(m_sqlRow[0]);
		}
		if(m_result!=NULL) mysql_free_result(m_result);///<释放结果资源
	}
	if(params==0)
	{
		*ifExist = false;
	}
	else
	{
		*ifExist = true;
	}
	return true;

}
///@brief CDataMySql类的删除视频记录的函数
/// 
///删除视频表中某一个记录
///@param[in|out] caseName 记录名;cameraName 监控点名称;originPath 原始视频路径|NULL
///@pre  NULL
///@return NULL
///@retval BOOL 数据库操作是否成功
///@post NULL
bool CDataMySql::DeleteDataFromVideoTable(CString caseName,CString cameraName,CString originPath)
{
	CString m_strSql;
	m_strSql.Format("delete from videoTable where caseName=\'%s\' and cameraName=\'%s\' and originPath=\'%s\'",caseName,cameraName,originPath);
	if(mysql_real_query(&m_mysql,(char*)(LPCTSTR)m_strSql,(UINT)m_strSql.GetLength())!=0)
	{ 
		print_error(&m_mysql,"error message");
		return false;
	}
	else
	{
		return true;
	}

}
///@brief CDataMySql类的删除图片记录的函数
/// 
///删除图片表中某一个记录
///@param[in|out] caseName 记录名;category 记录分类;imagePath 图片路径|NULL
///@pre  NULL
///@return NULL
///@retval BOOL 数据库操作是否成功
///@post NULL
bool CDataMySql::DeleteDataFromImageTable(CString caseName,CString category,CString imagePath)
{
	CString m_strSql;
	m_strSql.Format("delete from imageTable where caseName=\'%s\' and imageCategory=\'%s\' and imagePath=\'%s\'",caseName,category,imagePath);
	if(mysql_real_query(&m_mysql,(char*)(LPCTSTR)m_strSql,(UINT)m_strSql.GetLength())!=0)
	{ 
		print_error(&m_mysql,"error message");
		return false;
	}
	else
	{
		return true;
	}

}
///@brief CDataMySql类的获得当前记录信息的函数
/// 
///在当前记录表中获得当前记录的信息
///@param[in|out] curCaseTable 当前记录表名|caseData 获得的记录信息
///@pre  NULL
///@return NULL
///@retval BOOL 数据库操作是否成功
///@post NULL
bool CDataMySql::GetCurCaseDataFromCurCaseTable(CaseTable* caseData,CString curCaseTable)
{
	CString m_strSql;
	MYSQL_RES *m_result;
	MYSQL_ROW m_sqlRow;
	m_strSql.Format("select caseName,caseCategory,caseRecordTime,caseHappenTime,caseDescription,caseNote\
					from %s",curCaseTable);
	if(mysql_real_query(&m_mysql,(char*)(LPCTSTR)m_strSql,(UINT)m_strSql.GetLength())!=0)
	{ 
		print_error(&m_mysql,"error message");
		return false;
	}
	else
	{
		m_result=mysql_store_result(&m_mysql);///<保存查询到的数据到m_result
		if (m_result)
		{
			if (m_sqlRow=mysql_fetch_row(m_result))///<获得所有结果字符串
			{
				(*caseData).caseName = m_sqlRow[0];
				(*caseData).caseCategory = m_sqlRow[1];
				(*caseData).caseRecordTime = m_sqlRow[2];
				(*caseData).caseHappenTime = m_sqlRow[3];
				(*caseData).caseDescription = m_sqlRow[4];
				(*caseData).caseNote = m_sqlRow[5];
			}
			
		}
		if(m_result!=NULL) mysql_free_result(m_result);///<释放结果资源
		return true;
	}
}

///@brief CDataMySql类的图片信息是否已经存在的函数
/// 
///判断在图片表中是否已经存在某个记录某个图片分类下的图片
///@param[in|out] imgData 图片信息|ifExist 是否已经存在
///@pre  NULL
///@return NULL
///@retval BOOL 数据库操作是否成功
///@post NULL
bool CDataMySql::IfExistImgDataImageTable(ImageTable imgData,bool* ifExist)
{
	CString m_strSql;
	MYSQL_RES *m_result;
	MYSQL_ROW m_sqlRow;
	m_strSql.Format("select count(*) from imageTable where caseName=\'%s\'and imageCategory=\'%s\'and imagePath=\'%s\'",imgData.caseName,imgData.imageCategory,imgData.imagePath);
	int params = -1;
	if(mysql_real_query(&m_mysql,(char*)(LPCTSTR)m_strSql,(UINT)m_strSql.GetLength())!=0)
	{ 
		print_error(&m_mysql,"error message");
		return false;
	}
	else
	{
		m_result=mysql_store_result(&m_mysql);///<保存查询到的数据到m_result
		if (m_result)
		{
			m_sqlRow=mysql_fetch_row(m_result);///<获得所有结果字符串
			params = atoi(m_sqlRow[0]);
		}
		if(m_result!=NULL) mysql_free_result(m_result);///<释放结果资源
	}
	if(params==0)
	{
		*ifExist = false;
	}
	else
	{
		*ifExist = true;
	}
	return true;

}
///@brief CDataMySql类的获得某条线索的线索描述的函数
/// 
///在线索表中根据记录名、监控点名和相关文件路径获得该线索记录的线索描述
///@param[in|out] caseName 记录名;cameraName 监控点名;cluePath 相关联的文件路径|clueDesrip 获得的线索描述
///@pre  NULL
///@return NULL
///@retval BOOL 数据库操作是否成功
///@post NULL
bool CDataMySql::GetClueDescripFromClueTable(CString caseName,CString cameraName,CString cluePath,CString& clueDesrip)
{
	CString m_strSql;
	MYSQL_RES *m_result;
	MYSQL_ROW m_sqlRow;
	m_strSql.Format("select clueDescription from clueTable where caseName=\'%s\' and cameraName=\'%s\' and filePath=\'%s\'",caseName,cameraName,cluePath);
	if(mysql_real_query(&m_mysql,(char*)(LPCTSTR)m_strSql,(UINT)m_strSql.GetLength())!=0)
	{ 
		print_error(&m_mysql,"error message");
		return false;
	}
	else
	{
		m_result=mysql_store_result(&m_mysql);///<保存查询到的数据到m_result
		if (m_result)
		{
			m_sqlRow=mysql_fetch_row(m_result);///<获得所有结果字符串
			if(m_sqlRow==NULL)
			{
				clueDesrip.Empty();
			}
			else
			{
				clueDesrip = m_sqlRow[0];
			}
		}
		if(m_result!=NULL) mysql_free_result(m_result);///<释放结果资源
	}
	return true;

}
///@brief CDataMySql类的更新线索记录的函数
/// 
///在线索表中更新某条线索
///@param[in|out] clueData 要更新的线索|NULL
///@pre  NULL
///@return NULL
///@retval BOOL 数据库操作是否成功
///@post NULL
bool CDataMySql::UpdateClueDataInClueTable(ClueTable clueData)
{
	CString m_strSql;
//	MYSQL_RES *m_result;
	//MYSQL_ROW m_sqlRow;
	m_strSql.Format("update clueTable set clueHappenTime=\'%s\',cameraName=\'%s\',clueDescription=\'%s\', \
					latestModifyTime=\'%s\' where caseName=\'%s\'and cameraName=\'%s\' and filePath=\'%s\'", 
		clueData.clueHappenTime,clueData.cameraName,clueData.clueDescription,clueData.latestModifyTime,\
		clueData.caseName,clueData.cameraName,clueData.filePath);
	if(mysql_real_query(&m_mysql,(char*)(LPCTSTR)m_strSql,(UINT)m_strSql.GetLength())!=0)
	{ 
		print_error(&m_mysql,"error message");
		return false;
	}
	else
	{
		return true;
	}

}
///@brief CDataMySql类的获得所有记录信息的函数
/// 
///在记录表中获得所有记录信息
///@param[in|out] NULL|allCaseData 保存记录信息的vector
///@pre  NULL
///@return NULL
///@retval BOOL 数据库操作是否成功
///@post NULL
bool CDataMySql::GetAllCaseDataFromCaseTable(vector<CaseTable>* allCaseData)
{
	CString m_strSql;
	MYSQL_RES *m_result;
	MYSQL_ROW m_sqlRow;
	allCaseData->clear();
	m_strSql.Format("select caseName,caseCategory,caseRecordTime,caseHappenTime,caseDescription,caseNote from caseTable");

	if(mysql_real_query(&m_mysql,(char*)(LPCTSTR)m_strSql,(UINT)m_strSql.GetLength())!=0)
	{ 
		print_error(&m_mysql,"error message");
		return false;
	}
	else
	{
		m_result=mysql_store_result(&m_mysql);///<保存查询到的数据到m_result
		if (m_result)
		{
			m_sqlRow=mysql_fetch_row(m_result);///<获得所有结果字符串
			CaseTable caseData;
			while(m_sqlRow)
			{
				caseData.caseName = m_sqlRow[0];
				caseData.caseCategory = m_sqlRow[1];
				caseData.caseRecordTime = m_sqlRow[2];
				caseData.caseHappenTime = m_sqlRow[3];
				caseData.caseDescription = m_sqlRow[4];
				caseData.caseNote = m_sqlRow[5];
				allCaseData->push_back(caseData);
				m_sqlRow=mysql_fetch_row(m_result);
			}
		}
		if(m_result!=NULL) mysql_free_result(m_result);///<释放结果资源
	}
	return true;
}
///@brief CDataMySql类的删除某记录的所有信息的函数
/// 
///在记录表、线索表、图片表和视频表中，删除所有关于该记录的记录
///@param[in|out] caseName 记录名称|NULL
///@pre  NULL
///@return NULL
///@retval BOOL 数据库操作是否成功
///@post NULL
bool CDataMySql::DeleteCertainCaseInfor(CString caseName)
{
	CString m_strSql;
	m_strSql.Format("delete from caseTable where caseName=\'%s\'",caseName);
	if(mysql_real_query(&m_mysql,(char*)(LPCTSTR)m_strSql,(UINT)m_strSql.GetLength())!=0)
	{ 
		print_error(&m_mysql,"error message");
		return false;
	}
	m_strSql.Format("delete from clueTable where caseName=\'%s\'",caseName);
	if(mysql_real_query(&m_mysql,(char*)(LPCTSTR)m_strSql,(UINT)m_strSql.GetLength())!=0)
	{ 
		print_error(&m_mysql,"error message");
		return false;
	}
	m_strSql.Format("delete from imageTable where caseName=\'%s\'",caseName);
	if(mysql_real_query(&m_mysql,(char*)(LPCTSTR)m_strSql,(UINT)m_strSql.GetLength())!=0)
	{ 
		print_error(&m_mysql,"error message");
		return false;
	}
	m_strSql.Format("delete from videoTable where caseName=\'%s\'",caseName);
	if(mysql_real_query(&m_mysql,(char*)(LPCTSTR)m_strSql,(UINT)m_strSql.GetLength())!=0)
	{ 
		print_error(&m_mysql,"error message");
		return false;
	}
	return true;
}
///@brief CDataMySql类的获得某记录的图片的总数的函数
/// 
///在图片表中获得某记录下的图片的总数
///@param[in|out] caseName 记录名称|NULL
///@pre  NULL
///@return NULL
///@retval int 返回图片的数目，如果操作失败则返回-1
///@post NULL
int CDataMySql::GetCaseImgNumFromImageTable(CString caseName)
{
	CString m_strSql;
	int params = -1;
	MYSQL_RES *m_result;
	MYSQL_ROW m_sqlRow;
	m_strSql.Format("select count(distinct imagePath) from imageTable where caseName=\'%s\'",caseName);

	if(mysql_real_query(&m_mysql,(char*)(LPCTSTR)m_strSql,(UINT)m_strSql.GetLength())!=0)
	{ 
		print_error(&m_mysql,"error message");
	}
	else
	{
		m_result=mysql_store_result(&m_mysql);///<保存查询到的数据到m_result
		if (m_result)
		{
			m_sqlRow=mysql_fetch_row(m_result);///<获得所有结果字符串
			params = atoi(m_sqlRow[0]);
		}
		if(m_result!=NULL) mysql_free_result(m_result);///<释放结果资源
	}
	return params;
}
///@brief CDataMySql类的获得某记录下的所有视频信息的函数
/// 
///在视频表中获得某记录下的所有视频信息
///@param[in|out] caseName 记录名称|caseAllVideoData 所有视频信息的vector
///@pre  NULL
///@return NULL
///@retval BOOL 返回数据库操作是否成功
///@post NULL
bool CDataMySql::GetCaseVideoDataFromVideoTable(vector<VideoTable>* caseAllVideoData,CString caseName)
{
	CString m_strSql;
	MYSQL_RES *m_result;
	MYSQL_ROW m_sqlRow;
	caseAllVideoData->clear();
	m_strSql.Format("select caseName,cameraName,originPath,segPath,fusionPath from videoTable where caseName=\'%s\'",caseName);

	if(mysql_real_query(&m_mysql,(char*)(LPCTSTR)m_strSql,(UINT)m_strSql.GetLength())!=0)
	{ 
		print_error(&m_mysql,"error message");
		return false;
	}
	else
	{
		m_result=mysql_store_result(&m_mysql);///<保存查询到的数据到m_result
		if (m_result)
		{
			m_sqlRow=mysql_fetch_row(m_result);///<获得所有结果字符串
			VideoTable videoData;
			while(m_sqlRow)
			{
				videoData.caseName = m_sqlRow[0];
				videoData.cameraName = m_sqlRow[1];
				videoData.originPath = m_sqlRow[2];
				videoData.segPath = m_sqlRow[3];
				videoData.fusionPath = m_sqlRow[4];
				caseAllVideoData->push_back(videoData);
				m_sqlRow=mysql_fetch_row(m_result);
			}
		}
		if(m_result!=NULL) mysql_free_result(m_result);///<释放结果资源
	}
	return true;

}
///@brief CDataMySql类的获得某记录下的所有线索信息的函数
/// 
///在线索表中获得某记录下的所有线索信息
///@param[in|out] caseName 记录名称|caseClueData 所有线索信息的vector
///@pre  NULL
///@return NULL
///@retval BOOL 返回数据库操作是否成功
///@post NULL
bool CDataMySql::GetClueDataFromClueTable(vector<ClueTable>* caseClueData,CString caseName)
{
	CString m_strSql;
	MYSQL_RES *m_result;
	MYSQL_ROW m_sqlRow;
	caseClueData->clear();
	m_strSql.Format("select caseName,cameraName,clueHappenTime,clueCategory,latestModifyTime,filePath,clueDescription\
					from clueTable where caseName = \'%s\'",caseName) ; 

	if(mysql_real_query(&m_mysql,(char*)(LPCTSTR)m_strSql,(UINT)m_strSql.GetLength())!=0)
	{ 
		print_error(&m_mysql,"error message");
		return false;
	}
	else
	{
		m_result=mysql_store_result(&m_mysql);///<保存查询到的数据到m_result
		if (m_result)
		{
			m_sqlRow=mysql_fetch_row(m_result);///<获得所有结果字符串
			ClueTable clueData;
			while(m_sqlRow)
			{
				clueData.caseName = m_sqlRow[0];
				clueData.cameraName = m_sqlRow[1];
				clueData.clueHappenTime = m_sqlRow[2];
				clueData.clueCategory = m_sqlRow[3];
				clueData.latestModifyTime = m_sqlRow[4];
				clueData.filePath = m_sqlRow[5];
				clueData.clueDescription = m_sqlRow[6];
				caseClueData->push_back(clueData);
				m_sqlRow=mysql_fetch_row(m_result);
			}
		}
		if(m_result!=NULL) mysql_free_result(m_result);///<释放结果资源
	}
	return true;

}
///@brief CDataMySql类的获得某记录下的所有图片信息的函数
/// 
///在图片表中获得某记录下的所有图片信息
///@param[in|out] caseName 记录名称|caseAllImgData 所有图片信息的vector
///@pre  NULL
///@return NULL
///@retval BOOL 返回数据库操作是否成功
///@post NULL
bool CDataMySql::GetCaseImgDataFromImageTable(vector<ImageTable> *caseAllImgData,CString caseName)
{
	CString m_strSql;
	MYSQL_RES *m_result;
	MYSQL_ROW m_sqlRow;
	caseAllImgData->clear();
	m_strSql.Format("select caseName,imageCategory,imagePath from imageTable where caseName = \'%s\'",caseName) ; 

	if(mysql_real_query(&m_mysql,(char*)(LPCTSTR)m_strSql,(UINT)m_strSql.GetLength())!=0)
	{ 
		print_error(&m_mysql,"error message");
		return false;
	}
	else
	{
		m_result=mysql_store_result(&m_mysql);///<保存查询到的数据到m_result
		if (m_result)
		{
			m_sqlRow=mysql_fetch_row(m_result);///<获得所有结果字符串
			ImageTable imgData;
			while(m_sqlRow)
			{
				imgData.caseName = m_sqlRow[0];
				imgData.imageCategory= m_sqlRow[1];
				imgData.imagePath = m_sqlRow[2];
				caseAllImgData->push_back(imgData);
				m_sqlRow=mysql_fetch_row(m_result);
			}
		}
		if(m_result!=NULL) mysql_free_result(m_result);///<释放结果资源
	}
	return true;

}
///@brief CDataMySql类的将txt文件数据导入表的函数
/// 
///将txt文件数据导入表，文件中数据必须以“,”为列属性分隔符，以"\t\n"即换行符为行分隔符
///@param[in|out] txtPath txt文件路径 in,tableName 要导入数据的表名|NULL
///@pre  NULL
///@return NULL
///@retval BOOL 返回数据库操作是否成功
///@post NULL
bool CDataMySql::LoadTextData2Table(CString txtPath,CString tableName)
{
	CString m_strSql;///<mysql查询语句
	m_strSql.Format("LOAD DATA INFILE '%s' REPLACE INTO TABLE %s FIELDS TERMINATED BY ',' LINES TERMINATED BY '\r\n'",txtPath,tableName);
	if(mysql_real_query(&m_mysql,(char*)(LPCTSTR)m_strSql,(UINT)m_strSql.GetLength())!=0)
	{ 
		print_error(&m_mysql,"error message");
		return false;
	}
	else
	{
		return true;
	}
}
///@brief CDataMySql类的跟踪出错信息的函数
/// 
///
///@param[in|out] conn MYSQL结构 in,message 输出的出错信息 in|NULL
///@pre  NULL
///@return NULL
///@retval NULL
///@post NULL
void CDataMySql::print_error(MYSQL* conn,char* message)
{
	TRACE("%s\n",message);
	if(conn!=NULL)
	{
		TRACE("Error %u(%s):%s\n",mysql_errno(conn),mysql_sqlstate(conn),mysql_error(conn));
	}

}
///@brief CDataMySql类的将表数据保存到txt文件的函数
/// 
///将表数据保存到txt文件，生成文件中的数据以“,”为列属性分隔符，以"\t\n"即换行符为行分隔符
///@param[in|out] txtPath txt文件路径 in,tableName 要导入数据的表名|NULL
///@pre  NULL
///@return NULL
///@retval BOOL 返回数据库操作是否成功
///@post NULL
bool CDataMySql::StoreTableData2Text(CString txtPath,CString tableName)
{
	CString m_strSql;///<mysql查询语句
	m_strSql.Format("SELECT * INTO OUTFILE '%s' FIELDS TERMINATED BY ',' LINES TERMINATED BY '\r\n' FROM %s ",txtPath,tableName);
	if(mysql_real_query(&m_mysql,(char*)(LPCTSTR)m_strSql,(UINT)m_strSql.GetLength())!=0)
	{ 
		print_error(&m_mysql,"error message");
		return false;
	}
	else
	{
		return true;
	}
}

bool CDataMySql::FindTimePositionFromNewTraceTable(int NewFrame,vector <CvPoint> *m_Point,vector<int> *origFrame,CString tableName)
{
	CString m_strSql;
	MYSQL_RES *m_result;
	MYSQL_ROW m_sqlRow;
	m_strSql.Format("select nX,nY,origFrame from %s where newFrame = \'%d\'",tableName,NewFrame) ;
	if(mysql_real_query(&m_mysql,(char*)(LPCTSTR)m_strSql,(UINT)m_strSql.GetLength())!=0)
	{ 
		m_result = NULL;///<出错
		print_error(&m_mysql,"error message");
		return false;
	}
	else
	{
		m_result=mysql_store_result(&m_mysql);///<保存查询到的数据到m_result
		for (int i=0;i<m_result->row_count;i++)
		{
			m_sqlRow=mysql_fetch_row(m_result);///<获得所有结果字符串
			CvPoint p;
			int f;
			p.x = atoi(m_sqlRow[0]);
			p.y = atoi(m_sqlRow[1]);
			f = atoi(m_sqlRow[2]);
			m_Point->push_back(p);
			origFrame->push_back(f);
		}
		if(m_result!=NULL)mysql_free_result(m_result);///<释放结果资源
		return true;
	}
}

int CDataMySql::FindOrigParaFromFGTraceTable(CString FGTraceTable, CvRect rect)
{

	CString m_strSql;
	m_strSql.Format("select nOldPara from %s where nX =\'%d\' and nWidth = \'%d\'", FGTraceTable, rect.x, rect.width);
	MYSQL_RES *m_result;
	MYSQL_ROW m_sqlRow;
	int params = -1;
	if (mysql_real_query(&m_mysql, (char*)(LPCTSTR)m_strSql, (UINT)m_strSql.GetLength()) != 0)
	{
		print_error(&m_mysql, "error message");
	}
	else
	{
		m_result = mysql_store_result(&m_mysql);///<保存查询到的数据到m_result
		if (m_result)
		{
			m_sqlRow = mysql_fetch_row(m_result);///<获得所有结果字符串
			params = atoi(m_sqlRow[0]);
		}
		if (m_result != NULL) mysql_free_result(m_result);///<释放结果资源
	}
	return params;
}

int CDataMySql::FindSegIDFromFGTraceTable(CString FGTraceTable,int origPara)	                            ///<从new trace表中根据ROI信息找到原始视频帧数
{
	CString m_strSql;
	m_strSql.Format("select segID from %s where nOldPara =\'%d\' ", FGTraceTable, origPara);
	MYSQL_RES *m_result;
	MYSQL_ROW m_sqlRow;
	int segIDnum = -1;
	if(mysql_real_query(&m_mysql,(char*)(LPCTSTR)m_strSql,(UINT)m_strSql.GetLength())!=0)
	{ 
		print_error(&m_mysql,"error message");
	}
	else
	{
		m_result=mysql_store_result(&m_mysql);///<保存查询到的数据到m_result
		if (m_result)
		{
			m_sqlRow=mysql_fetch_row(m_result);///<获得所有结果字符串
			segIDnum = atoi(m_sqlRow[0]);
		}
		if(m_result!=NULL) mysql_free_result(m_result);///<释放结果资源
	}
	return segIDnum;
}

int CDataMySql::FindOrigFrameFromVideoFGTable(CString VideoFGTable, int origPara)
{

	CString m_strSql;
	m_strSql.Format("select startFrame from %s where nOldPara =\'%d\' ", VideoFGTable, origPara);
	MYSQL_RES *m_result;
	MYSQL_ROW m_sqlRow;
	int frameNO = -1;
	if(mysql_real_query(&m_mysql,(char*)(LPCTSTR)m_strSql,(UINT)m_strSql.GetLength())!=0)
	{ 
		print_error(&m_mysql,"error message");
	}
	else
	{
		m_result=mysql_store_result(&m_mysql);///<保存查询到的数据到m_result
		if (m_result)
		{
			m_sqlRow=mysql_fetch_row(m_result);///<获得所有结果字符串
			frameNO = atoi(m_sqlRow[0]);
		}
		if(m_result!=NULL) mysql_free_result(m_result);///<释放结果资源
	}
	return frameNO;
}