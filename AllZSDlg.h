#if !defined(AFX_ALLZSDLG_H__3C3CE9C6_0646_11D5_B99A_00E04C3DCAC5__INCLUDED_)
#define AFX_ALLZSDLG_H__3C3CE9C6_0646_11D5_B99A_00E04C3DCAC5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AllZSDlg.h : header file
//

#include "DescFormDlg.h"
#include "DragListCtrl.h"

/////////////////////////////////////////////////////////////////////////////
// CAllZSDlg dialog

class CAllZSDlg : public CDialog
{
// Construction
public:
	CAllZSDlg(CWnd* pParent = NULL);   // standard constructor
	//bIsOverlapΪTRUE��ʾӵ��OnlyMain��־,����������������ͼ����,ΪFALSE���ʾӵ��MainAble����
	void Init(int nSet, int nPeriod, LPCTSTR text, BOOL bIsOverlap = TRUE);
	void FillTree();
	long	m_lPos;				//��ǰָ������Ӧ�����е�λ��
	BOOL	m_bIsSpecialLine;	//��ǰ���������⻭��
	float   m_curParams[MAX_PARAMNUM];
// Dialog Data
	//{{AFX_DATA(CAllZSDlg)
	enum { IDD = IDD_ALLZSDLG };
	CButton	m_desbt;
	CTreeCtrl	m_tree;
	CStatic	m_OutStatic;
//	CDragListCtrl	m_List;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAllZSDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CAllZSDlg)
	virtual BOOL OnInitDialog();
//	afx_msg void OnDblclkList1(NMHDR* pNMHDR, LRESULT* pResult);
	virtual void OnOK();
//	afx_msg void OnItemchangedList1(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDescbtn();
	afx_msg void OnDblclkTree1(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSelchangedTree1(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	int		m_nSet;
	int		m_nPeriod;
	BOOL	m_bOverlap;	//�Ƿ�ֻ��ʾ��ͼ���ӵ�ָ�깫ʽ
	CDescFormDlg m_Form;
	CImageList m_ImageList;
	LPCTSTR m_lpszCurrActive;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ALLZSDLG_H__3C3CE9C6_0646_11D5_B99A_00E04C3DCAC5__INCLUDED_)
