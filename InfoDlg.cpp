// InfoDlg.cpp : implementation file
//

#include "stdafx.h"
#include "tdxw.h"
#include "InfoDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CInfoDlg dialog


CInfoDlg::CInfoDlg(CWnd* pParent /*=NULL*/, int nUF )
	: CDialog(CInfoDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CInfoDlg)
	m_strAmount = _T("");
	m_strHsl = _T("");
	m_strTime1 = _T("");
	m_strValue = _T("");
	m_strZhenFu = _T("");
	m_strLtgb = _T("");
	//}}AFX_DATA_INIT
	m_nStyle = FX_INFODLG;
	pView = pParent;
	m_nF = nUF;
	m_br.CreateSolidBrush(VipColor.BACKCOLOR);
}

CInfoDlg::~CInfoDlg()
{
	m_br.DeleteObject();
}

void CInfoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CInfoDlg)
	DDX_Control(pDX, IDC_VOLUME, m_Volume_Static);
	DDX_Control(pDX, IDC_ZANGFU, m_Zanfu_Static);
	DDX_Control(pDX, IDC_MIN, m_Min_Static);
	DDX_Control(pDX, IDC_CLOSE, m_Close_Static);
	DDX_Control(pDX, IDC_MAX, m_Max_Static);
	DDX_Control(pDX, IDC_OPEN, m_Open_Static);
	DDX_Text(pDX, IDC_AMOUNT, m_strAmount);
	DDX_Text(pDX, IDC_HSL, m_strHsl);
	DDX_Text(pDX, IDC_TIME1, m_strTime1);
	DDX_Text(pDX, IDC_VALUE, m_strValue);
	DDX_Text(pDX, IDC_ZHENFU, m_strZhenFu);
	DDX_Text(pDX, IDC_LTGB, m_strLtgb);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CInfoDlg, CDialog)
	//{{AFX_MSG_MAP(CInfoDlg)
	ON_WM_NCHITTEST()
	ON_MESSAGE(UM_NOTIFY_INFODLG,OnInfoDlgNotify)
	ON_WM_SYSCOMMAND()
	ON_WM_KILLFOCUS()
	ON_WM_ERASEBKGND()
	ON_WM_CTLCOLOR()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CInfoDlg message handlers

BOOL CInfoDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	m_Volume_Static.SetTextColor(GridColor.Grid_Volume_Color);

	m_Volume_Static.SetBkColor(VipColor.BACKCOLOR);
	m_Open_Static.SetBkColor(VipColor.BACKCOLOR);
	m_Close_Static.SetBkColor(VipColor.BACKCOLOR);
	m_Max_Static.SetBkColor(VipColor.BACKCOLOR);
	m_Min_Static.SetBkColor(VipColor.BACKCOLOR);
	m_Zanfu_Static.SetBkColor(VipColor.BACKCOLOR);

	SendMessage(UM_NOTIFY_INFODLG,1);
	
	return TRUE;
}

void CInfoDlg::OnOK() 
{	
}

void CInfoDlg::OnCancel() 
{
	pView->PostMessage(UM_DELETE_INFODLG, 0, m_nF+1);
}

UINT CInfoDlg::OnNcHitTest(CPoint point) 
{	
	UINT nRet = CDialog::OnNcHitTest(point);
	return (nRet==HTCLIENT)?HTCAPTION:nRet;
}

COLORREF CompareDlgInfoStr(CString tmpStr1,CString tmpStr2)
{
	if(tmpStr2.GetLength()==0)
		return GridColor.Grid_Level_Color;
	float tmpf1=atof(tmpStr1);
	float tmpf2=atof(tmpStr2);
	if(tmpf1>tmpf2+COMPPREC) 
		return GridColor.Grid_Up_Color;
	else if(tmpf1<tmpf2-COMPPREC) 
		return GridColor.Grid_Down_Color;
	else
		return GridColor.Grid_Level_Color;
}

void CInfoDlg::OnInfoDlgNotify(WPARAM wParam,LPARAM lParam)
{
	char tmpStr[80];
	if(wParam==1) 
	{
		SetWindowText("");
		
		m_strTime1="";
		m_strValue="";
		m_strAmount="";
		m_strHsl="";
		m_strLtgb="";
		m_strZhenFu="";

		m_Open_Static.SetText("");
		m_Max_Static.SetText("");
		m_Min_Static.SetText("");
		m_Close_Static.SetText("");
		m_Zanfu_Static.SetText("");
		m_Volume_Static.SetText("");

		UpdateData(FALSE);
		return;
	}
	m_nStyle = g_DlgInfo.style;
	if(m_nStyle==FX_INFODLG)
	{
		GetDlgItem(IDC_TIME1_STATIC)->SetWindowText(_F("时间"));
		GetDlgItem(IDC_TIME2_STATIC)->SetWindowText(_F("数值"));
	}
	else
	{
		GetDlgItem(IDC_TIME1_STATIC)->SetWindowText(_F("开始于"));
		GetDlgItem(IDC_TIME2_STATIC)->SetWindowText(_F("结束于"));
	}
	SetWindowText(g_DlgInfo.name);

	g_DlgInfo.lastclose[19]=0;
	m_Open_Static.SetTextColor(CompareDlgInfoStr(g_DlgInfo.open,g_DlgInfo.lastclose));
	m_Close_Static.SetTextColor(CompareDlgInfoStr(g_DlgInfo.close,g_DlgInfo.lastclose));
	m_Max_Static.SetTextColor(CompareDlgInfoStr(g_DlgInfo.max,g_DlgInfo.lastclose));
	m_Min_Static.SetTextColor(CompareDlgInfoStr(g_DlgInfo.min,g_DlgInfo.lastclose));
	m_Zanfu_Static.SetTextColor(CompareDlgInfoStr(g_DlgInfo.zhangfu,"0"));

	m_strTime1=g_DlgInfo.time1;
	m_strValue=g_DlgInfo.value;
	m_strAmount=g_DlgInfo.amount;
	m_strHsl=g_DlgInfo.hsl;
	m_strLtgb=g_DlgInfo.ltgb;
	m_strZhenFu=g_DlgInfo.zhenfu;

	if(g_DlgInfo.amount[0]==0&&g_DlgInfo.volinstk[0]!=0)	//三方行情期货
	{
		m_strAmount = g_DlgInfo.volinstk;
		GetDlgItem(IDC_AMO_STATIC)->SetWindowText(_F("持仓量"));
	}

	m_Open_Static.SetText(g_DlgInfo.open);
	m_Max_Static.SetText(g_DlgInfo.max);
	m_Min_Static.SetText(g_DlgInfo.min);
	m_Close_Static.SetText(g_DlgInfo.close);
	AllTrim(g_DlgInfo.zhangdie);
	AllTrim(g_DlgInfo.zhangfu);
	if(g_DlgInfo.zhangdie[0])
		sprintf(tmpStr,"%s(%s)",g_DlgInfo.zhangdie,g_DlgInfo.zhangfu);
	else
		tmpStr[0]=0;
	m_Zanfu_Static.SetText(tmpStr);
	m_Volume_Static.SetText(g_DlgInfo.volume);

	UpdateData(FALSE);
}

void CInfoDlg::OnSysCommand(UINT nID, LPARAM lParam) 
{
	if(nID == SC_MAXIMIZE) return;
	CDialog::OnSysCommand(nID, lParam);
}

void CInfoDlg::OnKillFocus(CWnd* pNewWnd) 
{
	CDialog::OnKillFocus(pNewWnd);
}

BOOL CInfoDlg::OnEraseBkgnd(CDC* pDC) 
{
	CRect rc;
	GetClientRect(&rc);
	pDC->FillSolidRect(&rc,VipColor.BACKCOLOR);
	return TRUE;
}

HBRUSH CInfoDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	if(nCtlColor==CTLCOLOR_STATIC && ( pWnd->GetDlgCtrlID()!=IDC_VOLUME && 
									   pWnd->GetDlgCtrlID()!=IDC_OPEN &&	
									   pWnd->GetDlgCtrlID()!=IDC_CLOSE &&
									   pWnd->GetDlgCtrlID()!=IDC_MAX &&
									   pWnd->GetDlgCtrlID()!=IDC_MIN &&
									   pWnd->GetDlgCtrlID()!=IDC_ZANGFU)
	  )
	{
		pDC->SetBkMode(TRANSPARENT);
		pDC->SetTextColor(VipColor.TXTCOLOR);
		return (HBRUSH)m_br;
	}
	return CDialog::OnCtlColor(pDC,pWnd,nCtlColor);
}

BOOL CInfoDlg::PreTranslateMessage(MSG* pMsg) 
{
	//将除ESC外的所有键盘输入都发给激活的视图
	if(pMsg->message==WM_KEYDOWN)
	{
		if(pMsg->wParam==96) //对于小键盘上的0作特殊处理
			pMsg->wParam=48;
		if((pMsg->wParam>=0x30&&pMsg->wParam<=0x39)||(pMsg->wParam>='a'&&pMsg->wParam<='z')
			||(pMsg->wParam>='A'&&pMsg->wParam<='Z')||pMsg->wParam== '.'||pMsg->wParam== '?')
			pMsg->hwnd = theViewManager.pActiveView->m_hWnd;
	}
	
	return CDialog::PreTranslateMessage(pMsg);
}
