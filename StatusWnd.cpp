#include "StdAfx.h"
#include "TdxW.h"
#include "StatusWnd.h"
#include "WarnGPDlg.h"
#include "MainFrm.h"
#include "MyInterface.h"

#define ZS_BAR		0
#define	DK_BAR		1
#define INFO_BAR	2
#define INFOURL_BAR	3
#define DEEP_BAR	4
#define	RD_BAR		5
#define	WN_BAR		6
#define	SYS_BAR		7

//用于状态栏等处的颜色
#define STATUS_DOWN_COLOR      RGB(0,128,0)
#define STATUS_UP_COLOR        RGB(255,0,0)
#define STATUS_LEVEL_COLOR     RGB(255,255,255)

CFont	NewFont16System,NewFont14Arial,NewFont14Arial_2,NewFont12Song;

CStatusWnd::CStatusWnd()
{
	// 注册当前类
	RegisterWindowClass(NULL,_T("RaderKing"));

	m_nZsNum=2;	
	NewFont16System.CreateFont(16,0,0,0,FW_SEMIBOLD,0,0,0,g_b2000XPSys?ANSI_CHARSET:DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,g_b2000XPSys?ANTIALIASED_QUALITY:DEFAULT_QUALITY,DEFAULT_PITCH|FF_DONTCARE,"System");
	NewFont14Arial.CreateFont(14,0,0,0,FW_SEMIBOLD,0,0,0,DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,ANTIALIASED_QUALITY,DEFAULT_PITCH|FF_DONTCARE,"Arial");
	NewFont14Arial_2.CreateFont(14,0,0,0,FW_NORMAL,0,0,0,DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,ANTIALIASED_QUALITY,DEFAULT_PITCH|FF_DONTCARE,"Arial");
	NewFont12Song.CreateFont(12,0,0,0,FW_NORMAL,0,0,0,DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,DEFAULT_PITCH|FF_DONTCARE,SONG_FONT);
}

BOOL CStatusWnd::RegisterWindowClass(HINSTANCE hInstance,LPCTSTR lpszClassName)
{
	WNDCLASS WndClass={0};
	ZeroMemory(&WndClass,sizeof(WndClass));
	WndClass.style=CS_PARENTDC|CS_DBLCLKS;
	WndClass.lpfnWndProc=::DefWindowProc;
	WndClass.hInstance=hInstance?hInstance:(AfxGetModuleState()->m_hCurrentInstanceHandle);
	WndClass.hCursor=AfxGetApp()->LoadStandardCursor(IDC_ARROW);
	WndClass.lpszClassName=lpszClassName;
	WndClass.hIcon=NULL;
	return AfxRegisterClass(&WndClass);
}

CStatusWnd::~CStatusWnd()
{
	KillTimer(1);
	NewFont16System.DeleteObject();
	NewFont14Arial.DeleteObject();
	NewFont14Arial_2.DeleteObject();
	NewFont12Song.DeleteObject();
}

BEGIN_MESSAGE_MAP(CStatusWnd, CWnd)
	//{{AFX_MSG_MAP(CStatusWnd)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_MOUSEMOVE()
	ON_WM_PAINT()
	ON_WM_TIMER()
	ON_WM_SIZE()
	ON_WM_CREATE()
	ON_WM_ERASEBKGND()
	//}}AFX_MSG_MAP
	ON_MESSAGE(UM_MDITABSELCHANGED,  OnTabSelChanged)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CStatusWnd message handlers

int CStatusWnd::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect rect;
	GetClientRect(&rect);
	m_wndSwitcher.DoCreate(this, 0, 0, 0);

	m_PreviewTool.InitPreviewTool(this);
	if(g_Scheme_King.bStatusSwitch) 
		SetTimer(1, 500, NULL);
	
	return 0;
}

void CStatusWnd::OnTimer(UINT nIDEvent) 
{
	if(nIDEvent==1)
		RefreshMDITab();
	
	CWnd::OnTimer(nIDEvent);
}

void CStatusWnd::OnSize(UINT nType, int cx, int cy) 
{
	CWnd::OnSize(nType, cx, cy);

	CRect zsrect,rect;
	GetClientRect(rect);
	GetItemRect(ZS_BAR, &zsrect);
	if(g_Scheme_King.bStatusSwitch&&m_wndSwitcher.m_hWnd)
		m_wndSwitcher.SetWindowPos(NULL, zsrect.right, rect.top, rect.Width()-zsrect.Width(), rect.Height(), SWP_NOZORDER);
}

void CStatusWnd::DrawItem(CDC *pDC,int nBarID)
{
	CRect rect;
	GetItemRect(nBarID,&rect);
	switch(nBarID)
	{
		case ZS_BAR:
		{
			if(g_nEmbedTabSel==LOGIN_ONLYWT && !OnLine) return;

			int tmpTop=rect.top+2;
			CRect zsRect = rect;
			//
			CFont *OldFont;
			if(g_GH.bBigStatusBar)
				OldFont=pDC->SelectObject(&NewFont16System);
			else
			{
				OldFont=pDC->SelectObject(&NewFont12Song);
				tmpTop+=1;
			}
			pDC->SetTextColor(VipColor.BIGTITLECOLOR);
			for(int i=0;i<m_nZsNum;i++)
			{
				if(i>0) 
					g_d.DrawLine(pDC,zsRect.left+i*zsRect.Width()/m_nZsNum-2,zsRect.top,zsRect.left+i*zsRect.Width()/m_nZsNum-2,zsRect.bottom,RGB(128,128,128));
				pDC->TextOut(zsRect.left+i*zsRect.Width()/m_nZsNum,tmpTop,g_StatusGP[i].lpScript);
			}
			pDC->SelectObject(OldFont);
			
			CRect sig_rect;
			m_ToolRectHelp = zsRect;
			for(i=0;i<m_nZsNum;i++)
			{
				sig_rect=zsRect;
				sig_rect.left=zsRect.left+i*zsRect.Width()/m_nZsNum;
				sig_rect.right=sig_rect.left+zsRect.Width()/m_nZsNum;				
				sig_rect.top+=1;
				DrawZSBar(pDC,sig_rect,i);
			}
			return;
		}
	}	
}

void CStatusWnd::OnPaint() 
{
	CRect	rect;
	GetClientRect(rect);
	if(rect.Width()<=0 || rect.Height()<=0)
		return;

	CDC		memDC,*pDC = GetDC();
	memDC.CreateCompatibleDC(pDC);
	CBitmap bmp,*pOldBmp;
	bmp.CreateCompatibleBitmap(pDC,rect.Width(),rect.Height());
	pOldBmp = memDC.SelectObject(&bmp);
	memDC.SetBkMode(TRANSPARENT);
	g_d.Bar(pDC, rect.left, rect.top, rect.right, rect.bottom, VipColor.BACKCOLOR);

	DrawItem(&memDC,ZS_BAR);
	CRect zsrect;
	GetItemRect(ZS_BAR, &zsrect);	
	pDC->BitBlt(0,0,zsrect.Width(),zsrect.Height(),&memDC,0,0,SRCCOPY);
	memDC.SelectObject(pOldBmp);
	bmp.DeleteObject();
	memDC.DeleteDC();
	ReleaseDC(pDC);

	CWnd::OnPaint();
}

BOOL CStatusWnd::OnEraseBkgnd(CDC* pDC) 
{
	return FALSE;
}

void CStatusWnd::GetItemRect(int nBarID,CRect *pRC)
{
	pRC->SetRectEmpty();
	if(nBarID<ZS_BAR || nBarID>SYS_BAR)
		return;
	CRect TotalRc;
	GetClientRect(TotalRc);
	int nWid=TotalRc.Width();
	int BarLeft[8]={0},BarWidth[8]={0};

	BarWidth[0]=618; 
	BarWidth[1]=108; BarWidth[3]=18; BarWidth[4]=18; BarWidth[5]=18; BarWidth[6]=18; BarWidth[7]=10;
	BarWidth[2]=max(0,nWid-BarWidth[0]-BarWidth[1]-BarWidth[3]-BarWidth[4]-BarWidth[5]-BarWidth[6]-BarWidth[7]);

	BarLeft[0]=0;
	BarLeft[1]=BarLeft[0]+BarWidth[0];
	BarLeft[2]=BarLeft[1]+BarWidth[1];
	BarLeft[3]=BarLeft[2]+BarWidth[2];
	BarLeft[4]=BarLeft[3]+BarWidth[3];
	BarLeft[5]=BarLeft[4]+BarWidth[4];
	BarLeft[6]=BarLeft[5]+BarWidth[5];
	BarLeft[7]=BarLeft[6]+BarWidth[6];
	//
	*pRC=TotalRc;
//	pRC->left=BarLeft[nBarID];
//	pRC->right=pRC->left+BarWidth[nBarID];
}

//弹出动画区菜单,信息对话框,警示对话框
#include "ShowInfoDlg.h"
void CStatusWnd::OnLButtonDown(UINT nFlags, CPoint point) 
{
	CRect rect_zs(0,0,0,0),rect_info(0,0,0,0),rect_infourl(0,0,0,0),rect_deep(0,0,0,0),rect_rd(0,0,0,0),rect_wn(0,0,0,0),rect_sys(0,0,0,0);
	GetItemRect(ZS_BAR,&rect_zs);	//指数区
	if(g_GC.bSupportStatusRoll)
	{
		rect_zs.right=rect_zs.left+ROLLIcon_Width+1;
		if(rect_zs.PtInRect(point))
		{
			if(g_GH.Roll.bShow)
			{
				CNewMenu RollInfoMenu;
				RollInfoMenu.CreatePopupMenu();
				RollInfoMenu.AppendMenu(MF_STRING,IDC_ROLLINFO_SET,_F("滚动资讯设置"));
				RollInfoMenu.AppendMenu(MF_STRING,IDC_ROLLINFO_CLOSE,_F("关闭滚动资讯"));
				CRect PopRc=rect_zs;
				ClientToScreen(&PopRc);
				RollInfoMenu.TrackPopupMenu(TPM_LEFTALIGN|TPM_BOTTOMALIGN,PopRc.left,PopRc.top,AfxGetMainWnd());			
			}
			else
				AfxGetMainWnd()->PostMessage(WM_COMMAND,IDM_MOVINGINFO);
		}
	}
	if(rect_info.PtInRect(point))
	{
		if(OnLine)
		{
			if(g_URLEndTime > 0 && g_URL[0]!=0)
				ShellExecute(NULL, _T("open"), g_URL, NULL, NULL, SW_SHOW);
		}
		else
			AfxGetMainWnd()->SendMessage(WM_COMMAND,ID_CONNECT);
	}
	if(rect_sys.PtInRect(point))
	{
		CShowInfoDlg dlg;
		dlg.DoModal();
	}
	if(g_nEmbedTabSel==LOGIN_ONLYWT && !OnLine) 
		return;
	if(rect_deep.PtInRect(point))
		AfxGetMainWnd()->SendMessage(WM_COMMAND,ID_DEEPWARN);
	if(rect_rd.PtInRect(point))
		AfxGetMainWnd()->SendMessage(WM_COMMAND,ID_WARNGP);
	if(rect_wn.PtInRect(point))
		AfxGetMainWnd()->SendMessage(WM_COMMAND,ID_TJWARNGP);

	CWnd::OnLButtonDown(nFlags, point);
}

void CStatusWnd::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	CRect rc,zsRect,sig_rect;
	GetItemRect(ZS_BAR,&rc);
	zsRect=rc;
	if(g_GC.bSupportStatusRoll)
		zsRect.left+=ROLLIcon_Width+1;
	for(int i=0;i < m_nZsNum;i++)
	{
		sig_rect=zsRect;
		sig_rect.left=zsRect.left+i*zsRect.Width()/m_nZsNum;
		sig_rect.right=sig_rect.left+zsRect.Width()/m_nZsNum;				
		if(sig_rect.PtInRect(point))
			ToZst(g_StatusGP[i].Code,g_StatusGP[i].setcode);
	}
	CWnd::OnLButtonDblClk(nFlags, point);
}

void DrawNoBorderBar(CDC *pDC,int x1,int y1,int x2,int y2,COLORREF color)
{
	CPen *pOldPen=(CPen *)pDC->SelectStockObject (NULL_PEN);
	CBrush NewBrush(color);
	CBrush *pOldBrush=(CBrush *)pDC->SelectObject(&NewBrush);
	pDC->Rectangle (x1,y1,x2,y2);
	pDC->SelectObject(pOldPen);
	pDC->SelectObject(pOldBrush);
	NewBrush.DeleteObject();
}

void DrawDiffWord(CDC *pDC,int x1,int y1,char *tmpstr,int colorFlag)
{
	if(tmpstr[0] == '=')
	{
		CBitmap* pBitmap=new CBitmap;
		if( colorFlag == 1)
			pBitmap->LoadBitmap(IDB_LEVEL_GREEN_ARROW);
		else
			pBitmap->LoadBitmap(IDB_LEVEL_RED_ARROW);

		CDC srcDC; 
		srcDC.CreateCompatibleDC(pDC);
		CBitmap* pOldBitmap = srcDC.SelectObject(pBitmap);
		pDC->BitBlt(x1,y1+3,6,7,&srcDC, 0, 0, SRCCOPY);
		srcDC.SelectObject(pOldBitmap);
		pBitmap->DeleteObject();
		delete pBitmap;
	}
	else if(tmpstr[0] == 0x18)
	{
		CBitmap* pBitmap=new CBitmap;
		if( colorFlag == 1)
			pBitmap->LoadBitmap(IDB_UP_GREEN_ARROW);
		else
			pBitmap->LoadBitmap(IDB_UP_RED_ARROW);

		CDC srcDC; 
		srcDC.CreateCompatibleDC(pDC);
		CBitmap* pOldBitmap = srcDC.SelectObject(pBitmap);
		pDC->BitBlt(x1,y1+3,6,7,&srcDC, 0, 0, SRCCOPY);
		srcDC.SelectObject(pOldBitmap);
		pBitmap->DeleteObject();
		delete pBitmap;
	}
	else if(tmpstr[0] == 0x19)
	{
		CBitmap* pBitmap=new CBitmap;
		if(colorFlag == 1)
			pBitmap->LoadBitmap(IDB_DOWN_GREEN_ARROW);
		else
			pBitmap->LoadBitmap(IDB_DOWN_RED_ARROW);		
		CDC srcDC; 
		srcDC.CreateCompatibleDC(pDC);
		CBitmap* pOldBitmap = srcDC.SelectObject(pBitmap);
		pDC->BitBlt(x1,y1+3,6,7,&srcDC, 0, 0, SRCCOPY);
		srcDC.SelectObject(pOldBitmap);
		pBitmap->DeleteObject();
		delete pBitmap;
	}

}

void DrawBJWord(CDC *pDC,int x1,int y1,float value1,float value2,BOOL bXSFlag,int xsflag)
{
	COLORREF color;
	char tempstr[10];
	if(value1 > value2)			color = STATUS_UP_COLOR;
	else if(value1 < value2)	color = STATUS_DOWN_COLOR;
	else						color = STATUS_LEVEL_COLOR;

	CFont *OldFont;
	if(g_GH.bBigStatusBar)	
		OldFont=pDC->SelectObject(&NewFont16System);
	else					
		OldFont=pDC->SelectObject(&NewFont14Arial);
	pDC->SetTextColor (color);
	pDC->SetBkMode(TRANSPARENT);
	if(bXSFlag)
	{
		if(value1 >= 10000.0 && xsflag>=2)
			sprintf(tempstr,"%7.1f",value1);
		else if(xsflag>=3)
			sprintf(tempstr,"%7.3f",value1);
		else if(xsflag==2)
			sprintf(tempstr,"%7.2f",value1);
		else if(xsflag==1)
			sprintf(tempstr,"%7.1f",value1);
		else
			sprintf(tempstr,"%7.0f",value1);
	}
	else
	{
		if (value1 < 10000.0)				sprintf(tempstr,"%4.0f",  value1);
		else if (value1 < 100000000.0)		sprintf(tempstr,_F("%4.0f万"),value1/10000.0);
		else if (value1 < 100000000000.0)	sprintf(tempstr,_F("%5.2f亿"),value1/100000000.0);
		else if (value1 < 1000000000000.0)	sprintf(tempstr,_F("%4.0f亿"),value1/100000000.0);
		else								sprintf(tempstr,_F("%.2f兆"), value1/1000000000000.0);
	}
	pDC->TextOut(x1,y1,tempstr);
	pDC->SelectObject (OldFont);
}

void DrawZAFWord(CDC *pDC,int x1,int y1,float ftmp)
{
	char tempstr[80];
	sprintf(tempstr,"%.2f%%",ftmp);

	CFont *OldFont;
	if(g_GH.bBigStatusBar)	
		OldFont=pDC->SelectObject(&NewFont16System);
	else					
		OldFont=pDC->SelectObject(&NewFont14Arial);
	int fcolor = RGB(0,0,0);
	if (ftmp > 0.00001)
		fcolor = RGB(255,0,0);
	else if (ftmp < -0.00001) 
		fcolor = RGB(0,128,0);
	pDC->SetTextColor(fcolor);
	pDC->SetBkMode(TRANSPARENT);
	pDC->TextOut(x1,y1,tempstr);

	pDC->SelectObject(OldFont);
}

void DrawJEWord(CDC *pDC,int x1,int y1,float ftmp)
{
	char tempstr[80];
	if (ftmp<10000.0)				sprintf(tempstr,"%4.0f",ftmp);
	else if (ftmp<100000000.0)		sprintf(tempstr,_F("%.0f万"),ftmp/10000.0);
    else if (ftmp<10000000000.0)	sprintf(tempstr,_F("%.2f亿"),ftmp/100000000.0);
    else if (ftmp<100000000000.0)	sprintf(tempstr,_F("%.1f亿"),ftmp/100000000.0);
	else if (ftmp<1000000000000.0)  sprintf(tempstr,_F("%.0f亿"),ftmp/100000000.0);
	else							sprintf(tempstr,_F("%.2f兆"),ftmp/1000000000000.0);

	CFont *OldFont;
	if(g_GH.bBigStatusBar)
		OldFont=pDC->SelectObject(&NewFont16System);
	else
		OldFont=pDC->SelectObject(&NewFont14Arial_2);

	pDC->SetTextColor (RGB(0,0,0));
	pDC->SetBkMode(TRANSPARENT);
	pDC->TextOut(x1,y1,tempstr);
	pDC->SelectObject (OldFont);
}

void CStatusWnd::DrawZSBar(CDC *pDC,CRect rect,int flag)
{
	if(flag<0 || flag>3) return;
	struct CurrStockData tmpHQ;
	struct MemStkInfo *pInfo;
	pInfo=g_pStockIO->Get_MemStkInfo(g_StatusGP[flag].Code,g_StatusGP[flag].setcode);
	if(pInfo)
	{
		int xsflag=GetXSFlag(pInfo);
		g_pStockIO->ReadCurrStockData(pInfo,&tmpHQ);
		float ftmp=tmpHQ.Now;
		if(ftmp<COMPPREC) ftmp=tmpHQ.Close;
		if(!g_GH.bBigStatusBar||TRUE)
		{
			int nTmp=72;
			DrawBJWord(pDC,rect.left+nTmp,rect.top+1,ftmp,tmpHQ.Close,TRUE,xsflag);
			if(tmpHQ.Now>COMPPREC && tmpHQ.Close>COMPPREC)
			{
				nTmp+=18;
				DrawBJWord(pDC,rect.left+6*7+nTmp,rect.top+1,(tmpHQ.Now-tmpHQ.Close),0.0,TRUE,xsflag);
				nTmp+=16;
				DrawZAFWord(pDC,rect.left+6*14+nTmp,rect.top+1,(tmpHQ.Now-tmpHQ.Close)/tmpHQ.Close*100.0);
				nTmp+=12;
			//	DrawJEWord(pDC,rect.left+6*21+nTmp,rect.top+1,(tmpHQ.Amount<COMPPREC&&tmpHQ.Volume>COMPPREC)?tmpHQ.Volume:tmpHQ.Amount);
				nTmp+=12;
				CString ZsDetail_Up;
				ZsDetail_Up.Format("涨%d", tmpHQ.Buyv[0]);
				pDC->SetTextColor(STATUS_UP_COLOR);
				pDC->TextOut(rect.left+6*21+nTmp, rect.top+1, ZsDetail_Up);
				nTmp+=12;
				CString ZsDetail_Dn;
				ZsDetail_Dn.Format("跌%d", tmpHQ.Sellv[0]);
				pDC->SetTextColor(STATUS_DOWN_COLOR);
				pDC->TextOut(rect.left+6*28+nTmp, rect.top+1, ZsDetail_Dn);
			}
		}
		else
		{
			int nTmp=72;
			DrawBJWord(pDC,rect.left+nTmp,rect.top+1,ftmp,tmpHQ.Close,TRUE,xsflag);
			if(tmpHQ.Now>COMPPREC)
			{
				nTmp+=18;
				DrawBJWord(pDC,rect.left+6*7+nTmp,rect.top+1,(tmpHQ.Now-tmpHQ.Close),0.0,TRUE,xsflag);
				nTmp+=12;
				DrawJEWord(pDC,rect.left+6*14+nTmp+2,rect.top+1,(tmpHQ.Amount<COMPPREC&&tmpHQ.Volume>COMPPREC)?tmpHQ.Volume:tmpHQ.Amount);
			}
		}
	}
}

void CStatusWnd::DrawtheBar(CDC *pDC,CRect rect,int flag,struct CurrStockData *zstmpp,short TotalStkNum)
{
	short x1,x2,startx;
	if(flag == 0)		startx = DKBar_Width/2;
	else				startx = 3*DKBar_Width/2+2;

	x2 = rect.left+startx-(float)zstmpp->Buyv[2]*DKBar_Width/(2*TotalStkNum);
	if (zstmpp->Buyv[1]>0)
	{
		x1 = x2-(float)zstmpp->Buyv[1]*DKBar_Width/(2*TotalStkNum);
		DrawNoBorderBar(pDC,x1,rect.top+1,x2+1,rect.top+7,RGB(128,0,0));
	}
	if(x2<rect.left+startx)
		DrawNoBorderBar(pDC,x2,rect.top+1,rect.left+startx,rect.top+7,RGB(255,0,0));
   
	x1=rect.left+startx+(float)zstmpp->Sellv[2]*DKBar_Width/(2*TotalStkNum);
	if (zstmpp->Sellv[1]>0)
	{
		x2=x1+(float)zstmpp->Sellv[1]*DKBar_Width/(2*TotalStkNum);
		DrawNoBorderBar(pDC,x1-1,rect.top+1,x2,rect.top+7,RGB(0,255,0));
	}
	if(x1>rect.left+startx)
		DrawNoBorderBar(pDC,rect.left+startx,rect.top+1,x1,rect.top+7,RGB(0,128,0));
}

void CStatusWnd::DrawDKBar(CDC *pDC,CRect rect,struct CurrStockData *shzstmpp,struct CurrStockData *szzstmpp)
{
	static short SHGPStkNum = g_pStockIO->GetStockNum(SH_GP);
	static short SZGPStkNum = g_pStockIO->GetStockNum(SZ_GP);
	short i,j;
	char tmpstr[3];
	static unsigned char shdif[6] = {' ',' ',' ',' ',' ',' '};
	static unsigned char szdif[6] = {' ',' ',' ',' ',' ',' '};
	static float shlead = shzstmpp->Lead,szlead = szzstmpp->Lead;

	CPen *pOldPen=(CPen *)pDC->SelectStockObject (WHITE_PEN);
	CBrush *pOldBrush=(CBrush *)pDC->SelectStockObject(NULL_BRUSH);

	pDC->Rectangle (rect.left,rect.top,rect.left+DKBar_Width,rect.top+7);
	pDC->Rectangle (rect.left+DKBar_Width+2,rect.top,rect.left+2*DKBar_Width+2,rect.top+7);

	for (j = 0; j < 2; j++ )
	for (i = 1; i < 4; i++ )
	{
		pDC->MoveTo(rect.left+i*DKBar_Width/4+j*(DKBar_Width+2),rect.top);
		pDC->LineTo(rect.left+i*DKBar_Width/4+j*(DKBar_Width+2),rect.top+7);
	}
	pDC->SelectObject(pOldPen);
	pDC->SelectObject(pOldBrush);

	DrawtheBar(pDC,rect,0,shzstmpp,SHGPStkNum);
	DrawtheBar(pDC,rect,1,szzstmpp,SZGPStkNum);

	//上海红绿军
	for(i=0;i<5;i++)	shdif[i]=shdif[i+1];
    if(shzstmpp->Lead>shlead+COMPPREC)      shdif[5]=0x18;
    else if(shzstmpp->Lead<shlead-COMPPREC) shdif[5]=0x19;
    else									shdif[5]='=';

    if(shzstmpp->Lead<0) shdif[5]+=0x80;
    tmpstr[1]='\0';
    for(i=0;i<6;i++)
    {
		if (shdif[i]>0x80)
		{	
			tmpstr[0] = shdif[i] - 0x80;
			DrawDiffWord(pDC,rect.left+DKBar_Width-9*6+i*8+8,rect.top+4,tmpstr,1); //1表示用绿色
		}
		else
		{	
			tmpstr[0] = shdif[i];
			DrawDiffWord(pDC,rect.left+DKBar_Width-9*6+i*8+8,rect.top+4,tmpstr,0);
		}
    }
	shlead=shzstmpp->Lead;

	//深圳红绿军
	for(i=0;i<5;i++) szdif[i] = szdif[i+1];
	if(szzstmpp->Lead>szlead+COMPPREC)		szdif[5]=0x18;
	else if(szzstmpp->Lead<szlead-COMPPREC)	szdif[5]=0x19;
	else									szdif[5]='=';
   
	if(szzstmpp->Lead<0) szdif[5]+=0x80;
	tmpstr[1] = '\0';
	for(i=0;i<6;i++)
	{
		if (szdif[i]>0x80)
		{  
			tmpstr[0] = szdif[i]-0x80;
			DrawDiffWord(pDC,rect.left+2*DKBar_Width-9*6+8+i*8+2,rect.top+4,tmpstr,1);	//1表示用绿色
		}
		else
		{
			tmpstr[0] = szdif[i];
			DrawDiffWord(pDC,rect.left+2*DKBar_Width-9*6+8+i*8+2,rect.top+4,tmpstr,0); //0表示用红色
		}
	}
	szlead = szzstmpp->Lead;
}

//////////////////////////////////////////////////////////////////////////
void	CStatusWnd::RefreshMDITab()
{
	for(int i=0;i<m_vTabInfo.size();i++)
		m_vTabInfo[i].bValid = 0;						//先都设为无效

	CMainFrame *pMainWnd = (CMainFrame*)AfxGetMainWnd();
	CWnd *pMDIChild = pMainWnd->GetNextMDIChildWnd_All();
	CWnd *pFocusWnd = pMDIChild;
	while (pMDIChild)
	{
		int nFindIndex = -1;
		for(int i=0;i<m_vTabInfo.size();i++)
		{
			if(m_vTabInfo[i].MDIWnd==pMDIChild->GetSafeHwnd())			//找到同一个了
			{
				nFindIndex = i;
				break;
			}
		}
		CString mdiTitle;
		pMDIChild->GetWindowText(mdiTitle);
		HICON hIcon=(HICON)pMDIChild->SendMessage(WM_GETICON,(WPARAM)ICON_SMALL,0); 
		if(hIcon==NULL) 
			hIcon=(HICON)GetClassLong(pMDIChild->GetSafeHwnd(),GCL_HICON);
		if(nFindIndex>=0)
		{
			m_vTabInfo[i].strTitle=mdiTitle;
			m_vTabInfo[i].hIcon=hIcon;
			m_wndSwitcher.ModifyButton(nFindIndex, mdiTitle, hIcon);
			m_vTabInfo[nFindIndex].bValid = 1;
		}
		else
		{
			m_wndSwitcher.AddButton(mdiTitle, hIcon);
			MDITabInfo newTab;
			newTab.bValid = 1;
			newTab.MDIWnd = pMDIChild->GetSafeHwnd();
			newTab.strTitle = mdiTitle;
			newTab.hIcon = hIcon;
			m_vTabInfo.push_back(newTab);
		}
		pMDIChild = pMainWnd->GetNextMDIChildWnd_All();
	}

	//清除无效的
	for(i=0;i<m_vTabInfo.size();i++)
	{
		if(m_vTabInfo[i].bValid==0)
		{
			m_wndSwitcher.RemoveButton(i);
			vector<MDITabInfo>::iterator iter = m_vTabInfo.begin();
			for(int j=0;j<i;j++) iter++;
			m_vTabInfo.erase(iter);
		}
	}
	for(i=0;i<m_vTabInfo.size();i++)
	{
		if(m_vTabInfo[i].MDIWnd==pFocusWnd->GetSafeHwnd())
		{
			m_wndSwitcher.SetSel(i);
			break;
		}
	}
}

void	CStatusWnd::OnTabSelChanged(WPARAM wParam,LPARAM lParam)
{
	int nSel = (int)wParam;
	if(nSel<0||nSel>=m_vTabInfo.size()) return;
	::BringWindowToTop(m_vTabInfo[nSel].MDIWnd);
}

BOOL CStatusWnd::PreTranslateMessage(MSG* pMsg)
{
	if(m_PreviewTool.m_hWnd!=NULL)  
		m_PreviewTool.RelayEvent(pMsg);
	return CWnd::PreTranslateMessage(pMsg);
}

#include "PreviewUnitDlg.h"
void CStatusWnd::OnMouseMove(UINT nFlags, CPoint point)
{
	CRect zsRect(m_ToolRectHelp);
	CRect sig_rect;
	for(int i=0;i<m_nZsNum;i++)
	{
		sig_rect=zsRect;
		sig_rect.left=zsRect.left+i*zsRect.Width()/m_nZsNum;
		sig_rect.right=sig_rect.left+zsRect.Width()/m_nZsNum;				
		sig_rect.top+=1;
		
		int realPos = i;
		MemStkInfo *pInfo=g_pStockIO->Get_MemStkInfo(g_StatusGP[realPos].Code,g_StatusGP[realPos].setcode);
		long nCurGp = GetSYSCurrentGPIndex();

		if(pInfo&&PtInRect(&sig_rect,point))
		{
			if((*g_pStockIO)[nCurGp]==pInfo) 
			{
				m_PreviewTool.DelPreviewTool(this);
				if(g_pPreviewUnitDlg&&g_pPreviewUnitDlg->m_hWnd)
					g_pPreviewUnitDlg->ShowWindow(SW_HIDE);
				continue;
			}

			CString tmpChar;
			tmpChar.Format("%d-",ZST_UNIT);
			for(int k=0;k<10;k++)
				tmpChar += "000000";
			tmpChar += "\r\n";
			CString Style;
			Style.Format("%d",ZST_MULTI);
			tmpChar += Style;
			tmpChar += "\r\n";
			CString GPCode;
			GPCode.Format("%d%s",pInfo->setcode,pInfo->Code);
			tmpChar += GPCode;
			for(k=0;k<16;k++)
			{
				CString strLine;
				strLine.Format("\r\n%d", k);
				tmpChar += strLine;
			}
			m_PreviewTool.AddPreviewTool(this, _F(tmpChar), sig_rect, TOOLTIP_HQ);
			return;
		}
	}
	m_PreviewTool.Pop();
}