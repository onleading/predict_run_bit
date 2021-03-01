// EditConDlg.cpp : implementation file
#include "stdafx.h"
#include "tdxw.h"
#include "EditConDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEditConDlg dialog


CEditConDlg::CEditConDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CEditConDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEditConDlg)
	m_fValue = 0.0f;
	m_bConCheck = FALSE;
	m_nValue = 0;
	//}}AFX_DATA_INIT
	bAdd = TRUE;
}

void CEditConDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEditConDlg)
	DDX_Control(pDX, IDC_UNIT_STATIC, m_Unit_Static);
	DDX_Control(pDX, IDC_TOPIC_STATIC, m_Topic_Static);
	DDX_Control(pDX, IDC_SEL_COMBO, m_Sel_Combo);
	DDX_Control(pDX, IDC_PREFIX_COMBO, m_Prefix_Combo);
	DDX_Text(pDX, IDC_FVALUE_EDIT, m_fValue);
	DDX_Check(pDX, IDC_CON_CHECK, m_bConCheck);
	DDX_Text(pDX, IDC_NVALUE_EDIT, m_nValue);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CEditConDlg, CDialog)
	//{{AFX_MSG_MAP(CEditConDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEditConDlg message handlers

BOOL CEditConDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	m_Topic_Static.SetTextColor(RGB(0,0,255));

	m_Prefix_Combo.SetItemHeight(0,16);
	m_Prefix_Combo.SetItemHeight(-1,16);
	m_Sel_Combo.SetItemHeight(0,16);
	m_Sel_Combo.SetItemHeight(-1,16);

	if(bAdd)
	{
		SetWindowText(_F("添加定制条件"));
		SetDlgItemText(IDOK,_F("加入条件"));
	}
	else
	{
		SetWindowText(_F("修改定制条件"));
		SetDlgItemText(IDOK,_F("确定修改"));
	}
	m_Topic_Static.SetWindowText(_F(tmpConTable->topic));
	m_Unit_Static.SetWindowText(_F(tmpConTable->UnitStr));
	EnableOrDisable(tmpConTable->ValueType);
	if(tmpConTable->ValueType == C_VT_FBJ || tmpConTable->ValueType == C_VT_NBJ)
	{
		m_Prefix_Combo.AddString(_F("大于"));
		m_Prefix_Combo.AddString(_F("等于"));
		m_Prefix_Combo.AddString(_F("小于"));
		if(bAdd) m_Prefix_Combo.SetCurSel(0);
		else	 
		{
			m_Prefix_Combo.SetCurSel(tmpUserCons->PreFix);
			if(tmpConTable->ValueType == C_VT_FBJ)
				m_fValue = tmpUserCons->Value.fValue;
			else
				m_nValue = tmpUserCons->Value.nValue;
		}
	}
	if(tmpConTable->ValueType == C_VT_TF)
	{
		if(!bAdd) m_bConCheck = tmpUserCons->Value.nValue;
	}
	if(tmpConTable->ValueType == C_VT_SEL1)
	{
		for(int i = 0;i < TOTAL_DY_NUM;i++)
			m_Sel_Combo.AddString(g_cwAddrTopic[i]);
		if(bAdd) m_Sel_Combo.SetCurSel(0);
		else	 m_Sel_Combo.SetCurSel(tmpUserCons->Value.nSel);
	}
	if(tmpConTable->ValueType == C_VT_SEL2)
	{
		for(int i = 0;i < TOTAL_HY_NUM;i++)
			m_Sel_Combo.AddString(g_cwHYstr[i]);
		if(bAdd) m_Sel_Combo.SetCurSel(0);
		else	 m_Sel_Combo.SetCurSel(tmpUserCons->Value.nSel);
	}

	UpdateData(FALSE);
	return TRUE;
}

void CEditConDlg::OnOK()
{
	UpdateData(TRUE);
	int iIndex;
	CString tmpStr;
	tmpUserCons->MsgNo = tmpConTable->MsgNo;
	switch(tmpConTable->ValueType)
	{
	case C_VT_FBJ:
		iIndex = m_Prefix_Combo.GetCurSel();
		tmpUserCons->PreFix = iIndex;
		tmpUserCons->Value.fValue = m_fValue;
		tmpStr.Format("%s %s %.2f%s",tmpConTable->topic,BjCmpStr[iIndex],m_fValue,tmpConTable->UnitStr);
		strcpy(tmpUserCons->ConStr,tmpStr);
		break;
	case C_VT_NBJ:
		iIndex = m_Prefix_Combo.GetCurSel();
		tmpUserCons->PreFix = iIndex;
		tmpUserCons->Value.nValue = m_nValue;
		tmpStr.Format("%s %s %d%s",tmpConTable->topic,BjCmpStr[iIndex],m_nValue,tmpConTable->UnitStr);
		strcpy(tmpUserCons->ConStr,tmpStr);
		break;
	case C_VT_TF:
		tmpUserCons->Value.nValue = m_bConCheck;
		tmpStr.Format("%s %s",m_bConCheck?"存在":"不存在",tmpConTable->topic);
		strcpy(tmpUserCons->ConStr,tmpStr);
		break;
	case C_VT_SEL1:
		iIndex = m_Sel_Combo.GetCurSel();
		tmpUserCons->Value.nSel = iIndex;
		tmpStr.Format("%s 是 %s",tmpConTable->topic,g_cwAddrTopic[iIndex]);
		strcpy(tmpUserCons->ConStr,tmpStr);
		break;
	case C_VT_SEL2:
		iIndex = m_Sel_Combo.GetCurSel();
		tmpUserCons->Value.nSel = iIndex;
		tmpStr.Format("%s 是 %s",tmpConTable->topic,g_cwHYstr[iIndex]);
		strcpy(tmpUserCons->ConStr,tmpStr);
		break;
	}
	CDialog::OnOK();
}

void CEditConDlg::EnableOrDisable(int flag)
{
	switch(flag)
	{
	case C_VT_FBJ:
		GetDlgItem(IDC_CON_CHECK)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_SEL_COMBO)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_NVALUE_EDIT)->ShowWindow(SW_HIDE);

		GetDlgItem(IDC_PREFIX_COMBO)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_FVALUE_EDIT)->ShowWindow(SW_SHOW);
		break;
	case C_VT_NBJ:
		GetDlgItem(IDC_CON_CHECK)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_SEL_COMBO)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_FVALUE_EDIT)->ShowWindow(SW_HIDE);

		GetDlgItem(IDC_PREFIX_COMBO)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_NVALUE_EDIT)->ShowWindow(SW_SHOW);
		break;
	case C_VT_TF:
		GetDlgItem(IDC_CON_CHECK)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_SEL_COMBO)->ShowWindow(SW_HIDE);

		GetDlgItem(IDC_PREFIX_COMBO)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_FVALUE_EDIT)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_NVALUE_EDIT)->ShowWindow(SW_HIDE);
		break;
	case C_VT_SEL1:
	case C_VT_SEL2:
		GetDlgItem(IDC_CON_CHECK)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_SEL_COMBO)->ShowWindow(SW_SHOW);

		GetDlgItem(IDC_PREFIX_COMBO)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_FVALUE_EDIT)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_NVALUE_EDIT)->ShowWindow(SW_HIDE);
		break;
	}
}
