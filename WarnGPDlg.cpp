// WarnGPDlg.cpp : implementation file
//

#include "stdafx.h"
#include "tdxw.h"
#include "WarnGPDlg.h"
#include "inifile.h"
#include "ComView.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CWarnGPDlg dialog

IMPLEMENT_DYNCREATE(CWarnGPDlg, CDialog)

static int WarnDlg_Cx=0,WarnDlg_Cy=0;
static int TJWarnDlg_Cx=0,TJWarnDlg_Cy=0;
CWarnGPDlg::CWarnGPDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CWarnGPDlg::IDD, pParent)
{
	m_nWarnType = WARN_RADAR;
	m_bNeed	   = FALSE;

	m_pWarnBuffer = NULL;
	m_nWarnNum = 0;

	m_hIcon = AfxGetApp()->LoadIcon(IDI_HEADER);
}

void CWarnGPDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CWarnGPDlg)
	DDX_Control(pDX, IDC_WARNGP_LIST, m_WarnGPList);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CWarnGPDlg, CDialog)
	//{{AFX_MSG_MAP(CWarnGPDlg)
	ON_MESSAGE(UPDATE_WARNGP,OnUpdateWarnGPMessage)
	ON_BN_CLICKED(ID_WARNGP_ZST, OnWarngpVip)
	ON_NOTIFY(NM_DBLCLK, IDC_WARNGP_LIST, OnDblclkWarngpList)
	ON_BN_CLICKED(ID_WARNGP_CFG, OnWarngpCfg)
	ON_WM_TIMER()
	ON_BN_CLICKED(ID_SWITCH, OnSwitch)
	ON_WM_SIZE()
	ON_WM_GETMINMAXINFO()
	ON_BN_CLICKED(ID_TOBLOCK, OnToblock)
	ON_BN_CLICKED(ID_TOTXT, OnToTxt)
	ON_WM_KILLFOCUS()
	ON_BN_CLICKED(ID_EXPORT, OnExport)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CWarnGPDlg::FillWarnList()
{
	switch(m_nWarnType) 
	{
	case WARN_RADAR:
		m_pWarnBuffer = g_WarnBuffer;
		m_nWarnNum = g_nWarnNum;
		break;
	case WARN_TJ:
		m_pWarnBuffer = g_TJWarnBuffer;
		m_nWarnNum = g_nTJWarnNum;
		break;
	default:
		return;
	}
	m_WarnGPList.SetRedraw(FALSE);
	m_WarnGPList.DeleteAllItems ();
	m_WarnGPList.SetBkColor(VipColor.BACKCOLOR);
	m_WarnGPList.SetTextBkColor(VipColor.BACKCOLOR);
	m_WarnGPList.SetTextColor(VipColor.TXTCOLOR);
	CString tempStr;
	LV_ITEM lvItem;
	lvItem.mask = LVIF_TEXT|LVIF_IMAGE|LVIF_STATE;
	lvItem.state = 0;
	lvItem.stateMask = 0;
	lvItem.iSubItem = 0;
	struct MemStkInfo *scodetmp;
	int i,xsflag = 2;
	for(i = 0;i < m_nWarnNum;i++)
	{
		//对于连续大量作特别处理,每次大量都放入Buffer中,只不过不显示
		if(m_pWarnBuffer[i].nReason==HUGEVOL && !g_WarnParam.bTickVol) continue;
		scodetmp = (*g_pStockIO)[m_pWarnBuffer[i].nStockNo];
		if(!scodetmp) continue;

		lvItem.iItem = 0;
		if(m_nWarnType==WARN_TJ) 
		{
			if(m_pWarnBuffer[i].nStockType==2 || m_pWarnBuffer[i].nStockType==4)
				lvItem.iImage = 4;
			else if(m_pWarnBuffer[i].nStockType==3 || m_pWarnBuffer[i].nStockType==5)
				lvItem.iImage = 5;
			else
				lvItem.iImage = 3;
		}
		else
			lvItem.iImage=CWarnManager::GetColorFromReason(m_pWarnBuffer[i].nReason,m_pWarnBuffer[i].InOutFlag);
		lvItem.pszText = scodetmp->Name;
		m_WarnGPList.InsertItem (&lvItem);

		tempStr.Format("%02d:%02d",m_pWarnBuffer[i].time.wHour,m_pWarnBuffer[i].time.wMinute);
		m_WarnGPList.SetItemText (0,1,tempStr);
		xsflag = GetXSFlag(scodetmp);
		if(xsflag==2)
			tempStr.Format("%5.2f",m_pWarnBuffer[i].fPrice);
		else
			tempStr.Format("%5.3f",m_pWarnBuffer[i].fPrice);
		m_WarnGPList.SetItemText (0,2,tempStr);
		tempStr.Format("%-5ld",m_pWarnBuffer[i].nVol);
		m_WarnGPList.SetItemText (0,3,tempStr);
		if(m_pWarnBuffer[i].nReason>=TJWARNBASE)
		{
			tempStr = (CString)(LPCSTR)(*g_pCalc)[TJ_TYPE][m_pWarnBuffer[i].nReason-TJWARNBASE].acName;
			if(tempStr.GetLength()<2)
				tempStr = (CString)(LPCSTR)(*g_pCalc)[TJ_TYPE][m_pWarnBuffer[i].nReason-TJWARNBASE].acCode;
			TrimString(tempStr);
		}
		else 
			tempStr = CWarnManager::GetStrFromReason(m_pWarnBuffer[i].nReason);
		if(m_nWarnType==WARN_TJ||!BelongZXG(scodetmp)||g_WarnParam2.nWarnFlag==1)
			m_WarnGPList.SetItemText (0,4,_F(tempStr));
		else
			m_WarnGPList.SetItemText (0,4,_F(tempStr)+(CString)_F("-自"));
		m_WarnGPList.SetItemData(0,65536*m_pWarnBuffer[i].nStockType+m_pWarnBuffer[i].nStockNo); //附带数据为其Index
	}
	m_WarnGPList.SetRedraw(TRUE);
}

BOOL CWarnGPDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	SetIcon(m_hIcon,TRUE);
	SetIcon(m_hIcon,FALSE);

    //从ini读出相应的配置
	int x,y,nCx,nCy, nCxw, nCyw;
	CRect rect, rcw;
	GetClientRect(&rect);
	nCx=rect.Width();
	nCy=rect.Height();
	GetWindowRect(&rcw);
	nCxw=rcw.Width();
	nCyw=rcw.Height();

	switch(m_nWarnType) 
	{
	case WARN_RADAR:
		SetWindowText(_F("市场雷达列表"));
		break;
	case WARN_TJ:
		SetWindowText(_F("条件预警列表"));
		break;
	default:
		break;
	}

	CIniFile inif;
	inif.SetFileName(g_WSXHStr+g_strUserCfg);
	if(m_nWarnType==WARN_RADAR)
	{
		inif.SetAppName("WarnGPDlg");
		x=inif.GetInt("left",ScreenCx-nCx-8);
		y=inif.GetInt("top",ScreenCy-nCy-80);
		if(WarnDlg_Cx>0) nCxw=WarnDlg_Cx;
		if(WarnDlg_Cy>0) nCyw=WarnDlg_Cy;
	}
	else if(m_nWarnType==WARN_TJ)
	{
		inif.SetAppName("TJWarnGPDlg");
		x=inif.GetInt("left",ScreenCx-nCx-8);
		y=inif.GetInt("top",ScreenCy-2*nCy-80-25);
		if(TJWarnDlg_Cx>0) nCxw=TJWarnDlg_Cx;
		if(TJWarnDlg_Cy>0) nCyw=TJWarnDlg_Cy;
	}
	else
	{
	}
	if(x>-180 && x<ScreenCx-30 && y>-80 && y<ScreenCy-30)
		SetWindowPos(&wndTop,x,y,nCxw,nCyw,SWP_SHOWWINDOW);

	m_ImageList.Create(16,16,ILC_COLOR8|ILC_MASK,2,1);
	HICON hIcon = ::LoadIcon(AfxGetResourceHandle(),MAKEINTRESOURCE(IDI_REDWARN));
	m_ImageList.Add(hIcon);
	hIcon = ::LoadIcon(AfxGetResourceHandle(),MAKEINTRESOURCE(IDI_GREENWARN));
	m_ImageList.Add(hIcon);
	hIcon = ::LoadIcon(AfxGetResourceHandle(),MAKEINTRESOURCE(IDI_WARNCON));
	m_ImageList.Add(hIcon);	
	hIcon = ::LoadIcon(AfxGetResourceHandle(),MAKEINTRESOURCE(IDI_WARNTJCON));
	m_ImageList.Add(hIcon);	
	hIcon = ::LoadIcon(AfxGetResourceHandle(),MAKEINTRESOURCE(IDI_LEVINBUY));
	m_ImageList.Add(hIcon);	
	hIcon = ::LoadIcon(AfxGetResourceHandle(),MAKEINTRESOURCE(IDI_LEVINSELL));
	m_ImageList.Add(hIcon);	

	m_WarnGPList.SetImageList(&m_ImageList,LVSIL_SMALL);

	m_WarnGPList.SetExtendedStyle (LVS_EX_FULLROWSELECT);

	static char *WarnColStr[5] = {"股票名称","时","价","量","描述"};
	int ColWidth[5] = {70,42,54,42,78};
	static char *WarnColStr_Serv[4] = {"股票名称","时","描述","周期"};
	int ColWidth_Serv[4] = {70,42,126,50};	
	int totalwidth=70+42+134+42,i;
	CRect rc;
	m_WarnGPList.GetClientRect(&rc);
	LV_COLUMN lvColumn;
	lvColumn.mask = LVCF_WIDTH|LVCF_TEXT|LVCF_FMT|LVCF_SUBITEM;
	lvColumn.fmt = LVCFMT_LEFT;
	for(i = 0; i < 5;i++)
	{
		lvColumn.pszText = _F(WarnColStr[i]);
		lvColumn.cx = ColWidth[i]*(rc.Width()-20)/totalwidth;
		lvColumn.iSubItem = i;
		m_WarnGPList.InsertColumn (i,&lvColumn);
	}
	RefreshSwitch();
	FillWarnList();
	SetTimer(0x10,1000,NULL);
	
	return TRUE;
}

void CWarnGPDlg::OnUpdateWarnGPMessage(WPARAM wParam,LPARAM lParam)
{
	m_bNeed = TRUE;
	if( (g_WarnParam.bPopup && m_nWarnType==WARN_RADAR) || 
		(g_WarnParam2.bTJPopup && m_nWarnType==WARN_TJ)
	  )
	{
		CWnd *pWnd = GetActiveWindow();
		HWND hWnd=::GetForegroundWindow();
		DWORD tmpProcessID;
		GetWindowThreadProcessId(hWnd,&tmpProcessID);
		if(tmpProcessID!=GetCurrentProcessId())
		{
			if(g_pMainWin) 
			{
				for(int n=0;n<=6;n++)
				{
					g_pMainWin->FlashWindow(n%2);
					Sleep(100);
				}
			}
		}
	}
}

void CWarnGPDlg::OnWarngpVip() 
{
	POSITION pos = m_WarnGPList.GetFirstSelectedItemPosition ();
	if(pos)
	{
		int nItem = m_WarnGPList.GetNextSelectedItem (pos);
		DWORD tmpWord = m_WarnGPList.GetItemData(nItem);
		WORD nStockType = HIWORD(tmpWord);
		WORD GPIndex = LOWORD(tmpWord);
		if(GPIndex < 0) return;
		MemStkInfo *tmpInfo = (*g_pStockIO)[GPIndex];
		if(tmpInfo)
			ToZst(tmpInfo->Code,tmpInfo->setcode,FALSE);
		//
		short tempCurrentNo;
		CMainFrame *tmpMainWnd=(CMainFrame *)AfxGetMainWnd();
		if(nStockType==2 || nStockType==4)
		{
			tempCurrentNo=GPIndex;
			tmpMainWnd->m_wndRestspace.PostMessage(UM_DOLEVINBUY_MESSAGE,IDC_LEVINBUY11-IDC_LEVINBUY0,-tempCurrentNo);	
		}
		else if(nStockType==3 || nStockType==5)
		{
			tempCurrentNo=GPIndex;
			tmpMainWnd->m_wndRestspace.PostMessage(UM_DOLEVINSELL_MESSAGE,IDC_LEVINSELL1-IDC_LEVINSELL0,-tempCurrentNo);
		}
	}
}

void CWarnGPDlg::OnCancel() 
{
	if(!IsIconic())
	{
		CRect rc;
		GetWindowRect(rc);
		CIniFile inif;
		inif.SetFileName(g_WSXHStr+g_strUserCfg);
		int nCxw=rc.Width();
		int nCyw=rc.Height();

		if(m_nWarnType==WARN_RADAR)
		{
			WarnDlg_Cx=nCxw;
			WarnDlg_Cy=nCyw;
			inif.SetAppName("WarnGPDlg");
		}
		else if(m_nWarnType==WARN_TJ)
		{
			TJWarnDlg_Cx=nCxw;
			TJWarnDlg_Cy=nCyw;
			inif.SetAppName("TJWarnGPDlg");
		}
		else
		{
		}
		inif.PutInt(rc.left,"left");
		inif.PutInt(rc.top,"top");
	}
	//
	GetParent()->PostMessage (WM_CANCELPRESSED,(WPARAM)this,0);
}

void CWarnGPDlg::OnDblclkWarngpList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	OnWarngpVip();	
	*pResult = 0;
}

void CWarnGPDlg::OnWarngpCfg() 
{
	switch(m_nWarnType) 
	{
	case WARN_RADAR:
		AfxGetMainWnd()->SendMessage(WM_COMMAND,ID_WARNCFG);
		break;
	case WARN_TJ:
		AfxGetMainWnd()->SendMessage(WM_COMMAND,ID_TJWARNCFG);
		break;
	default:
		return;
	}
}

void CWarnGPDlg::OnOK() 
{
	GetParent()->PostMessage (WM_CANCELPRESSED,(WPARAM)this,0);
}

void CWarnGPDlg::OnTimer(UINT nIDEvent) 
{
	if(nIDEvent == 0x10)
	{
		if(m_bNeed)
		{
			FillWarnList();
			RefreshSwitch();
			m_bNeed = FALSE;
		}
	}
	CDialog::OnTimer(nIDEvent);
}

void CWarnGPDlg::RefreshSwitch()
{
	switch(m_nWarnType) 
	{
	case WARN_RADAR:
		GetDlgItem(ID_SWITCH)->SetWindowText(g_bAllWarnSwitch?_F("关闭雷达"):_F("启动雷达"));
		if(g_bAllWarnSwitch)
			SetWindowText(_F("市场雷达[运行中]"));
		else
			SetWindowText(_F("市场雷达[已关闭]"));
		break;
	case WARN_TJ:
		GetDlgItem(ID_SWITCH)->SetWindowText(g_bTjWarnSwitch?_F("关闭预警"):_F("启动预警"));
		if(g_bTjWarnSwitch)
			SetWindowText(_F("条件预警[运行中]"));
		else
			SetWindowText(_F("条件预警[已关闭]"));
		break;
	}
}

void CWarnGPDlg::OnSwitch() 
{
	switch(m_nWarnType) 
	{
	case WARN_RADAR:
		g_bAllWarnSwitch = !g_bAllWarnSwitch;
		break;
	case WARN_TJ:
		g_bTjWarnSwitch = !g_bTjWarnSwitch;
		if(g_bTjWarnSwitch && g_pAbsConditionNum<1)
		{
			TDX_MessageBox(m_hWnd,"你没有指定预警股票,不能启动条件预警!",MB_OK|MB_ICONINFORMATION);
			g_bTjWarnSwitch=FALSE;
		}
		break;
	default:
		break;
	}
	RefreshSwitch();
}

BOOL CWarnGPDlg::PreTranslateMessage(MSG* pMsg) 
{
	//将除ESC外的所有键盘输入都发给激活的视图
	if(pMsg->message==WM_KEYDOWN && theViewManager.pActiveView)
	{
		if(pMsg->wParam==96) //对于小键盘上的0作特殊处理
			pMsg->wParam=48;
		if((pMsg->wParam>=0x30&&pMsg->wParam<=0x39)||(pMsg->wParam>='a'&&pMsg->wParam<='z')
			||(pMsg->wParam>='A'&&pMsg->wParam<='Z')||pMsg->wParam== '.'||pMsg->wParam== '?')
			pMsg->hwnd = theViewManager.pActiveView->m_hWnd;
	}
	
	return CDialog::PreTranslateMessage(pMsg);
}

void CWarnGPDlg::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);
	
	/*
	CButton *pSetA = (CButton*)GetDlgItem(ID_WARNGP_ZST);
	CButton *pSetS = (CButton*)GetDlgItem(ID_WARNGP_CFG);
	CButton *pSetC = (CButton*)GetDlgItem(ID_SWITCH);
	CButton *pSetB = (CButton*)GetDlgItem(ID_EXPORT);
	CRect rectw;
	GetWindowRect(rectw);
	if(pSetS) pSetS->MoveWindow(rectw.Width()-56, rectw.Height()-2-42, 48, 21);
	if(pSetA) pSetA->MoveWindow(rectw.Width()-56-5-48, rectw.Height()-2-42, 48, 21);
	if(pSetB) 
	{
		pSetB->MoveWindow(rectw.Width()-56-5-48-5-56, rectw.Height()-2-42, 56, 21);
		pSetB->Invalidate();
	}
	if(pSetC) pSetC->MoveWindow(0, rectw.Height()-2-42, 56, 21);
	if(m_WarnGPList.m_hWnd) m_WarnGPList.MoveWindow(0, 0, rectw.Width()-8, rectw.Height()-2-42-2);
	*/
}

void CWarnGPDlg::OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI) 
{
	lpMMI->ptMinTrackSize.x = 320;
	lpMMI->ptMinTrackSize.y = 150;
	
	CDialog::OnGetMinMaxInfo(lpMMI);
}

#include "GetBlockDlg.h"
void CWarnGPDlg::OnToblock() 
{
	if(m_pWarnBuffer && m_nWarnNum>0)
	{
		CGetBlockDlg dlg;
		if(dlg.DoModal() == IDOK)
		{
			if(HasGPInBlockAndContinue(dlg.nSelBlock))
				CarryoutToBlock(m_pWarnBuffer, m_nWarnNum, dlg.nSelBlock);
		}
	}
}

void CWarnGPDlg::OnToTxt()
{
	CString tmpFilename;
	CFileDialog file(FALSE,"*.txt","",OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,_F("文本文件(*.txt)|*.txt|所有文件(*.*)|*.*||"));
	if(file.DoModal()==IDOK)
	{
		tmpFilename=file.GetPathName();
		BOOL bRet = ListCtrl2Txt(&m_WarnGPList,tmpFilename);
		if(bRet && TDX_MessageBox(m_hWnd,"导出成功,是否打开此文件?",MB_ICONQUESTION|MB_OKCANCEL)==IDOK)
		{
			CString tmpStr;
			tmpStr.Format("\"%s\"",tmpFilename);
			::ShellExecute(NULL,NULL,tmpStr,NULL,NULL,SW_SHOW);
		}		
	}
}

void CWarnGPDlg::CarryoutToBlock(WARN_STRU* pWarnBuffer, int nWarnNum, int WhichBlock)
{
	if(pWarnBuffer&&WhichBlock>-3)
	{
		short nGpNum = 0;
		short aWarnGpIndex[MAXGPNUM]={-1};
		for(int i=0;i<nWarnNum;i++)
		{
			BOOL bBlockGp = FALSE;
			for(int j=0;j<nGpNum;j++)
			{
				if(aWarnGpIndex[j]==pWarnBuffer[i].nStockNo)
				{
					bBlockGp = TRUE;
					break;
				}
			}
			if(!bBlockGp) aWarnGpIndex[nGpNum++] = pWarnBuffer[i].nStockNo;
		}
		if(nGpNum>0) MakeBlock(aWarnGpIndex,nGpNum,WhichBlock,TRUE);//生成板块并显示板块
	}
}

void CWarnGPDlg::OnKillFocus(CWnd* pNewWnd) 
{	
	CDialog::OnKillFocus(pNewWnd);
}

void CWarnGPDlg::OnExport() 
{
	CMenu menu;
	menu.LoadMenu(IDR_OTHERMENU);
	CMenu *pMenu=(CMenu *)menu.GetSubMenu(1);
	
	CRect rect;
	GetDlgItem(ID_EXPORT)->GetWindowRect(&rect);
	pMenu->TrackPopupMenu(TPM_LEFTALIGN,rect.left,rect.bottom+2,this);		
}
