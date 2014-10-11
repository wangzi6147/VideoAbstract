/**
*@file ButtonBeautify.h
*@brief 主要为使用按钮美化的调用功能类的声明。
*
*此文件定义的CButtonBeautify类，其功能上主要完成按钮的美化。
*
*@author 作者 shen wei
*@version 版本号(0.0)
*@data 2012-5-23
*/
#if !defined(AFX_MYBUTTON_H__1B425AEB_661D_426C_998C_37C85EF445D5__INCLUDED_)
#define AFX_MYBUTTON_H__1B425AEB_661D_426C_998C_37C85EF445D5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
///@brief 定义的界面美化类
///
///UIbeautify，此类功能上功能上主要完成按钮的美化
///
///@invariant 
///
class CButtonBeautify : public CButton
{

public:
	CButtonBeautify();
	virtual ~CButtonBeautify();
	void SetBitmaps(CBitmap &nNormal, CBitmap &nSelect, CBitmap &nFocuse);///<设置图象或绘图操作应用的效果
	void DrawNormalState(CRect rc, CDC *pDC);	///<图象或绘图操作应用m_bmNormal的效果
	void DrawSelectedState(CRect rc, CDC *pDC);	///<图象或绘图操作应用m_bmSelect的效果
	void DrawFocusState(CRect rc, CDC *pDC);	///<图象或绘图操作应用m_bmFocuse的效果
	void DrawFace(CRect& rc, COLORREF clrTopRight, COLORREF clrBottomLeft,
		CDC* pDC);
	void DrawFrame(CRect& rc, COLORREF clrTopRight, COLORREF clrBottomLeft,
		CDC *pDC);///<设置图图片的样式
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);///<重载DrawItem可以绘制自己需要的控件外观
	virtual void PreSubclassWindow();						 ///<重载这个函数以完成属性值的设置

public:
	BOOL	m_bIsMouseMove;
	CBitmap m_bmNormal;
	CBitmap m_bmSelect;
	CBitmap m_bmFocuse;

protected:
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);		///<鼠标指针移动按钮时，触发该消息函数
	afx_msg LRESULT OnMouseLeave(WPARAM wParam, LPARAM lParam); ///<鼠标指针离开按钮时，触发该消息函数
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

///{{AFX_INSERT_LOCATION}}
/// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif ///< !defined(AFX_MYBUTTON_H__1B425AEB_661D_426C_998C_37C85EF445D5__INCLUDED_)
