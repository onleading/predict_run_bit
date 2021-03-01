#include "stdafx.h"
#include "UGPWeb.h"

#define GPWEB_AUTOROLL_SECOND	5*60

UGPWeb::UGPWeb(LPCOMINFO pComInfo):UWeb(pComInfo)
{
	m_nMode				= GPWEB_STD;
	m_nViewthemType		= GPWEB_VIEWTHEM_RP;

	m_bRoll				= FALSE;
	m_Spec_Setcode		= 0;
	m_Spec_Code[0]		= 0;
	m_LastRollSeconds	= 0;
}

UGPWeb::~UGPWeb()
{
}

void UGPWeb::SetMode(int nMode,int nViewthemType)
{
	m_nMode = nMode;
	m_nViewthemType = nViewthemType;
}

void UGPWeb::SetRoll(BOOL bRoll,short setcode, char* code)
{
	m_bRoll	= bRoll;

	m_Spec_Setcode = setcode;
	strncpy(m_Spec_Code,code,SH_CODE_LEN);
	m_Spec_Code[SH_CODE_LEN]=0;
}

BOOL UGPWeb::NavigateIt()
{
	return StartNavigate();

	if(IsHideStatus()) 
		return FALSE;
	if(!m_pExplorer) 
		return FALSE;
	try
	{
		m_pExplorer->SetSilent(TRUE);
		CString tmpURL=m_URL;
		if(!m_bZs && m_pStkInfo)
			tmpURL.Replace("xxxxxx",m_pStkInfo->Code);
		else if(m_nViewthemType==GPWEB_VIEWTHEM_GW)
			tmpURL.Replace("xxxxxx","400039");
		else
			tmpURL.Replace("xxxxxx","400038");
		if(!m_bIsWebURL && !TestFileExist(tmpURL))
			return FALSE;
		m_pExplorer->Navigate(tmpURL,NULL,NULL,NULL,NULL);
	}
	catch(...)
	{
	}
	return TRUE;
}

BOOL UGPWeb::GPChangeNavigate()
{
	if(IsHideStatus()) 
		return FALSE;
	/*
	if(m_nMode==GPWEB_STD && OnLine)
	{
		SendDataReq();
		return TRUE;
	}
	*/
	return NavigateIt();
}

void UGPWeb::SendDataReq()
{
	if(g_pBkData && m_pStkInfo)
	{
		short lpnStkIndex[1];
		struct IW_Req IWReq;
		memset(&IWReq,0,sizeof(struct IW_Req));
		IWReq.search_type = 0;
		IWReq.from_order = -1;
		IWReq.wantnum = MAX_ONE_IWTITLE_NUM;
		IWReq.setcode = m_pStkInfo->setcode;
		if(m_bZs)
			strcpy(IWReq.code,SH_DPCODE);
		else
			strcpy(IWReq.code,m_pStkInfo->Code);		
		g_pBkData->Subscrible(m_pView->m_hWnd,UM_BK_INFODATA_OK,0,0,WIDEDATA_IWDATA_GP,lpnStkIndex,1,2,NULL,&IWReq);
	}
}

void UGPWeb::AfterdDataACK()
{
	if(m_nMode==GPWEB_STD)	
	{
		int nStyle = 0;
		if(!g_d.IsBlackColorScheme(VipColor.BACKCOLOR))
			nStyle = 1;
		//对于个股综合资讯中的大盘,进行type_id的置换处理
		for(int i=0;i<g_nSwapIWTitleNum[0];i++)
		{
			if(g_SwapIWTitle[0][i].type_id==16001)
				g_SwapIWTitle[0][i].type_id=10001;
			else if(g_SwapIWTitle[0][i].type_id==16004)
				g_SwapIWTitle[0][i].type_id=10003;
		}
	//	if(g_bUseTDXRun)
	//		TDX_MakeGNPage_(GetExternWhichUnit(m_nWhichUnit,m_nInCom2),0,g_nSwapIWTitleNum[0],g_SwapIWTitle[0],0,NULL,0,nStyle);
	}
	NavigateIt();	
}
