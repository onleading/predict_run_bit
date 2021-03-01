#include "stdafx.h"
#include "tdxw.h"
#include "comview2.h"
#include "vesselwnd.h"
#include "UBaseInner.h"
#include "UBaseCtrl.h"

#include "UF10Ctrl.h"
#include "URmReportCtrl.h"

UBaseInner::UBaseInner(LPCOMINFO pComInfo,char *ModuleCode):UBase(pComInfo)
{
	m_bCreateSuccess = FALSE;
	m_bSmallSize = FALSE;

	strcpy(m_ModuleCode,ModuleCode);
	m_pModule = g_GNPlug.GetPluginFromModuleCode(m_ModuleCode);

	//创建控制类
	m_pUBaseCtrl = NULL;
	switch(pComInfo->nUnitType)
	{
	case F10_UNIT:
		m_pUBaseCtrl = new UF10Ctrl(pComInfo);
		break;
	case RMREPORT_UNIT:
		m_pUBaseCtrl = new URmReportCtrl(pComInfo);
		break;
	default:
		m_pUBaseCtrl = new UBaseCtrl(pComInfo, m_ModuleCode);
		break;
	}

	//创建窗口
	m_pVessel=NULL;
	m_pVessel=new CVesselWnd(this);
	m_pVessel->Create(_T("SysHeader32"),"",WS_CHILD|WS_VISIBLE,CRect(0,0,0,0),m_pView,IDD_VESSEL_DIALOG);
	if(!(::IsWindow(m_pVessel->m_hWnd))) 
		return;
	SkinWindowSet(m_pVessel->GetSafeHwnd(), 0);
	//显示Unit
	ShowUnit();
}


UBaseInner::~UBaseInner()
{
	DeleteUnit();
	TDEL(m_pUBaseCtrl)
	TDEL(m_pVessel)
}

//////////////////////////////////////////////////////////////////////////
void UBaseInner::DeleteUnit()
{
	if(m_pUBaseCtrl)
		m_pUBaseCtrl->ExitUnit();
	m_bCreateSuccess=FALSE;
}

void UBaseInner::ShowUnit()
{
	//如果当前插件需要刷新所有行情的话
	if(m_pModule && m_pModule->iInfo.bFirstRefreshAllHQ)
		RefreshAllHq();	
	m_bCreateSuccess = m_pUBaseCtrl->ShowUnit(m_pVessel->GetSafeHwnd());
}

void UBaseInner::Resize()
{
	if(!(::IsWindow(m_pVessel->m_hWnd))) 
		return;
	m_bSmallSize = (Width()<120 || Height()<100);

	BOOL bInSplit = FALSE;
	if(m_nInCom2&&m_pView->IsKindOf(RUNTIME_CLASS(CComView2)))
	{
		CComView2 *pComView2 = (CComView2*)m_pView;
		bInSplit = pComView2->GetBeInSplit();
	}

	if(bInSplit || m_bSmallSize)
		m_pVessel->SetWindowPos(NULL,left,top,0,0,SWP_SHOWWINDOW);
	else
		m_pVessel->SetWindowPos(NULL,left,top,Width(),Height(),SWP_SHOWWINDOW);

	//改变UNIT的大小
	m_pUBaseCtrl->ResizeUnit(Width(), Height());
}

int  UBaseInner::ProcessMsg(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{
	case WM_PAINT:
		{
			BOOL bInSplit = FALSE;
			if(m_nInCom2&&m_pView->IsKindOf(RUNTIME_CLASS(CComView2)))
			{
				CComView2 *pComView2 = (CComView2*)m_pView;
				bInSplit = pComView2->GetBeInSplit();
			}
			if(!m_bCreateSuccess || bInSplit || m_bSmallSize)
			{
				CREATE_TMPDC
				g_d.Bar(&m_MyDC,&m_rcIn,BACK_BRUSH);
				m_MyDC.SetBkMode(TRANSPARENT);
				pDC->BitBlt(left,top,Width(),Height(),&m_MyDC,0,0,SRCCOPY); //从内存DC拷贝到屏幕DC
				DESTORY_TMPDC
				return 1;
			}
		}
		break;
	case WM_SIZE:
		UBase::ProcessMsg(message,wParam,lParam);
		Resize();
		return 1;
	case UM_SET_STOCK:
		UBase::ProcessMsg(message,wParam,lParam);
		if(m_bCreateSuccess && m_pUBaseCtrl && m_pStkInfo) 
			m_pUBaseCtrl->SetStock(m_pStkInfo);
		return 1;
	case UM_GETDATA_ACK:
		m_pUBaseCtrl->GetDataAck(wParam, lParam);
		return 1;
	default:
		m_pUBaseCtrl->ProcessMsg(message,wParam,lParam);
		break;
	}
	return UBase::ProcessMsg(message,wParam,lParam);
}
