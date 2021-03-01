#if !defined(AFX_CONDITIONGPDLG_H__8AB8C7C1_69E1_11D3_B921_0000800104D5__INCLUDED_)
#define AFX_CONDITIONGPDLG_H__8AB8C7C1_69E1_11D3_B921_0000800104D5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ConditionGPDlg.h : header file
//

#include "DescFormDlg.h"
/////////////////////////////////////////////////////////////////////////////
// CConditionGPDlg dialog

#include  "Interface.h"
#include "Label.h"
#include "GradientProgressCtrl.h"

class CConditionGPDlg : public CDialog
{
	void SetDomainStr();
	void DisableOther();
	void EnableOther();
	void CarryoutToBlock(int WhichBlock);
public:
	CConditionGPDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CConditionGPDlg)
	enum { IDD = IDD_CHOICEGP_DIALOG };
	CButton	m_OutStatic;
	CStatic	m_SelectedNum;
	CStatic	m_TotalGPNum;
	CLabel	m_CStatus;
	CGradientProgressCtrl	m_Carry_Progress;
	CListBox	m_Condition_List;
	CComboBox	m_ChoiceGPIndex_Combo;
	CComboBox	m_CalcCycle_Combo;
	int		m_IsAndRadio;
	BOOL	m_bTQ;
	CTime	m_BeginTime;
	CTime	m_EndTime;
	BOOL	m_bDynaK;
	BOOL	m_bQJ;
	//}}AFX_DATA

	int CurrentIndex;

	FILEHEAD FileHead;

	struct CONDITIONINFO
	{
		char acCode[ZBCODE_LEN];//ָ��Ӣ������
		int  ParamNum;
		int  ParamValue[MAX_PARAMNUM];
		int  CalcCycle;				//�������� 0:5������ 1:15���� ...
	}ConditionInfo[MAX_TJCOMB_NUM];	//�������MAX_TJCOMB_NUM�����������

	TINDEXINFO	*pIndex[MAX_TJCOMB_NUM];
	int ConditionNum;

	short  awUsedStkp[MAXGPNUM];	//��Ź�Ʊ��ŵ�����
	short  SelectedStk[MAXGPNUM];   //�ɹ��ҵ��Ĺ�Ʊ�������
private:
	BOOL bRadioEnable;	//������EnableOther ��DisableOther ����������
	BOOL CanClearList; //�ж��´�ѡ������ѡ�ɹ�ʽʱ�Ƿ�Ӧ�����ConditionList

	struct GpDomain_Struct m_GpDomain[MAX_DOMAINNUM];
	int    m_nSelDomain;

	BOOL IsCarrying;
	BOOL IsCarried;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CConditionGPDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CConditionGPDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnIndexInfomation();
	afx_msg void OnAddcondition();
	afx_msg void OnDeletecondition();
	afx_msg void OnCarryout();
	afx_msg void OnSavecondition();
	afx_msg void OnLoadcondition();
	afx_msg void OnSelchangeChoicegpindexCombo();
	afx_msg void OnCarryoutBlock();
	afx_msg void OnChangedomainButton();
	afx_msg void OnSelchangeCalccycleCombo();
	afx_msg void OnQjCheck();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	CDescFormDlg m_Form;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CONDITIONGPDLG_H__8AB8C7C1_69E1_11D3_B921_0000800104D5__INCLUDED_)
