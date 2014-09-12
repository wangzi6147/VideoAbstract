/**
*@file GlobalFunctions.cpp
*@brief 工程中一些通用函数的实现
*
*定义由绝对路径得到名称，获取“\”或者“.”前字符，数据库调用并显示在combox等操作的行为
*
*@author 作者 zhang nannan ,包含email等
*@version 版本号(0.0)
*@data 2012-8-21
*/
#include "stdafx.h"
#include <afxwin.h>
#include <afxmt.h>
#include "GlobalFunctions.h"

///@brief 由绝对路径得到文件名（包含后缀）
///
///@param[in|out] absolutePath 文件绝对路径|videoName 去掉路径的文件名（包含后缀）
///@pre  NULL
///@return NULL
///@retval NULL
///@post NULL
void GetVideoNameFromAbsolutePath(CString* absolutePath,CString* videoName)
{
	const char* m_constVideoText = NULL;
	char * m_charVideoText =(char*) malloc(255*sizeof(char));
	char *p = NULL;
	char *pre_p = NULL;
	int index = 0;

	m_constVideoText = *absolutePath;

	strcpy(m_charVideoText,m_constVideoText);
	p = strtok(m_charVideoText,"/");
	
	//p = strtok(m_charVideoText,"\\");
	while(p)
	{
		pre_p = p;
		p = strtok(NULL,"/");
		//p = strtok(NULL,"\\");
		if(!p)
		{
			*videoName = pre_p;
		}
	}
	free(m_charVideoText);
}
void GetVideoNameFromAbsolutePath1(CString* absolutePath,CString* videoName)
{
	const char* m_constVideoText = NULL;
	char * m_charVideoText =(char*) malloc(255*sizeof(char));
	char *p = NULL;
	char *pre_p = NULL;
	int index = 0;

	m_constVideoText = *absolutePath;

	strcpy(m_charVideoText,m_constVideoText);
	//p = strtok(m_charVideoText,"/");

	p = strtok(m_charVideoText,"\\");
	while(p)
	{
		pre_p = p;
		//p = strtok(NULL,"/");
		p = strtok(NULL,"\\");
		if(!p)
		{
			*videoName = pre_p;
		}
	}
	free(m_charVideoText);
}
///@brief 得到字符串最右边的“.”前的文件名（即包含路径不包含后缀）
///
///@param[in|out] absolutePath 文件绝对路径|fileName 去掉后缀包含路径的文件名
///@pre  NULL
///@return NULL
///@retval NULL
///@post NULL
void GetFileNameBeforeDot(CString* absolutePath,CString* fileName)
{
	int dotNum = 0;
	for (int i=0;i<absolutePath->GetLength();i++)
	{
		dotNum = absolutePath->Find(".",i); 
		if (dotNum==-1)
		{
			dotNum=i;
			break;
		}
		else
		{
			i=dotNum;
		}
	}
	
	*fileName = absolutePath->Left(dotNum-1);
}
///@brief 得到字符串最右边的“/”左侧的子字符串
///
///@param[in|out] absolutePath 输入的字符串|fileName 获得的子字符串
///@pre  NULL
///@return NULL
///@retval NULL
///@post NULL
void GetFileNameBeforeSlider(CString* absolutePath,CString* fileName)
{
	int dotNum = 0;
	for (int i=0;i<absolutePath->GetLength();i++)
	{
		dotNum = absolutePath->Find("/",i); 
		if (dotNum==-1)
		{
			dotNum=i;
			break;
		}
		else
		{
			i=dotNum;
		}
	}

	*fileName = absolutePath->Left(dotNum-1);
}
///@brief 在droplist中显示案件名
///
///在数据库中查找所有的案件名，在combox中insert来显示在droplist中
///@param[in|out] droplistBox 列表框|NULL
///@pre  NULL
///@return NULL
///@retval NULL
///@post NULL
void ShowCaseDroplist(CComboBox* droplistBox)
{
	vector<CString> caseName;
	if(!m_MysqlHandle->GetAllCaseNameFromCaseTable(&caseName))
	{
		AfxMessageBox("数据库出错");
		return;
	}
	else
	{
		if(!caseName.empty())
		{
			CString itemNo;
			CString videoName;
			for(vector<CString>::size_type i=0;i!= caseName.size();i++)
			{
				(*droplistBox).AddString(caseName.at(i));
			}
		}
	}
	caseName.swap(vector<CString>());
	
}
///@brief 在droplist中显示监控点名
///
///在数据库中查找某案件下所有的监控点，在combox中insert来显示在droplist中
///@param[in|out] droplistBox 列表框;caseName 案件名|NULL
///@pre  NULL
///@return NULL
///@retval NULL
///@post NULL
void ShowCameraDroplist(CString* caseName,CComboBox* droplistBox)
{
	droplistBox->ResetContent();
	vector<CString> cameraName;
	if(!m_MysqlHandle->GetMoniorNameFromVideoTable(*caseName,&cameraName))
	{
		AfxMessageBox("数据库出错");
		return;
	}
	else
	{
		if(!cameraName.empty())
		{
			CString itemNo;
			CString videoName;
			for(vector<CString>::size_type i=0;i!= cameraName.size();i++)
			{
				(*droplistBox).AddString(cameraName.at(i));
			}
		}
	}
	cameraName.swap(vector<CString>());
}
///@brief 在droplist中显示图片类别
///
///在数据库中查找某案件下所有的图片类别，在combox中insert来显示在droplist中
///@param[in|out] droplistBox 列表框;caseName 案件名|NULL
///@pre  NULL
///@return NULL
///@retval NULL
///@post NULL
void ShowCategoryDroplist(CString* caseName,CComboBox* droplistBox)
{
	droplistBox->ResetContent();
	vector<CString> categoryName;
	if(!m_MysqlHandle->GetCategoryNameFromImageTable(*caseName,&categoryName))
	{
		AfxMessageBox("数据库出错");
		return;
	}
	else
	{
		if(!categoryName.empty())
		{
			CString itemNo;
			CString videoName;
			for(vector<CString>::size_type i=0;i!= categoryName.size();i++)
			{
				(*droplistBox).AddString(categoryName.at(i));
			}
		}
	}
	categoryName.swap(vector<CString>());
}

///@brief 字符串转换函数
///
///CString、string、char*之间的相互转换
///@param[in|out] CString、string、char*、标志位
///@pre  NULL
///@return NULL
///@retval NULL
///@post NULL
void MyStringConversion(CString *mycstring, string *mystring, char *mychar,int flag)
{
	///<flag = 1, CString2string
	///<flag = 2, string2CString
	///<flag = 3, CString2char
	///<flag = 4, char2CString
	///<flag = 5, string2char
	///<flag = 6, char2string
	switch(flag)
	{
	case 1:
		*mystring = mycstring->GetBuffer(mycstring->GetLength());
		mycstring->ReleaseBuffer();
		break;
	case 2:
		*mycstring = mystring->c_str();
		break;
	case 3:
		mychar =(LPSTR)(LPCTSTR)(*mycstring);
		break;
	case 4:
		mycstring->Format("%s", *mychar); 
		break;
	case 5: 
		mychar = const_cast<char*>(mystring->c_str());
		break;
	case 6:
		*mystring = mychar;
	}
}