// HMFlashLogo.cpp : implementation file
//

#include "stdafx.h"
#include "tdxw.h"
#include "HMFlashLogo.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CHMFlashLogo dialog


CHMFlashLogo::CHMFlashLogo(CWnd* pParent /*=NULL*/)
	: CDialog(CHMFlashLogo::IDD, pParent)
{
	//{{AFX_DATA_INIT(CHMFlashLogo)
	//}}AFX_DATA_INIT
}


void CHMFlashLogo::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CHMFlashLogo)
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CHMFlashLogo, CDialog)
	//{{AFX_MSG_MAP(CHMFlashLogo)
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CHMFlashLogo message handlers

void CHMFlashLogo::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	CRect rect;
	GetClientRect(&rect);

	CDC memDC;			//创建兼容性的DC
	CBitmap tempBitmap;
	memDC.CreateCompatibleDC (&dc);
	tempBitmap.CreateCompatibleBitmap (&dc,rect.Width(),rect.Height());
	CBitmap *pOldBitmap=memDC.SelectObject (&tempBitmap);
	memDC.FillSolidRect(0,0,rect.Width(),rect.Height(),GetSysColor(COLOR_BTNFACE));	
	memDC.SetBkMode (TRANSPARENT);

	HANDLE hOEMBitmap = LoadImage(NULL,HomePath+(CString)"oem.bmp",IMAGE_BITMAP,0,0,LR_LOADFROMFILE);
	if(!hOEMBitmap) return;
	//画
	CBitmap  tmpBitmap2;
	tmpBitmap2.Attach((HBITMAP)hOEMBitmap);
	CDC tmpdc;
	tmpdc.CreateCompatibleDC (&memDC);
	CBitmap *pOldBitmap2 = tmpdc.SelectObject (&tmpBitmap2);
	BITMAP bp={0};
	tmpBitmap2.GetBitmap(&bp);
	memDC.StretchBlt(0, 0, rect.Width(), rect.Height(), &tmpdc, 0, 0, bp.bmWidth, bp.bmHeight, SRCCOPY);
	tmpdc.SelectObject(pOldBitmap2);
	dc.BitBlt (0,0,rect.Width(),rect.Height(),&memDC,0,0,SRCCOPY);

	memDC.SelectObject(pOldBitmap);
	memDC.DeleteDC();
	if(hOEMBitmap)
		tmpBitmap2.Detach();
	DeleteObject(hOEMBitmap);
	
	// Do not call CDialog::OnPaint() for painting messages
}
