// SetCBDlg.cpp : implementation file
//

#include "stdafx.h"
#include "tdxw.h"
#include "SetCBDlg.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSetCBDlg dialog


CSetCBDlg::CSetCBDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSetCBDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSetCBDlg)
	m_fDown = 1.0f;
	m_nParam = 40;
	m_nWay = 1;
	//}}AFX_DATA_INIT
}


void CSetCBDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSetCBDlg)
	DDX_Control(pDX, IDC_LISTN, m_ctlList);
	DDX_Control(pDX, ID_TAB, m_ctlTab);
	DDX_Text(pDX, ID_DOWN, m_fDown);
	DDV_MinMaxFloat(pDX, m_fDown, 0.1f, 10.f);
	DDX_Text(pDX, ID_PARAM, m_nParam);
	DDV_MinMaxInt(pDX, m_nParam, 5, 500);
	DDX_Radio(pDX, ID_AV, m_nWay);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSetCBDlg, CDialog)
	//{{AFX_MSG_MAP(CSetCBDlg)
	ON_NOTIFY(TCN_SELCHANGE, ID_TAB, OnSelchangeTab)
	ON_BN_CLICKED(ID_ADD, OnAdd)
	ON_BN_CLICKED(ID_SUB, OnSub)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSetCBDlg message handlers

void CSetCBDlg::OnOK() 
{
	SaveSet();
	
	CDialog::OnOK();
}

void CSetCBDlg::OnCancel() 
{	
	CDialog::OnCancel();
}


void CSetCBDlg::SaveSet()
{
	return;
	UpdateData(TRUE);
	if ( m_fDown > 10 || m_fDown < 0.1 )
	{
		TDX_MessageBox(m_hWnd,"衰减系数不能超出0.1～10之间!",MB_OK|MB_ICONERROR);
		return;
	}

	m_Setup.Way = (COSTWAY)m_nWay;
	m_Setup.IsPrecious = 1;
	m_Setup.fWeakK = m_fDown;
	m_Setup.fdHSL = 0.1f;
	m_Setup.fJZD = 0.9f;

	memset(m_Setup.aN+m_Setup.nSkip[0],0,(MAX_MON-m_Setup.nSkip[0])*sizeof(short));
	memset(m_Setup.aN+(MAX_MON+m_Setup.nSkip[1]),0,(MAX_MON-m_Setup.nSkip[1])*sizeof(short));
	

	CString	FileName = g_WSXHStr + "cbset.dat";
	CFile	f;
	if ( f.Open(FileName,CFile::modeCreate|CFile::modeWrite) )
	{
		f.Write(&m_Setup,sizeof(m_Setup));
		f.Close();
	}
}

BOOL CSetCBDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();	
	m_ctlList.SetItemHeight(0,16);
	m_ctlList.SetItemHeight(-1,16);

	CString	FileName = g_WSXHStr+"cbset.dat";
	CFile	f;	
	if ( f.Open(FileName,CFile::modeRead) )
	{
		f.Read(&m_Setup,sizeof(m_Setup));
		f.Close();
	}
	else
	{
		m_Setup.Way = TRIANGLE;
		m_Setup.nSkip[0] = 6;
		m_Setup.nSkip[1] = 6;
		m_Setup.aN[0] = 5;	m_Setup.aN[1] = 10;
		m_Setup.aN[2] = 20;	m_Setup.aN[3] = 30;
		m_Setup.aN[4] = 60;	m_Setup.aN[5] = 100;
		memcpy(m_Setup.aN+MAX_MON,m_Setup.aN,MAX_MON*sizeof(short));
		m_Setup.fWeakK = 1;
		m_Setup.IsPrecious = 1;
	}
	m_nWay	 = m_Setup.Way;
	m_fDown    = m_Setup.fWeakK;
	
	char	buff[300];
	m_ctlTab.SetCurSel(0);
	m_ctlTab.InsertItem(0,_F("远期成本分布"));
	m_ctlTab.InsertItem(1,_F("近期成本分布"));
	for ( int i=0;i<m_Setup.nSkip[0];++i )
	{
		sprintf(buff,"%d天前成本",m_Setup.aN[i]);
		m_ctlList.InsertString(i,_F(buff));
		m_ctlList.SetItemData(i,m_Setup.aN[i]);
	}
	if(m_Setup.nSkip[0] > 0)
		m_ctlList.SetCurSel(0);	
	UpdateData(FALSE);	
	return TRUE;
}

void CSetCBDlg::OnSelchangeTab(NMHDR* pNMHDR, LRESULT* pResult) 
{
	char	buff[300];
	int i,index = m_ctlTab.GetCurSel();
	if ( index == 0 )
	{
		m_ctlList.ResetContent();
		for ( i=0;i<m_Setup.nSkip[0];++i )
		{
			sprintf(buff,"%d天前成本",m_Setup.aN[i]);
			m_ctlList.InsertString(i,_F(buff));
			m_ctlList.SetItemData(i,m_Setup.aN[i]);
		}
		if(m_Setup.nSkip[0] > 0)
			m_ctlList.SetCurSel(0);	
	}
	else
	{
		m_ctlList.ResetContent();
		for ( i=0;i<m_Setup.nSkip[1];++i )
		{
			sprintf(buff,"%d天内成本",m_Setup.aN[MAX_MON+i]);
			m_ctlList.InsertString(i,_F(buff));
			m_ctlList.SetItemData(i,m_Setup.aN[MAX_MON+i]);
		}
		if(m_Setup.nSkip[1] > 0)
			m_ctlList.SetCurSel(0);	
	}
	UpdateData(0);
	
	*pResult = 0;
}

void CSetCBDlg::OnAdd() 
{
	int		index,i;
	char	buff[200];
	UpdateData();
	if ( m_ctlList.GetCount() >= MAX_MON )
	{
		TDX_MessageBox(m_hWnd,"每种类型最多只能有6个!",MB_OK|MB_ICONERROR);
		return;
	}
	if( m_nParam < 5 || m_nParam > 500) return;
	index = m_ctlList.GetCurSel();
	++index ;
	if ( m_ctlTab.GetCurSel()  )
	{
		for ( i=0;i<m_ctlList.GetCount();++i )
		{
			if ( m_ctlList.GetItemData(i) == m_nParam )
			{
				TDX_MessageBox(m_hWnd,"不要重复用一个参数!",MB_OK|MB_ICONERROR);
				return;
			}
		}
		sprintf(buff,"%d天内成本",m_nParam);
		m_Setup.aN[MAX_MON+m_Setup.nSkip[1]] = m_nParam;
		m_Setup.nSkip[1]++;
	}
	else
	{
		for ( i=0;i<m_ctlList.GetCount();++i )
		{
			if ( m_ctlList.GetItemData(i) == m_nParam )
			{
				TDX_MessageBox(m_hWnd,"不要重复用一个参数!",MB_OK|MB_ICONERROR);
				return;
			}
		}
		sprintf(buff,"%d天前成本",m_nParam);
		m_Setup.aN[m_Setup.nSkip[0]] = m_nParam;
		m_Setup.nSkip[0]++;
	}
	m_ctlList.InsertString(index,_F(buff));
	m_ctlList.SetItemData(index,m_nParam);
	m_ctlList.SetCurSel(index);
}

void CSetCBDlg::OnSub() 
{
	int	index,i;
	if ( m_ctlList.GetCount() <= 0 )
		return;
	index = m_ctlList.GetCurSel();
	if ( m_ctlTab.GetCurSel()  )
	{
		for ( i=0;i<m_Setup.nSkip[1];++i )
			if ( m_Setup.aN[MAX_MON+i] == m_ctlList.GetItemData(index) )
			{
				memmove(&m_Setup.aN[MAX_MON+i],&m_Setup.aN[MAX_MON+i+1],(MAX_MON-i-1)*sizeof(short));
				--m_Setup.nSkip[1];
			}
	}
	else
	{
		for ( i=0;i<m_Setup.nSkip[0];++i )
			if ( m_Setup.aN[i] == m_ctlList.GetItemData(index) )
			{
				memmove(&m_Setup.aN[i],&m_Setup.aN[i+1],(MAX_MON-i-1)*sizeof(short));
				--m_Setup.nSkip[0];
			}
	}
	m_ctlList.DeleteString(index);
	if(index > 0)
		m_ctlList.SetCurSel(index-1);
}
