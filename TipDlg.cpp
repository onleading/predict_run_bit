#include "stdafx.h"
#include "resource.h"

#include <winreg.h>
#include <sys\stat.h>
#include <sys\types.h>
#include "TipDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTipDlg dialog

CTipDlg::CTipDlg(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_TIP, pParent)
{
	m_strTip="";
	bFileError = FALSE;
	if(!TipFile.Open("Tips.dat",CFile::modeRead|CFile::shareDenyNone))
	{
		bFileError = TRUE;
		m_strTip.LoadString(CG_IDS_FILE_ABSENT);
		return;
	}
}

void CTipDlg::GetTips()
{
	CString LineStr;
	int i = 0;
	BOOL bFind = FALSE;
	TipFile.Seek(0,CFile::begin);
	while(TipFile.ReadString(LineStr))
	{
		if(i == nTipFilePos)
		{
			bFind = TRUE;
			break;
		}
		i++;
	}
	if(bFind) m_strTip = LineStr;
	else if(i == 0) 
	{
		nTipFilePos = 0;
		m_strTip = "Tips文件中没有内容。";
	}
	else 
	{
		nTipFilePos = 0;
		TipFile.Seek(0,CFile::begin);
		TipFile.ReadString(m_strTip);
	}
	m_strTip = "\n   "+m_strTip;
	SetDlgItemText(IDC_TIPSTRING,_F(m_strTip));
}

CTipDlg::~CTipDlg()
{
	if(!bFileError)	TipFile.Close();
}
        
void CTipDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTipDlg)
	DDX_Check(pDX, IDC_STARTUP, m_bStartup);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CTipDlg, CDialog)
	//{{AFX_MSG_MAP(CTipDlg)
	ON_BN_CLICKED(IDC_NEXTTIP, OnNextTip)
	ON_WM_CTLCOLOR()
	ON_WM_PAINT()
	ON_BN_CLICKED(IDC_LASTTIP, OnLasttip)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTipDlg message handlers

void CTipDlg::OnNextTip()
{
	nTipFilePos += 1;
	GetTips();
}

void CTipDlg::OnLasttip() 
{
	if(nTipFilePos > 0)	nTipFilePos -= 1;
	GetTips();
}


HBRUSH CTipDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	if (pWnd->GetDlgCtrlID() == IDC_TIPSTRING)
		return (HBRUSH)GetStockObject(WHITE_BRUSH);

	return CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
}

void CTipDlg::OnOK()
{
	UpdateData(TRUE);
	bStartTips = m_bStartup;
	CDialog::OnOK();
}

BOOL CTipDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_bStartup = bStartTips;
	if (bFileError)
	{
		GetDlgItem(IDC_NEXTTIP)->EnableWindow(FALSE);
		GetDlgItem(IDC_LASTTIP)->EnableWindow(FALSE);
	}
	else	GetTips(); //如果没有错误，则读Tips

	UpdateData(FALSE);
	return TRUE;
}

void CTipDlg::OnPaint()
{
	CDialog::OnPaint();
}
