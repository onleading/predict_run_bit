// SelectDateDlg.cpp : implementation file
//

#include "stdafx.h"
#include "tdxw.h"
#include "SelectDateDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSelectDateDlg dialog


CSelectDateDlg::CSelectDateDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSelectDateDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSelectDateDlg)
	m_Date1 = 0;
	m_Date2 = 0;
	//}}AFX_DATA_INIT
	bSingle = TRUE;
}


void CSelectDateDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSelectDateDlg)
	DDX_Control(pDX, IDC_SEPARATOR_STATIC, m_Separator_Static);
	DDX_DateTimeCtrl(pDX, IDC_DATETIMEPICKER1, m_Date1);
	DDX_DateTimeCtrl(pDX, IDC_DATETIMEPICKER2, m_Date2);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSelectDateDlg, CDialog)
	//{{AFX_MSG_MAP(CSelectDateDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSelectDateDlg message handlers

BOOL CSelectDateDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	SetDateCtrlRange(GetDlgItem(IDC_DATETIMEPICKER1));
	SetDateCtrlRange(GetDlgItem(IDC_DATETIMEPICKER2));

	if(bSingle) 
	{
		GetDlgItem(IDC_DATETIMEPICKER2)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_SEPARATOR_STATIC)->ShowWindow(SW_HIDE);
		SetWindowText(_F("ѡ������"));
		CTime today = CTime::GetCurrentTime();
		m_Date2 = today;
	}
	
	UpdateData(FALSE);
	return TRUE;
}

void CSelectDateDlg::OnOK() 
{
	UpdateData(TRUE);

	CTime today = CTime::GetCurrentTime();

	int tempInt1 = m_Date1.GetYear();
	int tempInt2 = m_Date2.GetYear();
	if(tempInt1 < 1990 || tempInt2 < 1990 ||m_Date1 > today||m_Date2 > today|| m_Date1 > m_Date2)
	{
		TDX_MessageBox(m_hWnd,"�������벻����,��Ҫʱ��ȷ����������ʱ���Ƿ���ȷ!\r\n��һ���ڲ��ܴ��ڵڶ�����,�Ҷ����ܴ��ڽ�������(ȷ���㱾��ʱ������ȷ��)",MB_OK|MB_ICONEXCLAMATION);
		return;
	}

	CDialog::OnOK();
}
