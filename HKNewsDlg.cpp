// HKNewsDlg.cpp : implementation file
//

#include "stdafx.h"
#include "tdxw.h"
#include "HKNewsDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CHKNewsDlg dialog


CHKNewsDlg::CHKNewsDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CHKNewsDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CHKNewsDlg)
	//}}AFX_DATA_INIT
	memset(&m_Units,0,sizeof(UNITS));
	m_Rect.SetRect(0,0,0,0);
	m_hIcon = AfxGetApp()->LoadIcon(IDI_ZX2);
}


void CHKNewsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CHKNewsDlg)
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CHKNewsDlg, CDialog)
	//{{AFX_MSG_MAP(CHKNewsDlg)
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

#define FOR_UNIT(i) for( i=0;i<m_Units.Num && m_Units.pUnit[i];i++)
/////////////////////////////////////////////////////////////////////////////
// CHKNewsDlg message handlers

BOOL CHKNewsDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	SetIcon(m_hIcon,TRUE);
	SetIcon(m_hIcon,FALSE);
	
	CreateUnit(0,-1,HKNEWS_UNIT,HKNEWS_BIG);
	m_Units.Num = 1;
	ResizePad(COM_OnlyRead);	//第一次要读数据
	
	return TRUE;
}

//创建一个Unit
void CHKNewsDlg::CreateUnit(int which,int initGPNo,int type,int style)
{
	TDEL(m_Units.pUnit[which]);	//先删除原来的Unit

	m_Units.Type[which] = type;
	m_Units.Style[which] = style;
	m_Units.nStatus[which] = 0;	//为0表示显示出来
	COMINFO ComInfo;
	ComInfo.pWnd = this;
	ComInfo.nNowPad = 0;		//????,随便
	ComInfo.pUnits = &m_Units;
	ComInfo.nWhichUnit = which;
	ComInfo.nUnitType = type;
	ComInfo.nUnitStyle = style;
	ComInfo.nInCom2 = 0;
	ComInfo.nInHisDlg = 1;
	ComInfo.nInMultiPeriod = 0;
	switch(type)
	{
	case HKNEWS_UNIT:
		m_Units.pUnit[which] = new UHKNews(&ComInfo);
		break;
	default:
		break;
	}
}

void CHKNewsDlg::OnPaint() 
{
	CPaintDC dc(this);
	int i;
	FOR_UNIT(i)
		m_Units.pUnit[i]->ProcessMsg(WM_PAINT);
}

void CHKNewsDlg::OnOK() 
{	
}

void CHKNewsDlg::OnCancel() 
{
	int i;
	FOR_UNIT(i)
		TDEL(m_Units.pUnit[i]);
	CDialog::OnCancel();
}

void CHKNewsDlg::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);
	m_Rect.SetRect(0,0,cx,cy);
	ResizePad(COM_OnlySize);
}

//改变版面尺寸 flag=0,局部打位图,=1,读数据并局部打位图,=2,仅读数据,=3,不画
void CHKNewsDlg::ResizePad(int flag)
{
	long wParam,lParam;
	//MainPad
	wParam = MAKELONG(0,0);
	lParam = MAKELONG(m_Rect.Width(),m_Rect.Height());
	if(m_Units.pUnit[0])
	{
		m_Units.Rect[0] = CRect(0,0,m_Rect.Width(),m_Rect.Height());
		m_Units.pUnit[0]->ProcessMsg(WM_SIZE,wParam,lParam);	
		if(flag==COM_ReadDraw || flag==COM_OnlyDraw)
 			m_Units.pUnit[0]->ProcessMsg(WM_PAINT,flag);
		if(flag==COM_OnlyRead)
			m_Units.pUnit[0]->ProcessMsg(UM_READDATA);
	}
}

LRESULT CHKNewsDlg::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	switch(message)
	{
	case UM_VSCROLLBAR:
	case WM_LBUTTONDOWN:
	case UM_READDATA:
	case UM_FILLREALINFO_MSG:
	case UM_HKNEWSSWITCH_MSG:
		if(m_Units.pUnit[0])
			m_Units.pUnit[0]->ProcessMsg(message,wParam,lParam);
		break;
	default:
		break;
	}	
	return CDialog::WindowProc(message, wParam, lParam);
}

BOOL	CHKNewsDlg::OnEraseBkgnd(CDC* pDC) 
{
	return TRUE;
}