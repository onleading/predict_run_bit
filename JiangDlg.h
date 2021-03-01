/*
* Copyright (c) 2002,�人ͨ���ſ�����
* All rights reserved.
* 
* �ļ����ƣ�JiangDlg.h
* �ļ���ʶ��
* ժ    Ҫ����ɽ���ͼ����
*/

#if !defined(AFX_JIANGDLG_H__7C8410E3_02EC_483F_B7A5_A014B28FC4CB__INCLUDED_)
#define AFX_JIANGDLG_H__7C8410E3_02EC_483F_B7A5_A014B28FC4CB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// JiangDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CJiangDlg dialog
#include "resource.h"

struct	JIANGEN
{
	float		fVal;
	float		fBL;
};

class CJiangDlg : public CDialog
{
// Construction
public:
	CJiangDlg(CWnd* pParent = NULL);   // standard constructor

	short		m_nCalcMode;	//����ģʽ
	short		m_nMode;		//����ģʽ0��1
	float		m_fStep;		//ÿ���ڶ�Ӧ�۸��ݶ�
	float		m_fStartP;		//��ʼ�۸�
	long		m_nStartT;		//��ʼ����
	JIANGEN		m_aJiang[81];	//������
	CRect		m_aRect[10][10];
	char		m_aTxt[10][10][8];
	POINT		m_PtIn;			//ѡ��
// Dialog Data
	//{{AFX_DATA(CJiangDlg)
	enum { IDD = IDD_JIANG };
	CComboBox	m_ctlCb;
	float		m_fNowVal;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CJiangDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void	CalcIt();
	void	DrawGrid(CDC * pDC);
	void	GetTxt();
	// Generated message map functions
	//{{AFX_MSG(CJiangDlg)
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnPaint();
	virtual BOOL OnInitDialog();
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnSelchangeCombo();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_JIANGDLG_H__7C8410E3_02EC_483F_B7A5_A014B28FC4CB__INCLUDED_)
