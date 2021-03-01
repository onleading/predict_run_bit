// UHKNews.cpp: implementation of the UHKNews class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "tdxw.h"
#include "UHKNews.h"
#include "UnitView.h"
#include "HKNewsGet.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////////
vector<MENUINFOSTRUCT_PLUS> m_vConfig;
vector<MENUINFOSTRUCT_PLUS> m_vTitle;
char			m_strTypeSelID[64]={0}, m_strTitleSelID[64]={0}, m_strTypeFatherID[64]={0};
CString	m_NewsContent;
CCriticalSection	l_ThreadNews;
CHKNewsGet *l_pGetData=NULL;

struct ThreadInfo
{
	CWnd				*pView;
	MemStkInfo			*pStkInfo;
	MENUINFOSTRUCT_PLUS	*pCurSwitchTitle;
	CHKNewsGet			*pGetData;
};
ThreadInfo l_tmpInfo = {0};

DWORD WINAPI DownTitleList(LPVOID lParam)
{
	ThreadInfo *pInfo = (struct ThreadInfo *)lParam;

	l_ThreadNews.Lock();
	if(m_vConfig.empty())
	{
		m_vConfig.clear();
		if(pInfo->pGetData->GetNewsConfig(m_vConfig)&&m_vConfig.size()>0&&
			m_vConfig[0].mis.OuterID[0]&&m_vConfig[0].mis.uiTitleLen>0)
			pInfo->pCurSwitchTitle = &m_vConfig[0];
		pInfo->pView->PostMessage(UM_FILLREALINFO_MSG,1,0);
	}

	m_vTitle.clear();
	BOOL	bDataGetOK = TRUE;
	while (bDataGetOK && pInfo->pCurSwitchTitle)
	{
		if(pInfo->pStkInfo)
			bDataGetOK = pInfo->pGetData->GetTitleList(m_vTitle, pInfo->pCurSwitchTitle, pInfo->pStkInfo->setcode, pInfo->pStkInfo->Code);
		else 
			bDataGetOK = pInfo->pGetData->GetTitleList(m_vTitle, pInfo->pCurSwitchTitle);

		strcpy(m_strTypeSelID, pInfo->pCurSwitchTitle->mis.OuterID);
		pInfo->pView->PostMessage(UM_FILLREALINFO_MSG,2,0);
	}
	l_ThreadNews.Unlock();

	return 1;
}

DWORD WINAPI DownNewsContent(LPVOID lParam)
{
	ThreadInfo *pInfo = (struct ThreadInfo *)lParam;
	l_ThreadNews.Lock();
	if(pInfo->pCurSwitchTitle) 
	{
		if(pInfo->pStkInfo)
			pInfo->pGetData->GetContent(m_NewsContent, pInfo->pCurSwitchTitle, pInfo->pStkInfo->setcode, pInfo->pStkInfo->Code);
		else pInfo->pGetData->GetContent(m_NewsContent, pInfo->pCurSwitchTitle);

		strcpy(m_strTitleSelID, pInfo->pCurSwitchTitle->mis.OuterID);
	}
	l_ThreadNews.Unlock();

	pInfo->pView->PostMessage(UM_FILLREALINFO_MSG,3,0);

	return 1;
}
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

int l_nCurNewsSwitch=0;
UHKNews::UHKNews(LPCOMINFO pComInfo):UBase(pComInfo)
{
	m_vConfig.clear();
	m_vTitle.clear();

	m_nCurSwitch = l_nCurNewsSwitch;
	m_nCurSelType = m_nCurSelTitle = -1;
	TextH = 17;

	NewFont.CreateFont(g_nFontSize[1],0,0,0,g_nFontWeigth[1],0,0,0,(g_b2000XPSys&&stricmp(g_strFontName[1],"System")==0)?ANSI_CHARSET:DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,g_b2000XPSys?ANTIALIASED_QUALITY:DEFAULT_QUALITY,DEFAULT_PITCH|FF_DONTCARE,g_strFontName[1]);
	pVScrollBarType=new CMyScrollBar;
	pVScrollBarType->Create(_T("SysHeader32"),"",WS_CHILD|WS_VISIBLE,CRect(0,0,0,0),m_pView,ID_HK_VSCROLLTYPE);
	pVScrollBarType->Init(X_VERTICAL,m_pView->m_hWnd,UM_VSCROLLBAR,0);	
	if(pVScrollBarType)
	{
		pVScrollBarType->SetScrollRange(0,0);
		pVScrollBarType->SetScrollPos(0);
	}
	pVScrollBarTitle=new CMyScrollBar;
	pVScrollBarTitle->Create(_T("SysHeader32"),"",WS_CHILD|WS_VISIBLE,CRect(0,0,0,0),m_pView,ID_HK_VSCROLLLIST);
	pVScrollBarTitle->Init(X_VERTICAL,m_pView->m_hWnd,UM_VSCROLLBAR,1);
	if(pVScrollBarTitle)
	{
		pVScrollBarTitle->SetScrollRange (0,0);
		pVScrollBarTitle->SetScrollPos(0);
	}

	m_pContentEdit = NULL;
	if(m_nStyle==HKNEWS_BIG) 
	{
		m_pContentEdit = new CMyRichEdit;
		m_pContentEdit->Create(WS_VISIBLE|WS_CHILD|ES_AUTOVSCROLL|ES_READONLY|WS_VSCROLL|WS_TABSTOP|ES_MULTILINE,CRect(0,0,0,0),m_pView,NEWS_RICHEDIT);
		m_pContentEdit->SetMenuIndex(6);
		m_pContentEdit->ApplyTxtSize ();
		m_pContentEdit->GetDefaultCharFormat(cfm);
		PARAFORMAT pf;
		m_pContentEdit->GetParaFormat(pf);
		pf.cTabCount = 4;
		m_pContentEdit->SetParaFormat(pf);
		
		unsigned mask = ::SendMessage(m_pContentEdit->m_hWnd, EM_GETEVENTMASK, 0, 0);
		::SendMessage(m_pContentEdit->m_hWnd, EM_SETEVENTMASK, 0, mask | ENM_LINK);
		::SendMessage(m_pContentEdit->m_hWnd, EM_AUTOURLDETECT, true, 0);  //自动检测URL	
	}

	m_pListType = new CListBoxEx;
	m_pListType->Create(WS_CHILD|WS_VISIBLE|LBS_OWNERDRAWVARIABLE|LBS_HASSTRINGS|LBS_NOINTEGRALHEIGHT,CRect(0,0,0,0),m_pView,ID_EMINFO_LISTTYPE);
	m_pListType->m_pUnit = this;
	m_pListType->RegisterMsg(WM_LBUTTONUP);
	m_pListType->ResetContent();
	m_pListType->AddString("");

	m_pListTitle = new CListBoxEx;
	m_pListTitle->Create(WS_CHILD|WS_VISIBLE|LBS_OWNERDRAWVARIABLE|LBS_HASSTRINGS|LBS_NOINTEGRALHEIGHT,CRect(0,0,0,0),m_pView,ID_EMINFO_LISTTTITLE);
	m_pListTitle->m_pUnit = this;
	m_pListTitle->RegisterMsg(WM_LBUTTONUP);
	m_pListTitle->ResetContent();
	m_pListTitle->AddString("");

	m_pSwitch = new CNewZfjsTab(m_pView->m_hWnd);
	m_pSwitch->SetStyle(SwitchStyle_graybkbutton);
	m_pSwitch->Create(_T("NewZfjsTab"),"",WS_CHILD|WS_VISIBLE,CRect(0,0,0,0),m_pView,ID_HK_TYPESWITCH);

	m_pContentDlg = NULL;
	
	//设置控件颜色或字体
	RefreshScheme();
}

UHKNews::~UHKNews()
{
	TDEL(l_pGetData);

	TDEL(m_pContentDlg);
	TDEL(pVScrollBarType);
	TDEL(pVScrollBarTitle);
	TDEL(m_pListType);
	TDEL(m_pListTitle);
	TDEL(m_pContentEdit);
	l_nCurNewsSwitch=m_nCurSwitch;
}

//////////////////////////////////////////////////////////////////////////
void UHKNews::OnVScrollMessage(WPARAM wParam,LPARAM lParam)
{
	int nPos = (int)wParam;
	if(lParam==0)
	{
		pVScrollBarType->SetScrollPos(nPos);
		if(nPos >= 0 && nPos < m_vConfig.size())
			m_pListType->SetTopIndex(nPos);

		if(m_nCurSelType!=m_pListType->GetCurSel())
		{
			m_nCurSelType = m_pListType->GetCurSel();
			FillList();
		}
	}
	else if(lParam==1)
	{
		pVScrollBarTitle->SetScrollPos(nPos);
		if(nPos >= 0 && nPos < m_vTitle.size())
			m_pListTitle->SetTopIndex(nPos);
		
		if(m_nCurSelTitle!=m_pListTitle->GetCurSel())
		{
			m_nCurSelTitle = m_pListTitle->GetCurSel();
			FillContent();
		}
	}
}

void UHKNews::Resize()
{
	CRect rc=m_rcIn;

	float fBaseFlag = 1.0f;
	if(m_nStyle==HKNEWS_BIG) fBaseFlag = 0.25f;
	m_pListType->SetWindowPos(NULL,left,top+SWITCH_HEIGHT2,rc.Width()/4-15,fBaseFlag*rc.Height()-SWITCH_HEIGHT2,SWP_SHOWWINDOW);
	m_pListTitle->SetWindowPos(NULL,left+rc.Width()/4+1,top,3*rc.Width()/4-15,fBaseFlag*rc.Height(),SWP_SHOWWINDOW);
	if(m_nStyle==HKNEWS_BIG)
		m_pContentEdit->SetWindowPos(NULL,left,top+rc.Height()/4+1,rc.Width(),3*fBaseFlag*rc.Height(),SWP_SHOWWINDOW);

	pVScrollBarType->SetWindowPos(NULL,left+Width()/4-15,top+SWITCH_HEIGHT2,15,fBaseFlag*rc.Height()-SWITCH_HEIGHT2,SWP_SHOWWINDOW);
	int nVisualNum = (fBaseFlag*m_rcIn.Height()-SWITCH_HEIGHT2)/m_pListType->GetItemHeight();
	pVScrollBarType->SetScrollRange (0,max(m_pListType->GetCount()-nVisualNum,0));
	pVScrollBarType->SetScrollPos(pVScrollBarType->GetScrollPos());

	pVScrollBarTitle->SetWindowPos(NULL,right-15,top,15,fBaseFlag*rc.Height(),SWP_SHOWWINDOW);
	nVisualNum = (fBaseFlag*m_rcIn.Height())/m_pListTitle->GetItemHeight();
	pVScrollBarTitle->SetScrollRange (0,max(m_pListTitle->GetCount()-nVisualNum,0));
	pVScrollBarTitle->SetScrollPos(pVScrollBarTitle->GetScrollPos());

	m_pSwitch->SetWindowPos(NULL,left,top,rc.Width()/4,SWITCH_HEIGHT2,SWP_SHOWWINDOW);
	m_pSwitch->Invalidate();
}

void UHKNews::RefreshScheme()
{
 	pVScrollBarType->SetScrollColor(VipColor.AXISCOLOR,1,VipColor.BACKCOLOR,VipColor.AXISCOLOR);
	pVScrollBarTitle->SetScrollColor(VipColor.AXISCOLOR,1,VipColor.BACKCOLOR,VipColor.AXISCOLOR);
	NewFont.DeleteObject();
	NewFont.CreateFont(g_nFontSize[1],0,0,0,g_nFontWeigth[1],0,0,0,(g_b2000XPSys&&stricmp(g_strFontName[1],"System")==0)?ANSI_CHARSET:DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,g_b2000XPSys?ANTIALIASED_QUALITY:DEFAULT_QUALITY,DEFAULT_PITCH|FF_DONTCARE,g_strFontName[1]);
	LOGFONT lf;
	NewFont.GetLogFont(&lf);
 	if(::IsWindow(m_pListType->GetSafeHwnd()))
	{
		m_pListType->SetTextColor(VipColor.TXTCOLOR);
		m_pListType->SetBKColor(VipColor.BACKCOLOR);
		m_pListType->SetFocusColor(VipColor.FLOATCURSORCOLOR);
		m_pListType->SetSpecialItemColor(VipColor.LEVELCOLOR);
		m_pListType->SetFont(&NewFont);	
		m_pListType->SetItemHeight(labs(lf.lfHeight)+4);
	}
	if(::IsWindow(m_pListTitle->GetSafeHwnd()))
	{
		m_pListTitle->SetTextColor(VipColor.TXTCOLOR);
		m_pListTitle->SetBKColor(VipColor.BACKCOLOR);
		m_pListTitle->SetFocusColor(VipColor.FLOATCURSORCOLOR);
		m_pListTitle->SetSpecialItemColor(VipColor.LEVELCOLOR);
		m_pListTitle->SetFont(&NewFont);	
		m_pListTitle->SetItemHeight(labs(lf.lfHeight)+4);
	}
	m_pSwitch->SetFontColorSchema(VipColor.BACKCOLOR,VipColor.BACKCOLOR,VipColor.BACKCOLOR,VipColor.BACKCOLOR,VipColor.AXISCOLOR,VipColor.TXTCOLOR,VipColor.KNCOLOR,VipColor.LEVELCOLOR,RGB(128,0,0),12,SONG_FONT);
}

//////////////////////////////////////////////////////////////////////////
void UHKNews::FillRealTitleList()
{
	if(!m_pListTitle) return;

	m_pListTitle->SetRedraw(FALSE);
	m_pListTitle->ResetContent();
	if(!m_vTitle.empty())
	{
		int nCurSel = -1;
		for(int i=0;i<m_vTitle.size();i++)
		{
			m_pListTitle->AddString(m_vTitle[i].strShowName);
			m_pListTitle->SetItemData(m_pListTitle->GetCount()-1, (DWORD)&m_vTitle[i]);
			if(m_strTitleSelID[0]&&strcmp(m_vTitle[i].mis.OuterID, m_strTitleSelID)==0)
				nCurSel = m_pListTitle->GetCount()-1;
		}
		m_pListTitle->SetCurSel(nCurSel);
		m_nCurSelTitle = nCurSel;
	}
	else m_pListTitle->AddString("");
	m_pListTitle->SetRedraw(TRUE);
	if(pVScrollBarTitle)
	{
		int nVisualNum = (m_rcIn.Height()/4)/m_pListTitle->GetItemHeight();
		pVScrollBarTitle->SetScrollRange (0,max(m_pListTitle->GetCount()-nVisualNum,0));
		pVScrollBarTitle->SetScrollPos(0);
	}
}

void UHKNews::FillRealTypeList()
{
	if(!m_pListType) return;

	m_pListType->SetRedraw(FALSE);
	m_pListType->ResetContent();
	m_pSwitch->ClearAllLabel();
	if(!m_vConfig.empty())
	{
		int nCurSel = -1;
		int nTypeTab = 0;
		for(int i=0;i<m_vConfig.size();i++)
		{
			if(strcmp(m_vConfig[i].mis.iFatherOuterID, "0")==0)				//父id
			{
				m_pSwitch->InsertSwitchLabel(nTypeTab++, m_vConfig[i].strShowName, m_vConfig[i].strShowName, 
					UM_HKNEWSSWITCH_MSG, m_vConfig[i].mis.iInnerID);
				if(strlen(m_strTypeFatherID)<1) 
				{
					strcpy(m_strTypeFatherID, m_vConfig[i].mis.OuterID);
					m_pSwitch->SetCurLabel(0);
				}
				continue;
			}
		}

		for(i=0;i<m_vConfig.size();i++)
		{
			if(strlen(m_strTypeFatherID)<1||strcmp(m_vConfig[i].mis.iFatherOuterID, m_strTypeFatherID)==0)
			{
				m_pListType->AddString(m_vConfig[i].strShowName);
				m_pListType->SetItemData(m_pListType->GetCount()-1, (DWORD)&m_vConfig[i]);
				if(m_strTypeSelID[0]&&strcmp(m_vConfig[i].mis.OuterID, m_strTypeSelID)==0)
					nCurSel = m_pListType->GetCount()-1;
			}	
		}
		m_pListType->SetCurSel(nCurSel);
		m_nCurSelType = nCurSel;
	}
	else m_pListType->AddString("");
	m_pListType->SetRedraw(TRUE);
	if(pVScrollBarType)
	{
		int nVisualNum = (m_rcIn.Height()/4)/m_pListType->GetItemHeight();
		pVScrollBarType->SetScrollRange (0,max(m_pListType->GetCount()-nVisualNum,0));
		pVScrollBarType->SetScrollPos(0);
	}
	Resize();
}

void UHKNews::FillRealContent()
{
	if(!m_pContentEdit) 
	{
		if(m_pContentDlg)
		{
			if (m_pContentDlg->IsIconic()) 
				m_pContentDlg->ShowWindow(SW_RESTORE);
		}
		else
		{
			m_pContentDlg = new CHKNewsContentDlg;
			m_pContentDlg->m_pNewsUnit = this;
			m_pContentDlg->Create(IDD_HKNEWSCONTENT);
			m_pContentDlg->CenterWindow();
			m_pContentDlg->ShowWindow (SW_SHOW);
		}
		m_pContentDlg->UpdateContent("\r\n"+m_NewsContent);
		for(int i=0;i<m_vTitle.size();i++)
		{
			if(m_strTitleSelID[0]&&strcmp(m_vTitle[i].mis.OuterID, m_strTitleSelID)==0)
				m_pContentDlg->SetWindowText(m_vTitle[i].strShowName);
		}
		return;
	}

	m_pContentEdit->SetRedraw(FALSE);
	m_pContentEdit->SetWindowText("");
	m_pContentEdit->SetWindowText("\r\n"+m_NewsContent);
	m_pContentEdit->SetRedraw(TRUE);
	m_pContentEdit->Invalidate();
}

//请求列表
void UHKNews::FillList()
{
	if(!l_pGetData) 
		l_pGetData = new CHKNewsGet;
	l_tmpInfo.pView = m_pView;
	if(m_vConfig.size()&&m_pListType->GetCount()>0)
	{
		int nSel = m_pListType->GetCurSel();
		l_tmpInfo.pCurSwitchTitle = (MENUINFOSTRUCT_PLUS*)m_pListType->GetItemData(nSel);
	}

	DWORD	ThreadID;
	HANDLE	ThreadHandle = ::CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)DownTitleList,(LPVOID)(&l_tmpInfo),0,&ThreadID);

	l_tmpInfo.pGetData = l_pGetData;
}

void  UHKNews::FillContent()
{
	if(!l_pGetData) 
		l_pGetData = new CHKNewsGet;
	l_tmpInfo.pView = m_pView;
	if(m_vTitle.size()&&m_pListTitle->GetCount()>0)
	{
		int nSel = m_pListTitle->GetCurSel();
		l_tmpInfo.pCurSwitchTitle = (MENUINFOSTRUCT_PLUS*)m_pListTitle->GetItemData(nSel);
	}

	DWORD	ThreadID;
	HANDLE	ThreadHandle = ::CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)DownNewsContent,(LPVOID)(&l_tmpInfo),0,&ThreadID);

	l_tmpInfo.pGetData = l_pGetData;
}

//////////////////////////////////////////////////////////////////////////
int  UHKNews::ProcessMsg(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{
	case UM_READDATA:
		FillList();
		return 1;
	case UM_FILLREALINFO_MSG:
		{
			if(wParam==1) FillRealTypeList();
			if(wParam==2) FillRealTitleList();
			if(wParam==3) FillRealContent();
		}
		break;
	case WM_PAINT:
		{
			if(wParam == 1) 				
			{
				ProcessMsg(UM_READDATA,lParam);
				break;
			}

			CDC *pDC = m_pView->GetDC();
			pDC->SetBkMode(TRANSPARENT);
			if(!l_pGetData->GetConnectStatus()||m_vConfig.empty()) 
				g_d.Bar(pDC,&m_rcIn,BACK_BRUSH);
			CPen  *oldpen = g_d.SelectPen(pDC,KP_PEN);
			if(m_nStyle==HKNEWS_BIG) 
			{
				g_d.DrawLine(pDC, left, top+Height()/4, right, top+Height()/4);
				g_d.DrawLine(pDC, left+Width()/4, top, left+Width()/4, top+Height()/4);
			}
			else g_d.DrawLine(pDC, left+Width()/4, top, left+Width()/4, bottom);
			g_d.RestorePen(pDC,oldpen);

			m_pView->ReleaseDC(pDC);
		}
		return 1;
	case WM_SIZE:
		UBase::ProcessMsg(message,wParam,lParam);
		Resize();
		return 1;
	case UM_VSCROLLBAR:
		OnVScrollMessage(wParam,lParam);
		return 1;
	case WM_LBUTTONUP:
		{
			if(m_pListType&&m_nCurSelType!=m_pListType->GetCurSel())
			{
				m_nCurSelType = m_pListType->GetCurSel();
				FillList();
			}
			if(m_pListTitle&&m_nCurSelTitle!=m_pListTitle->GetCurSel())
			{
				m_nCurSelTitle = m_pListTitle->GetCurSel();
				FillContent();
			}
		}
		break;
	case UM_HKNEWSSWITCH_MSG:
		{
			if(wParam>=0&&wParam<m_vConfig.size())
			{
				strcpy(m_strTypeFatherID, m_vConfig[wParam].mis.OuterID);
				FillRealTypeList();
			}
		}
		break;
	default:
		break;
	}
	return UBase::ProcessMsg(message,wParam,lParam);
}

void UHKNews::KillDlg()
{
	if(m_pContentDlg)
	{
		m_pContentDlg->DestroyWindow();
		TDEL(m_pContentDlg);
	}
}
