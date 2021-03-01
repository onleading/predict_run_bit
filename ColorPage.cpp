// ColorPage.cpp : implementation file

#include "stdafx.h"
#include "TdxW.h"
#include "ColorPage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CColorPage property page

IMPLEMENT_DYNCREATE(CColorPage, CPropertyPage)

CColorPage::CColorPage() : CPropertyPage(CColorPage::IDD)
{
	bModify = FALSE;
	nCurSelScheme = CurrentScheme;
}

CColorPage::~CColorPage()
{
}

void CColorPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CColorPage)
	DDX_Control(pDX, IDC_SCHEMEITEM_LIST, m_SchemeItem_List);
	DDX_Control(pDX, IDC_SCHEME_COMBO, m_Scheme_Combo);
	DDX_Control(pDX, IDC_PREVIEW_BOX, m_Preview_Box);
	DDX_Control(pDX, IDC_COLOR_BOX, m_Color_Box);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CColorPage, CPropertyPage)
	//{{AFX_MSG_MAP(CColorPage)
	ON_WM_PAINT()
	ON_BN_CLICKED(IDC_OTHERCOLOR, OnOthercolor)
	ON_CBN_SELCHANGE(IDC_SCHEME_COMBO, OnSelchangeSchemeCombo)
	ON_LBN_SELCHANGE(IDC_SCHEMEITEM_LIST, OnSelchangeSchemeitemList)
	ON_WM_LBUTTONDOWN()
	ON_BN_CLICKED(IDC_SCHEME_SAVEAS, OnSchemeSaveas)
	ON_BN_CLICKED(IDC_DELETE_SCHEME, OnDeleteScheme)
	ON_BN_CLICKED(IDC_DEFULTFONT, OnDefaultfont)
	ON_COMMAND_RANGE(IDC_FONT1,IDC_FONT7,OnFontCfg)
	ON_BN_CLICKED(IDC_FONT, OnFont)
	ON_BN_CLICKED(IDC_HOMEPAGE_BTN, OnHomepageBtn)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CColorPage message handlers

BOOL CColorPage::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	GetDlgItem(IDC_HOMEPAGE_BTN)->ShowWindow(g_bHasHomePage);
	GetDlgItem(IDC_HOMEPAGE_BTN)->SetWindowText(g_GH.bShowHomePage?_F("关闭系统主页"):_F("显示系统主页"));
	
	for(int i = 0; i < SchemeNum; i++)
		m_Scheme_Combo.InsertString(i,_F(SchemeStr[i]));	//SchemeStr为一全局变量
	for(i = 0; i < 28; i++)
		m_SchemeItem_List.InsertString(i,_F(theItemContent[i].str));

	UI = Scheme[CurrentScheme];
	m_Scheme_Combo.SetCurSel(CurrentScheme);
	nCurSelScheme = CurrentScheme;
	if(nCurSelScheme == 0)
	{
		bNoSelf = FALSE;
		GetDlgItem(IDC_DELETE_SCHEME)->EnableWindow(FALSE);
		GetDlgItem(IDC_SCHEME_SAVEAS)->EnableWindow(TRUE);
	}
	else if(nCurSelScheme==1 || nCurSelScheme==2 || nCurSelScheme==3 || nCurSelScheme==4 || nCurSelScheme==5)
	{
		bNoSelf = TRUE;
		GetDlgItem(IDC_DELETE_SCHEME)->EnableWindow(FALSE);
		GetDlgItem(IDC_SCHEME_SAVEAS)->EnableWindow(FALSE);
	}
	else
	{
		bNoSelf = TRUE;
		GetDlgItem(IDC_DELETE_SCHEME)->EnableWindow(TRUE);
		GetDlgItem(IDC_SCHEME_SAVEAS)->EnableWindow(FALSE);
	}
	m_SchemeItem_List.SetCurSel(0);
	nCurSelItem = 0;

	bModify = FALSE;
	m_Preview_Box.GetWindowRect(&PreviewRect);
	ScreenToClient(&PreviewRect);
	m_Preview_Box.ShowWindow(SW_HIDE);

	return TRUE;
}

void CColorPage::OnPaint() 
{
	int i,j;
	CPaintDC dc(this);
	CRect rc = CRect(PreviewRect.left-1,PreviewRect.top+7,PreviewRect.right,PreviewRect.bottom+2);
    CDC memDC;								//作出内存DC
	CBitmap tempBitmap;						//作出临时位图
	memDC.CreateCompatibleDC (&dc);

	tempBitmap.CreateCompatibleBitmap (&dc,rc.Width ()+rc.left,rc.Height()+rc.top);
	CBitmap *pOldBitmap = memDC.SelectObject (&tempBitmap);
	memDC.SetBkMode(TRANSPARENT);
	g_d.Bar(&memDC,rc.left,rc.top,rc.right,rc.bottom,UI.VipColor.BACKCOLOR);
	CFont *oldfont = g_d.SelectFont(&memDC,GRAPH_FONT);
	//坐标颜色
	g_d.DrawLine(&memDC,rc.left+5,rc.top+17,rc.left+5,rc.top+114,UI.VipColor.AXISCOLOR);
	g_d.DrawLine(&memDC,rc.left+5,rc.top+114,rc.left+114,rc.top+114,UI.VipColor.AXISCOLOR);
	g_d.DisplayText(&memDC,rc.left+4,rc.top+115,UI.VipColor.AXISCOLOR,_F("坐标"));
	
	//时间颜色
	g_d.DisplayText(&memDC,rc.left+30,rc.top+115,UI.VipColor.TIMECOLOR,"99/01/01 12:12");
	g_d.DisplayText(&memDC,rc.left+3,rc.top+1,UI.VipColor.BIGTITLECOLOR,_F("大标题"));
	g_d.DisplayText(&memDC,rc.left+70,rc.top+1,UI.VipColor.TXTCOLOR,_F("图形文字"));
	g_d.DisplayText(&memDC,rc.left+122,rc.top+1,UI.VipColor.VOLCOLOR,_F("成交量:"));
	//高亮背景色
	memDC.SetBkMode(OPAQUE);
	memDC.SetBkColor(UI.VipColor.FLOATCURSORCOLOR);
	g_d.DisplayText(&memDC,rc.left+8,rc.top+18,UI.VipColor.TXTCOLOR,_F("高亮背景色"));
	memDC.SetBkMode(TRANSPARENT);

	//成交量
	g_d.DrawLine(&memDC,rc.left+165,rc.top+8,rc.right-5,rc.top+8,UI.VipColor.VOLCOLOR);

	static char *ZBStr[6] = {"输出一:","输出二:","输出三:","输出四:","输出五:","输出六:"};
	for(i = 0 ; i < 6;i++)
	{
		g_d.DisplayText(&memDC,rc.left+122,rc.top+16+13*i,UI.VipColor.ZBCOLOR[i],_F(ZBStr[i]));
		g_d.DrawLine(&memDC,rc.left+165,rc.top+22+13*i,rc.right-5,rc.top+22+13*i,UI.VipColor.ZBCOLOR[i]);
	}
	//美国线
	g_d.DisplayText(&memDC,rc.left+122,rc.top+16+13*i,UI.VipColor.USCOLOR,_F("其它线:"));
	g_d.DrawLine(&memDC,rc.left+165,rc.top+22+13*i,rc.right-5,rc.top+22+13*i,UI.VipColor.USCOLOR);

	g_d.DisplayText(&memDC,rc.left+122,rc.top+16+13*7,UI.VipColor.DRAWLINETOOLCOLOR,_F("画线工具:"));
	g_d.DrawLine(&memDC,rc.left+175,rc.top+22+13*7,rc.right-5,rc.top+22+13*7,UI.VipColor.DRAWLINETOOLCOLOR);

	//阳线,阴线,平线颜色
	CRect DrawRc;
	DrawRc.CopyRect(&rc);
	DrawRc.top += 8;
	g_d.Bar(&memDC,DrawRc.left+15,DrawRc.top+50,DrawRc.left+30,DrawRc.top+90,UI.VipColor.KNCOLOR);
	g_d.DrawLine(&memDC,DrawRc.left+22,DrawRc.top+40,DrawRc.left+22,DrawRc.top+100,UI.VipColor.KNCOLOR);
	g_d.Bar(&memDC,DrawRc.left+35,DrawRc.top+50,DrawRc.left+50,DrawRc.top+90,UI.VipColor.KNCOLOR);
	g_d.DrawLine(&memDC,DrawRc.left+42,DrawRc.top+40,DrawRc.left+42,DrawRc.top+100,UI.VipColor.KNCOLOR);
	g_d.DisplayText(&memDC,DrawRc.left+20,DrawRc.top+25,UI.VipColor.KNCOLOR,_F("阴线"));

	g_d.DrawRect(&memDC,DrawRc.left+55,DrawRc.top+50,DrawRc.left+70,DrawRc.top+90,UI.VipColor.KPCOLOR);
	g_d.DrawLine(&memDC,DrawRc.left+62,DrawRc.top+40,DrawRc.left+62,DrawRc.top+50,UI.VipColor.KPCOLOR);
	g_d.DrawLine(&memDC,DrawRc.left+62,DrawRc.top+90,DrawRc.left+62,DrawRc.top+100,UI.VipColor.KPCOLOR);
	g_d.DrawRect(&memDC,DrawRc.left+75,DrawRc.top+50,DrawRc.left+90,DrawRc.top+90,UI.VipColor.KPCOLOR);
	g_d.DrawLine(&memDC,DrawRc.left+82,DrawRc.top+40,DrawRc.left+82,DrawRc.top+50,UI.VipColor.KPCOLOR);
	g_d.DrawLine(&memDC,DrawRc.left+82,DrawRc.top+90,DrawRc.left+82,DrawRc.top+100,UI.VipColor.KPCOLOR);
	g_d.DisplayText(&memDC,DrawRc.left+60,DrawRc.top+25,UI.VipColor.KPCOLOR,_F("阳线"));

	g_d.DisplayText(&memDC,DrawRc.left+95,DrawRc.top+25,UI.VipColor.LEVELCOLOR,_F("平线"));
	g_d.DrawLine(&memDC,DrawRc.left+100,DrawRc.top+40,DrawRc.left+100,DrawRc.top+100,UI.VipColor.LEVELCOLOR);
	g_d.DrawLine(&memDC,DrawRc.left+110,DrawRc.top+40,DrawRc.left+110,DrawRc.top+100,UI.VipColor.LEVELCOLOR);

	//画出行情预览区
	CRect HQrc(rc.left+3,rc.top+130,rc.right-3,rc.bottom-2);
	static char *tempTitleStr[5] = {""," 名称"," 现价","  总量"," 其它"};
		
	int xtmp = 0,width = 30;
	g_d.DrawLine(&memDC,HQrc.left,HQrc.top,HQrc.left,HQrc.top+88,UI.GridColor.GridLineColor);
	for(j = 0;j < 5;j++)
	{
		g_d.DisplayText(&memDC,HQrc.left+xtmp+2,HQrc.top+4,UI.GridColor.GridTitleTextColor,_F(tempTitleStr[j]));
		if(j == 2)	g_d.DisplayText(&memDC,HQrc.left+xtmp+width-10,HQrc.top+4,UI.GridColor.GridTitleArrowColor,_F("↓"));
		xtmp += width;	
		g_d.DrawLine(&memDC,HQrc.left+xtmp-1,HQrc.top,HQrc.left+xtmp-1,HQrc.top+88,UI.GridColor.GridLineColor);
		switch(j)
		{
		case 0:
			width = 50;
			break;
		case 1:
			width = 40;
			g_d.DisplayText(&memDC,HQrc.left+30+2,HQrc.top+22+4,UI.GridColor.Grid_CodeName_Color,_F("证券一"));
			g_d.DisplayText(&memDC,HQrc.left+30+2,HQrc.top+44+4,UI.GridColor.Grid_CodeName_Color,_F("证券二"));
			g_d.DisplayText(&memDC,HQrc.left+30+2,HQrc.top+66+4,UI.GridColor.Grid_CodeName_Color,_F("证券三"));
			break;
		case 2:
			g_d.DisplayText(&memDC,HQrc.left+80+2,HQrc.top+22+4,UI.GridColor.Grid_Down_Color,"6.83");
			g_d.DisplayText(&memDC,HQrc.left+80+2,HQrc.top+44+4,UI.GridColor.Grid_Level_Color,"5.85");
			g_d.DisplayText(&memDC,HQrc.left+80+2,HQrc.top+66+4,UI.GridColor.Grid_Up_Color,"3.16");
			width = 50;
			break;
		case 3:
			g_d.DisplayText(&memDC,HQrc.left+120+2,HQrc.top+22+4,UI.GridColor.Grid_Volume_Color,"18414");
			g_d.DisplayText(&memDC,HQrc.left+120+2,HQrc.top+44+4,UI.GridColor.Grid_Volume_Color," 12981");
			g_d.DisplayText(&memDC,HQrc.left+120+2,HQrc.top+66+4,UI.GridColor.Grid_Volume_Color," 28291");
			width = 40;
			break;
		case 4:
			g_d.DisplayText(&memDC,HQrc.left+170+2,HQrc.top+22+4,UI.GridColor.Grid_Other_Color,"10.38");
			g_d.DisplayText(&memDC,HQrc.left+170+2,HQrc.top+44+4,UI.GridColor.Grid_Other_Color,"10.30");
			g_d.DisplayText(&memDC,HQrc.left+170+2,HQrc.top+66+4,UI.GridColor.Grid_Other_Color,"11.23");
			break;
		}
	}
	CString str;
	g_d.DrawLine(&memDC,HQrc.left,HQrc.top,HQrc.left+xtmp,HQrc.top,UI.GridColor.GridLineColor);
	for(i = 0; i < 3;i++)
	{	
		str.Format("%4d",i+1);
		g_d.DisplayText(&memDC,HQrc.left,HQrc.top+(i+1)*22+3,UI.VipColor.TXTCOLOR,str.GetBuffer(0));
		g_d.DrawLine(&memDC,HQrc.left,HQrc.top+(i+1)*22,HQrc.left+xtmp,HQrc.top+(i+1)*22,UI.GridColor.GridLineColor);
	}
	g_d.DrawLine(&memDC,HQrc.left,HQrc.top+(i+1)*22,HQrc.left+xtmp,HQrc.top+(i+1)*22,UI.GridColor.GridLineColor);

	int tempMode = memDC.SetROP2 (R2_XORPEN);
	g_d.DrawLine(&memDC,HQrc.left+30,HQrc.top+66,HQrc.left+79,HQrc.top+66,UI.GridColor.GridHighLineColor^UI.VipColor.BACKCOLOR,PS_SOLID,2);
	memDC.SetROP2 (tempMode);

	memDC.SetBkMode(OPAQUE);
	memDC.SetBkColor(UI.TxtColor.TxtBackColor);
	g_d.DisplayText(&memDC,HQrc.left+20,HQrc.bottom-16,UI.TxtColor.TxtForeColor,_F("资料和资讯文本"));
	memDC.SetBkColor(UI.TreeColor.TreeBackColor);
	g_d.DisplayText(&memDC,HQrc.left+120,HQrc.bottom-16,UI.TreeColor.TreeForeColor,_F("功能树文本"));

	dc.BitBlt (rc.left,rc.top,rc.Width(),rc.Height(),&memDC,rc.left,rc.top,SRCCOPY);
	
	g_d.RestoreFont(&memDC,oldfont);
	memDC.SelectObject(pOldBitmap);
	tempBitmap.DeleteObject();
	memDC.DeleteDC();

	DrawColorBox(&dc);	
}

void CColorPage::DrawColorBox(CDC *pDC,CPoint point)
{
	CRect Corc;
	m_Color_Box.GetWindowRect(&Corc);
	ScreenToClient(&Corc);

	int x = Corc.left + 6;
	int y = Corc.top + 12;

	//25种标准颜色
	static COLORREF StandardColor[5][5] =
	{
		RGB(0,0,0),RGB(255,255,255),RGB(255,0,0),RGB(0,255,0),RGB(0,0,255),
		RGB(255,255,0),RGB(0,255,255),RGB(255,0,255),RGB(128,128,0),RGB(128,128,128),
		RGB(128,0,128),RGB(0,128,128),RGB(0,0,128),RGB(0,128,0),RGB(128,0,0),
		RGB(192,192,192),RGB(128,0,255),RGB(255,0,128),RGB(0,128,255),RGB(0,255,128),
		RGB(128,255,0),RGB(255,128,0),RGB(0,128,192),RGB(0,192,128),RGB(128,192,0)
	};
	
	for(int j = 0; j < 5; j++)
	{
		for(int i = 0; i < 5;i++)
		{
			if(point.x == 0)
			{
				g_d.Bar(pDC,x+i*30,y+j*20,x+(i+1)*30,y+(j+1)*20,StandardColor[j][i]);
				g_d.DrawRect(pDC,x+i*30,y+j*20,x+(i+1)*30,
					y+(j+1)*20,RGB(192,192,192),PS_SOLID,2);

			}
			else
			{
				CRect tmp(x+i*30,y+j*20,x+(i+1)*30,y+(j+1)*20);
				if(tmp.PtInRect(point))
				{
					g_d.DrawRect(pDC,x+i*30,y+j*20,x+(i+1)*30,
							y+(j+1)*20,RGB(255,255,255),PS_SOLID,2);
					SetColor(StandardColor[j][i]);
				}
				else
				{
					g_d.DrawRect(pDC,x+i*30,y+j*20,x+(i+1)*30,
						y+(j+1)*20,RGB(192,192,192),PS_SOLID,2);
				}
			}
			x += 10;
		}
		x = Corc.left+6;
		y += 6;
	}
}

void CColorPage::SetColor(COLORREF Color)
{
	if(nCurSelItem < 0)
		return;
	if(bNoSelf)		//如果在标准方案中改变某一项，则自动切换到自定义方案
	{
		bNoSelf = FALSE;
		m_Scheme_Combo.SetCurSel(0);
		nCurSelScheme = 0;
		GetDlgItem(IDC_DELETE_SCHEME)->EnableWindow(FALSE);
		GetDlgItem(IDC_SCHEME_SAVEAS)->EnableWindow(TRUE);
	}
	bModify = TRUE;
	int ItemID = theItemContent[nCurSelItem].id;
	switch(ItemID)
	{
	case 0://背景
		UI.VipColor.BACKCOLOR = Color;
		UI.GridColor.GridBackColor = Color;
		break;
	case 1://阳线及上涨字
		UI.VipColor.KPCOLOR = Color;
		UI.GridColor.Grid_Up_Color = Color;
		break;
	case 2://阴线
		UI.VipColor.KNCOLOR=Color;
		break;
	case 3://下跌字
		UI.VipColor.DOWNCOLOR=Color;
		UI.GridColor.Grid_Down_Color = Color;
		break;
	case 4://平线
		UI.VipColor.LEVELCOLOR=Color;
		UI.GridColor.Grid_Level_Color = Color;
		break;
	case 5:
		UI.VipColor.USCOLOR=Color;
		break;
	case 6://坐标颜色
		UI.VipColor.AXISCOLOR=Color;
		break;
	case 7:
		UI.VipColor.BIGTITLECOLOR=Color;
		break;
	case 8://坐标文字颜色
		UI.VipColor.TIMECOLOR=Color;
		break;
	case 9://成交量颜色
		UI.VipColor.VOLCOLOR=Color;
		UI.GridColor.Grid_Volume_Color = Color;
		break;
	case 10:
		UI.VipColor.TXTCOLOR=Color;
		break;
	case 11:
		UI.VipColor.FLOATCURSORCOLOR=Color;
		break;
	case 12:
		UI.VipColor.DRAWLINETOOLCOLOR=Color;
		break;
	case 13://输出1颜色
	case 14://输出2颜色
	case 15://输出3颜色
	case 16://输出4颜色
	case 17://输出5颜色
	case 18://输出6颜色
		UI.VipColor.ZBCOLOR[nCurSelItem-13] = Color;
		break;
	case 19:
		UI.GridColor.GridLineColor = Color;
		break;
	case 20:
		UI.GridColor.GridHighLineColor = Color;
		break;
	case 21:
		UI.GridColor.GridTitleTextColor = Color;
		break;
	case 22:
		UI.GridColor.GridTitleArrowColor = Color;
		break;
	case 23:
		UI.GridColor.Grid_CodeName_Color = Color;
		break;
	case 24:
		UI.GridColor.Grid_Other_Color = Color;
		break;
	case 25:
		UI.TxtColor.TxtForeColor = Color;
		break;
	case 26:
		UI.TreeColor.TreeForeColor = Color;
		break;		
	case 27:
		UI.TreeColor.TreeBackColor = Color;
		UI.TxtColor.TxtBackColor = Color;		
		break;		
	}
	Invalidate(FALSE);
}

void CColorPage::OnOthercolor() 
{
	CColorDialog CDlg(0,2);
	if( CDlg.DoModal() == IDOK )
		SetColor(CDlg.m_cc.rgbResult);
}

void CColorPage::OnSelchangeSchemeCombo()
{
	if( nCurSelScheme != m_Scheme_Combo.GetCurSel() )
	{
		if(nCurSelScheme == 0) //如果从自定义方案切换到其它方案，则将自定义方案存盘
			Scheme[0] = UI;
		nCurSelScheme = m_Scheme_Combo.GetCurSel();
		if(nCurSelScheme != CB_ERR)
		{
			if(nCurSelScheme == 0)
			{
				bNoSelf = FALSE;
				GetDlgItem(IDC_DELETE_SCHEME)->EnableWindow(FALSE);
				GetDlgItem(IDC_SCHEME_SAVEAS)->EnableWindow(TRUE);
			}
			else if(nCurSelScheme==1 || nCurSelScheme==2 || nCurSelScheme==3 || nCurSelScheme==4 || nCurSelScheme==5)
			{
				bNoSelf = TRUE;
				GetDlgItem(IDC_DELETE_SCHEME)->EnableWindow(FALSE);
				GetDlgItem(IDC_SCHEME_SAVEAS)->EnableWindow(FALSE);
			}
			else
			{
				bNoSelf = TRUE;
				GetDlgItem(IDC_DELETE_SCHEME)->EnableWindow(TRUE);
				GetDlgItem(IDC_SCHEME_SAVEAS)->EnableWindow(FALSE);
			}
			UI = Scheme[nCurSelScheme];
			Invalidate(FALSE);
		}
	}
}

void CColorPage::OnSelchangeSchemeitemList() 
{
	nCurSelItem = m_SchemeItem_List.GetCurSel();

}

void CColorPage::OnLButtonDown(UINT nFlags, CPoint point) 
{
	CDC *pDC = GetDC();
	DrawColorBox(pDC,point);
	ReleaseDC(pDC);
	CPropertyPage::OnLButtonDown(nFlags, point);
}

#include "InputName.h"
void CColorPage::OnSchemeSaveas() 
{
	if(SchemeNum == 20)
	{
		TDX_MessageBox(m_hWnd,"配色方案太多，不能再添加!",MB_OK|MB_ICONEXCLAMATION);
		return;
	}
	CInputName dlg;
	dlg.HintInfo = "将此配色方案另存为:";
	dlg.Caption = "保存方案";
	if( dlg.DoModal() == IDOK )
	{
		TrimString(dlg.m_NewName);
		if(dlg.m_NewName.IsEmpty () == FALSE)
		{
			if( dlg.m_NewName.GetLength () > 20 )
				dlg.m_NewName = dlg.m_NewName.Left(20);
			int i;
			BOOL bFind = FALSE;
			//如果在当前方案中有此名称的方案，则覆盖之
			for(i = 0;i < SchemeNum;i++)
			{
				TrimString(SchemeStr[i]);
				if(SchemeStr[i] == dlg.m_NewName)
				{
					bFind = TRUE;
					break;
				}
			}
			if(bFind)	Scheme[i] = UI;
			else 
			{
				SchemeStr[SchemeNum] = dlg.m_NewName;
				Scheme[SchemeNum] = UI;
				SchemeNum++;

				m_Scheme_Combo.ResetContent();
				for(i = 0; i < SchemeNum; i++)
					m_Scheme_Combo.InsertString(i,_F(SchemeStr[i]));
				m_Scheme_Combo.SetCurSel(nCurSelScheme);
			}
		}
	}
}

void CColorPage::OnDeleteScheme() 
{
	if(nCurSelScheme == CurrentScheme)
	{
		TDX_MessageBox(m_hWnd,"不能删除当前正在使用的方案!",MB_OK|MB_ICONEXCLAMATION);
		return;
	}
	if(TDX_MessageBox(m_hWnd,"你确定删除此方案吗?",MB_OKCANCEL|MB_ICONINFORMATION) != IDOK )
		return;
	int i;
	for(i = nCurSelScheme;i < SchemeNum - 1;i++)
	{
		Scheme[i] = Scheme[i+1];
		SchemeStr[i] = SchemeStr[i+1];
	}
	if(nCurSelScheme < CurrentScheme)
		CurrentScheme--;
	SchemeNum --;
	nCurSelScheme --; //当前所选的方案向前移

	m_Scheme_Combo.ResetContent();
	for(i = 0; i < SchemeNum; i++)
		m_Scheme_Combo.InsertString(i,_F(SchemeStr[i]));	//SchemeStr为一全局变量

	UI = Scheme[nCurSelScheme];
	m_Scheme_Combo.SetCurSel(nCurSelScheme);
	if(nCurSelScheme == 0)
	{
		bNoSelf = FALSE;
		GetDlgItem(IDC_DELETE_SCHEME)->EnableWindow(FALSE);
		GetDlgItem(IDC_SCHEME_SAVEAS)->EnableWindow(TRUE);
	}
	if(nCurSelScheme == 1 || nCurSelScheme == 2 || nCurSelScheme == 3 || nCurSelScheme == 4 || nCurSelScheme == 5)
	{
		GetDlgItem(IDC_DELETE_SCHEME)->EnableWindow(FALSE);
		GetDlgItem(IDC_SCHEME_SAVEAS)->EnableWindow(FALSE);
	}
	Invalidate(FALSE);
}

void CColorPage::OnFontCfg(UINT nID)
{
	int iNo=nID-IDC_FONT1;
	LOGFONT lf;
	memset(&lf, 0, sizeof(LOGFONT));
	lf.lfHeight = g_nFontSize[iNo];
	lf.lfWeight = g_nFontWeigth[iNo];
	strcpy(lf.lfFaceName,g_strFontName[iNo]);
	CFontDialog dlg(&lf);
	if(dlg.DoModal() == IDOK)
	{
		memcpy(&lf, dlg.m_cf.lpLogFont, sizeof(LOGFONT));
		strncpy(g_strFontName[iNo],lf.lfFaceName,38);
		g_strFontName[iNo][38]=0;
		if(lf.lfHeight!=0)
			g_nFontSize[iNo] = lf.lfHeight;
		if(lf.lfWeight!=0)
			g_nFontWeigth[iNo] = lf.lfWeight;
		bModify = TRUE;
	}	
}

void CColorPage::OnFont() 
{
	CMenu menu;
	menu.LoadMenu(IDR_OTHERMENU);
	CMenu *pMenu=(CMenu *)menu.GetSubMenu(0);
	
	CRect rect;
	GetDlgItem(IDC_FONT)->GetWindowRect(&rect);
	pMenu->TrackPopupMenu(TPM_LEFTALIGN,rect.left,rect.bottom+2,this);	
}

void CColorPage::OnDefaultfont() 
{
	if(TDX_MessageBox(m_hWnd,"你确定将字体都恢复到系统缺省设置吗?",MB_YESNO|MB_DEFBUTTON2|MB_ICONQUESTION)!=IDYES)
		return;
	g_nFontWeigth[0]=g_nFontWeigth[1]=g_nFontWeigth[2]=g_nFontWeigth[3]=g_nFontWeigth[4]=g_nFontWeigth[5]=FW_LIGHT;
	g_nFontWeigth[6]=FW_BOLD;
	strcpy(g_strFontName[0],"Tahoma");
	g_nFontSize[0] = 18;
	strcpy(g_strFontName[1],"Tahoma");
	g_nFontSize[1] = 16;	
	if(!g_b2000XPSys) g_nFontWeigth[1]=FW_BOLD;
	strcpy(g_strFontName[2],KAI_FONT);
	g_nFontSize[2] = 18;	
	strcpy(g_strFontName[3],"Tahoma");
	g_nFontSize[3] = 16;
	strcpy(g_strFontName[4],KAI_FONT);
	g_nFontSize[4] = 16;
	strcpy(g_strFontName[5],"Tahoma");
	g_nFontSize[5] = 18;
	strcpy(g_strFontName[6],"System");
	g_nFontSize[6] = 18;

	bModify = TRUE;
}

void CColorPage::OnHomepageBtn() 
{
	if(TDX_MessageBox(m_hWnd,"你确定此操作吗?\r\n(关闭或显示主页后,重新进入此程序才生效)",MB_OKCANCEL|MB_ICONINFORMATION) != IDOK )
		return;
	g_GH.bShowHomePage=!g_GH.bShowHomePage;
	GetDlgItem(IDC_HOMEPAGE_BTN)->SetWindowText(g_GH.bShowHomePage?_F("关闭系统主页"):_F("显示系统主页"));
}
