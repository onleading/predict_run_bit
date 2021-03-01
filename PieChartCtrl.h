#if !defined(AFX_PIECHARTCTRL_H__31DF2BE2_22B5_11D2_813B_A9FDC8C0FA85__INCLUDED_)
#define AFX_PIECHARTCTRL_H__31DF2BE2_22B5_11D2_813B_A9FDC8C0FA85__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

// PieChartCtrl.h : header file
//
// Written by Yuheng Zhao (yuheng@ministars.com)
// Copyright (c) 1998.
//
// This code may be used in compiled form in any way you desire. This
// file may be redistributed unmodified by any means PROVIDING it is 
// not sold for profit without the authors written consent, and 
// providing that this notice and the authors name is included. If 
// the source code in  this file is used in any commercial application 
// then a simple email would be nice.
//
// This file is provided "as is" with no expressed or implied warranty.
// The author accepts no liability if it causes any damage whatsoever.
// It's free - so you get what you pay for.

#include <afxtempl.h>
// PieChartCtrl.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPieChartCtrl window
class CPieChartPiece : public CObject
{
public:
	CPieChartPiece() {;}
	~CPieChartPiece() {if (m_brushBack.m_hObject) m_brushBack.DeleteObject();}

	COLORREF m_colorBack;
	COLORREF m_colorText;
	CBrush m_brushBack;	
	int m_nAngle;
	CString m_strInfo;
};

typedef CTypedPtrArray <CObArray, CPieChartPiece*> CChartPieceArray;

class CPieChartCtrl : public CWnd
{
// Construction
public:
	CPieChartCtrl();

// Attributes
public:
	int m_nStartAngle; // 0-359
	COLORREF m_colorDefault;
	COLORREF m_colorLine;

	CChartPieceArray m_chartPieces;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPieChartCtrl)
	//}}AFX_VIRTUAL

// Implementation
public:
	int m_nTitleHeight;
	CFont m_fontTitle;
	CFont m_fontInfo;
	CString m_strTitle;
	CPoint m_ptOldPt;
	BOOL m_bCaptured;
	CRect m_rectChart;

	void SetTitle(const CString& str);
	void SetBackColor(COLORREF rgbBack);
	void SetBackDefaut(COLORREF rgbdefaut);
	void GetItemColor(int i, COLORREF& color);
	void Reset();
	void CountPoint(int nAngle, CPoint& pt, BOOL bPercent = FALSE);
	BOOL AddPiece(COLORREF colorBack, COLORREF colorText,
		int nAngle, const CString& str = _T(""));
	void RecalcRect();
	static BOOL RegisterWndClass(HINSTANCE hInstance);
	virtual ~CPieChartCtrl();
	void CountPoint1(int nAngle, CPoint& pt, CRect rect);
	void SetSelItem(int nSel);
	// Generated message map functions
protected:
	//{{AFX_MSG(CPieChartCtrl)
	afx_msg void OnPaint();
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	int m_nSel;
	COLORREF m_rgbback; 
};

/////////////////////////////////////////////////////////////////////////////


//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PIECHARTCTRL_H__31DF2BE2_22B5_11D2_813B_A9FDC8C0FA85__INCLUDED_)
