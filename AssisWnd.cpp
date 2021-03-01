// assiswnd.cpp : implementation file
//

#include "stdafx.h"
#include "tdxw.h"
#include "assiswnd.h"
#include "atlconv.h"
#include "mshtml.h"
#include <afxdisp.h>
#include <atlbase.h>
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAssisWnd

static int l_whichGn = 0;

IMPLEMENT_DYNCREATE(CAssisWnd, CWnd) //动态创建的类，在NCDESTROY中必须delete this

////////////////////////////////////////////////////////////////////////////////////////////////

CAssisWnd::CAssisWnd()
{
	KeyGuyFlag = FALSE;
	m_bLayoutFlag = FALSE;
	m_nDataFlag = 0;
	m_bStartFunc = 0;
	m_pExplorer = NULL;
	m_bWebMode  = FALSE;
	m_whichGn = -1;
}

CAssisWnd::~CAssisWnd()
{
}


BEGIN_MESSAGE_MAP(CAssisWnd, CWnd)
	ON_MESSAGE(UM_SHOWIT_MESSAGE,OnShowItMessage)
	ON_MESSAGE(UM_STARTFUNC_MESSAGE,OnStartFuncMessage)
	ON_MESSAGE(UM_IE_BACK_MESSAGE,OnIEBack)
	ON_MESSAGE(UM_IE_FORWARD_MESSAGE,OnIEForward)
	ON_MESSAGE(UM_IE_MAINPAGE_MESSAGE,OnIEMainPage)
	ON_MESSAGE(UM_DOLEVINBUY_MESSAGE,OnDoLevinBuyMessage)
	ON_MESSAGE(UM_DOLEVINSELL_MESSAGE,OnDoLevinSellMessage)
	ON_MESSAGE(UM_DOWTCANCEL_MESSAGE,OnDoWTCancelMessage)
	ON_MESSAGE(UM_DOQZSDS_MESSAGE,OnDoQzSDSMessage)
	//{{AFX_MSG_MAP(CAssisWnd)
	ON_WM_CREATE()
	ON_WM_ERASEBKGND()
	ON_WM_SIZE()
	ON_WM_DESTROY()
	ON_WM_NCDESTROY()
	ON_WM_PAINT()
	ON_WM_SHOWWINDOW()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CAssisWnd message handlers

BEGIN_EVENTSINK_MAP(CAssisWnd, CWnd)
    //{{AFX_EVENTSINK_MAP(CAssisWnd)
	ON_EVENT(CAssisWnd, IDC_CUSTOMIE, 259 /* DocumentComplete  */, DocumentComplete,VTS_DISPATCH VTS_PVARIANT)
	ON_EVENT(CAssisWnd, IDC_CUSTOMIE, 250 /* BeforeNavigate2 */, OnBeforeNavigate2, VTS_DISPATCH VTS_PVARIANT VTS_PVARIANT VTS_PVARIANT VTS_PVARIANT VTS_PVARIANT VTS_PBOOL)
	ON_EVENT(CAssisWnd, IDC_CUSTOMIE, 251 /* NewWindow2 */, OnNewWindow2, VTS_PDISPATCH VTS_PBOOL)
	ON_EVENT(CAssisWnd, IDC_CUSTOMIE, 263 /* WindowClosing */, OnWindowClosingExplorer1, VTS_BOOL VTS_PBOOL)
	//}}AFX_EVENTSINK_MAP
END_EVENTSINK_MAP()

char *FAIL_HTML_HEAD=
"	<html>\
	<head>\
		<meta http-equiv=\'Content-Type\' content=\'text/html; charset=gb2312\'>\
		<link href=\"%s\" rel=\"stylesheet\" type=\"text/css\">\
		<script Language=javascript>\
			function Click(){\
				window.event.returnValue=false;\
			}\
			document.oncontextmenu=Click;\
		</script>\
	</head>\
	<body><font face=\"%s\" style=\"font-size:%dpx;line-height:%dpx;\">%s\
";

void AfterWebPageFail(CWebBrowser2 *pIE,short whichGn,const char *failstr)
{
	if(!pIE) return;

	char PriDir[MAX_PATH];
	sprintf(PriDir,"%s",g_WSXHStr);

	CString sBase;
	sBase.Format(FAIL_HTML_HEAD,PriDir+(CString)"tmptdx.css",SONG_FONT,14,20,failstr);	
	CFile file;
	char tmpFile[MAX_PATH];
	sprintf(tmpFile,"%stmp\\tmp%02d.htm",PriDir,whichGn);
	if(file.Open(tmpFile,CFile::modeCreate|CFile::modeWrite))
	{
		file.Write(sBase,sBase.GetLength());
		file.Close();
	}
	pIE->SetSilent(TRUE);
	pIE->Navigate(tmpFile,NULL,NULL,NULL,NULL);
}

int CAssisWnd::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;	
	m_bLayoutFlag = TRUE;
	m_whichGn = l_whichGn;
	l_whichGn++;
	//将url中的tdx_username和tdx_password换成对应的用户名和密码
	CString tmpStr=g_ChatArea.ChatURL[m_whichGn];
	tmpStr.Replace("tdx_username",g_strLoginName);
	if(tmpStr.Find("tdx_md5password")>=0)
	{
		char md5Password[40];
		MD5_String(g_strPassword,md5Password);
		tmpStr.Replace("tdx_md5password",md5Password);
	}
	else
		tmpStr.Replace("tdx_password",g_strPassword);
	strcpy(g_ChatArea.ChatURL[m_whichGn],tmpStr);
	//
	if(bRestShow)
		PostMessage(UM_STARTFUNC_MESSAGE,0,0);
	return 0;
}

void CAssisWnd::OnShowItMessage(WPARAM wParam,LPARAM lParam)
{
	if(m_bStartFunc && g_ChatArea.SwitchRefresh[m_whichGn]==0) return;
	if(m_bLayoutFlag)
	{
		if(g_ChatArea.AreaType[m_whichGn]==AREA_NORMAL_WEBPAGE || g_ChatArea.AreaType[m_whichGn]==AREA_UID_PWD_WEBPAGE)
		{
			m_bStartFunc = 1;
			if(m_pExplorer)
			{
				try
				{
					m_pExplorer->Refresh();
				}
				catch(...)
				{
				}
				return;
			}
			m_pExplorer = new CWebBrowser2;
			CRect rect;
			GetClientRect(&rect);
			m_pExplorer->Create(NULL,NULL,WS_CHILD|WS_VISIBLE,rect,this,IDC_CUSTOMIE);
			m_bWebMode = TRUE;
			if(g_ChatArea.AreaType[m_whichGn]==AREA_UID_PWD_WEBPAGE && !g_bIsGuest)//如果是注册用户登录方式
			{
				for(int i=0;i < g_ChatArea.URLDataNum[m_whichGn];i++)
				{
					if(i==0 && g_ChatArea.URLData[m_whichGn][i].InputType == EDIT_INPUT_TYPE)
						strcpy(g_ChatArea.URLData[m_whichGn][i].SetValue,g_strLoginName);
					else if(i==1 && g_ChatArea.URLData[m_whichGn][i].InputType == EDIT_INPUT_TYPE)
						strcpy(g_ChatArea.URLData[m_whichGn][i].SetValue,g_strPassword);				
				}
				m_nDataFlag = 1;
			}
			try	//不让其出现错误对话框
			{
				m_pExplorer->SetSilent(TRUE);
				m_pExplorer->Navigate(g_ChatArea.ChatURL[m_whichGn],NULL,NULL,NULL,NULL);
			}
			catch(...)
			{
			}				
		}
	}
}

void CAssisWnd::OnStartFuncMessage(WPARAM wParam,LPARAM lParam)
{
	if(!g_bEmbWTCreated && g_ChatArea.AreaType[m_whichGn]==AREA_WT)
	{
		if(tc_mod && TC_GetJyStatus_(NULL,NULL,NULL)&JY_LOGINED)
		{
			TC_CreateAll_(m_hWnd,0);
			if(g_pMainWin && (((CMainFrame *)g_pMainWin)->m_wndRestspace).IsFullSize())
				TC_ArrangeWnd_(1);
			else
				TC_ArrangeWnd_(0);
			g_bEmbWTCreated=TRUE;
		}
		return;
	}
	if(m_bStartFunc) return;
	if(g_ChatArea.AreaType[m_whichGn]==AREA_MINE)
	{
		m_bStartFunc = 1;
		INFO_ShowFunc(m_hWnd,IGN_MINE,TRUE,0,0,NULL,0,NULL,NULL,-1,0);
	}
	else if(g_ChatArea.AreaType[m_whichGn]==AREA_FLZL)
	{
		m_bStartFunc = 1;
		INFO_ShowFunc(m_hWnd,IGN_FLZL,FALSE,0,0,NULL,0,NULL,NULL,-1,0);
	}
	else if(g_ChatArea.AreaType[m_whichGn]==AREA_GGBW)
	{
		m_bStartFunc = 1;
		INFO_ShowFunc(m_hWnd,IGN_GGBW,FALSE,0,0,NULL,0,NULL,NULL,-1,0);
	}
}

BOOL CAssisWnd::OnEraseBkgnd(CDC* pDC) 
{
	return TRUE;
}

void CAssisWnd::OnSize(UINT nType, int cx, int cy) 
{
	CWnd::OnSize(nType, cx, cy);
	if(m_bWebMode && m_pExplorer)
		m_pExplorer->MoveWindow(-2,-2,cx+4,cy+4);
	if(g_ChatArea.AreaType[m_whichGn]>=AREA_MINE && g_ChatArea.AreaType[m_whichGn]<=AREA_GGBW)
	{
		const MSG *pMsg=CWnd::GetCurrentMessage();
		if(pMsg->hwnd==m_hWnd)
		{
			switch(g_ChatArea.AreaType[m_whichGn])
			{
			case AREA_MINE:
				INFO_ProcessMsg(IGN_MINE,pMsg->hwnd,pMsg->message,pMsg->wParam,pMsg->lParam);
				break;
			case AREA_FLZL:
				INFO_ProcessMsg(IGN_FLZL,pMsg->hwnd,pMsg->message,pMsg->wParam,pMsg->lParam);
				break;
			case AREA_GGBW:
				INFO_ProcessMsg(IGN_GGBW,pMsg->hwnd,pMsg->message,pMsg->wParam,pMsg->lParam);
				break;
			}
		}
	}
	else if(g_ChatArea.AreaType[m_whichGn]==AREA_WT && TC_ArrangeWnd_)
	{
		if(g_pMainWin && (((CMainFrame *)g_pMainWin)->m_wndRestspace).IsFullSize())
			TC_ArrangeWnd_(1);
		else
			TC_ArrangeWnd_(0);
	}
}

void CAssisWnd::OnDestroy() 
{
	TDEL(m_pExplorer);
	CWnd::OnDestroy();
}

LRESULT CAssisWnd::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	return CWnd::WindowProc(message, wParam, lParam);
}

BOOL CAssisWnd::PreTranslateMessage(MSG* pMsg) 
{
	if (pMsg->message == WM_CHAR && KeyGuyFlag == FALSE && (g_ChatArea.AreaType[m_whichGn]==AREA_MINE||g_ChatArea.AreaType[m_whichGn]==AREA_FLZL||g_ChatArea.AreaType[m_whichGn]==AREA_GGBW))
	{
		if(ProcessKeyGuy(pMsg,this,KeyGuyFlag))
			return TRUE;
	}
	if(g_ChatArea.AreaType[m_whichGn]==AREA_WT && TC_PreTransMsg_)
		return TC_PreTransMsg_(pMsg);
	return CWnd::PreTranslateMessage(pMsg);
}

void CAssisWnd::DocumentComplete(LPDISPATCH /* pDisp */, VARIANT* URL) 
{
	USES_CONVERSION;
	if(m_nDataFlag==0 || !m_pExplorer || g_bIsGuest || g_ChatArea.AreaType[m_whichGn]!=AREA_UID_PWD_WEBPAGE )
		return;
	m_nDataFlag = 0;
	LPDISPATCH	pDisp = m_pExplorer->GetDocument();
	CComQIPtr<IHTMLDocument2,&IID_IHTMLDocument2> pDocument = pDisp;
	IHTMLElementCollection *pall;
	if(FAILED(pDocument->get_all(&pall)))	
		return;
	WCHAR	wsztmp[256];
	CString	SetValue;
	COleVariant	name,aTemp;
	long i=0,ItemSize=sizeof(wsztmp)/sizeof(wsztmp[0]),NOExitFlag=1;
	IEINPUTDATA tmpInputData;
	for(;i<g_ChatArea.URLDataNum[m_whichGn] && NOExitFlag;++i)
	{
		tmpInputData = g_ChatArea.URLData[m_whichGn][i];
		MultiByteToWideChar(CP_ACP,0,tmpInputData.FieldName,strlen(tmpInputData.FieldName)+1,wsztmp,ItemSize);
		name = ::SysAllocString((const OLECHAR*)wsztmp); 
		long tmpLen=0;
		pall->get_length(&tmpLen);
		if(FAILED(pall->item(name,aTemp,&pDisp))||pDisp==NULL)	
			break;
		switch(tmpInputData.InputType)
		{
		case	EDIT_INPUT_TYPE:
		{
			CComQIPtr<IHTMLInputTextElement,&IID_IHTMLInputTextElement> pInput = pDisp;
			if(pInput==NULL)	NOExitFlag = 0;
			else
			{
				SetValue = tmpInputData.SetValue;
				pInput->put_value(SetValue.AllocSysString());
			}
			break;
		}
		case	SELECT_INPUT_TYPE:
		{
			CComQIPtr<IHTMLSelectElement,&IID_IHTMLSelectElement> pSelect = pDisp;
			if(pSelect==NULL)	NOExitFlag = 0;
			else
			{
				SetValue = tmpInputData.SetValue;
				pSelect->put_value(SetValue.AllocSysString());
			}
			break;
		}
		case	BTN_SUBMIT_TYPE:
		{
			CComQIPtr<IHTMLButtonElement,&IID_IHTMLButtonElement> pBtn = pDisp;
			if(pBtn==NULL) 	NOExitFlag = 0;
			else
			{
				IHTMLFormElement	*pForm;
				pBtn->get_form(&pForm);
				pForm->submit();
			}
			break;
		}
		case	FORM_SUBMIT_TYPE:
		{
			CComQIPtr<IHTMLFormElement,&IID_IHTMLFormElement> pForm = pDisp;
			if(pForm!=NULL)	pForm->submit();
			NOExitFlag = 0;
			break;
		}
		default:break;
		}
	}
}

void CAssisWnd::ProcessSpecialLink(char *codename)
{
	if(strlen(codename) != 1+SH_CODE_LEN) return;
	MemStkInfo *pInfo;
	int setcode=codename[0]-'0';
	pInfo = g_pStockIO->Get_MemStkInfo(codename+1,setcode);
	if(pInfo) ToZst(pInfo->Code,pInfo->setcode,FALSE);
}

void CAssisWnd::OnBeforeNavigate2(LPDISPATCH pDisp, VARIANT FAR* URL, VARIANT FAR* Flags, VARIANT FAR* TargetFrameName, VARIANT FAR* PostData, VARIANT FAR* Headers, BOOL FAR* Cancel) 
{	
	USES_CONVERSION;
	if(m_pExplorer && g_ChatArea.AreaType[m_whichGn]==AREA_USERINFO)
	{
		char url[500];
		strcpy(url,W2A(URL->bstrVal));
		int len=strlen(url);
		for(int i=0;i<len;i++)
		{
			url[i]=toupper(url[i]);
		}
		if(memicmp(url,"HTTP://CMD/NEWNAME/",19)==0)
		{
			((CMainFrame *)AfxGetMainWnd())->ProcessNewUser(url+19);
			*Cancel=TRUE;	
		}
		else if(memicmp(url,"HTTP://CMD/",11)==0)
		{
			ProcessSpecialLink(url+11);
			*Cancel=TRUE;	
		}
	}
}

void CAssisWnd::OnNewWindow2(LPDISPATCH FAR* ppDisp, BOOL FAR* Cancel) 
{
}

void CAssisWnd::OnWindowClosingExplorer1(BOOL IsChildWindow, BOOL FAR* Cancel) 
{
	//不能用户在网页中关闭
	*Cancel=TRUE;
}

void CAssisWnd::OnNcDestroy() 
{
	CWnd::OnNcDestroy();
	
	delete this;	
}

void CAssisWnd::OnPaint() 
{
	CPaintDC dc(this);
	
	if(g_ChatArea.AreaType[m_whichGn]>=AREA_MINE && g_ChatArea.AreaType[m_whichGn]<=AREA_USERINFO)
	{
		CRect rect;
		GetClientRect(&rect);
		dc.FillSolidRect(&rect,VipColor.BACKCOLOR);
	}
}

void CAssisWnd::OnIEBack(WPARAM wParam,LPARAM lParam)
{
	if(m_pExplorer && m_bLayoutFlag)
	{
		try	//不让其出现错误对话框
		{
			m_pExplorer->GoBack();
		}
		catch(...)
		{
		}
	}
}

void CAssisWnd::OnIEForward(WPARAM wParam,LPARAM lParam)
{
	if(m_pExplorer && m_bLayoutFlag)
	{
		try	//不让其出现错误对话框
		{
			m_pExplorer->GoForward();
		}
		catch(...)
		{
		}
	}
}

void CAssisWnd::OnIEMainPage(WPARAM wParam,LPARAM lParam)
{
	if(m_pExplorer && m_bLayoutFlag && (g_ChatArea.AreaType[m_whichGn]==AREA_NORMAL_WEBPAGE || g_ChatArea.AreaType[m_whichGn]==AREA_UID_PWD_WEBPAGE) )
	{
		try	//不让其出现错误对话框
		{
			m_pExplorer->SetSilent(TRUE);
			m_pExplorer->Navigate(g_ChatArea.ChatURL[m_whichGn],NULL,NULL,NULL,NULL);
		}
		catch(...)
		{
		}
	}
}

void CAssisWnd::DoLevin(short which,MemStkInfo *tmpInfo,short type)
{
	if(!tmpInfo || !TC_DoLevinJy_) return;
	short xsflag=GetXSFlag(tmpInfo);
	struct CurrStockData tmpHQ;
	struct CurrStockData_L2 tmpHQ_L2;
	g_pStockIO->ReadCurrStockData(tmpInfo,&tmpHQ,&tmpHQ_L2);
	float tmpPrice=0.0;
	long tmpVolume=0;
	switch(which)
	{
	case 0: tmpPrice=tmpHQ.Now;break;
	case 1: 
		tmpPrice=tmpHQ.Buyp[0];
		tmpVolume=tmpHQ.Buyv[0];
		break;
	case 2: 
		tmpPrice=tmpHQ.Buyp[1];
		tmpVolume=tmpHQ.Buyv[0]+tmpHQ.Buyv[1];
		break;
	case 3: 
		tmpPrice=tmpHQ.Buyp[2];
		tmpVolume=tmpHQ.Buyv[0]+tmpHQ.Buyv[1]+tmpHQ.Buyv[2];
		break;
	case 4: 
		tmpPrice=tmpHQ.Buyp[3];
		tmpVolume=tmpHQ.Buyv[0]+tmpHQ.Buyv[1]+tmpHQ.Buyv[2]+tmpHQ.Buyv[3];
		break;
	case 5: 
		tmpPrice=tmpHQ.Buyp[4];
		tmpVolume=tmpHQ.Buyv[0]+tmpHQ.Buyv[1]+tmpHQ.Buyv[2]+tmpHQ.Buyv[3]+tmpHQ.Buyv[4];
		break;
	case 6: 
		tmpPrice=tmpHQ_L2.Buyp[0];
		tmpVolume=tmpHQ.Buyv[0]+tmpHQ.Buyv[1]+tmpHQ.Buyv[2]+tmpHQ.Buyv[3]+tmpHQ.Buyv[4]
			+tmpHQ_L2.Buyv[0];
		break;
	case 7: 
		tmpPrice=tmpHQ_L2.Buyp[1];
		tmpVolume=tmpHQ.Buyv[0]+tmpHQ.Buyv[1]+tmpHQ.Buyv[2]+tmpHQ.Buyv[3]+tmpHQ.Buyv[4]
			+tmpHQ_L2.Buyv[0]+tmpHQ_L2.Buyv[1];
		break;
	case 8: 
		tmpPrice=tmpHQ_L2.Buyp[2];
		tmpVolume=tmpHQ.Buyv[0]+tmpHQ.Buyv[1]+tmpHQ.Buyv[2]+tmpHQ.Buyv[3]+tmpHQ.Buyv[4]
			+tmpHQ_L2.Buyv[0]+tmpHQ_L2.Buyv[1]+tmpHQ_L2.Buyv[2];
		break;
	case 9: 
		tmpPrice=tmpHQ_L2.Buyp[3];
		tmpVolume=tmpHQ.Buyv[0]+tmpHQ.Buyv[1]+tmpHQ.Buyv[2]+tmpHQ.Buyv[3]+tmpHQ.Buyv[4]
			+tmpHQ_L2.Buyv[0]+tmpHQ_L2.Buyv[1]+tmpHQ_L2.Buyv[2]+tmpHQ_L2.Buyv[3];
		break;
	case 10: 
		tmpPrice=tmpHQ_L2.Buyp[4];
		tmpVolume=tmpHQ.Buyv[0]+tmpHQ.Buyv[1]+tmpHQ.Buyv[2]+tmpHQ.Buyv[3]+tmpHQ.Buyv[4]
			+tmpHQ_L2.Buyv[0]+tmpHQ_L2.Buyv[1]+tmpHQ_L2.Buyv[2]+tmpHQ_L2.Buyv[3]+tmpHQ_L2.Buyv[4];
		break;

	case 11: 
		tmpPrice=tmpHQ.Sellp[0];
		tmpVolume=tmpHQ.Sellv[0];
		break;
	case 12: 
		tmpPrice=tmpHQ.Sellp[1];
		tmpVolume=tmpHQ.Sellv[0]+tmpHQ.Sellv[1];
		break;
	case 13: 
		tmpPrice=tmpHQ.Sellp[2];
		tmpVolume=tmpHQ.Sellv[0]+tmpHQ.Sellv[1]+tmpHQ.Sellv[2];
		break;
	case 14: 
		tmpPrice=tmpHQ.Sellp[3];
		tmpVolume=tmpHQ.Sellv[0]+tmpHQ.Sellv[1]+tmpHQ.Sellv[2]+tmpHQ.Sellv[3];
		break;
	case 15: 
		tmpPrice=tmpHQ.Sellp[4];
		tmpVolume=tmpHQ.Sellv[0]+tmpHQ.Sellv[1]+tmpHQ.Sellv[2]+tmpHQ.Sellv[3]+tmpHQ.Sellv[4];
		break;
	case 16: 
		tmpPrice=tmpHQ_L2.Sellp[0];
		tmpVolume=tmpHQ.Sellv[0]+tmpHQ.Sellv[1]+tmpHQ.Sellv[2]+tmpHQ.Sellv[3]+tmpHQ.Sellv[4]
			+tmpHQ_L2.Sellv[0];
		break;
	case 17: 
		tmpPrice=tmpHQ_L2.Sellp[1];
		tmpVolume=tmpHQ.Sellv[0]+tmpHQ.Sellv[1]+tmpHQ.Sellv[2]+tmpHQ.Sellv[3]+tmpHQ.Sellv[4]
			+tmpHQ_L2.Sellv[0]+tmpHQ_L2.Sellv[1];
		break;
	case 18: 
		tmpPrice=tmpHQ_L2.Sellp[2];
		tmpVolume=tmpHQ.Sellv[0]+tmpHQ.Sellv[1]+tmpHQ.Sellv[2]+tmpHQ.Sellv[3]+tmpHQ.Sellv[4]
			+tmpHQ_L2.Sellv[0]+tmpHQ_L2.Sellv[1]+tmpHQ_L2.Sellv[2];
		break;
	case 19: 
		tmpPrice=tmpHQ_L2.Sellp[3];
		tmpVolume=tmpHQ.Sellv[0]+tmpHQ.Sellv[1]+tmpHQ.Sellv[2]+tmpHQ.Sellv[3]+tmpHQ.Sellv[4]
			+tmpHQ_L2.Sellv[0]+tmpHQ_L2.Sellv[1]+tmpHQ_L2.Sellv[2]+tmpHQ_L2.Sellv[3];
		break;
	case 20: 
		tmpPrice=tmpHQ_L2.Sellp[4];
		tmpVolume=tmpHQ.Sellv[0]+tmpHQ.Sellv[1]+tmpHQ.Sellv[2]+tmpHQ.Sellv[3]+tmpHQ.Sellv[4]
			+tmpHQ_L2.Sellv[0]+tmpHQ_L2.Sellv[1]+tmpHQ_L2.Sellv[2]+tmpHQ_L2.Sellv[3]+tmpHQ_L2.Sellv[4];
		break;
	}
	if(tmpPrice<COMPPREC)		tmpPrice=tmpHQ.Now;
	if(tmpPrice<COMPPREC)		tmpPrice=tmpHQ.Close;
	if(!g_ExternLevinCfg.bCanDbClickPrice || !g_ExternLevinCfg.bTradeAllWtl)	
		tmpVolume=0;
	tmpVolume=tmpVolume*tmpInfo->Unit;
	TC_DoLevinJy_(type,which,tmpInfo->setcode,tmpInfo->Code,tmpInfo->Name,FloatToStr2(tmpPrice,xsflag),IntToStr(tmpVolume),LF_NORMAIL);
	g_pMainWin->SetForegroundWindow();
}

void CAssisWnd::OnDoLevinBuyMessage(WPARAM wParam,LPARAM lParam)
{
	if(g_ChatArea.AreaType[m_whichGn]!=AREA_WT) return;
	BOOL bLevin=FALSE;
	if(lParam<0) 
	{
		bLevin=TRUE;
		lParam=-lParam;
	}
	MemStkInfo *tmpInfo=(*g_pStockIO)[lParam];
	if(bLevin && testzs(tmpInfo)) 
	{
		TDX_MessageBox(m_hWnd,"指数不能闪电买入",MB_OK|MB_ICONERROR);
		return;
	}
	DoLevin(wParam,tmpInfo,bLevin?0:2);
}

void CAssisWnd::OnDoLevinSellMessage(WPARAM wParam,LPARAM lParam)
{
	if(g_ChatArea.AreaType[m_whichGn]!=AREA_WT) return;
	BOOL bLevin=FALSE;
	if(lParam<0) 
	{
		bLevin=TRUE;
		lParam=-lParam;
	}
	MemStkInfo *tmpInfo=(*g_pStockIO)[lParam];
	if(bLevin && testzs(tmpInfo)) 
	{
		TDX_MessageBox(m_hWnd,"指数不能闪电卖出",MB_OK|MB_ICONERROR);
		return;
	}
	DoLevin(wParam,tmpInfo,bLevin?1:3);
}

void CAssisWnd::OnDoWTCancelMessage(WPARAM wParam,LPARAM lParam)
{
	if(g_ChatArea.AreaType[m_whichGn]!=AREA_WT) return;
	MemStkInfo *tmpInfo=(*g_pStockIO)[wParam];
	if(!tmpInfo) return;
	if(TC_DoLevinJy_) TC_DoLevinJy_(4,0,tmpInfo->setcode,tmpInfo->Code,"","","",LF_NORMAIL);
}

void CAssisWnd::OnDoQzSDSMessage(WPARAM wParam,LPARAM lParam)
{
	if(g_ChatArea.AreaType[m_whichGn]!=AREA_WT) return;
	if(TC_DoLevinJy_) TC_DoLevinJy_(6,0,-1,"","","","",LF_NORMAIL);
}

void CAssisWnd::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CWnd::OnShowWindow(bShow, nStatus);
	
	if(!bShow && (g_bEmbWTCreated && LayoutFlag) && m_whichGn>=0 && g_ChatArea.AreaType[m_whichGn]==AREA_WT)
	{
		if(TC_BeforeHideWT_) 
			TC_BeforeHideWT_();
	}
}
