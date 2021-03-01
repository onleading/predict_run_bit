////////////////////////////////////////////////////////////////
// Vckbase Online Journal -- Feb 2000
// Compiles with Visual C++ 6.0, runs on Windows 98 and probably NT too.
//
// ---
// AboutHtml shows how to implement an HTML About Dialog using a
// new class, CHtmlCtrl, that lets you use CHtmlView as a control in a dialog.


#include "StdAfx.h"
#include "HtmlCtrl.h"
#include "UWeb2.h"
#include "UBase.h"
#include "comview2.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNAMIC(CHtmlCtrl, CHtmlView)
BEGIN_MESSAGE_MAP(CHtmlCtrl, CHtmlView)
	ON_WM_DESTROY()
	ON_WM_ERASEBKGND()
//	ON_WM_MOUSEACTIVATE()
END_MESSAGE_MAP()
//////////////////
CHtmlCtrl::CHtmlCtrl()
{
	m_hStatic = ::CreateWindow("STATIC","",WS_VISIBLE|WS_CHILD|WS_BORDER,0,0,0,0,
		AfxGetMainWnd()->GetSafeHwnd(),NULL,AfxGetInstanceHandle(),NULL);
	m_pInnerExplorer = m_pOuterExplorer = NULL;
	m_bNeedTrans2Outer = FALSE;
}

CHtmlCtrl::~CHtmlCtrl()
{
	if(m_pInnerExplorer)
	{
		m_pInnerExplorer->DestroyWindow();
		TDEL(m_pInnerExplorer);
	}
}

// Create control in same position as an existing static control with
// the same ID (could be any kind of control, really)
//
BOOL CHtmlCtrl::CreateFromWnd(HWND hWnd, UINT nID, CWnd* pParent)
{
	CStatic wndStatic;
	if (!wndStatic.SubclassWindow(hWnd))
		return FALSE;

	// Get static control rect, convert to parent's client coords.
	CRect rc;
	wndStatic.GetWindowRect(&rc);
	pParent->ScreenToClient(&rc);
	wndStatic.DestroyWindow();

	// create HTML control (CHtmlView)
	return Create(NULL,						 // class name
		NULL,										 // title
		(WS_CHILD | WS_VISIBLE ),			 // style
		rc,										 // rectangle
		pParent,									 // parent
		nID,										 // control ID
		NULL);									 // frame/doc context not used
}

////////////////
// Override to avoid CView stuff that assumes a frame.
//
void CHtmlCtrl::OnDestroy()
{
	// This is probably unecessary since ~CHtmlView does it, but
	// safer to mimic CHtmlView::OnDestroy.
	if (m_pBrowserApp) 
	{
		m_pBrowserApp->Release();
		m_pBrowserApp = NULL;
	}
	CWnd::OnDestroy(); // bypass CView doc/frame stuff
}

////////////////
// Override to avoid CView stuff that assumes a frame.
/*
//
int CHtmlCtrl::OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT msg)
{
	// bypass CView doc/frame stuff
	return CWnd::OnMouseActivate(pDesktopWnd, nHitTest, msg);
}
*/

//////////////////
// Override navigation handler to pass to "app:" links to virtual handler.
// Cancels the navigation in the browser, since app: is a pseudo-protocol.
//
void CHtmlCtrl::OnBeforeNavigate2( LPCTSTR lpszURL,
	DWORD nFlags,
	LPCTSTR lpszTargetFrameName,
	CByteArray& baPostedData,
	LPCTSTR lpszHeaders,
	BOOL* pbCancel )
{
	//如果是inner的，什么都不做，传递参数而已
	if(m_pOuterExplorer&&m_bNeedTrans2Outer)
	{
		m_pOuterExplorer->Navigate2(lpszURL);
		m_bNeedTrans2Outer = FALSE;
		return;
	}

	//控制命令
	const char APP_PROTOCOL[] = "app:";
	int len = _tcslen(APP_PROTOCOL);
	if (_tcsnicmp(lpszURL, APP_PROTOCOL, len)==0) 
	{
		OnAppCmd(lpszURL + len);
		*pbCancel = TRUE;
	}

	/*
	//股票关联
	if(strstr(lpszURL, "nc.shtml")&&strstr(lpszURL, "realstock/company/"))
	{
		short nSetcode = 0;
		char Code[SH_CODE_LEN+1]={0};

		char *pStockEntry = strstr(lpszURL, "realstock/company/");
		if(strncmp(&pStockEntry[18],"sh",2)==0)
			nSetcode = 1;
		strncpy(Code, &pStockEntry[20], SH_CODE_LEN);
		long nIDIndex = g_pStockIO->GetIndex(Code, nSetcode);
		if(nIDIndex>=0)		//控制股票关联
		{
			if(theViewManager.pActiveView->IsKindOf(RUNTIME_CLASS(CComView2)))	
			{
				//如果是ComView2，则看其是否支持外部股票切换
				CComView2 *pView = (CComView2 *)theViewManager.pActiveView;
				if(pView->Get_Extern_GPSwitch())
					pView->ChangeSyncUnitGP(nIDIndex);
				else ToZst(Code, nSetcode);
			}
		}
	}
	*/
	//股票关联
	if(strstr(lpszURL, "JYG_web")&&strstr(lpszURL, "stockid="))
	{
		short nSetcode = 0;
		char Code[SH_CODE_LEN+1]={0};

		char *pStockEntry = strstr(lpszURL, "stockid=");
		strncpy(Code, &pStockEntry[8], SH_CODE_LEN);
		long nIDIndex = g_pStockIO->GetIndex(Code);
		if(nIDIndex>=0)		//控制股票关联
		{
			if(theViewManager.pActiveView->IsKindOf(RUNTIME_CLASS(CComView2)))	
			{
				//如果是ComView2，则看其是否支持外部股票切换
				CComView2 *pView = (CComView2 *)theViewManager.pActiveView;
				if(pView->Get_Extern_GPSwitch())
					pView->ChangeSyncUnitGP(nIDIndex);
				else ToZst(Code, nSetcode);
			}
		}
	}
}

void   CHtmlCtrl::OnNewWindow2(   LPDISPATCH*   ppDisp,   BOOL*   Cancel   )   
{
	if(!m_pInnerExplorer)
	{
		m_pInnerExplorer = new CHtmlCtrl;
		m_pInnerExplorer->CreateFromWnd(m_hStatic, ID_HTMLSTATIC, AfxGetMainWnd());
	}
	if(m_pInnerExplorer)
	{
		m_pInnerExplorer->SetRegisterAsBrowser(TRUE);
		m_pInnerExplorer->SetOuterExplorer(this);
		m_pInnerExplorer->m_bNeedTrans2Outer = TRUE;
		*ppDisp = m_pInnerExplorer->GetApplication();
	}
}

BOOL CHtmlCtrl::PreTranslateMessage(MSG* pMsg) 
{
	if ((pMsg->message == WM_RBUTTONDOWN)||(pMsg->message == WM_RBUTTONDBLCLK))
	{
		/*
		CPoint point(pMsg->pt);
		ScreenToClient(&point);
		
		IHTMLDocument2* pdoc2=NULL;
		IHTMLElement* pElement=NULL;
		IDispatch* pDisp=NULL;
		pDisp=GetHtmlDocument();
		pDisp->QueryInterface(IID_IHTMLDocument2,(void**)&pdoc2);
		pDisp->Release();
		
		pdoc2->elementFromPoint(point.x,point.y,&pElement);
		pdoc2->Release();
		if(pElement)
		{
			BSTR ID;
			pElement->get_id(&ID);
			pElement->Release();
			CString str=(LPCTSTR)(_bstr_t)ID;
			if(str=="Layer1")
			{
				CMenu menu;
				menu.LoadMenu(IDR_MENU1);
				CMenu* pmenu=menu.GetSubMenu(0);
				pmenu->TrackPopupMenu(0,pMsg->pt.x,pMsg->pt.y,this);
			}
		}
		*/
		return TRUE;//如果想完全屏蔽掉,不显示任何菜单,直接返回TRUE就行,上面这些代码演示了怎么对html中特定ID的元素弹出自己想要显示的菜单 
	}
	else
		return CHtmlView::PreTranslateMessage(pMsg);
}

//////////////////
// Called when the browser attempts to navigate to "app:foo"
// with "foo" as lpszWhere. Override to handle app commands.
//
void CHtmlCtrl::OnAppCmd(LPCTSTR lpszWhere)
{
	// default: do nothing
}

BOOL CHtmlCtrl::OnEraseBkgnd(CDC* pDC) 
{
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
//功能性函数
//////////////////////////////////////////////////////////////////////////
char *HTML_HEAD=
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
	<body><font face=\"%s\" style=\"font-size:%dpx;line-height:%dpx;\">\
";

void CHtmlCtrl::NavigateEmptyWebPage()
{
	CString sBase;
	sBase.Format(HTML_HEAD,g_WSXHStr+(CString)"tmptdx.css",g_strFontName[2],abs(g_nFontSize[2]),abs(g_nFontSize[2])+6);	
	CFile file;
	char tmpFile[100];
	sprintf(tmpFile,"%stmp.htm",g_WSXHStr);
	if(file.Open(tmpFile,CFile::modeCreate|CFile::modeWrite|CFile::shareDenyNone))
	{
		file.Write(sBase,sBase.GetLength());
		file.Close();
	}
	Navigate(tmpFile);
}

#define MAX_NEWSCONTENT_LEN 30000 

void CHtmlCtrl::WriteIEFromContent(int infoformat,char *pContent,long &ContentLen)
{
	CWnd *pFocus = CWnd::GetFocus();

	if(infoformat==1) //因为是链接，直接打开
	{
		pContent[min(120,ContentLen)] = 0;	//赋结束符,链接不能太长
		Navigate(pContent);
	}
	else
	{
		BOOL bStopReplace = FALSE;
		for(int i=0;i<ContentLen;i++)
		{
			if(pContent[i]=='>') 
				bStopReplace=FALSE;
			else if(pContent[i]=='<') 
				bStopReplace=TRUE;
			else if(!bStopReplace && pContent[i]==' ')
			{
				memmove(pContent+i+6,pContent+i+1,ContentLen-i-1);
				ContentLen+=5;
				memcpy(pContent+i,"&nbsp;",6);
				i+=5;
				if(ContentLen>MAX_NEWSCONTENT_LEN) 
					break;
			}
		}
		pContent[ContentLen] = 0;	//赋结束符

		CString pContentBase = pContent;
		if((pContent[0]=='\r' && pContent[1]=='\n') || 
		   (pContent[0]==' ' && pContent[1]=='\r' && pContent[2]=='\n')
		  )		//除掉开始的回车换行
			pContentBase = pContent+2;
		CString sRep = "\\\"";
		pContentBase.Replace(sRep,"\"");
		sRep = "\\\'";
		pContentBase.Replace(sRep,"\'");
		sRep = "\\\\";
		pContentBase.Replace(sRep,"\\");
		sRep = "\n"; //sRep = "\r\n";
		pContentBase.Replace(sRep,"<br>");
		sRep = "\r"; //sRep = "\r\n";
		pContentBase.Replace(sRep,"<br>");
		CString sBase;
		sBase.Format(HTML_HEAD,g_WSXHStr+(CString)"tmptdx.css",g_strFontName[2],abs(g_nFontSize[2]),abs(g_nFontSize[2])+6);	
		sBase += pContentBase;
		WriteIE(sBase);
	}
	if(pFocus) pFocus->SetFocus();
}

void CHtmlCtrl::WriteIE(const char *str)
{
	HRESULT hr;
	LPDISPATCH pDispatch = GetHtmlDocument();
	if( pDispatch != NULL )
	{
		IHTMLDocument2* pHtmlDoc=NULL;
		hr = pDispatch->QueryInterface( __uuidof( IHTMLDocument2 ),(void**)&pHtmlDoc );
		if(SUCCEEDED(hr))
		{
			HRESULT hresult = S_OK;
			VARIANT *param;
			SAFEARRAY *sfArray;
			USES_CONVERSION;
			BSTR bstr = SysAllocString(A2OLE(str));
			// Creates a new one-dimensional array
			sfArray = SafeArrayCreateVector(VT_VARIANT, 0, 1);			
			if (sfArray == NULL || pHtmlDoc == NULL)
				goto cleanup;			
			hresult = SafeArrayAccessData(sfArray,(LPVOID*) & param);
			param->vt = VT_BSTR;
			param->bstrVal = bstr;
			hresult = SafeArrayUnaccessData(sfArray);
			hresult = pHtmlDoc->write(sfArray);
			hresult = pHtmlDoc->close(); //应该先write,再close,否则焦点容易被IE抢走 ????
cleanup:
			SysFreeString(bstr);
			if (sfArray != NULL) 
				SafeArrayDestroy(sfArray);
			pHtmlDoc->Release();
		}
		pDispatch->Release();
    }
}