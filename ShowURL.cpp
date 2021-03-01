// ShowURL.cpp : implementation file
//

#include "stdafx.h"
#include "tdxw.h"
#include "ShowURL.h"
#include "Comview.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CShowURL dialog


CShowURL::CShowURL(CWnd* pParent /*=NULL*/,BOOL bGPMutal,BOOL bServiceWeb)
	: CDialog(CShowURL::IDD, pParent)
{
	//{{AFX_DATA_INIT(CShowURL)
	//}}AFX_DATA_INIT
	m_bLayouted = FALSE;

	m_URL = "";
	m_nCx = 0;
	m_nCy = 0;
	m_strTitle = "";
	m_nCurGP = -1;

	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	m_bGPMutal = bGPMutal;
	m_bServiceWeb = bServiceWeb;
}


void CShowURL::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CShowURL)
	DDX_Control(pDX, IDC_EXPLORER1, m_IE);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CShowURL, CDialog)
	//{{AFX_MSG_MAP(CShowURL)
	ON_WM_ERASEBKGND()
	ON_WM_CREATE()
	ON_WM_TIMER()
	ON_WM_SIZE()
	ON_WM_LBUTTONDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BEGIN_EVENTSINK_MAP(CShowURL, CDialog)
    //{{AFX_EVENTSINK_MAP(CShowURL)
	ON_EVENT(CShowURL, IDC_EXPLORER1, 250 /* BeforeNavigate2 */, OnBeforeNavigate2, VTS_DISPATCH VTS_PVARIANT VTS_PVARIANT VTS_PVARIANT VTS_PVARIANT VTS_PVARIANT VTS_PBOOL)
	//}}AFX_EVENTSINK_MAP
END_EVENTSINK_MAP()

/////////////////////////////////////////////////////////////////////////////
// CShowURL message handlers

void CShowURL::SetTitle(const char *pTitle)
{
	m_strTitle = pTitle;
	SetWindowText(m_strTitle);
}

BOOL CShowURL::SetInfo(int nCx,int nCy,const char *pURL,int nGPNo)
{
	m_URL = pURL;
	m_nCx = nCx;
	m_nCy = nCy;
	m_nCurGP = nGPNo;
	return TRUE;
}

static int	ShowURL_x=0,ShowURL_y=0,ShowURL_Cx=0,ShowURL_Cy=0;
int CShowURL::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	/*
	if(m_bGPMutal)
	{
		if(ShowURL_x==0)
		{
			ShowURL_x=(ScreenCx-620)/2;
			ShowURL_y=270;
			ShowURL_Cx=720;
			ShowURL_Cy=480;
		}
		MoveWindow(ShowURL_x,ShowURL_y,ShowURL_Cx,ShowURL_Cy);
	}
	else
		SetWindowPos(&wndTop,0,0,m_nCx,m_nCy,SWP_NOMOVE|SWP_SHOWWINDOW);
	*/
	
	return 0;
}

void CShowURL::OnCancel() 
{
	CRect rc;
	GetWindowRect(rc);
	
	if(!IsIconic() && !IsZoomed())
	{
		ShowURL_x = rc.left;
		ShowURL_y = rc.top;
		ShowURL_Cx = rc.Width();
		ShowURL_Cy = rc.Height();
	}
	GetParent()->PostMessage (WM_CANCELPRESSED,(WPARAM)this,0);	
}

BOOL CShowURL::OnInitDialog() 
{
	CDialog::OnInitDialog();
	SetIcon(m_hIcon,TRUE);
	SetIcon(m_hIcon,FALSE);

	SetWindowText(m_strTitle);
	
	CRect rc;
	GetClientRect(&rc);
	m_IE.MoveWindow(rc,FALSE);
	m_IE.SetSilent(TRUE);
	
	SetTimer(0x100,100,NULL);

	ChangeStock(m_nCurGP);
	
	m_bLayouted = TRUE;

	return TRUE;
}

void CShowURL::ChangeStock(int nGPNo) 
{
	m_nCurGP = nGPNo;
	StartNavigate();
}

void CShowURL::StartNavigate()
{
	CString tmpURL=m_URL;
	if(m_nCurGP>=0)
	{
		MemStkInfo *tmpInfo = (*g_pStockIO)[m_nCurGP];
		TransferToRealURL(tmpURL,tmpInfo);
	}
	else
		TransferToRealURL(tmpURL,NULL);
	//开始浏览
	try
	{
		if(WebContentDownload((char*)(LPCSTR)tmpURL, (char*)(LPCSTR)(g_WSXHStr+"tmppage.htm")))
		{
			FILE *fpt=fopen(g_WSXHStr+"tmppage.htm","rb");
			long nFileLen = tdx_filelength(fpt);
			char *content = new char[nFileLen+1];
			memset(content, 0, nFileLen+1);
			fread(content, 1, nFileLen, fpt);
			fclose(fpt);
			CString StringContent = content;
			TDELA(content);
			StringContent.Replace("通达信", "");
			FILE *fpt1=fopen(g_WSXHStr+"tmppage.htm","wb");
			fwrite((char*)(LPCSTR)StringContent, 1, StringContent.GetLength(), fpt1);
			m_IE.Navigate(g_WSXHStr+"tmppage.htm",NULL,NULL,NULL,NULL);
			fclose(fpt1);
		}
		else 
			m_IE.Navigate(tmpURL,NULL,NULL,NULL,NULL);
	}
	catch(...)
	{
	}
}

void CShowURL::StartNavigate(CString Url)
{
	CString tmpURL=Url;
	if(m_nCurGP>=0)
	{
		MemStkInfo *tmpInfo = (*g_pStockIO)[m_nCurGP];
		TransferToRealURL(tmpURL,tmpInfo);
	}
	else
		TransferToRealURL(tmpURL,NULL);
	//开始浏览
	try
	{
		m_IE.Navigate(tmpURL,NULL,NULL,NULL,NULL);
	}
	catch(...)
	{
	}
}

BOOL CShowURL::OnEraseBkgnd(CDC* pDC) 
{
	return TRUE;		
}

void CShowURL::OnOK() 
{

}

void CShowURL::OnTimer(UINT nIDEvent) 
{
	if(m_bGPMutal && nIDEvent==0x100)
	{
		if(theViewManager.pActiveView->IsKindOf(RUNTIME_CLASS(CComView)))
		{
			int nTmpCurGP=GetSYSCurrentGPIndex();
			if(nTmpCurGP!=-1 && nTmpCurGP!=m_nCurGP)
			{
				m_nCurGP=nTmpCurGP;
				ChangeStock(m_nCurGP);
			}
		}
		else
		{
			KillTimer(0x100);
			OnCancel();
			return;
		}
	}	
	CDialog::OnTimer(nIDEvent);
}

void CShowURL::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);

	if(m_bLayouted)
	{
		CRect rc;
		GetClientRect(&rc);
		m_IE.MoveWindow(rc,FALSE);
	}
}

void CShowURL::OnBeforeNavigate2(LPDISPATCH pDisp, VARIANT FAR* URL, VARIANT FAR* Flags, VARIANT FAR* TargetFrameName, VARIANT FAR* PostData, VARIANT FAR* Headers, BOOL FAR* Cancel) 
{	
	USES_CONVERSION;
	
	char url[501]={0};
	strncpy(url,W2A(URL->bstrVal),500);
	if(BeforeNavigate_Process(url))
		*Cancel = TRUE;
}

