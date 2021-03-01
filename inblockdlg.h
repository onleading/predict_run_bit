#if !defined(AFX_INBLOCKDLG_H__2399D21C_CD91_44F6_84EC_B53516EB610E__INCLUDED_)
#define AFX_INBLOCKDLG_H__2399D21C_CD91_44F6_84EC_B53516EB610E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// inblockdlg.h : header file
//
#include "HQGrid.h"
#include "Label.h"
/////////////////////////////////////////////////////////////////////////////
// CInBlockDlg dialog
typedef struct  
{
	int		nGroup;
	char	strGroupName[50];
	short	nRow;
	int		nItem;
	char	strItemName[50];
}INFYINFO;

class CInBlockDlg : public CDialog
{
	void InitList();
	void StartSearch();
	BOOL ParseHeadInfo(ColHeadInfo &colinfo,char *colstr);
	void FindInFyGroup(int nGroup);

	INBLOCKINFO m_BlockInf[TOTAL_HY_NUM+TOTAL_DY_NUM+200];
	INFYINFO m_FyInf[MAX_FY_GROUP*MAX_FYNUM_IN_GROUP];
public:
	CInBlockDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CInBlockDlg)
	enum { IDD = IDD_INBLOCK_DIALOG };
	CLabel		m_Info_Static;
	CListCtrl	m_Block_List2;
	CListCtrl	m_Block_List;
	BOOL		m_bWithGP;
	//}}AFX_DATA
	CImageList	m_ImageList;
	int			m_nCurGP;

	int			m_nCurSelBlock;
	int			m_nBlockNum;
	int			m_nCurSelFy;
	int			m_nFyNum;

	BOOL		m_bFindInFY;
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CInBlockDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CInBlockDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void SearchBlock(WPARAM wParam,LPARAM lParam);
	afx_msg void OnDblclkBlockList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnWithgpCheck();
	afx_msg void OnFindinfy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_INBLOCKDLG_H__2399D21C_CD91_44F6_84EC_B53516EB610E__INCLUDED_)
