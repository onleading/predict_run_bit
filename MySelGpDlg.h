#if !defined(AFX_MYSELGP_H__64359DED_8CBB_4B6C_92C6_3279506E7093__INCLUDED_)
#define AFX_MYSELGP_H__64359DED_8CBB_4B6C_92C6_3279506E7093__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MySelGp.h : header file
//
#pragma warning(disable:4786)
#pragma warning(disable:4788)
#include <map>
using namespace std;

#include "DescFormDlg.h"
/////////////////////////////////////////////////////////////////////////////
// CMySelGpDlg dialog
#include "Interface.h"
#include "Label.h"
#include "GradientProgressCtrl.h"

typedef struct
{
	BYTE		nType;
	char		name[20];
	HTREEITEM	htm;
}typeinfo;

extern typeinfo TreeTypeInfo[15];
extern typeinfo TreeTJInfo[6];

class   MyStrategy;
struct  SingleCon;

class CMySelGpDlg : public CDialog
{
	void SetDomainStr();
	void ParsStyle(int &nStyleType, int &nStyleIndex);
	void FillTree(int nTab);
	void FillConInfo();
	void ShowTypeInfo();
	void ShowConInfo();
	void RefreshOther();
	void DisableOther();
	void EnableOther();
	void CarryoutToBlock(int WhichBlock);
	void DynaShow(void);
// Construction
public:
	CMySelGpDlg(CWnd* pParent = NULL);   // standard constructor
	~CMySelGpDlg();

// Dialog Data
	//{{AFX_DATA(CMySelGpDlg)
	enum { IDD = IDD_SELGP_DIALOG };
	CTabCtrl	m_tabctrl;
	CLabel	m_status;
	CButton	m_outStatic;
	CStatic	m_selectedNum;
	CComboBox	m_CalcCycle_Combo;
	CGradientProgressCtrl	m_Carry_Progress;
	CTreeCtrl	m_zbTree;
	CStatic	m_totalGpNum;
	CComboBox	m_calcProc;
	CComboBox	m_mainLine;
	CListBox	m_Condition_List;
	CTime	m_endTime;
	CString	m_otherLine;
	int		m_isAndRadio;
	CString	m_dynaStatic;
	BOOL	m_bTQ;
	//}}AFX_DATA
	CTime   m_beginTime;

	FILEHEAD FileHead;

	struct CONDITIONINFO
	{
		char acCode[ZBCODE_LEN];	//ָ��Ӣ������
		int  ParamNum;
		int  ParamValue[MAX_PARAMNUM];
		int  CalcCycle;				//�������� 0:5������ 1:15���� ...
	}ConditionInfo[MAX_TJCOMB_NUM];	//�������MAX_TJCOMB_NUM�����������

	TINDEXINFO	*pIndex[MAX_TJCOMB_NUM];
	int ConditionNum;

	short  awUsedStkp[MAXGPNUM];	//��Ź�Ʊ��ŵ�����
	short  SelectedStk[MAXGPNUM];   //�ɹ��ҵ��Ĺ�Ʊ�������
private:
	CImageList m_ImageList;
	BOOL bRadioEnable;	//������EnableOther ��DisableOther ����������
	BOOL CanClearList; //�ж��´�ѡ������ѡ�ɹ�ʽʱ�Ƿ�Ӧ�����ConditionList

	struct GpDomain_Struct m_GpDomain[MAX_DOMAINNUM];
	int    m_nSelDomain;

	short m_nCurType;                   //ָ������
	short m_nCurIndex;                  //ָ������

	BOOL  m_bSort;                      //�Ƿ�Ϊ����ѡ��

	BOOL IsCarrying;
	BOOL IsCarried;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMySelGpDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CMySelGpDlg)
	afx_msg void OnAddcondition();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnCarryout();
	afx_msg void OnCarryoutBlock();
	afx_msg void OnChangedomainButton();
	afx_msg void OnDeletecondition();
	afx_msg void OnIndexInfomation();
	virtual BOOL OnInitDialog();
	afx_msg void OnLoadcondition();
	afx_msg void OnSavecondition();
	afx_msg void OnSelchangeCalccycleCombo();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnSelchangedZbtree(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSelchangeProc();
	afx_msg void OnSelchangeMainline();
	afx_msg void OnSelchangeTab(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:

	MyStrategy *m_pMyStrategy; 
	SingleCon  *m_pCurSiCon;

	TINDEXINFO *m_pCurIndexInfo;
	CDescFormDlg m_Form;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MYSELGP_H__64359DED_8CBB_4B6C_92C6_3279506E7093__INCLUDED_)
