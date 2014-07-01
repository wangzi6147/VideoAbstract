/**
*@file GlobalFunctions.cpp
*@brief 工程中一些用的多的通用函数的实现
*
*定义由绝对路径得到名称，获取“\”或者“.”前字符，数据库调用并显示在combox等操作的行为
*
*@author 作者 zhang nannan ,包含email等
*@version 版本号(0.0)
*@data 2012-8-21
*/
#pragma once

#include <string.h>
#include "LibMySql.h"
extern CDataMySql* m_MysqlHandle;

void GetVideoNameFromAbsolutePath(CString* absolutePath,CString* videoName); ///<由绝对路径得到文件名（包含后缀）
void GetVideoNameFromAbsolutePath1(CString* absolutePath,CString* videoName); 
void GetFileNameBeforeDot(CString* absolutePath,CString* fileName);         ///<得到字符串最右边的“.”前的文件名（即包含路径不包含后缀）
void GetFileNameBeforeSlider(CString* absolutePath,CString* fileName);      ///<得到字符串最右边的“/”左侧的子字符串
void ShowCaseDroplist(CComboBox* droplistBox);                              ///<在droplist中显示案件名
void ShowCameraDroplist(CString* caseName,CComboBox* droplistBox);          ///<在droplist中显示监控点名
void ShowCategoryDroplist(CString* caseName,CComboBox* droplistBox);        ///<在droplist中显示图片类别
