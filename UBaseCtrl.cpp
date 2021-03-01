#include "stdafx.h"
#include "tdxw.h"
#include "UBaseCtrl.h"

UBaseCtrl::UBaseCtrl(LPCOMINFO pComInfo, char *ModuleCode)
{
	m_pModule = g_GNPlug.GetPluginFromModuleCode(ModuleCode);
	memcpy(&m_ComInfo, pComInfo, sizeof(m_ComInfo));
	//定制版面和常规版面中,不能重复
	m_nWhichUnit = GetExternWhichUnit(m_ComInfo.nWhichUnit,m_ComInfo.nInCom2);
}

UBaseCtrl::~UBaseCtrl()
{
}

void	UBaseCtrl::ExitUnit()
{
	if(m_pModule)
		m_pModule->ExitModule(m_nWhichUnit);
}

BOOL	UBaseCtrl::ShowUnit(HWND hVessel)
{
	char errMsg[MAX_PATH]={0};
	if(m_pModule)
		return m_pModule->ShowModule(hVessel, errMsg, m_nWhichUnit);
	return FALSE;
}

void	UBaseCtrl::ResizeUnit(int cx, int cy)
{
	if(m_pModule)
		m_pModule->ArrangeWnd(cx, cy, m_nWhichUnit);
}

void	UBaseCtrl::GetDataAck(WPARAM wParam, LPARAM lParam)
{

}

void	UBaseCtrl::SetStock(MemStkInfo *pStock)
{
	if(m_pModule)
		m_pModule->SetCurrentGPCode(pStock->setcode,pStock->Code,m_nWhichUnit);
}

int		UBaseCtrl::ProcessMsg(UINT message, WPARAM wParam, LPARAM lParam)
{
	if(m_pModule)
		return m_pModule->ProcMsg(message, wParam, lParam, m_nWhichUnit);
	return 0;
}

int		UBaseCtrl::BroadcastUnitMsg(UINT message, WPARAM wParam, LPARAM lParam)
{
	if(!m_ComInfo.pWnd)	
		return 0;
	MSG ss = {0};
	ss.hwnd = m_ComInfo.pWnd->m_hWnd;
	ss.message = message;
	ss.wParam = wParam;
	ss.lParam = lParam;
	return m_ComInfo.pWnd->SendMessage(UM_UNIT_BROADCAST, (unsigned int)&ss, m_ComInfo.nWhichUnit);
}
