// BlockColorPage.cpp : implementation file
//

#include "stdafx.h"
#include "tdxw.h"
#include "BlockColorPage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CBlockColorPage property page

IMPLEMENT_DYNCREATE(CBlockColorPage, CPropertyPage)

CBlockColorPage::CBlockColorPage() : CPropertyPage(CBlockColorPage::IDD)
{
	//{{AFX_DATA_INIT(CBlockColorPage)
	//}}AFX_DATA_INIT
}

CBlockColorPage::~CBlockColorPage()
{
}

void CBlockColorPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CBlockColorPage)
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CBlockColorPage, CPropertyPage)
	//{{AFX_MSG_MAP(CBlockColorPage)
	ON_WM_LBUTTONDOWN()
	ON_WM_RBUTTONDOWN()
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBlockColorPage message handlers

BOOL CBlockColorPage::OnInitDialog()
{
	CPropertyPage::OnInitDialog();
	FillName(0);
		
	return TRUE;
}

void CBlockColorPage::ChgColor(int nColorID)
{
	if(nColorID<1 || nColorID>8) return;
	
	CColorDialog CColorDlg(g_BlockColor[nColorID-1].clrBlock,CC_FULLOPEN);
	if(CColorDlg.DoModal() == IDOK )
		g_BlockColor[nColorID-1].clrBlock = CColorDlg.m_cc.rgbResult;

	FillColor(nColorID);
}

#include "GetBlockDlg.h"
void CBlockColorPage::ChgBlock(int nBlockID,BOOL bDelete)
{
	if(nBlockID<1 || nBlockID>8) 
		return;
	if(bDelete)
		g_BlockColor[nBlockID-1].BlockName[0] = 0;
	else
	{
		CGetBlockDlg dlg;
		if(dlg.DoModal() == IDOK)
			strcpy(g_BlockColor[nBlockID-1].BlockName,GetBlockName(dlg.nSelBlock+2));
	}

	FillName(nBlockID);
}

void CBlockColorPage::FillColor(int nColorID)
{
	CWnd *pWnd	= NULL;
	CDC	*pDC	= NULL;
	CRect rect;
	switch(nColorID)
	{
	case 0:
		FillColor(1);
		FillColor(2);
		FillColor(3);
		FillColor(4);
		FillColor(5);
		FillColor(6);
		FillColor(7);
		FillColor(8);
		break;
	case 1:
		pWnd = GetDlgItem(IDC_BLOCK1COLOR);
		break;
	case 2:
		pWnd = GetDlgItem(IDC_BLOCK2COLOR);
		break;
	case 3:
		pWnd = GetDlgItem(IDC_BLOCK3COLOR);
		break;
	case 4:
		pWnd = GetDlgItem(IDC_BLOCK4COLOR);
		break;
	case 5:
		pWnd = GetDlgItem(IDC_BLOCK5COLOR);
		break;
	case 6:
		pWnd = GetDlgItem(IDC_BLOCK6COLOR);
		break;
	case 7:
		pWnd = GetDlgItem(IDC_BLOCK7COLOR);
		break;
	case 8:
		pWnd = GetDlgItem(IDC_BLOCK8COLOR);
		break;
	}
	if(pWnd)
	{
		pDC = pWnd->GetDC();
		if(pDC)
		{
			CBrush	*pOldBrush;
			CPen	*pOldPen;
			CBrush	CurBrush(g_BlockColor[nColorID-1].clrBlock);
			CPen	CurPen(PS_SOLID,1,g_BlockColor[nColorID-1].clrBlock);
			
			pWnd->GetClientRect(rect);
			rect.DeflateRect(1,1,1,1);
			pOldPen		= pDC->SelectObject(&CurPen);
			pOldBrush	= pDC->SelectObject(&CurBrush);
			pDC->Rectangle(rect);
			pDC->SelectObject(pOldBrush);
			pDC->SelectObject(pOldPen);
			pWnd->ReleaseDC(pDC);
		}
	}
}

void CBlockColorPage::FillName(int nBlockID)
{
	CWnd *pWnd	= NULL;
	switch(nBlockID)
	{
	case 0:
		FillName(1);
		FillName(2);
		FillName(3);
		FillName(4);
		FillName(5);
		FillName(6);
		FillName(7);
		FillName(8);
		break;
	case 1:
		pWnd = GetDlgItem(IDC_BLOCKCOLOR_NAME1);
		break;
	case 2:
		pWnd = GetDlgItem(IDC_BLOCKCOLOR_NAME2);
		break;
	case 3:
		pWnd = GetDlgItem(IDC_BLOCKCOLOR_NAME3);
		break;
	case 4:
		pWnd = GetDlgItem(IDC_BLOCKCOLOR_NAME4);
		break;
	case 5:
		pWnd = GetDlgItem(IDC_BLOCKCOLOR_NAME5);
		break;
	case 6:
		pWnd = GetDlgItem(IDC_BLOCKCOLOR_NAME6);
		break;
	case 7:
		pWnd = GetDlgItem(IDC_BLOCKCOLOR_NAME7);
		break;
	case 8:
		pWnd = GetDlgItem(IDC_BLOCKCOLOR_NAME8);
		break;
	}
	if(pWnd)
		pWnd->SetWindowText(_F(g_BlockColor[nBlockID-1].BlockName));
}

void CBlockColorPage::OnLButtonDown(UINT nFlags, CPoint point) 
{
	CPropertyPage::OnLButtonDown(nFlags, point);

	int nColorID = 0;

	if(CheckPointPos(this,IDC_BLOCK1COLOR,point)) nColorID = 1;
	else if(CheckPointPos(this,IDC_BLOCK2COLOR,point)) nColorID = 2;
	else if(CheckPointPos(this,IDC_BLOCK3COLOR,point)) nColorID = 3;
	else if(CheckPointPos(this,IDC_BLOCK4COLOR,point)) nColorID = 4;
	else if(CheckPointPos(this,IDC_BLOCK5COLOR,point)) nColorID = 5;
	else if(CheckPointPos(this,IDC_BLOCK6COLOR,point)) nColorID = 6;
	else if(CheckPointPos(this,IDC_BLOCK7COLOR,point)) nColorID = 7;
	else if(CheckPointPos(this,IDC_BLOCK8COLOR,point)) nColorID = 8;
	else
	{
		if(CheckPointPos(this,IDC_BLOCKCOLOR_NAME1,point)) nColorID = 1;
		else if(CheckPointPos(this,IDC_BLOCKCOLOR_NAME2,point)) nColorID = 2;
		else if(CheckPointPos(this,IDC_BLOCKCOLOR_NAME3,point)) nColorID = 3;
		else if(CheckPointPos(this,IDC_BLOCKCOLOR_NAME4,point)) nColorID = 4;
		else if(CheckPointPos(this,IDC_BLOCKCOLOR_NAME5,point)) nColorID = 5;
		else if(CheckPointPos(this,IDC_BLOCKCOLOR_NAME6,point)) nColorID = 6;
		else if(CheckPointPos(this,IDC_BLOCKCOLOR_NAME7,point)) nColorID = 7;
		else if(CheckPointPos(this,IDC_BLOCKCOLOR_NAME8,point)) nColorID = 8;
		else return;

		ChgBlock(nColorID,FALSE);
		return;
	}

	ChgColor(nColorID);
}

void CBlockColorPage::OnRButtonDown(UINT nFlags, CPoint point) 
{	
	CPropertyPage::OnRButtonDown(nFlags, point);

	int nColorID = 0;
	
	if(CheckPointPos(this,IDC_BLOCKCOLOR_NAME1,point)) nColorID = 1;
	else if(CheckPointPos(this,IDC_BLOCKCOLOR_NAME2,point)) nColorID = 2;
	else if(CheckPointPos(this,IDC_BLOCKCOLOR_NAME3,point)) nColorID = 3;
	else if(CheckPointPos(this,IDC_BLOCKCOLOR_NAME4,point)) nColorID = 4;
	else if(CheckPointPos(this,IDC_BLOCKCOLOR_NAME5,point)) nColorID = 5;
	else if(CheckPointPos(this,IDC_BLOCKCOLOR_NAME6,point)) nColorID = 6;
	else if(CheckPointPos(this,IDC_BLOCKCOLOR_NAME7,point)) nColorID = 7;
	else if(CheckPointPos(this,IDC_BLOCKCOLOR_NAME8,point)) nColorID = 8;
	else return;

	ChgBlock(nColorID,TRUE);
}

void CBlockColorPage::OnPaint() 
{
	CPaintDC dc(this);

	FillColor(0);
}

