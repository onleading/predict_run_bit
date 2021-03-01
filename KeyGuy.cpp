// KeyGuy.cpp : implementation file
//
#include "stdafx.h"
#include "TdxW.h"
#include "KeyGuy.h"
#include "struct.h"
#include "GridView.h"
#include "ComView.h"
#include "ComView2.h"
#include "TxtView.h"
#include "MainFrm.h"
#include "NewsView.h"
#include "BigMenuView.h"
#include "HomePageView.h"
#include "IEBrowserView.h"
#include "AdvHqview.h"
#include "JbfxView.h"
#include "Interface.h"
#include "MyInterface.h"
#include "JJView.h"
#include "TVView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CKeyGuy dialog

CKeyGuy::CKeyGuy(CWnd* pParent /*=NULL*/)
	: CDialog(CKeyGuy::IDD, pParent)
{
	//{{AFX_DATA_INIT(CKeyGuy)
	m_edit = _T("");
	//}}AFX_DATA_INIT
	m_pNewFont=NULL;
	bAlwaysOn=FALSE;
	CSize tmp=g_GUI.GetBitmapSize(IDB_KEYGUYDLG);
	m_wnd_rect.SetRect(0,0,tmp.cx,tmp.cy);
}

CKeyGuy::~CKeyGuy()
{
	if(m_pNewFont)
	{
		m_pNewFont->DeleteObject();
		delete m_pNewFont;
	}
}

void CKeyGuy::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CKeyGuy)
	DDX_Control(pDX, IDC_LIST, m_ListControl);
	DDX_Control(pDX, IDC_EDIT, m_EditControl);
	DDX_Text(pDX, IDC_EDIT, m_edit);
	DDV_MaxChars(pDX, m_edit, 8);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CKeyGuy, CDialog)
	//{{AFX_MSG_MAP(CKeyGuy)
	ON_WM_SHOWWINDOW()
	ON_EN_CHANGE(IDC_EDIT, OnChangeEdit)
	ON_LBN_DBLCLK(IDC_LIST, OnDblclkList)
	ON_WM_CREATE()
	ON_WM_NCHITTEST()
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDOWN()
	ON_WM_ACTIVATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CKeyGuy::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CRect rect;
	GetWindowRect(&rect);
	SetWindowPos(NULL,RightBottomPoint.x-rect.Width()-1,RightBottomPoint.y-rect.Height()-1,0,0,SWP_NOSIZE);
	UpdateData(TRUE);
	int len = m_edit.GetLength();
	m_EditControl.SetSel(len,len);
	OnChangeEdit();

	CDialog::OnShowWindow(bShow, nStatus);
}

void CKeyGuy::OnChangeEdit()
{
	int		i,j;
	BOOL	bPYFlag;
	char	tempstr[50];	//最多不能超过50个字符
	char	string[50];

	UpdateData(TRUE);
	m_ListControl.SetRedraw(FALSE);
 	m_ListControl.ResetContent();
	for(i = 0;i<MAXKGITEM;i++)
	{
		RowList[i].Type=-1;
		RowList[i].Content[0]='\0';
	}

	strcpy(tempstr,m_edit);
	AllTrim(tempstr);
	if(!bAlwaysOn && strlen(tempstr)<1)
	{
		PostMessage(WM_KEYDOWN,VK_ESCAPE);
		return;
	}
	nRowNum = GetMatchedStr(tempstr,bHasCmd,MAXKGITEM,lpnIndex);

	if(nRowNum > 0)
	{
		if (tempstr[0]>='A'&&tempstr[0]<='Z')  
			bPYFlag = TRUE;
		else	
			bPYFlag = FALSE;

		char *p,*p1;
		int tempInt;
		CString tempStr;
		TINDEXINFO *tmpIndexInfo;
		for(i = 0;i < nRowNum;i++)
		{
			j = lpnIndex[i];
			//////////////////////////////////////////////////////////////////////////
			//created by guo
			if(j>=KEYGUY_FUND)
			{
				j -= KEYGUY_FUND;
				if(!g_pFundBase || j>=g_nFundNum) continue;
				if(bPYFlag)
					tempStr.Format("%-9s %s",GetPYStr(g_pFundBase[j].szName,9),g_pFundBase[j].szName);
				else
					tempStr.Format("%-6s %s",g_pFundBase[j].szSymbol,g_pFundBase[j].szName);
				m_ListControl.AddString(tempStr);
				RowList[i].Type = 7;//基金平台对应的基金类型为7
				strcpy(RowList[i].Content,g_pFundBase[j].szSymbol);
				m_ListControl.SetItemData(i,7);
			}
			else if(j>=KEYGUY_DSCODE)
			{
				j -= KEYGUY_DSCODE; j += DS_FROM; 
				MemStkInfo	*pStkInfo = (*g_pStockIO)[j];
				if(!pStkInfo) continue;
				tempStr.Format("%-6s %s",pStkInfo->Code,pStkInfo->Name);
				
				m_ListControl.AddString(tempStr);
				RowList[i].Type = 20+pStkInfo->setcode;	//三方市场
				strncpy(RowList[i].Content, (LPCSTR)IntToStr((int)pStkInfo->setcode,2), 2);
				strcpy(RowList[i].Content+2,pStkInfo->Code);
				m_ListControl.SetItemData(i,5);
			}
			else if(j >= KEYGUY_SEPCZB)
			{
				j-=KEYGUY_SEPCZB;
				tmpIndexInfo = g_SpeclIndex+j;
				if(tmpIndexInfo->nDelStatus&HIDE_INDEX) 
					continue;
				tempStr.Format("%-7s %s",tmpIndexInfo->acCode,tmpIndexInfo->acName);
				m_ListControl.AddString(_F(tempStr));
				RowList[i].Type = 4;	//特殊指标
				strcpy(RowList[i].Content,tmpIndexInfo->acCode);
				m_ListControl.SetItemData(i,4);
			}
			else if(j >= KEYGUY_BLOCK)
			{
				j-=KEYGUY_BLOCK;
				if(j == 0)
					m_ListControl.AddString(_F("自选股"));
				else if(j == 1)
					m_ListControl.AddString(_F("股票池"));
				else if(j < DY_START+2)	//自定义板块
					m_ListControl.AddString(g_BlockInfo[j-2].Name+(CString)_F("[板块]"));
				else if(j < HY_START+2)	//地方板块
					m_ListControl.AddString(_F(g_cwAddrTopic[j-DY_START-2])+(CString)_F("[板块]"));
				else if(j < CON_START+2)//行业板块
					m_ListControl.AddString(_F(g_cwHYstr[j-HY_START-2])+(CString)_F("[板块]"));
				else if(j < ZHB_START+2)//概念板块
					m_ListControl.AddString(_F(g_ConceptBlock[j-CON_START-2].strBlockName)+(CString)_F("[板块]"));
				else if(j < ZJHHY_START+2)//组合板块
					m_ListControl.AddString(_F(g_ZHBBlock[j-ZHB_START-2].zhmc)+(CString)_F("[板块]"));
				else if(j < HKHY_START+2)//证监会行业
				{
					char *strHYName = g_pConstMng->GetHYFromCommandIDXH(j-ZJHHY_START-2);
					m_ListControl.AddString(_F(strHYName)+(CString)_F("[板块]"));
				}
				strcpy(RowList[i].Content,IntToStr(j-2));
				RowList[i].Type = 3;//板块
				m_ListControl.SetItemData(i,0);
			}
			else if(j >= KEYGUY_MAINLINE)
			{
				j-=KEYGUY_MAINLINE;
				tempStr.Format("%-7s %s",g_MainLineName[j].KeyString,g_MainLineName[j].InfoString);
				m_ListControl.AddString(_F(tempStr));
				RowList[i].Type = 5;	//主图类型
				strcpy(RowList[i].Content,g_MainLineName[j].KeyString); 
				m_ListControl.SetItemData(i,5);
			}
			else if(j >= KEYGUY_DSTYPE)
			{
				j-=KEYGUY_DSTYPE;
				tempStr.Format(".%-7d %s",600+j,DSGetMarketNameFromGuiSet(j));
				m_ListControl.AddString(_F(tempStr));
				RowList[i].Type = 6;	//三方行情类型
				strcpy(RowList[i].Content,IntToStr(j)); 
				m_ListControl.SetItemData(i,6);
			}
			else if(j >= KEYGUY_ZB)
			{
				j-=KEYGUY_ZB;
				tmpIndexInfo = g_pCalc->_CallDTopViewCalc_8(ZB_TYPE,j);
				if(!tmpIndexInfo || tmpIndexInfo->nDelStatus&HIDE_INDEX) 
				{	
					RowList[i].Type = -1;	//隐藏
					strcpy(RowList[i].Content,"");
					continue;
				}
				tempStr.Format("%-7s %s",tmpIndexInfo->acCode,tmpIndexInfo->acName);
				m_ListControl.AddString(_F(tempStr));
				RowList[i].Type = 1;	//指标
				strcpy(RowList[i].Content,tmpIndexInfo->acCode); 
				m_ListControl.SetItemData(i,1);
			}
			else if (j>=KEYGUY_OPER) 
			{   
				j-=KEYGUY_OPER;
				if(j>=g_nCmdTable)
				{
					CString str = (CString)g_vDynaCmdTab[j-g_nCmdTable].CmdInfo.Code + "  ";
					if(strcmp(g_DefCusPadName, g_vDynaCmdTab[j-g_nCmdTable].ChName)==0)
						str += (CString)g_vDynaCmdTab[j-g_nCmdTable].ChName+"(初始)";
					else
						str += (CString)g_vDynaCmdTab[j-g_nCmdTable].ChName;
					m_ListControl.AddString(_F(str));
					RowList[i].Type = 2;	//命令
					strcpy(RowList[i].Content,g_vDynaCmdTab[j-g_nCmdTable].CmdInfo.Code); 
				}
				else
				{
					p = g_pCmdTable[j].Code;
					p1 = p;
					if( (strlen(g_pCmdTable[j].Code) == 2&&g_pCmdTable[j].Code[0] == '7'&&g_pCmdTable[j].Code[1] != '0') 
						|| (g_pCmdTable[j].Code[0] == '.' && g_pCmdTable[j].Code[1] == '7' && strcmp(g_pCmdTable[j].Code,".7")) )	//如果代码为.7XX或7X，不能是70,.7则动态生成
					{
						if(g_pCmdTable[j].Code[0] == '7') p1 += 1;
						else p1 += 2;
						tempInt = atoi(p1);
						if(tempInt>0 && tempInt <= XXMNum)
						{
							tempStr = (CString)p +"   "+XXMTopic[tempInt-1];
							m_ListControl.AddString(_F(tempStr));
						}
						else
							m_ListControl.AddString(_F("空消息项"));
					}
					else
						m_ListControl.AddString(_F(g_pCmdTable[j].Name));
					RowList[i].Type = 2;	//命令
					strcpy(RowList[i].Content,g_pCmdTable[j].Code); 
				}
				m_ListControl.SetItemData(i,2);
			}	
			else if(j>=KEYGUY_OLDNAME)				//兼容旧名
			{
				TDXSTKOLDNAME *pAllFaceInfo=NULL;
				long  nFaceNum = g_pStockIO->GetAllFaceData(pAllFaceInfo);
				j-=KEYGUY_OLDNAME;
				if(j<nFaceNum&&pAllFaceInfo)
				{
					MemStkInfo *pInfo = g_pStockIO->Get_MemStkInfo(pAllFaceInfo[j].Code, pAllFaceInfo[j].SetCode);
					if(pInfo)
					{
						if (bPYFlag)
							sprintf( string,"%-7s %s(旧名)",GetPYStr(pAllFaceInfo[j].Name,5),pAllFaceInfo[j].Name);
						else
							sprintf( string,"%-7s %s(旧名)",pAllFaceInfo[j].Code,pAllFaceInfo[j].Name);
						
						RowList[i].Type = 0;	//股票
						if(pInfo->setcode == SZ)//多股票支持，将setcode放在Content中
							RowList[i].Content[0] = '0';
						else
							RowList[i].Content[0] = '1';
						strcpy(RowList[i].Content+1,pInfo->Code);

						m_ListControl.AddString(_F(string));
						m_ListControl.SetItemData(i,3);
					}
				}
			}
			else
			{
				if (bPYFlag)
					sprintf( string,"%-7s %s",(*g_pStockIO)[j]->SpellCode,(*g_pStockIO)[j]->Name);
				else
					sprintf( string,"%-7s %s",(*g_pStockIO)[j]->Code,(*g_pStockIO)[j]->Name);

				RowList[i].Type = 0;	//股票
				if((*g_pStockIO)[j]->setcode == SZ)	//多股票支持，将setcode放在Content中
					RowList[i].Content[0] = '0';
				else
					RowList[i].Content[0] = '1';
				strcpy(RowList[i].Content+1,(*g_pStockIO)[j]->Code);
				
				m_ListControl.AddString(string);
				m_ListControl.SetItemData(i,3);
			}
	   }
	   m_ListControl.SetCurSel(0);
	}
	m_ListControl.SetRedraw(TRUE);
}

BOOL CKeyGuy::OnInitDialog() 
{
	CDialog::OnInitDialog();

	m_pNewFont=new CFont;
	m_pNewFont->CreateFont(14,0,0,0,FW_MEDIUM,0,0,0,DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,DEFAULT_PITCH|FF_DONTCARE,SONG_FONT);
	m_EditControl.SetFont(m_pNewFont);
	m_ListControl.SetFont(m_pNewFont);
	SetWindowPos(&wndTop,0,0,0,0,SWP_NOSIZE|SWP_NOMOVE);

	/*
	CRect rc;
	GetClientRect(rc);

	m_edit_rect.SetRect(4,24,0,0);
	m_edit_rect.right=m_edit_rect.left+165;
	m_edit_rect.bottom=m_edit_rect.top+22;
	m_EditControl.MoveWindow(m_edit_rect);
	m_list_rect.SetRect(4,48,0,0);
	m_list_rect.right=m_list_rect.left+165;
	m_list_rect.bottom=m_list_rect.top+176;
	m_ListControl.MoveWindow(m_list_rect);
	m_sysbt_close_rect.SetRect(150,3,0,0);
	m_sysbt_close_rect.right=m_sysbt_close_rect.left+17;
	m_sysbt_close_rect.bottom=m_sysbt_close_rect.top+17;
	m_title_rect.SetRect(0,0,173,22);
	*/
	
	return TRUE;
}

BOOL CKeyGuy::PreTranslateMessage(MSG* pMsg) 
{
	SHORT CtrlDown;
	CtrlDown=::GetKeyState(VK_CONTROL);
	if(pMsg->message==WM_KEYDOWN)
	{
		if(	   pMsg->wParam==VK_PRIOR || pMsg->wParam==VK_NEXT
			|| pMsg->wParam==VK_UP    || pMsg->wParam==VK_DOWN
			||(pMsg->wParam==VK_HOME && CtrlDown<0)
			||(pMsg->wParam==VK_END  && CtrlDown<0)
			||(pMsg->wParam==VK_PRIOR && CtrlDown<0)
			||(pMsg->wParam==VK_NEXT && CtrlDown<0))
			pMsg->hwnd = m_ListControl.m_hWnd;
	}
	return CDialog::PreTranslateMessage(pMsg);
}

void CKeyGuy::OnOK() 
{
	CString tempstr;
	int tempint;
	
	tempint = m_ListControl.GetCurSel();

	if(tempint >= 0)
	{
		int nTemp = 0;
		while(nTemp<=tempint)
		{
			if(RowList[nTemp].Type==-1&&!strcmp(RowList[nTemp].Content, ""))
				tempint++;
			++nTemp;
		}
		ReturnType = RowList[tempint].Type;
		ReturnValue = RowList[tempint].Content;
	}
	else
		ReturnType = -1;	//没有返回值

	CDialog::OnOK();
}

void CKeyGuy::OnDblclkList() 
{
	OnOK();
}

//****************************************************//
//					消息处理和转发函数                //
// 如果是股票和指标类型，则自已处理，否则调用MainFrame//
// 中的OnCommCommand函数,先判断其WindowType和MessageNo//
//****************************************************//
BOOL CKeyGuy::OnCommonProcess(int Type,const char *Value)
{
	BOOL	DSFlag = FALSE;
	MemStkInfo	*pStkInfo=NULL;
	if(Type >= 20)	//Type大于20表示是第三方市场
	{
		DSFlag = TRUE;
		pStkInfo = g_pStockIO->GetDSStkInfo(Type-20,Value+2);
		if(!pStkInfo) return FALSE;
		Type = 0;
	}
	switch(Type)
	{
	case 0:	//如果是股票类型
		{
			CMainFrame *pMainFrame = (CMainFrame *)(AfxGetApp()->m_pMainWnd);
			if(pMainFrame && pMainFrame->m_pZHSortDlg)
				pMainFrame->m_pZHSortDlg->PostMessage(WM_COMMAND,IDCANCEL);
			//如果当前视图为基本资料。
			if(theViewManager.pActiveView->IsKindOf(RUNTIME_CLASS(CTxtView)))
			{
				TxtCurGPCode = Value+1;
				TxtCurSetCode = Value[0]-'0';
				theViewManager.pActiveView->SendMessage(WM_RICHEDIT_MESSAGE);
				break;
			}
			else if(theViewManager.pActiveView->IsKindOf(RUNTIME_CLASS(CGridView)))
			{
				//让行情图自行处理这个股票，如果处理不成功，则转到CComView
				if( ((CGridView *)theViewManager.pActiveView)->ProcessGPCode(Value+1,Value[0]-'0') )
					break;
			}

			else if(theViewManager.pActiveView->IsKindOf(RUNTIME_CLASS(CAdvHqView)))
			{
				//让高级行情功能自行处理这个股票，如果处理不成功，则转到CComView
				if( ((CAdvHqView *)theViewManager.pActiveView)->ProcessGPCode(Value+1,Value[0]-'0') )
					break;
			}			
			else if(theViewManager.pActiveView->IsKindOf(RUNTIME_CLASS(CTvView)))
			{
				if(Value[0]-'0'==SH)
				{
					//让高级行情功能自行处理这个股票，如果处理不成功，则转到CComView
					if( ((CTvView *)theViewManager.pActiveView)->ProcessGPCode(Value+1) )
						break;
				}
			}						
			else if(theViewManager.pActiveView->IsKindOf(RUNTIME_CLASS(CJbfxView)))
			{
				JbfxCurGPCode = Value+1;
				JbfxCurSetCode = Value[0]-'0';
				theViewManager.pActiveView->SendMessage(WM_CHANGEGP_MESSAGE);
				break;
			}
			else if(theViewManager.pActiveView->IsKindOf(RUNTIME_CLASS(CComView2)))	
			{
				//如果是ComView2，则看其是否支持外部股票切换
				CComView2 *pView = (CComView2 *)theViewManager.pActiveView;
				if(pView->Get_Extern_GPSwitch())
				{
					if(DSFlag == TRUE && pStkInfo)	//表示是第三方市场
						pView->ChangeSyncUnitGP(pStkInfo->Index);
					else
					{						
						char setcode = Value[0]-'0';
						char Code[100];
						strcpy(Code,Value);
						int iNo = g_pStockIO->GetIndex(Code+1,setcode);
						pView->ChangeSyncUnitGP(iNo);
					}
					break;
				}
			}
			const char *TmpCode = Value+1;
			short TmpSetCode = Value[0]-'0';
			if(DSFlag && pStkInfo)
			{
				TmpCode = pStkInfo->Code;
				TmpSetCode = pStkInfo->setcode;
			}
			//在组合图中显示(包括在多股同列中处理)
			if(theViewManager.pActiveView->IsKindOf(RUNTIME_CLASS(CComView)) && 
				((CComView*)theViewManager.pActiveView)->ChangeMultiGP(TmpCode,TmpSetCode))
			{
			}
			else 
				ShowGpInCom(TmpCode,TmpSetCode);
			break;
		}
	case 6:
		{
			g_pMainWin->PostMessage(WM_COMMAND,IDM_DS1+atol(Value),0);
			break;
		}
	case 5:
		{
			for(int i=0;i<g_MainLineNum;i++)
			{
				if(stricmp(g_MainLineName[i].KeyString,Value) == 0)
					break;
			}
			if(i<g_MainLineNum)
			{
				switch(i)
				{
				case 0:	g_nMainZbFlag = 0;	g_GH.bRealUPK = FALSE;	break;
				case 1:	g_nMainZbFlag = 0;	g_GH.bRealUPK = TRUE;	break;
				case 2:	g_nMainZbFlag = 1;	break;
				case 3:	g_nMainZbFlag = 2;	break;
				case 4:	g_nMainZbFlag = 3;	break;
				case 5:	g_nMainZbFlag = 4;	break;
				default:return FALSE;
				}
				if(theViewManager.pActiveView && theViewManager.pActiveView->IsKindOf(RUNTIME_CLASS(CComView2)))	
				{
					//如果是ComView2，则看其是否支持外部股票切换
					CComView2 *pView = (CComView2 *)theViewManager.pActiveView;
					if(pView->Get_Extern_GPSwitch())
					{
						pView->SendMessage(WM_PAINT);
						break;
					}
				}
				else OpenComView(1,NULL,0,i+KEYGUY_MAINLINE);
			}
			break;
		}
	case 1://指标
	case 4://特殊指标
		{
			int MessageNo,nTotalZb = g_pCalc->_CallDTopViewCalc_7(ZB_TYPE);
			TINDEXINFO *tmpIndexInfo;
			if(Type == 1)
			{
				for(int i = 0;i < nTotalZb;i++)
				{
					tmpIndexInfo = g_pCalc->_CallDTopViewCalc_8(ZB_TYPE,i);
					if(!tmpIndexInfo || tmpIndexInfo->nDelStatus&HIDE_INDEX) 
						continue;
					if(strcmp(tmpIndexInfo->acCode,Value) == 0)
					{
						MessageNo = i + KEYGUY_ZB;
						break;
					}
				}
				if(i >= nTotalZb)	break;
			}
			else	//特殊指标
			{
				for(int i = 0;i < g_nSpeclIndexNum;i++)
				{
					tmpIndexInfo = g_SpeclIndex+i;
					if(tmpIndexInfo->nDelStatus&HIDE_INDEX) 
						continue;
					if(strcmp(tmpIndexInfo->acCode,Value) == 0)
					{
						MessageNo = i + KEYGUY_SEPCZB;
						break;
					}
				}
				if(i >= g_nSpeclIndexNum)	break;
			}
			if(theViewManager.pActiveView->IsKindOf(RUNTIME_CLASS(CComView2)))	
			{
				//如果是ComView2，则看其是否支持外部股票切换
				CComView2 *pView = (CComView2 *)theViewManager.pActiveView;
				if(pView->Get_Extern_GPSwitch())
				{
					pView->SendMessage(WM_KEYGUY_MESSAGE,MessageNo);
					break;
				}
			}
			else OpenComView(1,NULL,0,MessageNo);
			break;
		}
	case 2://命令
		{
			int i = 0,WindowType,MessageNo;
			BOOL bFind = FALSE;
			while(strcmp(g_pCmdTable[i].Code, ""))
			{
				if(strcmp(g_pCmdTable[i].Code,Value)==0)
				{
					bFind = TRUE;
					break;
				}
				i++;
			}
			if(bFind)
			{
				WindowType = g_pCmdTable[i].wParam;
				MessageNo = g_pCmdTable[i].lParam;
				return ((CMainFrame *)(AfxGetApp()->m_pMainWnd))->OnCommCommand(WindowType,MessageNo);
			}
			else
			{
				for(int i=0;i<g_vDynaCmdTab.size();i++)
				{
					if(strcmp(g_vDynaCmdTab[i].CmdInfo.Code, Value)==0)
					{
						bFind = TRUE;
						break;
					}
				}
				if(bFind)
				{
					WindowType = g_vDynaCmdTab[i].CmdInfo.wParam;
					MessageNo = g_vDynaCmdTab[i].CmdInfo.lParam;
					return ((CMainFrame *)(AfxGetApp()->m_pMainWnd))->OnCommCommand(WindowType,MessageNo);
				}
			}
			break;
		}
	case 3://板块
		{
			int Which = atoi(Value);

			BOOL bToGrid=TRUE;
			if(theViewManager.pActiveView->IsKindOf(RUNTIME_CLASS(CComView2)))
			{
				CComView2 *tempWnd = (CComView2 *)theViewManager.pActiveView;
				if(tempWnd->HasNoFixedHQGrid())
				{
					tempWnd->SendMessage(WM_CHANGEBLOCK,Which);
					bToGrid=FALSE;
				}
			}
			if(bToGrid)
			{
				g_bOldGridDomain=FALSE;
				CGridView *tempWnd = (CGridView *)SwitchToView("Grid");
				if(tempWnd)
					tempWnd->SendMessage(WM_CHANGEBLOCK,Which);
				g_bOldGridDomain=TRUE;
			}

			break;
		}
	case 7://基金分析平台
		{
			if(theViewManager.pActiveView->IsKindOf(RUNTIME_CLASS(CJJView)))
				((CJJView*)theViewManager.pActiveView)->SelectSomeRow(Value);
			else
			{
				g_bOldGridDomain=FALSE;
				CJJView *tempWnd = (CJJView *)SwitchToView("Jj");
				if(tempWnd)
					tempWnd->SelectSomeRow(Value);
				g_bOldGridDomain=TRUE;
			}
			break;
		}
	default:
		break;
	}
	return FALSE;		
}

int CKeyGuy::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	SetWindowText("键盘精灵");
	SetWindowPos(&wndTopMost,0,0,m_wnd_rect.Width(),m_wnd_rect.Height(),SWP_NOMOVE);

//	g_GUI.GetBitmapRgn(m_wnd_rgn,IDB_KEYGUYDLG,RGB(0,255,0));
//	SetWindowRgn((HRGN)m_wnd_rgn,TRUE);
	return 0;
}

UINT CKeyGuy::OnNcHitTest(CPoint point) 
{
	int tmp =  CDialog::OnNcHitTest(point);
	ScreenToClient(&point);
	if(m_title_rect.PtInRect(point) && !(m_sysbt_close_rect.PtInRect(point)))
		return HTCAPTION;
	return tmp;
}

BOOL CKeyGuy::OnEraseBkgnd(CDC* pDC) 
{
//	CRect rc;
//	GetClientRect(rc);
//	g_GUI.DrawXPBmp(pDC,IDB_KEYGUYDLG,CPoint(0,0),CPoint(0,0));
//	g_d.Draw3DBar(pDC, rc, RGB(128,128,128));
//	g_d.DisplayText(pDC, rc, VipColor.KPCOLOR, DT_LEFT, "键盘精灵");
	return TRUE;
//	return CDialog::OnEraseBkgnd(pDC);
}

void CKeyGuy::OnLButtonDown(UINT nFlags, CPoint point) 
{
	CPoint ptCursor;
	GetCursorPosToWnd(ptCursor);

	if(m_sysbt_close_rect.PtInRect(ptCursor))
		PostMessage(WM_SYSCOMMAND,SC_CLOSE,0);
	CDialog::OnLButtonDown(nFlags, point);
}

void CKeyGuy::GetCursorPosToWnd(CPoint &pt)
{
	CPoint ptCursor;
	GetCursorPos(&ptCursor);
	CRect rcWnd;
	GetWindowRect(&rcWnd);
	ptCursor.Offset(-rcWnd.left,-rcWnd.top);
	pt=ptCursor;
}

void CKeyGuy::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized) 
{
	CDialog::OnActivate(nState, pWndOther, bMinimized);	
	if(nState==WA_INACTIVE)	
		PostMessage(WM_COMMAND,IDCANCEL);
}
