// AllTestMapDlg.cpp : implementation file
//

#include "stdafx.h"
#include "tdxw.h"
#include "TestYieldWnd.h"
#include "AllTestMapDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAllTestMapDlg dialog


CAllTestMapDlg::CAllTestMapDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAllTestMapDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CAllTestMapDlg)
	//}}AFX_DATA_INIT
}

CAllTestMapDlg::~CAllTestMapDlg()
{

}

void CAllTestMapDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAllTestMapDlg)
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAllTestMapDlg, CDialog)
	//{{AFX_MSG_MAP(CAllTestMapDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAllTestMapDlg message handlers

BOOL CAllTestMapDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	CRect rectDraw;
	GetClientRect(&rectDraw);
	SetWindowText(_F("¼¨Ð§Í¼"));
	m_pDrawWnd = new CTestYieldWnd;
	m_pDrawWnd->Create(NULL, NULL, WS_VISIBLE|WS_CHILD, rectDraw, this, NULL);
	m_pDrawWnd->ModifyStyle(WS_BORDER,0);

	m_pDrawWnd->SetData(m_aTestData, m_sTestData);
	m_pDrawWnd->Invalidate();
	m_pDrawWnd->UpdateData();
	
	return TRUE;
}

BOOL CAllTestMapDlg::PreTranslateMessage(MSG* pMsg) 
{
	switch (pMsg->message)
	{
	case WM_KEYDOWN:
	case WM_KEYUP:
		switch (pMsg->wParam)
		{
		case VK_RETURN:
		case VK_LEFT:
		case VK_RIGHT:
		case VK_UP:
		case VK_DOWN:
		case VK_HOME:
		case VK_END:
		case VK_ESCAPE:
			m_pDrawWnd->SendMessage (pMsg->message, pMsg->wParam, pMsg->lParam);
			// Windowless controls won't be able to call SendMessage.
			// Instead, just respond to the message here.
			return TRUE;
		}
		break;
	}

	return CDialog::PreTranslateMessage(pMsg);
}
