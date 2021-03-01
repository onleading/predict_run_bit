#include "stdafx.h"
#include "TdxW.h"
#include "MenuWin.h"

////////////////////////////////////////////
void PushMenu(short MenuID,short nNowNo)
{    if (g_nMenuStackTop>=19) return;
     g_nMenuStack[g_nMenuStackTop]=MenuID;
     g_nMenuStack[g_nMenuStackTop+1]=nNowNo;
     g_nMenuStackTop+=2;
}

void PopMenu(short &MenuID,short &nNowNo)
{    if (g_nMenuStackTop<=0) return;
     MenuID=g_nMenuStack[g_nMenuStackTop-2];
     nNowNo=g_nMenuStack[g_nMenuStackTop-1];
     g_nMenuStackTop-=2;
}

////////////////////////////////////////////
CMenuWin::CMenuWin(CWnd *pParent)
{
	m_pView= pParent;

	SetMenuID(0);

	DrawFlag	= MENU_DRAW_ALL;
	m_nStartx1	= m_nStartx2 = 0;
	for(int i = 0;i < nMenuNum;i++)
		m_rcItemRect[i].SetRectEmpty();
}

CMenuWin::~CMenuWin()
{}

void CMenuWin::SetMenuID(int nMenuNo)
{
	m_nMenuID = nMenuNo;
	nStartNo = g_awMenuStartNum[2*nMenuNo];
	if(nMenuNo != MENU_XXM)
		nMenuNum = g_awMenuStartNum[2*nMenuNo+1];
	else	//如果是公告消息,菜单条数为XXMNum
	{
		nMenuNum = XXMNum;
		if(g_bHasInfo) 
			nMenuNum++;
		if(g_bHasWebInfo||strlen(g_Url.WebInfoURL)>0) 
			nMenuNum++;
		if(g_bExtern_Info) 
			nMenuNum++;
	}
	nColNum = (nMenuNum > 13)?2:1;
    nNowNo = 0;
	if (g_cwMenuTopic[nMenuNo][0]!='\0')
			strcpy(lpszTopic,_F(g_cwMenuTopic[nMenuNo]));
}

void CMenuWin::SetTopic(char * topic)
{  
	strcpy(lpszTopic,topic+6);
}

void CMenuWin::SetNow(int NowNo)
{
	if (NowNo < 0 && NowNo >= nMenuNum)   return;
	nNowNo = NowNo;
}

void CMenuWin::SizeIt()
{
	CRect rc;
	m_pView->GetClientRect(&rc);
	CopyRect(&rc);
	DrawFlag = MENU_DRAW_ALL;
	DrawIt();
}

void CMenuWin::DrawIt()
{
	CDC *pDC = m_pView->GetDC();

	if(DrawFlag == MENU_DRAW_PUTBITMAP) //如果从位图中拷贝到DC上
	{
		CDC tempMemDC;
		tempMemDC.CreateCompatibleDC (pDC);
		CBitmap *pOldBitmap = tempMemDC.SelectObject (&tempBitmap);
		//从内存DC拷贝到屏幕DC
		pDC->BitBlt (left,top,Width(),Height(),&tempMemDC,0,0,SRCCOPY);
		tempMemDC.SelectObject(pOldBitmap);
		tempMemDC.DeleteDC();
		m_pView->ReleaseDC(pDC);

		return;		
	}
	tempBitmap.DeleteObject ();
	CDC memDC;							//作出内存DC
	memDC.CreateCompatibleDC (pDC);
	tempBitmap.CreateCompatibleBitmap (pDC,Width (),Height());
	CBitmap *pOldBitmap = memDC.SelectObject (&tempBitmap);
	memDC.FillSolidRect(this,MenuColor.MenuBackColor);
	memDC.SetBkMode (TRANSPARENT);

	//画广告条
	DrawAdvert(&memDC);
	//画菜单标题项
	DrawTitleStr(&memDC);
	//画菜单项
	CFont NewFont;
	NewFont.CreateFont(32,0, 0, 0, FW_BOLD, 0, 0, 0, DEFAULT_CHARSET,OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, g_b2000XPSys?ANTIALIASED_QUALITY:DEFAULT_QUALITY,DEFAULT_PITCH|FF_DONTCARE, "楷体_GB2312");
	CFont *OldFont=memDC.SelectObject(&NewFont);
	//得到菜单项的最大长度
	int nMaxMenuLen = GetMaxMenuLen(&memDC);
	if(nColNum == 1)
		m_nStartx1 = (Width()-nMaxMenuLen)/2;
	else
	{
		m_nStartx1 = (Width()-2*nMaxMenuLen)/3;
		m_nStartx2 = 2*m_nStartx1+nMaxMenuLen;
	}
	m_nStartx1-=10;
	m_nStartx2-=10;
	//重新计算各个菜单项的区域，应在计算出m_nStartx1等之后
	GetItemRect(&memDC);

	memDC.SetTextColor(MenuColor.MenuColor1);
	
	for(int i = 0;i < nMenuNum;i++)
	{
		if(i < 13)
			memDC.TextOut(m_nStartx1,i*MENU_HEIGHT+FIRSTMENU_TOP,_F(g_pMenu[nStartNo+i].Name));
		else
			memDC.TextOut(m_nStartx2,(i-13)*MENU_HEIGHT+FIRSTMENU_TOP,_F(g_pMenu[nStartNo+i].Name));
	}

	//画指示
	POINT pt;
	if(nNowNo < 13)
	{
		pt.x = m_nStartx1 - 40;
		pt.y = nNowNo*MENU_HEIGHT+FIRSTMENU_TOP;
	}
	else
	{
		pt.x = m_nStartx2 - 40;
		pt.y = (nNowNo-13)*MENU_HEIGHT+FIRSTMENU_TOP;
	}
	HICON hIcon = ::LoadIcon(AfxGetResourceHandle(),MAKEINTRESOURCE(IDI_POINTHAND));
	memDC.DrawIcon(pt,hIcon);
	
	//从内存DC拷贝到屏幕DC
	pDC->BitBlt (left,top,Width(),Height(),&memDC,0,0,SRCCOPY);
	
	memDC.SelectObject(OldFont);
	memDC.SelectObject(pOldBitmap);
	NewFont.DeleteObject();
	memDC.DeleteDC();
	m_pView->ReleaseDC(pDC);

    DrawFlag = MENU_DRAW_PUTBITMAP;
}

int CMenuWin::Handle(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{
	case WM_PAINT:
		DrawIt();
		break;
	case UM_CHANGE_MENU_COLOR:
		DrawFlag = MENU_DRAW_ALL;
		DrawIt();
		break;
	case WM_SIZE:
		SizeIt();
		break;
	case UM_SHOW_PAD:
		{
			SetTitle("");
			SizeIt();
			break;
		}
	case UM_HIDE_PAD:
		break;
	case WM_MOUSEMOVE:
		{
			POINT pt;
			pt.x = LOWORD(wParam);
			pt.y = HIWORD(wParam);
			if(IsOnItem(pt) != -1)
				SetCursor(m_hCanPress);
			else
				SetCursor(LoadCursor(NULL,IDC_ARROW));
			break;
		}
	case WM_LBUTTONDOWN:
		{
			POINT pt;
			pt.x = LOWORD(wParam);
			pt.y = HIWORD(wParam);
			int ntmp;
			if((ntmp=IsOnItem(pt)) != -1)
			{
				if(nNowNo != ntmp)
				{
					nNowNo = ntmp;
					DrawFlag = MENU_DRAW_ALL;
					DrawIt();
				}
				else 
					Process();
			}
			break;
		}
	case WM_LBUTTONDBLCLK:
		{
			POINT pt;
			pt.x = LOWORD(wParam);
			pt.y = HIWORD(wParam);
			int ntmp;
			if((ntmp=IsOnItem(pt)) != -1)
			{
				nNowNo = ntmp;
				Process();
			}
			break;
		}
	case WM_KEYDOWN:
		{
			UINT nChar = (UINT)wParam;
			BOOL bChanged = FALSE;
			if((nChar==VK_RETURN)||(nChar==VK_UP)||(nChar==VK_DOWN)||(nChar==VK_LEFT)||(nChar==VK_RIGHT))
			{
				switch(nChar)
				{
				case VK_RETURN:
					Process();
					break;
				case VK_UP:
					if(nNowNo != 0)
						nNowNo = nNowNo-1;
					else
						nNowNo = nMenuNum-1;
					bChanged = TRUE;
					break;
				case VK_DOWN:
					nNowNo = (nNowNo+1)%nMenuNum;
					bChanged = TRUE;
					break;
				case VK_RIGHT:
					if(nMenuNum > 13 && nNowNo < nMenuNum - 13)
					{
						nNowNo += 13;
						bChanged = TRUE;
					}
					break;
				case VK_LEFT:
					if(nNowNo >= 13)
					{
						nNowNo -= 13;
						bChanged = TRUE;
					}
					break;
				}
				if(bChanged)
				{
					DrawFlag = MENU_DRAW_ALL;
					DrawIt();
				}
			}
			break;
		}
	default:
		break;
	}
	return -1;
}

//进入下一级菜单前的处理
void CMenuWin::Process()
{
	SetCursor(LoadCursor(NULL,IDC_ARROW));

	int i = nStartNo + nNowNo;
	int ntmpNextID = g_pMenu[i].NextMenuID;
	int ntmpItemID = g_pMenu[i].ItemID;
	if(ntmpNextID != MENU_LEAF)
		PushMenu(m_nMenuID,nNowNo);
	m_pView->SendMessage(UM_SWAP_MENU,ntmpNextID,ntmpItemID);
}

//一些私有成员函数
int CMenuWin::GetMaxMenuLen(CDC *pDC)
{
	int ntmp,nMaxLen = 0;
	for(int i = 0;i < nMenuNum;i++)
	{	
		ntmp = (pDC->GetTextExtent(g_pMenu[nStartNo+i].Name)).cx;
		if(ntmp > nMaxLen) nMaxLen = ntmp;
	}
	return nMaxLen;
}

void CMenuWin::GetItemRect(CDC *pDC)
{
	CSize tmpSize;
	for(int i = 0;i < nMenuNum;i++)
	{
		tmpSize = pDC->GetTextExtent(g_pMenu[nStartNo+i].Name);
		if(i < 13)
		{
			m_rcItemRect[i].top = i*MENU_HEIGHT+FIRSTMENU_TOP;
			m_rcItemRect[i].left = m_nStartx1;
			m_rcItemRect[i].bottom = i*MENU_HEIGHT+FIRSTMENU_TOP+tmpSize.cy;
			m_rcItemRect[i].right = m_nStartx1+tmpSize.cx;
		}
		else
		{
			m_rcItemRect[i].top = (i-13)*MENU_HEIGHT+FIRSTMENU_TOP;
			m_rcItemRect[i].left = m_nStartx2;
			m_rcItemRect[i].bottom = (i-13)*MENU_HEIGHT+FIRSTMENU_TOP+tmpSize.cy;
			m_rcItemRect[i].right = m_nStartx2+tmpSize.cx;
		}
	}
}

void CMenuWin::DrawAdvert(CDC *pDC)
{
	CFont NewFont;
	NewFont.CreateFont(14,0,0,0,FW_NORMAL,0,0,0,DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,DEFAULT_PITCH|FF_DONTCARE,SONG_FONT);
	CFont *OldFont=pDC->SelectObject(&NewFont);
	CSize size = pDC->GetTextExtent(g_ManyHostInfo.NotifyStr);
	int Startx = Width()-size.cx;
	int StartY = Height()-size.cy;
	pDC->SetTextColor(MenuColor.MenuColor3);
	pDC->TextOut(Startx-2,StartY-2,_F(g_ManyHostInfo.NotifyStr));
	pDC->SelectObject(OldFont);	
}

void CMenuWin::DrawTitleStr(CDC *pDC)
{
	CFont NewFont;
	NewFont.CreateFont(32,0, 0, 0, FW_BOLD, 0, 0, 0, DEFAULT_CHARSET,OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, g_b2000XPSys?ANTIALIASED_QUALITY:DEFAULT_QUALITY,DEFAULT_PITCH|FF_DONTCARE, "楷体_GB2312");
	CFont *OldFont=pDC->SelectObject(&NewFont);

	int nMaxLen = pDC->GetTextExtent(lpszTopic).cx;
	int Startx	= (Width()-nMaxLen)/2;
	pDC->SetTextColor(MenuColor.MenuTitleColor);
	pDC->TextOut(Startx,MENUTITLE_TOP,lpszTopic);
	pDC->SelectObject(OldFont);	
}

int CMenuWin::IsOnItem(POINT pt)
{
	int nRet = -1;
	for(int i = 0;i < nMenuNum;i++)
	{
		if(m_rcItemRect[i].PtInRect(pt))
		{
			nRet = i;
			break;
		}
	}
	return nRet;
}
