#include "stdafx.h"
#include "UWeb.h"
#include "ComView.h"

UWeb::UWeb(LPCOMINFO pComInfo):UBase(pComInfo)
{
	m_bCreated = FALSE;
	m_pExplorer = NULL;
	m_URL = "";
	m_bIsWebURL = FALSE;
	m_nLastCurGP = -1;
	m_bGPRelation = FALSE;
	m_bSimpleWeb = TRUE;
}

UWeb::~UWeb()
{
	TDEL(m_pExplorer);
}

//是否是最小化状态，最小化状态不读取数据,不画
BOOL UWeb::IsHideStatus()
{
	return (m_rcIn.Width()<1 || m_rcIn.Height()<1);
}

void NavigateEmptyWebPage(CWebBrowser2 *pIE)
{
	//不是黑背景的话,就不需要空网页
	if(!g_d.IsBlackColorScheme(VipColor.BACKCOLOR)) 
		return;
	if(!pIE) return;
	try
	{
		if(TestFileExist(HomePath+"webs\\tdxwebbk.htm"))
		{
			pIE->SetSilent(TRUE);
			pIE->Navigate(HomePath+"webs\\tdxwebbk.htm",NULL,NULL,NULL,NULL);
		}
	}
	catch(...)
	{
	}
}

BOOL UWeb::GPChangeNavigate()
{
	return StartNavigate();
}

BOOL UWeb::StartNavigate()
{
	if(IsHideStatus()) 
		return FALSE;
	if(!m_pExplorer) 
		return FALSE;
	try
	{
		m_pExplorer->SetSilent(TRUE);
		CString tmpURL=m_URL;

		if(strstr(m_URL, "gppage"))
		{
			char IDCode[20]={0};
			sprintf(IDCode, "%d%s", m_pStkInfo->setcode, m_pStkInfo->Code);
			long nID = atol(IDCode);
		//	tmpURL.Format("%swebs\\gppage_%d.htm",g_WSXHStr,nID);
			tmpURL.Format("http://%s\/gppage\/gppage_%d.htm",g_strDNSUrl[g_nUseDNSUrl],nID);
		}

		if(m_bGPRelation)
			TransferToRealURL(tmpURL,m_pStkInfo);
		else
			TransferToRealURL(tmpURL,NULL);
		if(!m_bIsWebURL && !TestFileExist(tmpURL))
			return FALSE;
		m_pExplorer->Navigate(tmpURL,NULL,NULL,NULL,NULL);
	}
	catch(...)
	{
	}
	return TRUE;
}

void UWeb::SetURL(CString tmpURL,BOOL bAtOnceRun,BOOL bMustGPRelation)
{
	m_URL = tmpURL;
	m_bIsWebURL = (strnicmp(m_URL,"http",4)==0 || m_URL[1]!=':');
	m_nLastCurGP = -1;
	m_bGPRelation = FALSE;
	if(m_URL.Find("xxxxxx")>=0)
		m_bGPRelation=TRUE;
	if(bMustGPRelation)
		m_bGPRelation=TRUE;
	if(bAtOnceRun)
		StartNavigate();
}

void UWeb::SetSimpleWeb(BOOL bSimpleWeb)
{
	m_bSimpleWeb = bSimpleWeb;
}

CString UWeb::GetURL()
{
	return m_URL;
}

int  UWeb::ProcessMsg(UINT message, WPARAM wParam, LPARAM lParam)
{
	if(m_nCurGP==-1) //如果Unit为空，则另行处理
	{
		if(message==WM_PAINT)
		{
			CREATE_TMPDC
			g_d.Bar(&m_MyDC,&m_rcIn,BACK_BRUSH);

			pDC->BitBlt (left,top,Width(),Height(),&m_MyDC,0,0,SRCCOPY); //从内存DC拷贝到屏幕DC
			DESTORY_TMPDC
			return 1;
		}
		return UBase::ProcessMsg(message,wParam,lParam);
	}
	switch(message)
	{
	case WM_SIZE:
		UBase::ProcessMsg(WM_SIZE,wParam,lParam);
		if(!m_bCreated)
		{
			try
			{
				m_pExplorer = new CWebBrowser2;
				m_pExplorer->Create(NULL,NULL,WS_CHILD|WS_VISIBLE,CRect(0,0,0,0),m_pView,IDC_IEUNIT);
				//创建后,先赋成本地网页
				if(m_bIsWebURL)
					NavigateEmptyWebPage(m_pExplorer);
				//
				if(m_URL.GetLength()>0)
				{
					if(!StartNavigate() && !m_bIsWebURL)
						NavigateEmptyWebPage(m_pExplorer);
				}
			}
			catch (...) 
			{
				m_pExplorer=NULL;
			}
			if(m_nInCom2 && m_bSimpleWeb)
			{
				m_btnOpen.Create(NULL,WS_VISIBLE|WS_CHILD,CRect(0,0,0,0),m_pView,ID_OPENURL);
				m_btnOpen.SetTransparent(FALSE);
				m_btnOpen.SetIcon(IDI_FIND,IDI_FIND);
				m_btnOpen.DrawBorder(FALSE);
			}
			m_bCreated = TRUE;
		}
		if(m_pExplorer && m_pExplorer->m_hWnd && ::IsWindow(m_pExplorer->m_hWnd))
		{
			int nMargin=0;
			if(m_nInCom2)
			{
				nMargin=WEB_HEAD_HEIGHT;
				if(m_bSimpleWeb)
					m_btnOpen.MoveWindow(left,top,16,16);
			}
			m_pExplorer->SetWindowPos(NULL,left,top+nMargin,Width(),Height()-nMargin,SWP_SHOWWINDOW);
		}
		return 1;		
	case WM_PAINT:
		{
			if(wParam == 1)
			{				
				if(m_bGPRelation && m_nLastCurGP!=m_nCurGP)
				{
					//如果浏览成功的话
					if(GPChangeNavigate())
						m_nLastCurGP = m_nCurGP;
				}
			}
			CDC *pDC = m_pView->GetDC();
			CRect rc;
			rc.CopyRect(this);
			int nMargin=0;
			if(m_nInCom2)
				nMargin=WEB_HEAD_HEIGHT;
			rc.bottom=rc.top+nMargin;
			if(m_nInCom2 && m_bSimpleWeb)
				rc.left+=16;
			g_d.Bar(pDC,&rc,OVERLAP_BRUSH);
			m_pView->ReleaseDC(pDC);
		}
		return 1;
//	case UM_BK_INFODATA_OK:	//后台UWeb等数据回应处理
//		AfterdDataACK();
//		return 1;
	case WM_TIMER:
		OnTimer();
		return 1;
	default:
		//个股复合资讯回应处理
//		if(message==UM_GGCJZXTITLEOK)
//			AfterdDataACK();
		break;
	}
	return UBase::ProcessMsg(message,wParam,lParam);
}
