#if !defined(AFX_PREVIEWUNITDlg_H__36949044_C475_11D3_8B1F_00E04C66B9E1__INCLUDED_)
#define AFX_PREVIEWUNIT_H__36949044_C475_11D3_8B1F_00E04C66B9E1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "CaptionButton.h"
#include "UZhSort.h"
#include "UnitView.h"
#include "PopPreViewTool.h"

class CPreviewUnitDlg : public CWnd
{
public:
	CPreviewUnitDlg();   // standard constructor
	~CPreviewUnitDlg();

	void CreateUnit(int which,int initGPNo,int type,int style=0);

	int		GetMainUnitType()
	{
		if(m_Units.Num>0&&m_Units.pUnit[0])
			return m_Units.Type[0];
		return -1;
	}
	void	SetToolTarget(CWnd *pOwner, CPopPreViewTool *pPreviewTool)
	{
		m_pOwnerWnd = pOwner;
		m_pPreviewTool = pPreviewTool;
	}

// Dialog Data
	//{{AFX_DATA(CPreviewUnitDlg)
	enum { IDD = IDD_ZHSORT_DIALOG };
	//}}AFX_DATA
	
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPreviewUnitDlg)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

private:
	CPopPreViewTool *m_pPreviewTool;
	CWnd	*m_pOwnerWnd;
	CRect	m_Rect;			//画区域
	UNITS	m_Units;		//存放各Units信息的结构
	//远程使用
	short	m_nMainID;
	void	SetMainID();
// Implementation
protected:
	BOOL Create(CWnd* pParentWnd);
	void ResizePad(int flag=COM_OnlyDraw);
	void OnGpVip(int iIndex, BOOL bZST=TRUE);
	int	 HitTest(CPoint point);
	BOOL ChangeUnitGP(UBase *pUnit,int iNo);

	void OnGetDataAck(WPARAM wParam,LPARAM lParam);
	void GetMainID(WPARAM wParam,LPARAM lParam);	//采用消息的方式，直接填写g_nMainID
	// Generated message map functions
	//{{AFX_MSG(CHisDlg)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnClose();
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT nIDEvent);

	afx_msg void OnChangeZhSortSC(UINT nID);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif
