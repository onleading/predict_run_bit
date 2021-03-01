/*
* Copyright (c) 2002,武汉通达信开发部
* All rights reserved.
* 
* 文件名称：JiangDlg.h
* 文件标识：
* 摘    要：完成江恩图表功能
*/

#include "stdafx.h"
#include "tdxw.h"
#include "JiangDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CJiangDlg dialog


CJiangDlg::CJiangDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CJiangDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CJiangDlg)
	m_fStartP = 0.0f;
	m_fNowVal = 0.0f;
	//}}AFX_DATA_INIT
	m_nMode		= 0;		//两种模式0、1
	m_fStep		= g_fJiangEnStep;		//每周期对应价格梯度
	m_fStartP	= 1;		//开始价格
	m_nStartT	= 0;		//开始周期
	m_nCalcMode = g_nJiangEnMode;
	m_PtIn.x	= -1;
	m_PtIn.y	= -1;
	memset(m_aJiang,0,sizeof(m_aJiang));
}


void CJiangDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CJiangDlg)
	DDX_Control(pDX, IDC_COMBO, m_ctlCb);
	DDX_Text(pDX, IDC_NOWVAL, m_fNowVal);
	DDX_Text(pDX, IDC_STEP, m_fStep);
	DDV_MinMaxFloat(pDX, m_fStep, 1.e-004f, 1.e+007f);
	DDX_Text(pDX, IDC_START, m_fStartP);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CJiangDlg, CDialog)
	//{{AFX_MSG_MAP(CJiangDlg)
	ON_WM_PAINT()
	ON_WM_RBUTTONDOWN()
	ON_WM_LBUTTONDOWN()
	ON_WM_ERASEBKGND()
	ON_WM_KEYDOWN()
	ON_CBN_SELCHANGE(IDC_COMBO, OnSelchangeCombo)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CJiangDlg message handlers

void CJiangDlg::OnOK() 
{
	UpdateData();
	CalcIt();
	m_fNowVal = m_aJiang[atol(m_aTxt[m_PtIn.x][m_PtIn.y])-1].fVal;
	UpdateData(0);
}

void CJiangDlg::OnCancel() 
{
	g_fJiangEnStep = m_fStep;
	g_nJiangEnMode = m_nCalcMode;
	CDialog::OnCancel();
}

void CJiangDlg::DrawGrid(CDC * pDC)
{
	int		i,j,oldm;
	float	xStep,yStep;
	CRect	rc,rc2;
	CDC		MemDC;
	CBitmap	Bit;
	GetClientRect(&rc2);
	GetClientRect(&rc);
	rc.top += 22;
	xStep = (rc.right-rc.left)/9.0;
	yStep = (rc.bottom-rc.top)/9.0;
	MemDC.CreateCompatibleDC(pDC);
	Bit.CreateCompatibleBitmap(pDC,rc2.Width(),rc2.Height());
	/////////////////////////////////
	MemDC.SelectObject(&Bit);
	oldm = MemDC.SetBkMode(TRANSPARENT);
	
	MemDC.FillSolidRect(rc2.left,rc2.top,rc2.Width(),rc2.Height(),GetSysColor(COLOR_MENU));
	for ( i=0;i<9;++i )
	{
		for ( j=0;j<9;++j )
		{
			m_aRect[i][j].SetRect(rc.left+j*xStep,rc.top+i*yStep,
					rc.left+(j+1)*xStep,rc.top+(i+1)*yStep);
			if ( i == m_PtIn.x && j == m_PtIn.y )
				MemDC.FillSolidRect(m_aRect[m_PtIn.x]+m_PtIn.y,RGB(0,255,255));
			else
			{
				if ( i ==4 || j== 4 )
				{
					MemDC.FillSolidRect(m_aRect[i]+j,RGB(255,255,0));
				}
				if ( i +j == 8 || i == j )
				{
					MemDC.FillSolidRect(m_aRect[i]+j,RGB(255,0,0));
				}
			}
			MemDC.Draw3dRect(m_aRect[i]+j,RGB(255,255,255),RGB(128,128,128));

			MemDC.TextOut(m_aRect[i][j].left+xStep/3-0.5,
						 m_aRect[i][j].top+yStep/3-0.5,m_aTxt[i][j]);
		}
		g_d.DrawLine(&MemDC,rc.left,rc.top+i*yStep,rc.right,rc.top+i*yStep,RGB(255,255,255));
		g_d.DrawLine(&MemDC,rc.left+i*xStep,rc.top,rc.left+i*xStep,rc.bottom,RGB(255,255,255));
	}
	MemDC.SetBkMode(oldm );
	pDC->BitBlt(rc2.left,rc2.top,rc2.Width(),rc2.Height(),&MemDC,rc2.left,rc2.top,SRCCOPY);
	
}
void CJiangDlg::GetTxt()
{
	int		i,j;
	memset(m_aTxt,0,sizeof(m_aTxt));
	if ( m_nMode )
	{
		int		nC=1,k;
		i = j = 4; 
		sprintf(m_aTxt[i][j],"%d",nC++);
		for ( k=0;k<4;++k )
		{
			//j不动，i减小，不得偏2离K+1
			for ( i=4+k , j=5+k; 4-i<=k; --i )	
				sprintf(m_aTxt[i][j],"%d",nC++);
			//i不动
			for ( ;	4-j<=k;		--j )				
				sprintf(m_aTxt[i][j],"%d",nC++);
			////////////////////////
			for ( ; i-4<=k;		++i )
				sprintf(m_aTxt[i][j],"%d",nC++);
			/////////////////////////////
			for ( ; j-4<=k+1;	++j )
				sprintf(m_aTxt[i][j],"%d",nC++);
		}
	}
	else
	{
		for ( i=0;i<9;++i )
			for ( j=0;j<9;++j )
				sprintf(m_aTxt[i][j],"%d",(j+1)*9-i);
	}
}

void CJiangDlg::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	DrawGrid(&dc);
	// Do not call CDialog::OnPaint() for painting messages
}

void CJiangDlg::CalcIt()
{
	int i;
	if ( m_nCalcMode == 0 )
	{
		for ( i=0;i<81;++i )
			m_aJiang[i].fVal = m_fStartP + m_fStep*i;
	}
	else
	{
		for ( i=0;i<81;++i )
			m_aJiang[i].fVal = m_fStartP - m_fStep*i;
	}
}

BOOL CJiangDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	GetTxt();

	CalcIt();
	
	m_PtIn.x = m_PtIn.y = 4;
	m_fNowVal = m_aJiang[0].fVal;
	
	m_ctlCb.InsertString(0,_F("阻力位"));
	m_ctlCb.InsertString(1,_F("支撑位"));
	m_ctlCb.SetCurSel(m_nCalcMode);

	UpdateData(FALSE);

	SetFocus();

	return TRUE;
}

void CJiangDlg::OnRButtonDown(UINT nFlags, CPoint point) 
{
	
	m_nMode = !m_nMode;	
	GetTxt();
	Invalidate();
	CDialog::OnRButtonDown(nFlags, point);
}

void CJiangDlg::OnLButtonDown(UINT nFlags, CPoint point) 
{
	int	i,j;
	UpdateData();
	CalcIt();
	for ( i=0;i<9;++i )
		for ( j=0;j<9;++j )
			if ( m_aRect[i][j].PtInRect(point) )
			{
				m_PtIn.x = i;
				m_PtIn.y = j;
				
				m_fNowVal = m_aJiang[atol(m_aTxt[i][j])-1].fVal;
				GetTxt();
				UpdateData(0);
				Invalidate();
				break;
			}
	CDialog::OnLButtonDown(nFlags, point);
}

BOOL CJiangDlg::OnEraseBkgnd(CDC* pDC) 
{
	return 0;	
}


void CJiangDlg::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	
	CDialog::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CJiangDlg::OnSelchangeCombo() 
{
	m_nCalcMode = !m_nCalcMode;
	m_ctlCb.SetCurSel(m_nCalcMode);
	CalcIt();
	OnOK();
}
