#include "stdafx.h"
#include "Marquee.h"

/////////////////////////////////////////////////////////////////////////////
// CMarquee

CMarquee::CMarquee()
{
	// 注册当前类
	RegisterWindowClass(NULL,_T("MyMarquee"));

	m_crBackColor			= RGB(0,0,0);
	m_crSysInfoTxt			= RGB(247,251,140);
	m_crCustomStockTxt[0]	= RGB(247,251,140);
	m_crCustomStockTxt[1]	= RGB(195,198,198);
	m_crCustomStockTxt[2]	= RGB(255,48,49);
	m_crCustomStockTxt[3]	= RGB(0,231,0);
	m_crCustomStockTxt[4]	= RGB(198,195,0);
	m_crCustomStockTxt[5]	= RGB(255,255,255);

	m_strText				= "";
	m_nEventID				= 0;
	m_stViewport.head		= 0;
	m_stViewport.tail		= 0;
	m_nMoveSpd				= 1;
	m_bTimerFlag			= FALSE;
	m_nScrollMode			= SM_HSCROLL;
	m_nDisplayType			= DT_SYSINFO;
	m_nStopCount			= 0;
	m_nStopTime				= 30;
	m_bEnableStop			= TRUE;		
	m_bRandom				= FALSE;
	m_hMsgWnd				= NULL;	
	m_uMsgID				= 0;
}

BOOL CMarquee::RegisterWindowClass(HINSTANCE hInstance,LPCTSTR lpszClassName)
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

CMarquee::~CMarquee()
{
}

BEGIN_MESSAGE_MAP(CMarquee, CWnd)
	//{{AFX_MSG_MAP(CMarquee)
	ON_WM_PAINT()
	ON_WM_TIMER()
	ON_WM_CREATE()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_ERASEBKGND()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMarquee message handlers

int CMarquee::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	//获取WM消息
	ModifyStyle(0,SS_NOTIFY);
	
	return 0;
}

/////////////////////////////////////////////////////////////////////////////
// Set 

void CMarquee::SetFeedBackMsg(HWND hMsgWnd,UINT uMsgID,DWORD Reserved)
{
	m_hMsgWnd	= hMsgWnd;	
	m_uMsgID	= uMsgID;
}

void CMarquee::SetRandom(BOOL bFlag)
{
	m_bRandom = bFlag;
}

void CMarquee::SetBackColor(COLORREF crBack)
{
	m_crBackColor = crBack;
}

void CMarquee::SetCustomStockTxtColor(COLORREF cr1, COLORREF cr2, COLORREF cr3, COLORREF cr4, COLORREF cr5,COLORREF cr6)
{
	m_crCustomStockTxt[0] = cr1;	// 股票名称
	m_crCustomStockTxt[1] = cr2;	// 走平颜色
	m_crCustomStockTxt[2] = cr3;	// 上涨颜色
	m_crCustomStockTxt[3] = cr4;	// 下跌颜色
	m_crCustomStockTxt[4] = cr5;	// 成交量颜色 
	m_crCustomStockTxt[5] = cr6;	// 换手率和量比
}

void CMarquee::SetSysInfoTextColor(const COLORREF crTxt)
{
	m_crSysInfoTxt = crTxt;
}

void CMarquee::SetDisplayType(DISPLAYTYPE nDt)
{
	m_nDisplayType	= nDt;
}

void CMarquee::SetScrollMode(SCROLLMODE nMode)
{
	m_nScrollMode = nMode;
}

void CMarquee::SetStopTime(int nCount)
{
	if (nCount <= 0)
		return;
	m_nStopTime = nCount;
}

void CMarquee::SetMoveSpeed(const UINT &nSpd)
{
	if(!nSpd)
		m_nMoveSpd = 1;
	else
		m_nMoveSpd = nSpd;
}

void CMarquee::EnableStop(BOOL bFlag)
{
	m_bEnableStop = bFlag;
}

/////////////////////////////////////////////////////////////////////////////

void CMarquee::ClearCustomStock()
{
	m_vecCustomStk.clear();
}

void CMarquee::ClearSysInfo()
{
	m_vecSysInfo.clear();
}

void CMarquee::AddSysInfo(const MARQUEEINFO &si)
{
	m_vecSysInfo.push_back(si);
	if (m_bRandom) 
	{	
		for (int j=1;j<si.weight;j++)
			m_vecSysInfo.push_back(si);		
		//随机顺序
		std::random_shuffle(m_vecSysInfo.begin(),m_vecSysInfo.end());
	}
}

void CMarquee::AddCustomStock(const MARQUEESTOCK &si)
{
	m_vecCustomStk.push_back(si);
}

//初始化
void CMarquee::InitPos()
{
	m_vecCustomPos.clear();
	m_vecSysInfoPos.clear();
	CPaintDC dc(this);
	VIEWPORT pos;
	int nLen = 0;
	//横向滚动
	if(m_nScrollMode == 1)
	{
		if (m_nDisplayType == 2) //股票
		{
			
			for(int i=0;i<m_vecCustomStk.size();i++)
			{
				pos.head = nLen;

				//自适应宽度
				pos.tail = pos.head  + 7*10 +(dc.GetTextExtent(m_vecCustomStk[i].sStkName).cx
					+dc.GetTextExtent(m_vecCustomStk[i].fStkCurrPrice).cx 
					+dc.GetTextExtent(m_vecCustomStk[i].fStkClsPrice).cx
					+dc.GetTextExtent(m_vecCustomStk[i].sStkVolumn).cx
					+dc.GetTextExtent(m_vecCustomStk[i].fStkTR).cx
					+dc.GetTextExtent(m_vecCustomStk[i].fStkVR).cx)*2 + 70;
								
				m_vecCustomPos.push_back(pos);
				nLen = pos.tail;
			}
		}
		else
		{
			for(int i=0;i<m_vecSysInfo.size();i++)
			{
				pos.head = nLen;
				pos.tail = pos.head + dc.GetTextExtent(m_vecSysInfo[i].content).cx + 
					dc.GetTextExtent(m_vecSysInfo[i].url).cx + 50;
				m_vecSysInfoPos.push_back(pos);
				nLen = pos.tail;
			}
		}
	}
	//纵向滚动
	else
	{
		if(m_nDisplayType == 2) //自选股
		{
			CRect rc = m_rcClient;
			for(int i=0;i<m_vecCustomStk.size();i++)
			{
				pos.head = nLen;
				pos.tail = pos.head + dc.GetTextExtent(m_vecCustomStk[i].sStkName).cy + 2;
				m_vecCustomPos.push_back(pos);
				nLen = pos.tail;
			}
		}
		else //系统消息
		{
			CRect rc = m_rcClient;
			for(int i=0;i<m_vecSysInfo.size();i++)
			{
				pos.head = nLen;
				pos.tail = pos.head + dc.GetTextExtent(m_vecSysInfo[i].content).cy + 2;
				m_vecSysInfoPos.push_back(pos);
				nLen = pos.tail;
			}
		}
	}
	m_nLength = nLen;
}

void CMarquee::Stop()
{
	KillTimer(m_nEventID);
	m_nEventID	= 0;
}

void CMarquee::StartScroll(UINT nEventID,UINT nInterval)
{
	if(!nEventID || !nInterval)
		return;

	if(m_nEventID)
		KillTimer(m_nEventID);

	InitPos();

	m_nEventID = nEventID;
	m_nMoveSpd = nInterval;

	SetTimer(m_nEventID, m_nMoveSpd, NULL);

	if(m_nScrollMode == SM_HSCROLL)
	{
		CRect rc = m_rcClient;
		m_stViewport.head = -rc.Width();
		m_stViewport.tail = 0;
	}
	else
	{
		CRect rc = m_rcClient;
		CPaintDC dc(this);
		m_stViewport.head = -dc.GetTextExtent("汉").cy;
		m_stViewport.tail = 0;
	}
}

void CMarquee::OnMouseMove(UINT nFlags, CPoint point) 
{
	if (m_nDisplayType == DT_SYSINFO) 
	{
		CRect rc = m_rcClient;
		CPaintDC dc(this);
		if (rc.PtInRect(point)) 
		{
			for(int i=0;i<m_vecSysInfo.size();i++)
			{
				CRect rect;
				if (m_nScrollMode == SM_HSCROLL) 
				{
					int nBegin  = rc.Width() + m_vecSysInfoPos[i].head - m_stViewport.tail;
					int nEnd	= nBegin + dc.GetTextExtent(m_vecSysInfo[i].content).cx;
					rect.SetRect( nBegin, 0, nEnd, rc.bottom);
				}
				else
				{
					int nBegin	= rc.left;
					int nEnd	= nBegin + dc.GetTextExtent(m_vecSysInfo[i].content).cx;
					rect.SetRect(nBegin,rc.top,nEnd,rc.bottom);
				}

				if (rect.PtInRect(point) && m_vecSysInfo[i].url[0] != '\0') 
				{
					HCURSOR hCur;
					hCur = ::LoadCursor(NULL, MAKEINTRESOURCE(32649));
					::SetCursor(hCur);
				}				
			}
		}
	}	
	else
	{
		CRect rc = m_rcClient;
		CPaintDC dc(this);

		if (rc.PtInRect(point)) 
		{
			for(int i=0;i<m_vecCustomStk.size();i++)
			{
				CRect rect;
				if (m_nScrollMode == SM_HSCROLL) 
				{
					int nBegin  = rc.Width() + m_vecCustomPos[i].head - m_stViewport.tail;
					int nEnd	= nBegin + 300;
					rect.SetRect( nBegin, 0, nEnd, rc.bottom);
				}
				else
				{
					int nBegin	= rc.left;
					int nEnd	= nBegin + 300;
					rect.SetRect(nBegin,rc.top,nEnd,rc.bottom);
				}

				if (rect.PtInRect(point)) 
				{
					HCURSOR hCur;
					hCur = ::LoadCursor(NULL, MAKEINTRESOURCE(32649));
					::SetCursor(hCur);					
				}				
			}
		}
	}
	CWnd::OnMouseMove(nFlags, point);
}

void CMarquee::OnLButtonDown(UINT nFlags, CPoint point) 
{
	//如果是系统信息
	if (m_nDisplayType == DT_SYSINFO)
	{
		CRect rc = m_rcClient;
		CPaintDC dc(this);

		if (rc.PtInRect(point)) 
		{
			for(int i=0;i<m_vecSysInfo.size();i++)
			{
				CRect rect;
				if (m_nScrollMode == SM_HSCROLL) 
				{
					int nBegin  = rc.Width() + m_vecSysInfoPos[i].head - m_stViewport.tail;
					int nEnd	= nBegin + dc.GetTextExtent(m_vecSysInfo[i].content).cx;
					rect.SetRect( nBegin, 0, nEnd, rc.bottom);
				}
				else
				{
					int nBegin	= rc.left;
					int nEnd	= nBegin + dc.GetTextExtent(m_vecSysInfo[i].content).cx;
					rect.SetRect(nBegin,rc.Height() + rc.top - m_stViewport.tail + m_vecSysInfoPos[i].head,nEnd,rc.bottom- m_stViewport.tail+m_vecSysInfoPos[i].tail);
				}

				if(rect.PtInRect(point)) 
				{	
					switch(m_vecSysInfo[i].flag)
					{
						//弹网页
						case IT_LINK:
							if(m_vecSysInfo[i].url[0] != '\0')
								ShellExecute(NULL,_T("Open"),m_vecSysInfo[i].url,NULL,NULL,SW_SHOW);
							break;
						//弹窗口
						case IT_POP:
							if(m_hMsgWnd)
								::PostMessage(m_hMsgWnd,m_uMsgID,MARQUEE_POPINFO,i);
							break;
						default:
							break;
					}	
				}				
			}
		}
	}
	//如果是股票信息
	else if (m_nDisplayType == DT_CUSTOMSTOCK)
	{
		CRect rc = m_rcClient;
		CPaintDC dc(this);		
		if (rc.PtInRect(point)) 
		{
			for(int i=0;i<m_vecCustomStk.size();i++)
			{
				CRect rect;
				if (m_nScrollMode == SM_HSCROLL) 
				{
					int nBegin  = rc.Width() + m_vecCustomPos[i].head - m_stViewport.tail;
					int nEnd	= nBegin + 300;
					rect.SetRect( nBegin, 0, nEnd, rc.bottom);
				}
				else
				{
					int nBegin	= rc.left;
					int nEnd	= nBegin + 300;
					rect.SetRect(nBegin,rc.Height() + rc.top - m_stViewport.tail + m_vecCustomPos[i].head,nEnd,rc.bottom- m_stViewport.tail+m_vecCustomPos[i].tail);
				}

				if (rect.PtInRect(point)) 
				{
					if(m_hMsgWnd)
						::PostMessage(m_hMsgWnd,m_uMsgID,MARQUEE_SHOWGP,LPARAM(m_vecCustomStk[i].nStkCode));
				}
			}
		}
	}
	CWnd::OnLButtonDown(nFlags, point);
}

void CMarquee::OnSize(UINT nType, int cx, int cy) 
{
	CWnd::OnSize(nType, cx, cy);

	GetClientRect(m_rcClient);
}

void CMarquee::OnTimer(UINT nIDEvent) 
{
	if(nIDEvent == m_nEventID)
	{
		if (m_bTimerFlag)
		{
			m_nStopCount++;
			if(m_nStopCount > m_nStopTime)
			{
				m_bTimerFlag = FALSE;
				m_stViewport.head++;
				m_stViewport.tail++;
			}
		}
		else
		{
			m_stViewport.head++;
			m_stViewport.tail++;
		}
		DrawIt();
	}
	CWnd::OnTimer(nIDEvent);
}

void CMarquee::DrawIt()
{
	CRect	rect;
	GetClientRect(rect);
	if(rect.Width()<=0 || rect.Height()<=0)
		return;

	CDC		memDC,*pDC = GetDC();
	//画背景
	memDC.CreateCompatibleDC(pDC);
	CBitmap bmp,*pOldBmp;
	bmp.CreateCompatibleBitmap(pDC,rect.Width(),rect.Height());
	pOldBmp = memDC.SelectObject(&bmp);

	memDC.SetBkMode(TRANSPARENT);
	memDC.SetTextColor(m_crSysInfoTxt);
	memDC.FillSolidRect(rect,m_crBackColor);
	memDC.Draw3dRect(rect,RGB(128,128,128),RGB(128,128,128));
	CFont font;
	font.CreateFont(14,0,0,0,FW_NORMAL,0,0,0,DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,DEFAULT_PITCH|FF_DONTCARE,SONG_FONT);
	CFont *pOldFont = memDC.SelectObject(&font);	
	//横向滚动
	if(m_nScrollMode==SM_HSCROLL) 
	{
		//显示系统信息
		if (m_nDisplayType == DT_SYSINFO) 
		{
			CRect rc = m_rcClient;
			int x1	= m_stViewport.tail;
			
			//循环播放
			if (m_stViewport.head > m_nLength) 
			{
				m_stViewport.head = -rc.Width();
				m_stViewport.tail = 0;
			}
			
			CRect rcItem;
			memDC.SetTextColor(m_crSysInfoTxt);

			for (int i=0;i<m_vecSysInfo.size();i++)
			{	
				//一屏暂停
				if(m_bEnableStop)
				{
					if(m_stViewport.head == m_vecSysInfoPos[i].head && !m_bTimerFlag)
					{
						m_bTimerFlag = TRUE;
						m_nStopCount = 0;
					}
				}

				if(m_stViewport.tail <= m_vecSysInfoPos[i].tail && m_stViewport.head >= m_vecSysInfoPos[i].head|| //subset
				   m_stViewport.tail >= m_vecSysInfoPos[i].tail && m_stViewport.head <= m_vecSysInfoPos[i].head|| //include
				   m_stViewport.tail >= m_vecSysInfoPos[i].head && m_stViewport.head <= m_vecSysInfoPos[i].head|| //left
				   m_stViewport.head <= m_vecSysInfoPos[i].tail && m_stViewport.tail >= m_vecSysInfoPos[i].tail) //right
				{
					rcItem.SetRect(rc.Width() - x1 + m_vecSysInfoPos[i].head ,rc.top,rc.Width()-x1 + m_vecSysInfoPos[i].tail,rc.bottom);
					memDC.DrawText(m_vecSysInfo[i].content,rcItem,DT_VCENTER|DT_LEFT|DT_SINGLELINE);
				}
			}
		}
		//显示股票
		else if (m_nDisplayType == DT_CUSTOMSTOCK) 
		{
			CRect rc = m_rcClient;
			CRect rcItem = rc;
			//循环播放
			if (m_stViewport.head > m_nLength) 
			{
				m_stViewport.head = -rc.Width();
				m_stViewport.tail = 0;
			}
			
			int x1	= m_stViewport.tail;
			
			for (int i=0;i<m_vecCustomStk.size();i++)
			{	
				//一屏暂停
				if(m_bEnableStop)
				{
					if(m_stViewport.head == m_vecCustomPos[i].head && !m_bTimerFlag)
					{
						m_bTimerFlag = TRUE;
						m_nStopCount = 0;
					}
				}

				CString strTmp;
				if(m_stViewport.tail <= m_vecCustomPos[i].tail && m_stViewport.head >= m_vecCustomPos[i].head|| //subset
				   m_stViewport.tail >= m_vecCustomPos[i].tail && m_stViewport.head <= m_vecCustomPos[i].head|| //include
				   m_stViewport.tail >= m_vecCustomPos[i].head && m_stViewport.head <= m_vecCustomPos[i].head|| //left
				   m_stViewport.head <= m_vecCustomPos[i].tail && m_stViewport.tail >= m_vecCustomPos[i].tail) //right
				{			
					//股票名称
					memDC.SetTextColor(m_crCustomStockTxt[0]);

					rcItem.SetRect(rc.Width() - x1 + m_vecCustomPos[i].head ,rc.top,rc.Width()-x1 + m_vecCustomPos[i].tail,rc.bottom);
					memDC.DrawText(m_vecCustomStk[i].sStkName,rcItem,DT_VCENTER|DT_LEFT|DT_SINGLELINE);
					
					FLOAT fChange = m_vecCustomStk[i].fStkCurrPrice - m_vecCustomStk[i].fStkClsPrice;
					if (fChange == 0.00)
						memDC.SetTextColor(m_crCustomStockTxt[1]);
					else if(fChange > 0.00)
						memDC.SetTextColor(m_crCustomStockTxt[2]);
					else
						memDC.SetTextColor(m_crCustomStockTxt[3]);
					
					//当前价		
					int x2	= max(60,memDC.GetTextExtent(strTmp).cx +0+10);
					rcItem.SetRect(rc.Width() - x1 + m_vecCustomPos[i].head + x2 ,rc.top,rc.Width()-x1 + m_vecCustomPos[i].tail + 60,rc.bottom);
					strTmp.Format("%.2f",m_vecCustomStk[i].fStkCurrPrice);
					memDC.DrawText(strTmp,rcItem,DT_VCENTER|DT_LEFT|DT_SINGLELINE);
					
					//涨跌值
					x2		= max(100,memDC.GetTextExtent(strTmp).cx +x2+10);
					rcItem.SetRect(rc.Width() - x1 + m_vecCustomPos[i].head + x2 ,rc.top,rc.Width()-x1 + m_vecCustomPos[i].tail + 100,rc.bottom);
					strTmp.Format("%.2f",fChange);
					memDC.DrawText(strTmp,rcItem,DT_VCENTER|DT_LEFT|DT_SINGLELINE);
					
					//涨跌幅
					x2		= max(150,memDC.GetTextExtent(strTmp).cx +x2+10);
					rcItem.SetRect(rc.Width() - x1 + m_vecCustomPos[i].head + x2 ,rc.top,rc.Width()-x1 + m_vecCustomPos[i].tail + 150,rc.bottom);
					strTmp.Format("%.2f%%",fChange/m_vecCustomStk[i].fStkClsPrice);
					memDC.DrawText(strTmp,rcItem,DT_VCENTER|DT_LEFT|DT_SINGLELINE);
					
					//成交量
					x2		= max(210,memDC.GetTextExtent(strTmp).cx +x2+10);
					rcItem.SetRect(rc.Width() - x1 + m_vecCustomPos[i].head + x2 ,rc.top,rc.Width()-x1 + m_vecCustomPos[i].tail + 200,rc.bottom);
					memDC.SetTextColor(m_crCustomStockTxt[4]);
					strTmp.Format("%s",m_vecCustomStk[i].sStkVolumn);
					memDC.DrawText(strTmp,rcItem,DT_VCENTER|DT_LEFT|DT_SINGLELINE);

					//换手率和量比
					x2		= max(250,memDC.GetTextExtent(strTmp).cx +x2+10);
					rcItem.SetRect(rc.Width() - x1 + m_vecCustomPos[i].head + x2 ,rc.top,rc.Width()-x1 + m_vecCustomPos[i].tail + 200,rc.bottom);
					memDC.SetTextColor(m_crCustomStockTxt[5]);
					strTmp.Format("%.2f%%%.2f",m_vecCustomStk[i].fStkTR,m_vecCustomStk[i].fStkVR);
					memDC.DrawText(strTmp,rcItem,DT_VCENTER|DT_LEFT|DT_SINGLELINE);
				}
			}
		}
	}
	//纵向滚动
	else if(m_nScrollMode==SM_VSCROLL)
	{
		//显示系统信息
		if (m_nDisplayType == DT_SYSINFO) 
		{	
			CRect rc = m_rcClient;
			int	  y1 = m_stViewport.tail;
			
			//循环播放
			if (m_stViewport.head > m_nLength) 
			{
				m_stViewport.head = -memDC.GetTextExtent("龙").cy;
				m_stViewport.tail = 0;
			}
			
			CRect rcItem;
			for (int i=0;i<m_vecSysInfo.size();i++)
			{	
				//一屏暂停
				if(m_bEnableStop)
				{
					if(m_stViewport.head == m_vecSysInfoPos[i].head && !m_bTimerFlag)
					{
						m_bTimerFlag = TRUE;
						m_nStopCount = 0;
					}
				}
				memDC.SetTextColor(m_crSysInfoTxt);
				
				if(m_stViewport.tail <= m_vecSysInfoPos[i].tail && m_stViewport.head >= m_vecSysInfoPos[i].head|| //subset
				   m_stViewport.tail >= m_vecSysInfoPos[i].tail && m_stViewport.head <= m_vecSysInfoPos[i].head|| //include
				   m_stViewport.tail >= m_vecSysInfoPos[i].head && m_stViewport.head <= m_vecSysInfoPos[i].head|| //left
				   m_stViewport.head <= m_vecSysInfoPos[i].tail && m_stViewport.tail >= m_vecSysInfoPos[i].tail) //right
				{
					rcItem.SetRect(0 ,1+rc.Height() + rc.top - y1 + m_vecSysInfoPos[i].head, rc.Width(), rc.bottom - y1 + m_vecSysInfoPos[i].tail);
					memDC.DrawText(m_vecSysInfo[i].content,rcItem,DT_LEFT|DT_SINGLELINE);
				}
			}
		}
		//垂直滚动自选股
		else 
		{
			CRect rc = m_rcClient;
			CRect rcItem = rc;

			int y1		= m_stViewport.tail;
			//循环播放
			if (m_stViewport.head > m_nLength) 
			{
				m_stViewport.head = -memDC.GetTextExtent("龙").cy;
				m_stViewport.tail = 0;
			}

			for (int i=0;i<m_vecCustomStk.size();i++)
			{	
				//一屏暂停
				if(m_bEnableStop)
				{
					if(m_stViewport.head == m_vecCustomPos[i].head && !m_bTimerFlag)
					{
						m_bTimerFlag = TRUE;
						m_nStopCount = 0;
					}
				}

				CString strTmp;
				if(m_stViewport.tail <= m_vecCustomPos[i].tail && m_stViewport.head >= m_vecCustomPos[i].head|| //subset
				   m_stViewport.tail >= m_vecCustomPos[i].tail && m_stViewport.head <= m_vecCustomPos[i].head|| //include
				   m_stViewport.tail >= m_vecCustomPos[i].head && m_stViewport.head <= m_vecCustomPos[i].head|| //top cross
				   m_stViewport.head <= m_vecCustomPos[i].tail && m_stViewport.tail >= m_vecCustomPos[i].tail) //down cross
				{
					//股票名称
					memDC.SetTextColor(m_crCustomStockTxt[0]);
					rcItem.SetRect(0 , 1 + rc.bottom - y1 + m_vecCustomPos[i].head, rc.Width(), rc.bottom - y1 + m_vecCustomPos[i].tail);
					memDC.DrawText(m_vecCustomStk[i].sStkName,rcItem,DT_LEFT|DT_SINGLELINE);
					
					FLOAT fChange = m_vecCustomStk[i].fStkCurrPrice - m_vecCustomStk[i].fStkClsPrice;
					if (fChange == 0.00)
						memDC.SetTextColor(m_crCustomStockTxt[1]);
					else if(fChange > 0.00)
						memDC.SetTextColor(m_crCustomStockTxt[2]);
					else
						memDC.SetTextColor(m_crCustomStockTxt[3]);
				
					//当前价
					int x1 = max(60,memDC.GetTextExtent(strTmp).cx +0+10);				
					rcItem.SetRect(x1 ,rc.Height()+rc.top - y1 + m_vecCustomPos[i].head , rc.Width(), rc.bottom - y1 + m_vecCustomPos[i].tail);
					strTmp.Format("%.2f",m_vecCustomStk[i].fStkCurrPrice);
					memDC.DrawText(strTmp,rcItem,DT_LEFT|DT_SINGLELINE);
					
					//涨跌值
					x1 = max(100,memDC.GetTextExtent(strTmp).cx + x1 + 10);
					rcItem.SetRect(x1 ,rc.Height()+rc.top - y1 + m_vecCustomPos[i].head , rc.Width(), rc.bottom - y1 + m_vecCustomPos[i].tail);
					strTmp.Format("%.2f",fChange);
					memDC.DrawText(strTmp,rcItem,DT_LEFT|DT_SINGLELINE);
					
					//涨跌幅
					x1 = max(150,memDC.GetTextExtent(strTmp).cx+x1 + 10) ;
					rcItem.SetRect(x1,rc.Height()+rc.top - y1 + m_vecCustomPos[i].head , rc.Width(), rc.bottom - y1 + m_vecCustomPos[i].tail);
					strTmp.Format("%.2f%%",fChange/m_vecCustomStk[i].fStkClsPrice);
					memDC.DrawText(strTmp,rcItem,DT_LEFT|DT_SINGLELINE);
					
					//成交量
					x1 = max(210,memDC.GetTextExtent(strTmp).cx+x1+10);
					rcItem.SetRect( x1,rc.Height()+rc.top - y1 + m_vecCustomPos[i].head , rc.Width(), rc.bottom - y1 + m_vecCustomPos[i].tail);
					memDC.SetTextColor(m_crCustomStockTxt[4]);
					strTmp.Format("%s",m_vecCustomStk[i].sStkVolumn);
					memDC.DrawText(strTmp,rcItem,DT_LEFT|DT_SINGLELINE);

					//换手率和量比
					x1 = max(250,memDC.GetTextExtent(strTmp).cx+x1+10);
					rcItem.SetRect( x1,rc.Height()+rc.top - y1 + m_vecCustomPos[i].head, rc.Width(), rc.bottom - y1 + m_vecCustomPos[i].tail);
					memDC.SetTextColor(m_crCustomStockTxt[5]);
					strTmp.Format("%.2f%%%.2f",m_vecCustomStk[i].fStkTR,m_vecCustomStk[i].fStkVR);
					memDC.DrawText(strTmp,rcItem,DT_LEFT|DT_SINGLELINE);
				}
			}
		}
	}
	
	pDC->BitBlt(0,0,rect.Width(),rect.Height(),&memDC,0,0,SRCCOPY);

	memDC.SelectObject(pOldFont);
	memDC.SelectObject(pOldBmp);
	font.DeleteObject();
	bmp.DeleteObject();
	memDC.DeleteDC();
	ReleaseDC(pDC);
}

void CMarquee::OnPaint() 
{
	DrawIt();
	CWnd::OnPaint();
}

BOOL CMarquee::OnEraseBkgnd(CDC* pDC) 
{
	return TRUE;
}
