#include "stdafx.h"
#include "loaddll.h"
#include "UDialog.h"
#include "comview2.h"
#include "resource.h"
#include "InputName.h"

UDialog::UDialog(LPCOMINFO pComInfo,char *PadCode):UBase(pComInfo)
{
	m_bShowed = FALSE;
	m_bCreateSuccess = FALSE;
	m_bSmallSize = FALSE;
	m_GlobalUnitID = 0;
	m_nUnitType = pComInfo->nUnitType;
	strcpy(m_PadCode,PadCode);

	m_pModuleInfo = NULL;
	//创建出来
	if(!g_ExternGNCtrl.LoadModule(pComInfo->nUnitStyle, m_nSubModule, m_pModuleInfo))
		return;

	//创建窗口
	m_pVessel=NULL;
	m_pVessel=new CVesselWnd(this,m_nUnitType);
	m_pVessel->Create(_T("SysHeader32"),"",WS_CHILD|WS_VISIBLE,CRect(0,0,0,0),m_pView,IDD_VESSEL_DIALOG);
	if(!(::IsWindow(m_pVessel->m_hWnd))) 
		return;
	SkinWindowSet(m_pVessel->GetSafeHwnd(), 0);

	if(strcmp(PadCode, "tmp")==0) 
		ShowUnit("", "");
}

void UDialog::DeleteUnit()
{
//	if(!m_bCreateSuccess)  //如果没有创建成功,则不删除	
//		return;

	//删除
	m_pModuleInfo->ModuleExit(m_GlobalUnitID, m_nSubModule, (char*)(LPCSTR)m_strAppName, (char*)(LPCSTR)m_strGnIni);
	m_bCreateSuccess=FALSE;
}

UDialog::~UDialog()
{
	DeleteUnit();
	TDEL(m_pVessel);
}

void UDialog::ShowUnit(char *lpAppName, char *lpFileName)
{
	char errMsg[256]={0};
	m_GlobalUnitID=++l_GlobalUnitID;

	//配色
	SCHEME_COLOR tmpColor={0};
	SCHEME_FONT  tmpFont={0};
	SCHEME_MENU  tmpMenu={0};
	Set_Scheme(tmpColor, tmpFont, tmpMenu);
	m_pModuleInfo->ModuleScheme(tmpColor, tmpFont, tmpMenu);

	m_bCreateSuccess = m_pModuleInfo->ModuleShow(m_GlobalUnitID, m_nSubModule, m_pVessel->GetSafeHwnd(), lpAppName, lpFileName, errMsg);
}

void UDialog::Resize()
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
	m_pModuleInfo->ModuleResize(m_GlobalUnitID, m_nSubModule, Width(), Height());
}

DWORD    UDialog::GetGlobalID()
{
	return m_GlobalUnitID;
}

void	UDialog::HideSelf()
{
	m_pVessel->ShowWindow(SW_HIDE);
}


int  UDialog::ProcessMsg(UINT message, WPARAM wParam, LPARAM lParam)
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
		if(m_bCreateSuccess&&m_pModuleInfo&&m_pStkInfo) 
		{
			GP_Info m_GPInfo = {0};
			m_GPInfo.setcode = m_pStkInfo->setcode;
			strcpy(m_GPInfo.Code, m_pStkInfo->Code);
			m_pModuleInfo->ModuleDataTrans(m_GlobalUnitID, m_nSubModule, GP_OPDATA, sizeof(GP_Info), &m_GPInfo);
		}
		return 1;
	default:
		break;
	}
	return UBase::ProcessMsg(message,wParam,lParam);
}
