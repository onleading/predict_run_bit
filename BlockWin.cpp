#include "stdafx.h"
#include "TdxW.h"
#include "BlockWin.h"

CBlockWin::CBlockWin(CWnd *pParent)
{
	m_pView = pParent;
	DrawFlag	= MENU_DRAW_ALL;
	m_nType = VAL_BJFX_BLOCK;
	bHasScrollBar = FALSE;
	pVScrollBar=new CScrollBar;
	pVScrollBar->Create(WS_CHILD|WS_VISIBLE|SBS_RIGHTALIGN|SBS_VERT,CRect(0,0,0,0),m_pView,ID_BLOCK_VSCROLL);

	GetBlock();
}

CBlockWin::~CBlockWin()
{
	if(pVScrollBar)	delete pVScrollBar;
}

void CBlockWin::SetType(int type)
{
	m_nType = type;
}

void CBlockWin::DrawTitleStr(CDC *pDC)
{
	CFont NewFont;
	NewFont.CreateFont(34,0, 0, 0, FW_BOLD, 0, 0, 0, DEFAULT_CHARSET,OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, g_b2000XPSys?ANTIALIASED_QUALITY:DEFAULT_QUALITY,DEFAULT_PITCH|FF_DONTCARE, "楷体_GB2312");
	CFont *OldFont=pDC->SelectObject(&NewFont);

	int nMaxLen = pDC->GetTextExtent("选择板块").cx;
	int Startx	= (Width()-nMaxLen)/2;
	pDC->SetTextColor(MenuColor.MenuTitleColor);
	pDC->TextOut(Startx,MENUTITLE_TOP,"选择板块");
	pDC->SelectObject(OldFont);	
	NewFont.DeleteObject();
}

//每次OnSize时都重新将NowNo和CurPage置位
void CBlockWin::CalcSize()
{
	m_nNowNo = m_nCurPage = 0;
	//计算本页可容纳的最大行与最大列
	m_nMaxCol = (Width() - 2*BLOCKITEM_STARTX + BLOCKITEM_GAP)/(BLOCKITEM_WIDTH+BLOCKITEM_GAP);
	m_nMaxCol = MAX(m_nMaxCol,1);
	m_nMaxRow = (Height() - FIRSTMENU_TOP)/BLOCKITEM_HEIGHT;
	m_nMaxRow = MAX(m_nMaxRow,1);
	if(m_nMaxCol * m_nMaxRow < m_nItemNum)
	{
		if(m_nItemNum % (m_nMaxRow*m_nMaxCol) == 0)
			m_nPageNum = m_nItemNum/(m_nMaxRow*m_nMaxCol);
		else
			m_nPageNum = m_nItemNum/(m_nMaxRow*m_nMaxCol) + 1;
		pVScrollBar->SetWindowPos(0,right-16,FIRSTMENU_TOP,16,Height()-FIRSTMENU_TOP,SWP_SHOWWINDOW);
		pVScrollBar->SetScrollRange(0,m_nPageNum-1);
		pVScrollBar->SetScrollPos(m_nCurPage);
		ShowVScrollBar(TRUE);
		SCROLLINFO theScrollInfo;
		pVScrollBar->GetScrollInfo(&theScrollInfo);
		theScrollInfo.nPage = 1;
		pVScrollBar->SetScrollInfo (&theScrollInfo);
		if(m_nMaxCol > 1)
			m_nColGap = BLOCKITEM_GAP - 16/(m_nMaxCol-1);
		else
			m_nColGap = BLOCKITEM_GAP;
	}
	else
	{
		m_nPageNum = 1;
		ShowVScrollBar(FALSE);
		m_nColGap = BLOCKITEM_GAP;
	}
	AfterChangePage(m_nCurPage);
}

void CBlockWin::AfterChangePage(int nCurPage)
{
	m_nStartItem = nCurPage*m_nMaxCol*m_nMaxRow;
	m_nEndItem	= (nCurPage==m_nPageNum-1)?m_nItemNum-1:(m_nStartItem+m_nMaxCol*m_nMaxRow-1);
}

void CBlockWin::SizeIt()
{
	CRect rc;
	m_pView->GetClientRect(&rc);
	CopyRect(&rc);
	//计算一些变量
	CalcSize();
	//重画
	DrawFlag = MENU_DRAW_ALL;
	DrawIt();
	//重新刷新滚动条
	pVScrollBar->Invalidate();
}

int CBlockWin::IsOnItem(POINT pt)
{
	int nRet = -1;
	CRect rc;
	for(int i = m_nStartItem;i <= m_nEndItem;i++)
	{
		rc.top = ((i-m_nStartItem)/m_nMaxCol)*BLOCKITEM_HEIGHT+FIRSTMENU_TOP;
		rc.left = ((i-m_nStartItem)%m_nMaxCol)*(BLOCKITEM_WIDTH+BLOCKITEM_GAP)+BLOCKITEM_STARTX;
		rc.bottom = rc.top+BLOCKITEM_HEIGHT-4;
		rc.right = rc.left + BLOCKITEM_WIDTH;
		if(rc.PtInRect(pt))
		{
			nRet = i;
			break;
		}
	}
	return nRet;
}

void CBlockWin::Process()
{
	SetCursor(LoadCursor(NULL,IDC_ARROW));
	if(m_nItemNum < 1) return;
	m_pView->SendMessage(UM_MENU_SETBLOCK,m_nType,GetBlockNo());
}

void CBlockWin::GetBlock()
{
	for(int i = 0;i < g_nBlockNum;i++)
		strcpy(m_pDataBuff[i],g_BlockInfo[i].Name);
	for(i = 0;i < TOTAL_DY_NUM;i++)
		strcpy(m_pDataBuff[i+g_nBlockNum],g_cwAddrTopic[i]);
	for(i = 0;i < TOTAL_HY_NUM;i++)
		strcpy(m_pDataBuff[i+g_nBlockNum+TOTAL_DY_NUM],g_cwHYstr[i]);
	for(i = 0;i < g_ConceptBlockNum;i++)
		strcpy(m_pDataBuff[i+g_nBlockNum+TOTAL_DY_NUM+TOTAL_HY_NUM],g_ConceptBlock[i].strBlockName);
	for(i = 0;i < g_ZHBBlockNum;i++)
		strcpy(m_pDataBuff[i+g_nBlockNum+TOTAL_DY_NUM+TOTAL_HY_NUM+g_ConceptBlockNum],g_ZHBBlock[i].zhmc);
	m_nItemNum = g_nBlockNum+TOTAL_DY_NUM+TOTAL_HY_NUM+g_ConceptBlockNum+g_ZHBBlockNum;
}

void CBlockWin::DrawRect(CDC *pDC,int x,int y,int cx,int cy)
{
	pDC->MoveTo(x,y);
	pDC->LineTo(x+cx,y);
	pDC->LineTo(x+cx,y+cy);
	pDC->LineTo(x,y+cy);
	pDC->LineTo(x,y);
}

void CBlockWin::DrawIt()
{
	CDC *pDC = m_pView->GetDC();

	if(DrawFlag == MENU_DRAW_PUTBITMAP) //如果从位图中拷贝到DC上
	{
		CDC tempMemDC;
		tempMemDC.CreateCompatibleDC (pDC);
		CBitmap *pOldBitmap = tempMemDC.SelectObject (&tempBitmap);
		//从内存DC拷贝到屏幕DC
		if(bHasScrollBar)
		{
			pDC->BitBlt (left,top,Width()-16,Height(),&tempMemDC,0,0,SRCCOPY);
			pDC->FillSolidRect(right-16,top,16,FIRSTMENU_TOP,MenuColor.MenuBackColor);
		}
		else
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
	//画菜单标题项
	DrawTitleStr(&memDC);
	
	//画菜单项
	CFont NewFont;
	NewFont.CreateFont(24,0, 0, 0, FW_BOLD, 0, 0, 0, DEFAULT_CHARSET,OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, g_b2000XPSys?ANTIALIASED_QUALITY:DEFAULT_QUALITY,DEFAULT_PITCH|FF_DONTCARE, "楷体_GB2312");
	CFont *OldFont=memDC.SelectObject(&NewFont);

	CPen NewPen,*OldPen;
	NewPen.CreatePen (PS_SOLID,1,GridColor.Grid_Up_Color);
	OldPen = memDC.SelectObject(&NewPen);

	int nRowtmp,nColtmp,nStartx,nStarty;
	for(int i = m_nStartItem;i <= m_nEndItem;i++)
	{
		nRowtmp = (i-m_nStartItem)/m_nMaxCol;
		nColtmp = (i-m_nStartItem)%m_nMaxCol;
		nStartx = BLOCKITEM_STARTX+nColtmp*(BLOCKITEM_WIDTH+m_nColGap);
		nStarty = FIRSTMENU_TOP+nRowtmp*BLOCKITEM_HEIGHT;
		if(i < g_nBlockNum) 
			memDC.SetTextColor(MenuColor.MenuColor1);
		else if(i < g_nBlockNum+TOTAL_DY_NUM)
			memDC.SetTextColor(MenuColor.MenuColor2);
		else if(i < g_nBlockNum+TOTAL_DY_NUM+TOTAL_HY_NUM)
			memDC.SetTextColor(MenuColor.MenuColor3);
		else if(i < g_nBlockNum+g_ZHBBlockNum+TOTAL_DY_NUM+TOTAL_HY_NUM)
			memDC.SetTextColor(MenuColor.MenuColor4);
		else
			memDC.SetTextColor(MenuColor.MenuColor1);
		memDC.TextOut(nStartx,nStarty,m_pDataBuff[i]);
		if(m_nNowNo == i) //如果是选中项，还要画焦点框
			DrawRect(&memDC,nStartx-1,nStarty-1,BLOCKITEM_WIDTH+2,BLOCKITEM_HEIGHT-8+2);
	}
	//顺便计算选中项所处的当前行与当前列
	m_nCurRow = (m_nNowNo - m_nStartItem)/m_nMaxCol;
	m_nCurCol = (m_nNowNo - m_nStartItem)%m_nMaxCol;

	//从内存DC拷贝到屏幕DC
	if(bHasScrollBar)
	{
		pDC->BitBlt (left,top,Width()-16,Height(),&memDC,0,0,SRCCOPY);
		pDC->FillSolidRect(right-16,top,16,FIRSTMENU_TOP,MenuColor.MenuBackColor);
	}
	else
		pDC->BitBlt (left,top,Width(),Height(),&memDC,0,0,SRCCOPY);

	memDC.SelectObject(OldPen);
	memDC.SelectObject(OldFont);
	memDC.SelectObject(pOldBitmap);
	NewFont.DeleteObject();
	NewPen.DeleteObject();
	memDC.DeleteDC();
	m_pView->ReleaseDC(pDC);

    DrawFlag = MENU_DRAW_PUTBITMAP;
}

int CBlockWin::Handle(UINT message, WPARAM wParam, LPARAM lParam)
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
		SetTitle("");
		SizeIt();
		break;
	case UM_HIDE_PAD://善后处理
		ShowVScrollBar(FALSE);
		break;
	//处理竖立滚动条消息
	case WM_VSCROLL:
		{
			UINT nSBCode = LOWORD(wParam);
			UINT nPos = HIWORD(wParam);
			CScrollBar *pScrollBar = (CScrollBar *)lParam;
			if(pScrollBar == pVScrollBar)
			{
				switch(nSBCode)
				{
				case SB_LINEUP:
				case SB_PAGEUP:
					if(m_nCurPage > 0)
						m_nCurPage--;
					break;
				case SB_LINEDOWN:
				case SB_PAGEDOWN:
					if(m_nCurPage != m_nPageNum-1)
						m_nCurPage++;
					break;
				case SB_THUMBTRACK:
					m_nCurPage = nPos;
					break;
				default:
					break;
				}
				AfterChangePage(m_nCurPage);
				m_nNowNo = m_nStartItem;
				DrawFlag = MENU_DRAW_ALL;
				DrawIt();
				pVScrollBar->SetScrollPos(m_nCurPage);
			}
			break;
		}
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
				if(m_nNowNo != ntmp)
				{
					m_nNowNo = ntmp;
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
				m_nNowNo = ntmp;
				Process();
			}
			break;
		}
	case WM_KEYDOWN:
		{
			UINT nChar = (UINT)wParam;
			BOOL bChanged = FALSE;
			if(nChar==VK_RETURN||nChar==VK_PRIOR||nChar==VK_NEXT||nChar==VK_UP||nChar==VK_DOWN||nChar==VK_LEFT||nChar==VK_RIGHT)
			{
				switch(nChar)
				{
				case VK_RETURN:
					Process();
					break;
				case VK_PRIOR:
					if(m_nCurPage == 0) break;
					m_nCurPage --;
					AfterChangePage(m_nCurPage);
					m_nNowNo -= m_nMaxCol*m_nMaxRow;
					pVScrollBar->SetScrollPos(m_nCurPage);
					bChanged = TRUE;
					break;
				case VK_NEXT:
					if(m_nCurPage == m_nPageNum-1) break;
					m_nCurPage ++;
					AfterChangePage(m_nCurPage);
					m_nNowNo = MIN(m_nNowNo+m_nMaxCol*m_nMaxRow,m_nItemNum-1);
					pVScrollBar->SetScrollPos(m_nCurPage);
					bChanged = TRUE;
					break;
				case VK_UP:
					if(m_nCurRow != 0)
					{
						m_nNowNo = m_nNowNo-m_nMaxCol;
						bChanged = TRUE;
					}
					else if(m_nCurPage > 0)
					{
						m_nCurPage--;
						AfterChangePage(m_nCurPage);
						m_nNowNo -= m_nMaxCol;
						pVScrollBar->SetScrollPos(m_nCurPage);
						bChanged = TRUE;
					}
					break;
				case VK_DOWN:
					if(m_nCurRow != m_nMaxRow-1)
					{
						m_nNowNo = MIN(m_nNowNo+m_nMaxCol,m_nItemNum-1);
						bChanged = TRUE;
					}
					else if(m_nCurPage != m_nPageNum-1)
					{
						m_nCurPage++;
						AfterChangePage(m_nCurPage);
						m_nNowNo = MIN(m_nNowNo+m_nMaxCol,m_nItemNum-1);
						pVScrollBar->SetScrollPos(m_nCurPage);
						bChanged = TRUE;
					}
					break;
				case VK_RIGHT:
					if(m_nNowNo == m_nItemNum-1 || m_nCurCol == m_nMaxCol-1)
						m_nNowNo = m_nStartItem+m_nCurRow*m_nMaxCol;
					else
						m_nNowNo++;
					bChanged = TRUE;
					break;
				case VK_LEFT:
					if(m_nCurCol == 0)
						m_nNowNo = MIN(m_nStartItem+(m_nCurRow+1)*m_nMaxCol-1,m_nItemNum);
					else
						m_nNowNo--;
					bChanged = TRUE;
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

void CBlockWin::ShowVScrollBar(BOOL bShow)
{
	if(pVScrollBar)
		pVScrollBar->ShowWindow(bShow?SW_SHOW:SW_HIDE);
	bHasScrollBar = bShow;
}

int CBlockWin::GetBlockNo()
{
	if(m_nNowNo < g_nBlockNum) return m_nNowNo;
	else if(m_nNowNo < g_nBlockNum+TOTAL_DY_NUM) return DY_START+m_nNowNo-g_nBlockNum;
	else if(m_nNowNo < g_nBlockNum+TOTAL_DY_NUM+TOTAL_HY_NUM) return HY_START+m_nNowNo-g_nBlockNum-TOTAL_DY_NUM;
	else if(m_nNowNo < g_nBlockNum+g_ConceptBlockNum+TOTAL_DY_NUM+TOTAL_HY_NUM) return CON_START+m_nNowNo-g_nBlockNum-TOTAL_DY_NUM-TOTAL_HY_NUM;
	else return ZHB_START+m_nNowNo-g_nBlockNum-g_ConceptBlockNum-TOTAL_DY_NUM-TOTAL_HY_NUM;
}
