// TxtView.cpp : implementation file

#include "stdafx.h"
#include "TdxW.h"
#include "TxtView.h"
#include "NewsView.h"
#include "cwcl.h"
#include "ProgressWnd.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define AD_HEIGHT	66
#define MAX_TXT_SIZE	260*1000
/////////////////////////////////////////////////////////////////////////////
char    strGBBQComment[][13]={"除权除息","送配股上市","非流通股上市","国家股配售",
						  "股本变化","增发新股","股份回购","增发新股上市",
						  "转配股上市","可转债上市","流通股扩股","非流通股缩股","送认购权证","送认沽权证"};
char    strGBBQTopic[][10]  ={" 权息日"," 类别","送股","红利","配股","配股价","前流通盘","后流通盘","前总股本","后总股本"};

/////////////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNCREATE(CTxtView, CView)

CTxtView::CTxtView()
{
	theViewManager.AddView("基本资料", this);
	KeyGuyFlag = FALSE;
	m_bQuitting = FALSE;

	CurrentJBM = g_CurrentJBM;
	if(CurrentJBM < JBM_TDX)
		CurrentJBM = JBM_TDX;
	else if(CurrentJBM > g_TdxCfg.JbmNum-1)	
		CurrentJBM = g_TdxCfg.JbmNum-1;

	m_TxtCurGPCode = "";
	m_TxtCurSetCode = -1;

	m_nMainID = 0;
	bRemoteFileOpen = FALSE;
	m_bLayoutFlag = FALSE;
	m_bCanClickFL = FALSE;

	m_whichfl = 0;

	m_pFont = NULL;

	m_pTmpBuf = new char[MAX_TXT_SIZE];
	memset(m_pTmpBuf,0,MAX_TXT_SIZE*sizeof(char));
	m_pExplorer = NULL;	
	m_bGPRelation = FALSE;

	m_bSegmentTxt = g_bSupportETF; //采用片短TXT方式

	memset(&m_LSetInfoN, 0, sizeof(LinkedSetInfo));
	memset(&m_LSetInfoP, 0, sizeof(LinkedSetInfo));

	m_nPageGPNum = -1;
	for(int i=0;i<MAXGPNUM;i++)
		m_nPageGPIndex[i] = -1;
	m_nCurPageGP = -1;
	m_nMostLeft = 0;
	for(i=0;i<3;i++) 
		m_PreNowNext_Index[i] = -1;
	for(i=0;i<5;i++)
		m_PreNext_Rect[i].SetRect(0, 0, 0, 0);
	m_nRichEditTop=0;
	m_SelColor=VipColor.KPCOLOR;
	m_ForeColor=TreeColor.TreeForeColor;
	m_BackColor=TxtColor.TxtBackColor;

	m_bDS = FALSE;
}

CTxtView::~CTxtView()
{
	theViewManager.RemoveView(this);
	g_CurrentJBM = CurrentJBM;

	TDEL(m_pFont);
	TDEL(m_pTmpBuf);
	TDEL(m_pExplorer);
}

BEGIN_MESSAGE_MAP(CTxtView, CView)
	ON_COMMAND(ID_INBLOCK,OnInBlock)
	ON_MESSAGE(WM_RICHEDIT_MESSAGE,OnRichEditMessage)
	ON_MESSAGE(UM_GETDATA_ACK,OnGetDataAck)
	ON_COMMAND_RANGE(ID_RIGHT_ALLSELECT,ID_RIGHT_ADDNOTE,OnRightClick)
	ON_COMMAND_RANGE(ID_TDX_JBZL,ID_JBM_MENU8,OnSelectJBM)
	ON_UPDATE_COMMAND_UI(ID_RIGHT_COPY,OnUpdateCopy)
	ON_UPDATE_COMMAND_UI(ID_RIGHT_ADDNOTE,OnUpdateAddNote)
	ON_UPDATE_COMMAND_UI_RANGE(ID_TDX_JBZL,ID_JBM_MENU8,OnUpdateSelectJBM)
	ON_COMMAND_RANGE(IDC_BASEF10ID,IDC_BASEF10ID_END,OnClickFLButton)
	
	ON_COMMAND(ID_MARK_CANCEL, OnCancelMark)
	ON_COMMAND_RANGE(ID_MARK_ADD1, ID_MARK_ADD10, OnMarkStockFlag)
	ON_UPDATE_COMMAND_UI_RANGE(ID_MARK_ADD1, ID_MARK_ADD10, OnUpdateMarkStockFlag)
	//{{AFX_MSG_MAP(CTxtView)
	ON_WM_ERASEBKGND()
	ON_WM_RBUTTONUP()
	ON_COMMAND(ID_RIGHT_ADDBLOCK, OnRightAddblock)
	ON_COMMAND(ID_RIGHT_ADDZXG, OnRightAddZxg)
	ON_COMMAND(ID_DELFROMBLOCK,OnDelFromBlock)
	ON_COMMAND(ID_OTHERJBM, OnOtherjbm)
	ON_UPDATE_COMMAND_UI(ID_OTHERJBM, OnUpdateOtherjbm)
	ON_WM_DESTROY()
	ON_COMMAND(ID_TXTFIND, OnTxtfind)
	ON_COMMAND(ID_FINDRESULT, OnFindresult)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_LBUTTONDOWN()
	ON_NOTIFY( EN_LINK, TXT_RICHEDIT, OnRichEditExLink )
	//}}AFX_MSG_MAP
	// Standard printing commands
END_MESSAGE_MAP()

BEGIN_EVENTSINK_MAP(CTxtView, CView)
    //{{AFX_EVENTSINK_MAP(CTxtView)
	ON_EVENT(CTxtView, IDC_IEUNIT, 259 /* DocumentComplete  */, DocumentComplete,VTS_DISPATCH VTS_PVARIANT)
	//}}AFX_EVENTSINK_MAP
END_EVENTSINK_MAP()
/////////////////////////////////////////////////////////////////////////////
// CTxtView drawing

void CTxtView::DocumentComplete(LPDISPATCH /* pDisp */, VARIANT* URL) 
{
	if(::IsWindow(m_RichEdit.m_hWnd))
		m_RichEdit.SetFocus();
}

void CTxtView::SetSelColor()
{
	m_SelColor=RGB(255,255,0);
//	if(CurrentJBM>0)
//		m_SelColor=VipColor.VOLCOLOR;
//	else if(CurrentJBM>1)
//		m_SelColor=RGB(192,0,192);
 	for(int i=0;i<MAX_F10CAPTION;i++)
		m_btnF10[i].SetSelColor(m_SelColor);
}

int CTxtView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	m_RichEdit.Create(WS_VISIBLE|WS_CHILD|ES_AUTOVSCROLL|ES_READONLY|WS_VSCROLL|WS_TABSTOP|ES_MULTILINE,CRect(0,0,0,0),this,TXT_RICHEDIT);
	m_RichEdit.SetMenuIndex(3);
	if(g_OEM.bEmbedWTVersion)
	{
		m_ForeColor=VipColor.TXTCOLOR;
		m_BackColor=VipColor.BACKCOLOR;
		m_RichEdit.m_bUseTxtColor=FALSE;
	}
	//使用从INI中读入的大小	
	m_RichEdit.ApplyTxtSize();
	m_RichEdit.GetDefaultCharFormat(cfm);
	m_pFont = new CFont;
	m_pFont->CreateFont(g_nFontSize[2],0,0,0,g_nFontWeigth[2],0,0,0,(g_b2000XPSys&&stricmp(g_strFontName[2],"System")==0)?ANSI_CHARSET:DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,g_b2000XPSys?ANTIALIASED_QUALITY:DEFAULT_QUALITY,DEFAULT_PITCH|FF_DONTCARE,g_strFontName[2]);

	//创建动态F10分类按钮
 	for(int i=0;i<MAX_F10CAPTION;i++)
		m_btnF10[i].Create("",WS_CHILD|BS_OWNERDRAW,CRect(0,0,0,0),this,IDC_BASEF10ID+i);
 	for(i=0;i<MAX_F10CAPTION;i++)
	{
		SkinWindowSet(m_btnF10[i].GetSafeHwnd() ,0) ; //SKIN_CLASS_NOSKIN
		m_btnF10[i].SetTextColor(VipColor.TXTCOLOR);
		m_btnF10[i].SetBkColor(VipColor.BACKCOLOR);
		m_btnF10[i].SetHoverColor(VipColor.BACKCOLOR);
		m_btnF10[i].SetFont(m_pFont);
	}
	SetSelColor();
	m_bLayoutFlag = TRUE;

	if(g_URLTitle[0]==0 && g_URL[0])
	{
		m_pExplorer = new CWebBrowser2;
		if(!m_pExplorer || !m_pExplorer->Create(NULL,NULL,WS_CHILD|WS_VISIBLE|WS_BORDER,CRect(0,0,0,0),this,IDC_IEUNIT))
			TDEL(m_pExplorer);
		g_URL[80]=0;
		if(strstr(g_URL,"xxxxxx"))
			m_bGPRelation=TRUE;
		if(!m_bGPRelation && m_pExplorer && m_pExplorer->m_hWnd && ::IsWindow(m_pExplorer->m_hWnd))
		{
			m_pExplorer->SetSilent(TRUE);
			m_pExplorer->Navigate(g_URL,NULL,NULL,NULL,NULL);
		}
	}
	return 0;
}

//响应"股票资料"的消息
void CTxtView::OnRichEditMessage(WPARAM wParam, LPARAM lParam)
{
	m_bSegmentTxt = g_bSupportETF; //采用片短TXT方式
	//重新得到CurrentJBM的值
	if(CurrentJBM!=g_CurrentJBM)
	{
		CurrentJBM = g_CurrentJBM;
		Resize();
	}	
	if(CurrentJBM < JBM_TDX)
		CurrentJBM = JBM_TDX;
	else if(CurrentJBM > g_TdxCfg.JbmNum-1)	
		CurrentJBM = g_TdxCfg.JbmNum-1;
	SetSelColor();
	UncheckAllButton();
	m_RichEdit.SetWindowText("");
	m_TxtCurGPCode = TxtCurGPCode;
	m_TxtCurSetCode = TxtCurSetCode;
	if(m_TxtCurGPCode != "")
	{
		MemStkInfo *tmpStkInfo = g_pStockIO->Get_MemStkInfo(m_TxtCurGPCode,m_TxtCurSetCode);
		if(!tmpStkInfo) return;
		m_TxtCurSetCode = tmpStkInfo->setcode;
		MainIDPlus(m_nMainID,TXTVIEW_WIN);	//即使是本地，也应将MainID变一下，否则ACK会出问题
		
		if(m_bGPRelation && m_pExplorer && m_pExplorer->m_hWnd && ::IsWindow(m_pExplorer->m_hWnd))
		{
			CString tmpURL=g_URL;
			TransferToRealURL(tmpURL,tmpStkInfo);
			m_pExplorer->SetSilent(TRUE);
			m_pExplorer->Navigate(tmpURL,NULL,NULL,NULL,NULL);
		}
		if(CurrentJBM == JBM_TDX)			//得到通达信资讯
		{
			GetTdxJBZL(m_TxtCurGPCode,m_TxtCurSetCode);
			if(g_bDebugMode)
				m_RichEdit.LineScroll(25);
		}
		else if(CurrentJBM >= 0)	//远程基本资料
			GetRemoteFile(m_TxtCurGPCode,m_TxtCurSetCode);
		MemStkInfo *tempMemStkInfo=g_pStockIO->Get_MemStkInfo(m_TxtCurGPCode,m_TxtCurSetCode);
		if(tempMemStkInfo)
		{
		//	GetParent()->SetWindowText ((CString)_F("基本资料-")+tempMemStkInfo->Name);
		//	AfxGetMainWnd()->SetWindowText ((CString)_F("基本资料-")+tempMemStkInfo->Name);
		//	theViewManager.SetViewName(tempMemStkInfo->Name,this);
			AfxGetMainWnd()->SetWindowText(g_strTitleName);
		}
	}
	else
	{
		m_RichEdit.SetWindowText(_F("请用键盘精灵选择您要查看资料的股票.按TAB键或点右键切换其它类型的基本资料."));
	//	GetParent()->SetWindowText (_F("基本资料"));
	//	AfxGetMainWnd()->SetWindowText (_F("基本资料"));
	//	theViewManager.SetViewName("",this);
		AfxGetMainWnd()->SetWindowText(g_strTitleName);
	}
	m_RichEdit.SetFocus();

	Get3Tomo(m_PreNowNext_Index);
}

void CTxtView::OnDestroy() 
{
	m_bQuitting = TRUE;
	CView::OnDestroy();	
}

void CTxtView::OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView)
{
	theViewManager.OnActivateView(bActivate, this);
	g_bSepEsc = FALSE;
	CView::OnActivateView(bActivate, pActivateView, pDeactiveView);
	if(bActivate) m_RichEdit.SetFocus();//一定要bActive为真，才可SetFocus;
	if(!m_bQuitting && bActivate)
	{
		AfxGetMainWnd()->SendMessage(UM_HIDESHOWBUTTON,HideShowBtn_Jbm);
	}
}

void CTxtView::OnInitialUpdate() 
{
	CView::OnInitialUpdate();
	Modify_CorpWebMenu();
		
	//默认标题
//	GetParent()->SetWindowText (_F("基本资料"));
}

/////////////////////////////////////////////////
void CTxtView::UpdateButton()
{
	for(int i=0;i<MAX_F10CAPTION;i++)
		m_btnF10[i].ShowWindow(SW_HIDE);
 	for(i=0;i<min(g_F10CaptionNum,MAX_F10CAPTION);i++)
	{
	   if(m_bSegmentTxt&&g_bHasF10Cfg)
		   m_btnF10[i].SetWindowText(g_NewF10Caption[i].sTitle);
	   else
			m_btnF10[i].SetWindowText(g_F10Caption[i].title);
	   m_btnF10[i].ShowWindow(SW_SHOW);
	}
}

void CTxtView::UncheckAllButton()
{
	int i;
	for(i=0;i<MAX_F10CAPTION;i++) //先全部取消选中
	{
		if(m_btnF10[i].IsBtnChecked())
		   m_btnF10[i].CheckButton(FALSE);
	}
}

int CTxtView::GetCurSelBtn()
{
	int count=min(g_F10CaptionNum,MAX_F10CAPTION);
	for(int i=0;i<count;i++)
	{
		if(m_btnF10[i].IsBtnChecked())
			return i;
	}
	return -1;
}

#define  BTN_WIDTH	73
#define  BTN_HEIGHT 25

void CTxtView::JustifyButton(CRect rcdlg)
{
	int nBtnWidth=BTN_WIDTH;
	if(g_F10CaptionNum>16) //支持超过16个标题
		nBtnWidth=62;
	//安排安钮的位置，最多两排 ,每个按钮高BTN_HEIGHT，宽nBtnWidth 先计算第一排可以放几个按钮
	int k=(rcdlg.Width()-140)/(nBtnWidth),left=0;	
	if(k<0) k=0;
	m_nRichEditTop=2*BTN_HEIGHT+6;
	m_RichEdit.SetWindowPos(NULL,16,m_nRichEditTop,rcdlg.Width()+1-16,rcdlg.Height()-m_nRichEditTop,SWP_NOZORDER);
	if(g_F10CaptionNum < k)
	{
		int top = 1,left = (rcdlg.Width()-g_F10CaptionNum*nBtnWidth)/2+8;
		for(int i=0;i<g_F10CaptionNum;i++)
		{
			m_btnF10[i].SetWindowPos(NULL,left,top,nBtnWidth,BTN_HEIGHT,SWP_SHOWWINDOW);
 			left+=(nBtnWidth-1);
		}		
	}
	else
	{
		int visiblenum = min(g_F10CaptionNum,2*k);
		int singlenum = (visiblenum+1)/2;
		int top = 1;
		left = (rcdlg.Width()-singlenum*nBtnWidth)/2+8;
		for(int i=0;i<singlenum;i++)
		{
			m_btnF10[i].SetWindowPos(NULL,left,top,nBtnWidth,BTN_HEIGHT,SWP_SHOWWINDOW);
 			left+=(nBtnWidth-1);
		}
		top+=(BTN_HEIGHT-1);
		left = (rcdlg.Width()-singlenum*nBtnWidth)/2+8;	
		for(i=singlenum;i<visiblenum;i++)
		{
			m_btnF10[i].SetWindowPos(NULL,left,top,nBtnWidth,BTN_HEIGHT,SWP_SHOWWINDOW);
 			left+=(nBtnWidth-1);
		}
		for(;i<g_F10CaptionNum;i++)
			m_btnF10[i].SetWindowPos(NULL,0,0,0,0,SWP_SHOWWINDOW);
		m_nMostLeft = left;
	}
	UpdateButton();
}

void CTxtView::OnClickFLButton(UINT nID)
{
	if(!m_bCanClickFL || g_F10CaptionNum<1) return;

	m_whichfl = nID-IDC_BASEF10ID;
	if(m_whichfl<0 || m_whichfl>=g_F10CaptionNum) 
		m_whichfl=0;
	UncheckAllButton();
	m_btnF10[m_whichfl].CheckButton(TRUE);
	if(m_bSegmentTxt&&g_bHasF10Cfg)
	{
		g_nMainID = MainIDPlus(m_nMainID,TXTVIEW_WIN);
		g_nAssisID = NEW_JBMTXT_ASSISID;
		//抛砖引玉
		if(m_bDS)
			g_pGetData->DSGetNewJbmText(m_TxtCurGPCode,m_TxtCurSetCode,m_whichfl,g_NewF10Caption[m_whichfl].sFilePath,g_NewF10Caption[m_whichfl].nOffset,g_NewF10Caption[m_whichfl].nLength,CurrentJBM);		
		else
			g_pGetData->GetNewJbmText(m_TxtCurGPCode,m_TxtCurSetCode,m_whichfl,g_NewF10Caption[m_whichfl].sFilePath,g_NewF10Caption[m_whichfl].nOffset,g_NewF10Caption[m_whichfl].nLength,CurrentJBM);		
	}
	else
		ProcessF10Content(m_whichfl);
	m_RichEdit.SetFocus();
}

void CTxtView::ProcessF10Content(int which)
{
	long nCurLine = m_RichEdit.GetFirstVisibleLine();
	if(nCurLine < 0 || g_F10Caption[which].pos < 0) 
		return;
	long nWhichLine = m_RichEdit.LineFromChar(g_F10Caption[which].pos);
	if(nWhichLine < 0) 
		return;
	m_RichEdit.LineScroll(nWhichLine-nCurLine);
}

/////////////////////////////////////////////////
BOOL CTxtView::PreTranslateMessage(MSG* pMsg) 
{
	if (pMsg->message == WM_CHAR && KeyGuyFlag == FALSE )
	{
		if(ProcessKeyGuy(pMsg,this,KeyGuyFlag))
			return TRUE;
		return FALSE;
		//支持快捷键
		if(CurrentJBM != JBM_TDX && g_bHasF10Cfg && g_F10CaptionNum>0)
		{
			if(pMsg->wParam == VK_SPACE || pMsg->wParam == VK_BACK)
			{
				if(pMsg->wParam == VK_SPACE)
					m_whichfl=(m_whichfl+1)%g_F10CaptionNum;
				else
				{
					m_whichfl=m_whichfl-1;
					if(m_whichfl<0) m_whichfl=g_F10CaptionNum-1;
				}
  				OnClickFLButton(m_whichfl+IDC_BASEF10ID);
				return TRUE;					
			}
		}
		if(pMsg->wParam == VK_TAB) //支持正反两种顺序
		{
			if(FiltTabKeyCom()) return TRUE;
			SHORT CtrlDown=::GetKeyState(VK_SHIFT);
			if(CtrlDown < 0) //逆序
			{
				if(CurrentJBM == JBM_TDX)	 CurrentJBM = g_TdxCfg.JbmNum - 1;
				else  CurrentJBM = CurrentJBM - 1;
			}
			else	//正序
			{
				if(CurrentJBM == g_TdxCfg.JbmNum-1) 
					CurrentJBM = JBM_TDX;
				else
					CurrentJBM = CurrentJBM + 1;
			}
			g_CurrentJBM = CurrentJBM;
			Resize();
			SendMessage(WM_RICHEDIT_MESSAGE);
			return TRUE;
		}
	}
	//Ctrl+A时不处理控件的默认处理
	if(pMsg->message == WM_KEYDOWN && (pMsg->wParam == 'a' || pMsg->wParam == 'A' ))
	{
		short sRet=::GetKeyState(VK_CONTROL);
		if(sRet < 0)
		{
			OnRightClick(ID_RIGHT_ALLSELECT);
			return TRUE;
		}
	}
	if(pMsg->message==WM_KEYDOWN && (pMsg->wParam=='z'||pMsg->wParam=='Z'))
	{
		SHORT CtrlDown=::GetKeyState(VK_CONTROL);
		if(CtrlDown<0)
		{
			if(::GetKeyState(VK_SHIFT)<0)
				PostMessage(WM_COMMAND,ID_DELFROMBLOCK);
			else
				PostMessage(WM_COMMAND,ID_RIGHT_ADDBLOCK);
			return TRUE;
		}
	}
	if(pMsg->message == WM_KEYDOWN && (pMsg->wParam == VK_NEXT || pMsg->wParam == VK_PRIOR ))
	{
		short sRet=::GetKeyState(VK_CONTROL);
		if(sRet < 0)
		{
			if(m_bLayoutFlag && OnLine && g_bHasF10Cfg && CurrentJBM >= 0)
			{
				if(pMsg->wParam == VK_PRIOR)
				{
					TxtCurGPCode = (*g_pStockIO)[m_PreNowNext_Index[0]]->Code;
					TxtCurSetCode = (*g_pStockIO)[m_PreNowNext_Index[0]]->setcode;
					SendMessage(WM_RICHEDIT_MESSAGE);
				}
				else
				{
					TxtCurGPCode = (*g_pStockIO)[m_PreNowNext_Index[2]]->Code;
					TxtCurSetCode = (*g_pStockIO)[m_PreNowNext_Index[2]]->setcode;
					SendMessage(WM_RICHEDIT_MESSAGE);
				}
			}
			return TRUE;
		}
	}	
	return CView::PreTranslateMessage(pMsg);
}

void CTxtView::OnRightClick(UINT nID)
{
	switch(nID)
	{
	case ID_RIGHT_ALLSELECT:
		{
			long i=m_RichEdit.GetTextLength();
			m_RichEdit.SetSel(0,i);
			break;
		}
	case ID_RIGHT_COPY:
		{
			m_RichEdit.Copy();
			break;
		}
	case ID_RIGHT_ADDNOTE:
		{
			char *Separator = "***********************************************************************\r\n\r\n\r\n";
			int tempLen = strlen(Separator);
			char *pWriteBuf;
			long lStart,lEnd,lLen;
			m_RichEdit.GetSel(lStart,lEnd);
			lLen = lEnd - lStart;
			//申请内存空间
			pWriteBuf = new char[lLen+1];
			m_RichEdit.GetSelText(pWriteBuf);
			CFile theFile;
			BOOL bRet = FALSE;
			CString tmpStr;
			if(theFile.Open(g_WSXHStr+"Coolinfo.Txt",CFile::modeCreate|CFile::modeWrite|CFile::modeNoTruncate))
			{
				theFile.SeekToEnd();
				theFile.Write(Separator,tempLen);
				theFile.Write(pWriteBuf,lLen);
				theFile.Close();
			}
			delete pWriteBuf;
			break;
		}
	}
}


//改变JBM配置
void CTxtView::OnSelectJBM(UINT nID)
{
	//CurrentJBM能为-1，故ID_TDX_JBZL与ID_JBM_MENU1的ID号应相邻
	CurrentJBM = nID - ID_JBM_MENU1;
	g_CurrentJBM = CurrentJBM;
	Resize();
	SendMessage(WM_RICHEDIT_MESSAGE);
}

void CTxtView::OnUpdateSelectJBM(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck( CurrentJBM  == (pCmdUI->m_nID - ID_JBM_MENU1));
}

void CTxtView::OnUpdateCopy(CCmdUI* pCmdUI)
{
	long StartChar,EndChar;
	m_RichEdit.GetSel(StartChar,EndChar);
	if(StartChar == EndChar)
		pCmdUI->Enable(FALSE);
	else
		pCmdUI->Enable(TRUE);
}

void CTxtView::OnUpdateAddNote(CCmdUI* pCmdUI)
{
	long StartChar,EndChar;
	m_RichEdit.GetSel(StartChar,EndChar);
	if(StartChar == EndChar)
		pCmdUI->Enable(FALSE);
	else
		pCmdUI->Enable(TRUE);
}

void CTxtView::GetTdxJBZL(const char *code,short setcode)
{
	m_RichEdit.ShowWindow(SW_SHOW);
	MemStkInfo *tempStkInfo = g_pStockIO->Get_MemStkInfo(code,setcode);
	if(!tempStkInfo) return;
	int itmp = GetStockType(tempStkInfo->setcode,tempStkInfo->Code);
	BOOL m_bQz = ((itmp==CODE_SHQZ) || (itmp==CODE_SZQZ));
	
	CString PostFix="";
	if(tempStkInfo->J_HalfYearFlag==3)
		PostFix="㈠";
	else if(tempStkInfo->J_HalfYearFlag==6)
		PostFix="㈡";
	else if(tempStkInfo->J_HalfYearFlag==9)
		PostFix="㈢";
	else if(tempStkInfo->J_HalfYearFlag==12)
		PostFix="㈣";

	CString TotalStr = "",TotalStr2 = "",TotalStrQX = "",TotalStrQX2 = "",TotalFCStr2 = "",TotalFCStr = "",tempStr;
	float ftmp;
	if( testzs(tempStkInfo) ) 
	{
		TotalStr2 = "指数没有财务数据.";
		m_RichEdit.SetWindowText(_F(TotalStr2));
		return;
	}
	if(tempStkInfo->J_gxrq <= 0) 
	{
		TotalStr2 = "该股票无财务数据.按TAB键或点右键切换其它类型的基本资料";
		m_RichEdit.SetWindowText(_F(TotalStr2));
		return;
	}
	tempStr= "本软件提供的数据仅供参考,力求但不保证内容的可靠性和准确性.据此操作,风险自担.\r\n";
	TotalStr2 += tempStr;
	CTime date = LongToTime(tempStkInfo->J_gxrq);
	tempStr.Format("更新时间:%d年%d月%d日\t",date.GetYear(),date.GetMonth(),date.GetDay());
	TotalStr2 += tempStr;
	int nGpKind = GetStockType(setcode,code);
	switch(nGpKind)
	{
	case CODE_SZBG:
		TotalStr2 += "注意：以下数据的货币单位（元）为港币\r\n\r\n";
		break;
	case CODE_SHBG:
		TotalStr2 += "注意：以下数据的货币单位（元）为美元\r\n\r\n";
		break;
	default:
		TotalStr2 += "注意：以下数据的货币单位（元）为人民币\r\n\r\n";
		break;
	}
	tempStr.Format((CString)"%40s(%s)"+_F("基本资料\r\n"),tempStkInfo->Name,tempStkInfo->Code);
	TotalStr2 = _F(TotalStr2)+tempStr;
	
	tempStr="                              ──────────────";
	TotalStr += tempStr+"\r\n";
	tempStr.Format("%18s: %13.2f万股","总股本",tempStkInfo->J_zgb);
	TotalStr += tempStr;
	switch(nGpKind)
	{
	case CODE_SZBG:
	case CODE_SHBG:
		tempStr.Format("%18s: %13.2f万股\r\n","流通B股",tempStkInfo->ActiveCapital);
		break;
	default:
		tempStr.Format("%18s: %13.2f万股\r\n","流通A股",tempStkInfo->ActiveCapital);
		break;
	}
	TotalStr += tempStr;
	switch(nGpKind)
	{
	case CODE_SHBG:
	case CODE_SZBG:
		char parAgCode[SH_CODE_LEN+1];short porAgSetCode;
		if(tempStkInfo->ActiveCapital!=tempStkInfo->J_bg)
			tempStr.Format("%18s: %13.2f万股","流通A股",tempStkInfo->J_bg);
		else if(FindParAgForBg(parAgCode, porAgSetCode, code, setcode))
		{
			MemStkInfo *parAgStkInfo = g_pStockIO->Get_MemStkInfo(parAgCode,porAgSetCode);
			tempStr.Format("%18s: %13.2f万股","流通A股",parAgStkInfo->ActiveCapital);
		}
		else tempStr.Format("%18s: %13.2f万股","流通A股",0);
		break;
	default:
		tempStr.Format("%18s: %13.2f万股","流通B股",m_bQz?0.0:tempStkInfo->J_bg);
		break;
	}
	TotalStr += tempStr;
	tempStr.Format("%18s: %13.2f万股\r\n\r\n","H股",m_bQz?0.0:tempStkInfo->J_hg);
	TotalStr += tempStr;
	tempStr.Format("%18s: %13.2f万元","总资产",tempStkInfo->J_zzc);
	TotalStr += tempStr;
	tempStr.Format("%18s: %13.2f万元\r\n","流动资产",m_bQz?0.0:tempStkInfo->J_ldzc);
	TotalStr += tempStr;
	tempStr.Format("%18s: %13.2f万元","固定资产",m_bQz?0.0:tempStkInfo->J_gdzc);
	TotalStr += tempStr;
	tempStr.Format("%18s: %13.2f万元\r\n","无形资产",m_bQz?0.0:tempStkInfo->J_wxzc);
	TotalStr += tempStr;
	tempStr.Format("%18s: %13.2f万元","长期投资",m_bQz?0.0:tempStkInfo->J_cqtz);
	TotalStr += tempStr;
	tempStr.Format("%18s: %13.2f万元\r\n","流动负债",tempStkInfo->J_ldfz);
	TotalStr += tempStr;
	tempStr.Format("%18s: %13.2f万元","长期负债",tempStkInfo->J_cqfz);
	TotalStr += tempStr+"\r\n";
	tempStr.Format("%18s: %13.2f万元","资本公积金",tempStkInfo->J_zbgjj);
	TotalStr += tempStr;
	tempStr.Format("%18s: %13.2f万元\r\n","净资产",tempStkInfo->J_jzc);
	TotalStr += tempStr;
	tempStr.Format("%18s: %13.2f万元","主营收入",tempStkInfo->J_zysy);
	TotalStr += tempStr;
	tempStr.Format("%18s: %13.2f万元\r\n","主营利润",tempStkInfo->J_zyly);
	TotalStr += tempStr;
	tempStr.Format("%18s: %13.2f万元","应收帐款",tempStkInfo->J_yszk);
	TotalStr += tempStr;
	tempStr.Format("%18s: %13.2f万元\r\n","营业利润",tempStkInfo->J_yyly);
	TotalStr += tempStr;
	tempStr.Format("%18s: %13.2f万元","投资收益",tempStkInfo->J_tzsy);
	TotalStr += tempStr;
	tempStr.Format("%18s: %13.2f万元\r\n","经营现金流量",tempStkInfo->J_jyxjl);
	TotalStr += tempStr;
	tempStr.Format("%18s: %13.2f万元","总现金流量",tempStkInfo->J_zxjl);
	TotalStr += tempStr;
	tempStr.Format("%18s: %13.2f万元\r\n","存货",tempStkInfo->J_ch);
	TotalStr += tempStr;
	tempStr.Format("%18s: %13.2f万元","利润总额",tempStkInfo->J_lyze);
	TotalStr += tempStr;
	tempStr.Format("%18s: %13.2f万元\r\n","税后利润",tempStkInfo->J_shly);
	TotalStr += tempStr;
	tempStr.Format("%18s: %13.2f万元","净利润",tempStkInfo->J_jly);
	TotalStr += tempStr;
	tempStr.Format("%18s: %13.2f万元\r\n","未分配利润",tempStkInfo->J_wfply);
	TotalStr += tempStr;
	//地域
	if(tempStkInfo->J_addr<1 || tempStkInfo->J_addr>=TOTAL_DY_NUM)
		tempStr.Format("%18s: %17s","所属地区","");
	else
		tempStr.Format("%18s: %17s","所属地区",g_cwAddrTopic[tempStkInfo->J_addr-1]);
	TotalStr += tempStr;
	//行业
	if(tempStkInfo->J_hy<1 || tempStkInfo->J_hy>=TOTAL_HY_NUM)
		tempStr.Format("%18s: %17s\r\n","所属行业","其它行业");
	else
		tempStr.Format("%18s: %17s\r\n","所属行业",g_cwHYstr[tempStkInfo->J_hy-1]);
	TotalStr += tempStr+"\r\n";

	if(tempStkInfo->J_zgb > 1 && fabs(tempStkInfo->J_jzc)>COMPPREC)
			ftmp = tempStkInfo->J_jzc/tempStkInfo->J_zgb;
	else
			ftmp = tempStkInfo->J_tzmgjz;

	if(m_bQz)
	{
		tempStr.Format("%18s: %15.3f元\r\n","行权价",ftmp);
		TotalStr += tempStr;
	}
	else
	{
		tempStr.Format("%18s: %15.3f元","每股净资产",ftmp);
		TotalStr += tempStr;
		tempStr.Format("%18s: %15.3f元\r\n","季报调整净资",tempStkInfo->J_tzmgjz);
		TotalStr += tempStr;
	}
	if(tempStkInfo->J_zgb > 1)
			ftmp = tempStkInfo->J_wfply/tempStkInfo->J_zgb;
	else	ftmp = 0.00;
	tempStr.Format("%18s: %15.3f元","每股未分配",ftmp);
	TotalStr += tempStr;

	if(tempStkInfo->J_zgb > 1 && fabs(tempStkInfo->J_jly)>COMPPREC)
	{
		if(fabs(tempStkInfo->J_mgsy2)>COMPPREC)
			ftmp = tempStkInfo->J_mgsy2;
		else
			ftmp = tempStkInfo->J_jly/tempStkInfo->J_zgb;
	}
	else	ftmp = 0.00;
	tempStr.Format("%16s%s: %15.3f元\r\n","每股收益",PostFix,ftmp);
	TotalStr += tempStr;

	if(tempStkInfo->J_zgb > 1)
			ftmp = tempStkInfo->J_zbgjj/tempStkInfo->J_zgb;
	else	ftmp = 0.00;
	tempStr.Format("%18s: %15.3f元","每股公积金",ftmp);
	TotalStr += tempStr;

	if(tempStkInfo->J_zzc > 1)
			ftmp = tempStkInfo->J_jzc*100.0/tempStkInfo->J_zzc;
	else	ftmp = 0.00;
	tempStr.Format("%18s: %16.2f%%\r\n","股东权益比",ftmp);
	TotalStr += tempStr;

	if(tempStkInfo->J_zgb > 1)
			ftmp = tempStkInfo->J_jyxjl/tempStkInfo->J_zgb;
	else	ftmp = 0.00;
	tempStr.Format("%18s: %15.3f元","每股经营现金流",ftmp);
	TotalStr += tempStr;

	if(tempStkInfo->J_zysy > 1)
			ftmp = tempStkInfo->J_zyly*100.0/tempStkInfo->J_zysy;
	else	ftmp = 0.00;
	tempStr.Format("%18s: %16.2f%%\r\n","主营业务利润率",ftmp);
	TotalStr += tempStr;
	
	if(tempStkInfo->J_ldfz > 1)
			ftmp = (tempStkInfo->J_ldzc-tempStkInfo->J_ch)/tempStkInfo->J_ldfz;
	else	ftmp = 0.00;
	tempStr.Format("%18s: %17.3f","速动比率",ftmp);
	TotalStr += tempStr;

	if(tempStkInfo->J_zzc > 1)
			ftmp = (tempStkInfo->J_ldfz+tempStkInfo->J_cqfz)*100.0/tempStkInfo->J_zzc;
	else	ftmp = 0.00;
	tempStr.Format("%18s: %16.2f%%\r\n","资产负债率",ftmp);
	TotalStr += tempStr;

//	if(tempStkInfo->Close > COMPPREC &&  tempStkInfo->J_zxjl > COMPPREC)
//			ftmp = (tempStkInfo->J_zgb*tempStkInfo->Close)/tempStkInfo->J_zxjl;
//	else	ftmp = 0.00;
//	tempStr.Format("%18s: %17.3f","市现率",ftmp);
//	TotalStr += tempStr;
//
//	if(tempStkInfo->Close > COMPPREC &&  tempStkInfo->J_zysy > COMPPREC)
//			ftmp = (tempStkInfo->J_zgb*tempStkInfo->Close)/tempStkInfo->J_zysy;
//	else	ftmp = 0.00;
//	tempStr.Format("%18s: %16.2f\r\n","市销率",ftmp);
//	TotalStr += tempStr;

	if(tempStkInfo->J_jzc > 1)
			ftmp = tempStkInfo->J_jly/tempStkInfo->J_jzc*100.0;
	else	ftmp = 0.00;
	tempStr.Format("%16s%s: %16.2f%%","净资产收益率",PostFix,ftmp);
	TotalStr += tempStr;

	tempStr = "\r\n";
	if(tempStkInfo->J_HalfYearFlag==3)
		tempStr.Format("%18s: %17s\r\n","报告类别","第一季度季报");
	else if(tempStkInfo->J_HalfYearFlag==6)
		tempStr.Format("%18s: %17s\r\n","报告类别","第二季度季报");		
	else if(tempStkInfo->J_HalfYearFlag==9)
		tempStr.Format("%18s: %17s\r\n","报告类别","第三季度季报");		
	else if(tempStkInfo->J_HalfYearFlag==12)
		tempStr.Format("%18s: %17s\r\n","报告类别","第四季度季报");		
	TotalStr += tempStr;
	//得到权息资料	
	TotalStrQX2.Format((CString)"\r\n%40s(%s)"+_F("权息资料\r\n"),tempStkInfo->Name,tempStkInfo->Code);
	TotalStrQX="                              ──────────────";
	TotalStrQX += "\r\n";
	tempStr.Format("%6s ",strGBBQTopic[0]);
	TotalStrQX += tempStr;
	tempStr.Format("%-10s ",strGBBQTopic[1]);
	TotalStrQX += tempStr;
	tempStr.Format("%6s ",strGBBQTopic[2]);
	TotalStrQX += tempStr;
	tempStr.Format("%3s ",strGBBQTopic[3]);
	TotalStrQX += tempStr;
	tempStr.Format("%2s ",strGBBQTopic[4]);
	TotalStrQX += tempStr;
	tempStr.Format("%6s ",strGBBQTopic[5]);
	TotalStrQX += tempStr;
	tempStr.Format("%10s ",strGBBQTopic[6]);
	TotalStrQX += tempStr;
	tempStr.Format("%10s ",strGBBQTopic[7]);
	TotalStrQX += tempStr;
	tempStr.Format("%10s ",strGBBQTopic[8]);
	TotalStrQX += tempStr;
	tempStr.Format("%10s ",strGBBQTopic[9]);
	TotalStrQX += tempStr + "\r\n";
	tempStr="───────────────────────────────────────────";
	TotalStrQX += tempStr + "\r\n";
	int nMaxCWData = 200,nNowCWData;
	CWDATA *pCWData = new CWDATA[nMaxCWData];
	nNowCWData = g_pStockIO->GetTQInfo(tempStkInfo,pCWData,nMaxCWData);
	
	for(int i=nNowCWData-1;i >= 0;i--)
	{	
		switch(pCWData[i].Type)
		{
		case 2:	//送配股上市
		case 3: //非流通股上市
		case 5: //股本变化
		case 7: //上市公司从市场回购本公司的股份
		case 8: //增发新股上市
		case 9: //转配股上市
		case 10://可转债上市
			tempStr.Format("%6d %-11s %4s %4s %4s %6s %10.1f %10.1f %10.1f %10.1f ",
							pCWData[i].Date,strGBBQComment[pCWData[i].Type-1],"","","",
							"",pCWData[i].B01,pCWData[i].B03,pCWData[i].B02,pCWData[i].B04
							);
			TotalStrQX += tempStr+"\r\n";
			break;
		case 1:
			tempStr.Format("%6d ",pCWData[i].Date);
			TotalStrQX += tempStr;
			tempStr.Format("%-11s ",strGBBQComment[pCWData[i].Type-1]);
			TotalStrQX += tempStr;
			if(pCWData[i].B03>COMPPREC)
				tempStr.Format("%4.2f ",pCWData[i].B03);
			else
				tempStr.Format("%4s ","");
			TotalStrQX += tempStr;

			if(pCWData[i].B01>COMPPREC)
				tempStr.Format("%4.2f ",pCWData[i].B01);
			else
				tempStr.Format("%4s ","");
			TotalStrQX += tempStr;

			if(pCWData[i].B04>COMPPREC)
				tempStr.Format("%4.2f ",pCWData[i].B04);
			else
				tempStr.Format("%4s ","");
			TotalStrQX += tempStr;

			if(pCWData[i].B02>COMPPREC)
				tempStr.Format("%6.2f ",pCWData[i].B02);
			else
				tempStr.Format("%6s ","");
			TotalStrQX += tempStr;
			tempStr.Format("%10s %10s %10s %10s","","","","");
			TotalStrQX += tempStr+"\r\n";
			break;
		case 6: //增发新股
			tempStr.Format("%6d %-11s %4s %4s %4s ",pCWData[i].Date,strGBBQComment[pCWData[i].Type-1],"","","");
			TotalStrQX += tempStr;
			if(pCWData[i].B02>COMPPREC) 
				tempStr.Format("%6.2f ",pCWData[i].B02);
			else
				tempStr.Format("%6s ","");
			TotalStrQX += tempStr;
			tempStr.Format("%10s %7.1f %10s %10s","",pCWData[i].B03,"","");
			TotalStrQX += tempStr+"\r\n";
			break;
		case 11:
		case 12:
			tempStr.Format("%6d ",pCWData[i].Date);
			TotalStrQX += tempStr;
			tempStr.Format("%-11s ",strGBBQComment[pCWData[i].Type-1]);
			TotalStrQX += tempStr;
			if(pCWData[i].B03>COMPPREC)
				tempStr.Format("比例:%4.2f ",pCWData[i].B03);
			else
				tempStr.Format("%4s ","");
			TotalStrQX += tempStr+"\r\n";
			break;		
		case 13:
		case 14:
			tempStr.Format("%6d ",pCWData[i].Date);
			TotalStrQX += tempStr;
			tempStr.Format("%-11s ",strGBBQComment[pCWData[i].Type-1]);
			TotalStrQX += tempStr;
			if(pCWData[i].B03>COMPPREC)
				tempStr.Format("份数:%4.2f ",pCWData[i].B03);
			else
				tempStr.Format("%4s ","");
			TotalStrQX += tempStr;
			if(pCWData[i].B01>COMPPREC)
				tempStr.Format("行权价:%4.2f ",pCWData[i].B01);
			else
				tempStr.Format("%4s ","");
			TotalStrQX += tempStr+"\r\n";
			break;
		default:
			tempStr.Format("%6d %-11s ",pCWData[i].Date,"未知股本变动");
			TotalStrQX += tempStr;
			tempStr.Format("%4s %4s %4s %6s %11s %11s %11s %11s ","","","","","","","","");
			TotalStrQX += tempStr+"\r\n";
			break;
		}
	}
//////////////////////////////////////////////////////////////////////////
//更名信息
	TDXSTKOLDNAME FCData[100]={0};
	short nFCNum = g_pStockIO->GetFaceData(tempStkInfo, FCData, 100);
	if(nFCNum>0)
	{
		TotalFCStr2.Format((CString)"\r\n%40s(%s)"+_F("更名信息\r\n"),tempStkInfo->Name,tempStkInfo->Code);
		TotalFCStr="                              ──────────────";
		TotalFCStr += "\r\n";
		TotalFCStr += " 更新日\t\t原名\t\t更新名\t\t更名原因";
		TotalFCStr += "\r\n";
		TotalFCStr += "───────────────────────────────────────────";
		TotalFCStr += "\r\n";
		for(i=nFCNum-1;i>=0;i--)
		{
			char newName[31]={0};
			if(i<nFCNum-1) strcpy(newName, FCData[i+1].Name);
			else strcpy(newName, tempStkInfo->Name);
			tempStr.Format("%08d\t\t%-8s\t%-8s\t%-s", FCData[i].Date, FCData[i].Name, newName, FCData[i].Reason);
			TotalFCStr += tempStr+"\r\n";
		}
	}

	if(strlen(g_AdvertInfo.F10AdvertStr1)>0)
	{
		TotalFCStr+="───────────────────────────────────────────\r\n";
		TotalFCStr+=g_AdvertInfo.F10AdvertStr1;
		if(strlen(g_AdvertInfo.F10AdvertStr2)>0)
		{
			TotalFCStr+="\r\n";
			TotalFCStr+=g_AdvertInfo.F10AdvertStr2;
		}
	}

	TDEL(pCWData);
	m_RichEdit.SetWindowText(TotalStr2+_F(TotalStr)+TotalStrQX2+_F(TotalStrQX)+_F(TotalFCStr2)+_F(TotalFCStr));
	ProcessFind();
}

void CTxtView::Resize()
{
	if(!m_bLayoutFlag) return;
	CRect rcdlg;
	GetClientRect(&rcdlg);
	if(m_pExplorer && m_pExplorer->m_hWnd && ::IsWindow(m_pExplorer->m_hWnd))
	{
		m_pExplorer->SetWindowPos(NULL,rcdlg.left,rcdlg.bottom-AD_HEIGHT+1,rcdlg.Width()+4,AD_HEIGHT,SWP_SHOWWINDOW);
		rcdlg.bottom-=AD_HEIGHT;
	}
	if(!OnLine || !g_bHasF10Cfg || CurrentJBM == JBM_TDX)
	{
		for(int i=0;i<MAX_F10CAPTION;i++)
			m_btnF10[i].ShowWindow(SW_HIDE);
		m_nRichEditTop=rcdlg.top;
		m_RichEdit.SetWindowPos(NULL,rcdlg.left+16,m_nRichEditTop,rcdlg.Width()-16,rcdlg.Height(),SWP_NOZORDER);
	}
	else
		JustifyButton(rcdlg);
	Invalidate(TRUE);
}

void CTxtView::OnSize(UINT nType, int cx, int cy) 
{
	CView::OnSize(nType, cx, cy);
	
	Resize();
}

void CTxtView::SetPageGP(short *lpnIndex,short nTotalGP,short nCurGP,LinkedSetInfo *pLSetInfo)
{
	if(nTotalGP == -1)
	{	
		m_nPageGPNum = -1;
		return;
	}
	for(int i = 0;i < nTotalGP;i++)
	{	
		if(lpnIndex[i] == nCurGP)
			break;
	}
	m_nCurPageGP = i;	//重新给当前股票赋值，传进来的是股票序号，应转成数组下标
	memcpy(m_nPageGPIndex,lpnIndex,nTotalGP*sizeof(short));
	m_nPageGPNum = nTotalGP;

	if(pLSetInfo) 
	{
		memcpy(&m_LSetInfoN, pLSetInfo, sizeof(LinkedSetInfo));
		m_LSetInfoN.pMsgRtnWnd = this;
		memcpy(m_LSetInfoN.lpnUsedStkIndex,lpnIndex,nTotalGP*sizeof(short));
		memcpy(&m_LSetInfoP, pLSetInfo, sizeof(LinkedSetInfo));
		m_LSetInfoP.pMsgRtnWnd = this;
		memcpy(m_LSetInfoP.lpnUsedStkIndex,lpnIndex,nTotalGP*sizeof(short));
	}
	
	MemStkInfo *pInfo = (*g_pStockIO)[nCurGP];
	if(pInfo)
	{
		TxtCurGPCode = pInfo->Code;
		TxtCurSetCode = pInfo->setcode;
	}
	SendMessage(WM_RICHEDIT_MESSAGE);
}

int CTxtView::GetNextPageGP(short *lpnIndex,short num,BOOL bNext)
{
	if(num < 0 || num > MULHQ_WANTNUM) return 0;
	int i,n=0,tmpIndex;
	short nCurGP = g_pStockIO->GetIndex(m_TxtCurGPCode,m_TxtCurSetCode);
	int nStartRow = m_LSetInfoP.nStartRow;
	if(m_nPageGPNum>0)
	{
		m_nCurPageGP = -1;
		for(i = nStartRow;i < nStartRow+m_nPageGPNum;i++)
		{	
			if(m_nPageGPIndex[i%m_nPageGPNum] == nCurGP)
			{
				m_nCurPageGP = i%m_nPageGPNum;	//重新给当前股票赋值，传进来的是股票序号，应转成数组下标
				break;
			}
		}
	}
	if(m_nCurPageGP<0)
	{
		m_nPageGPNum = -1;
		for(int i=0;i<MAXGPNUM;i++)
			m_nPageGPIndex[i] = -1;
		m_nCurPageGP = -1;
	}
	BOOL bNeedRedraw = FALSE;
	short nTmpPageGpNum = m_nCurPageGP;
	memset(lpnIndex,-1,num*sizeof(short));
	if(m_nPageGPNum > 0)	//如果有股票集合
	{
		if(!bNext)
		{
			int nPreFirst = nTmpPageGpNum-num;
			if(m_LSetInfoP.bUsed&&nPreFirst<m_LSetInfoP.nStartRow)
			{
				m_LSetInfoP.nStartRow = (m_nCurPageGP-m_LSetInfoP.nValidRow+m_nPageGPNum)%m_nPageGPNum;
				if(m_LSetInfoP.nStartRow+m_LSetInfoP.nValidRow>m_nPageGPNum)
					m_LSetInfoP.nStartRow = 0;
				memcpy(m_LSetInfoP.lpnUsedStkIndex, m_nPageGPIndex, MAXGPNUM*sizeof(short));
				if(IsRemoteGet(&m_LSetInfoP))
					AfxGetMainWnd()->SendMessage(UM_WIDEDATA_SORT, (WPARAM)&m_LSetInfoP, 0);
				bNeedRedraw = TRUE;
				
				m_nPageGPNum = m_LSetInfoP.nTolRow;
				if(nTmpPageGpNum-num<0)	nTmpPageGpNum = max(0,m_nPageGPNum-max(1,m_nPageGPNum%num));
				else	nTmpPageGpNum = nTmpPageGpNum-num;
				memcpy(m_nPageGPIndex, m_LSetInfoP.lpnUsedStkIndex, MAXGPNUM*sizeof(short));
			}
			else if(nTmpPageGpNum-num<0)	
				nTmpPageGpNum = max(0,m_nPageGPNum-max(1,m_nPageGPNum%num));
			else					
				nTmpPageGpNum = nTmpPageGpNum-num;
		}
		else
		{
			int nNextLast = nTmpPageGpNum+2*num-1;
			if(m_LSetInfoN.bUsed&&nNextLast>=min(m_LSetInfoN.nStartRow+m_LSetInfoN.nVisualRow, m_LSetInfoN.nTolRow))
			{
				if(nTmpPageGpNum+num>=m_nPageGPNum) m_LSetInfoN.nStartRow = 0;
				else m_LSetInfoN.nStartRow = nTmpPageGpNum+num;
				memcpy(m_LSetInfoN.lpnUsedStkIndex, m_nPageGPIndex, MAXGPNUM*sizeof(short));
				if(IsRemoteGet(&m_LSetInfoN))
					AfxGetMainWnd()->SendMessage(UM_WIDEDATA_SORT, (WPARAM)&m_LSetInfoN, 0);
				bNeedRedraw = TRUE;
				
				m_nPageGPNum = m_LSetInfoN.nTolRow;
				nTmpPageGpNum = m_LSetInfoN.nStartRow;
				memcpy(m_nPageGPIndex, m_LSetInfoN.lpnUsedStkIndex, MAXGPNUM*sizeof(short));
			}
			else if(nTmpPageGpNum+num>=m_nPageGPNum)	
				nTmpPageGpNum = 0;
			else								
				nTmpPageGpNum = nTmpPageGpNum+num;
		}
		for(i=nTmpPageGpNum,n=0;i<m_nPageGPNum && n<num;i++,n++)
		{
			tmpIndex = m_nPageGPIndex[i];
			lpnIndex[n] = tmpIndex;
		}
		nCurGP = lpnIndex[0];
		if(bNeedRedraw) Invalidate();
	}
	else					//如果是所有股票集合
	{
		int allgp = 0; 
		int nBasegp = 0;
		allgp = g_pStockIO->GetStockNum(ALL_ALL);
		if(nCurGP>=DS_FROM) 
			nCurGP = 0;
	//	m_nPageGPNum = allgp;
	//	for(i=0;i<allgp;i++)
	//	{
	//		m_nPageGPIndex[i] = i;
	//		if(i==nCurGP) m_nCurPageGP = i;
	//	}
		if(!bNext)
		{
			if((nCurGP-nBasegp)-num < 0)	nCurGP = nBasegp + max(0,allgp-max(1,allgp%num));
			else					nCurGP = nCurGP-num;
		}
		else
		{
			if((nCurGP-nBasegp)+num >= allgp)	nCurGP = nBasegp;
			else						nCurGP = nCurGP+num;
		}
		for(i=nCurGP-nBasegp,n=0;i<allgp && n<num;i++,n++)
			lpnIndex[n] = nBasegp + i;
	}
	return n;
}

void CTxtView::Get3Tomo(short *lp2Index)
{
	for(int i=0;i<3;i++)
		lp2Index[i] = -1;
	lp2Index[1] = g_pStockIO->GetIndex(m_TxtCurGPCode,m_TxtCurSetCode);
	GetNextPageGP(&lp2Index[0], 1, FALSE);
	GetNextPageGP(&lp2Index[2], 1, TRUE);
}

void CTxtView::OnDraw(CDC* pDC) 
{	
	CRect rect;
	GetClientRect(&rect);
	if(CurrentJBM < JBM_TDX)
	{
		pDC->SetTextColor(m_ForeColor);
		pDC->SetBkMode(TRANSPARENT);
		pDC->TextOut(100,100,"暂无内容，请按TAB键或点右键切换");
	}
	if(m_bLayoutFlag && OnLine && g_bHasF10Cfg && CurrentJBM >= 0)
	{
		if(m_pExplorer && m_pExplorer->m_hWnd && ::IsWindow(m_pExplorer->m_hWnd))
			rect.bottom-=AD_HEIGHT;
		CPen thePen; //画间隔线
		thePen.CreatePen(PS_SOLID,1,RGB(64,64,64));
		CPen *pOldPen = pDC->SelectObject(&thePen);
		pDC->MoveTo(0,m_nRichEditTop-7);
		pDC->LineTo(rect.Width(),m_nRichEditTop-7);
		if(g_F10CaptionNum==0)
		{
			CFont NewFont;
			NewFont.CreateFont(g_nFontSize[2],0,0,0,g_nFontWeigth[2],0,0,0,(g_b2000XPSys&&stricmp(g_strFontName[2],"System")==0)?ANSI_CHARSET:DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,g_b2000XPSys?ANTIALIASED_QUALITY:DEFAULT_QUALITY,DEFAULT_PITCH|FF_DONTCARE,g_strFontName[2]);
			CFont *oldFont=pDC->SelectObject(&NewFont);
			pDC->SetTextColor(m_ForeColor);
			pDC->SetBkMode(TRANSPARENT);
			rect.bottom=rect.top+BTN_HEIGHT+3;
			pDC->DrawText(_F("无相关资料"),rect,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
			pDC->SelectObject(oldFont);
			NewFont.DeleteObject();
		}
		pDC->SelectObject(pOldPen);
		thePen.DeleteObject();
		for(int i=0;i<5;i++)
			m_PreNext_Rect[i].SetRect(0, 0, 0, 0);
		//画前后
		CFont *oldFont = g_d.SelectFont(pDC, SMALL_FONT);
		pDC->SetBkMode(TRANSPARENT);
		CSize sz_jbm = pDC->GetTextExtent("维赛特");
		CSize sz_name = pDC->GetTextExtent("亿安科技");
		if(m_bDS && g_bLongDSName)
			sz_name = pDC->GetTextExtent("恒生金融分类指数");
		CSize sza = pDC->GetTextExtent("▲");
		sz_name.cx+=2;
		sz_jbm.cx+=2;
		int nAWidth = sz_name.cx + sza.cx + 3;
		BOOL bShowName = TRUE;
		if(rect.right-m_nMostLeft<nAWidth) bShowName = FALSE;
		if(!bShowName) nAWidth = sza.cx + 3;
		int x = rect.right - nAWidth, y = rect.top+6;
		char strShow[256]={0};
		
		//
		for(i=0;i<g_TdxCfg.JbmNum;i++)
		{
			if(strstr(g_TdxCfg.Jbmname[i], "港澳"))
			{
				CurrentJBM = i;
				break;
			}
		}
		//不要了吧
		return;

		CSize szar;
		int x1 = x;
		if(m_PreNowNext_Index[0]>=0)
		{
			if(bShowName) 
			{
				g_d.DisplaySameWidtchText(pDC, x, y, m_ForeColor, (*g_pStockIO)[m_PreNowNext_Index[0]]->Name);
				x1 = x+sz_name.cx-2;
				szar = g_d.DisplayText(pDC, x1, y, m_ForeColor, _F("▲"));
			}
			else 
				szar = g_d.DisplayText(pDC, x, y, m_ForeColor, _F("▲"));
			m_PreNext_Rect[0].top = y;
			m_PreNext_Rect[0].left = x;
			m_PreNext_Rect[0].bottom = m_PreNext_Rect[0].top+szar.cy;
			m_PreNext_Rect[0].right = x1+szar.cx;
			m_PreNext_Rect[0].InflateRect(4,4,1,3);
			g_d.DrawRect(pDC,m_PreNext_Rect[0],m_ForeColor);
		}
		if(m_PreNowNext_Index[2]>=0)
		{
			y += sz_name.cy+12;
			if(bShowName) 
			{
				g_d.DisplaySameWidtchText(pDC, x, y, m_ForeColor, (*g_pStockIO)[m_PreNowNext_Index[2]]->Name);
				x1 = x+sz_name.cx-2;
				szar = g_d.DisplayText(pDC, x1, y+2, m_ForeColor, _F("▼"));
			}
			else 
			{
				szar = g_d.DisplayText(pDC, x, y+2, m_ForeColor, _F("▼"));
			}
			m_PreNext_Rect[1].top = y;
			m_PreNext_Rect[1].left = x;
			m_PreNext_Rect[1].bottom = m_PreNext_Rect[1].top+szar.cy;
			m_PreNext_Rect[1].right = x1+szar.cx;
			m_PreNext_Rect[1].InflateRect(4,4,1,3);
			g_d.DrawRect(pDC,m_PreNext_Rect[1],m_ForeColor);
		}
		//
		x = 3, y = rect.top+6;
		if(!m_bDS && g_TdxCfg.JbmNum>1 && CurrentJBM>=0 && CurrentJBM<g_TdxCfg.JbmNum)
		{
			if(g_TdxCfg.JbmNum==2)
			{
				m_PreNext_Rect[2].SetRect(x,y,x+sz_name.cx,y+sz_name.cy);
				m_PreNext_Rect[2].InflateRect(2,4,0,3);
				g_d.DisplaySameWidtchText(pDC, x+2, y, (CurrentJBM==0)?m_SelColor:m_ForeColor, _F(g_TdxCfg.Jbmname[0]));
				g_d.DrawRect(pDC,m_PreNext_Rect[2],m_ForeColor);

				m_PreNext_Rect[3]=m_PreNext_Rect[2];
				m_PreNext_Rect[3].OffsetRect(0,m_PreNext_Rect[2].Height()+4);				
				g_d.DisplaySameWidtchText(pDC, x+2, m_PreNext_Rect[3].top+4, (CurrentJBM==1)?m_SelColor:m_ForeColor, _F(g_TdxCfg.Jbmname[1]));
				g_d.DrawRect(pDC,m_PreNext_Rect[3],m_ForeColor);
			}
			else
			{
				m_PreNext_Rect[2].SetRect(x,y,x+sz_jbm.cx,y+sz_name.cy);
				m_PreNext_Rect[2].InflateRect(2,4,0,3);
				g_d.DisplaySameWidtchText(pDC, x+1, y, (CurrentJBM==0)?m_SelColor:m_ForeColor, _F(g_TdxCfg.Jbmname[0]));
				g_d.DrawRect(pDC,m_PreNext_Rect[2],m_ForeColor);

				m_PreNext_Rect[3]=m_PreNext_Rect[2];
				m_PreNext_Rect[3].OffsetRect(0,m_PreNext_Rect[2].Height()+4);				
				g_d.DisplaySameWidtchText(pDC, x+1, m_PreNext_Rect[3].top+4, (CurrentJBM==1)?m_SelColor:m_ForeColor, _F(g_TdxCfg.Jbmname[1]));
				g_d.DrawRect(pDC,m_PreNext_Rect[3],m_ForeColor);

				m_PreNext_Rect[4]=m_PreNext_Rect[2];
				m_PreNext_Rect[4].OffsetRect(m_PreNext_Rect[2].Width()+4,0);				
				g_d.DisplaySameWidtchText(pDC, m_PreNext_Rect[4].left+x+1, y, (CurrentJBM==2)?m_SelColor:m_ForeColor, _F(g_TdxCfg.Jbmname[2]));
				g_d.DrawRect(pDC,m_PreNext_Rect[4],m_ForeColor);
			}
		}
		else
		{
			MemStkInfo *tmpStkInfo = g_pStockIO->Get_MemStkInfo(m_TxtCurGPCode,m_TxtCurSetCode);
			if(tmpStkInfo)
			{
				CRect tmpRect(x,y,x+sz_name.cx+8,y+2*sz_name.cy+10);
				tmpRect.InflateRect(2,2,0,2);
				g_d.DrawRect(pDC,tmpRect,m_ForeColor);
				g_d.DisplayText(pDC,x+2,y+2,m_ForeColor,"%6s",tmpStkInfo->Code);
				g_d.DisplaySameWidtchText(pDC, x+2, y+22, m_ForeColor, tmpStkInfo->Name);
			}
		}
		pDC->SelectObject(oldFont);
	}

	CView::OnDraw(pDC);
}

BOOL CTxtView::OnEraseBkgnd(CDC* pDC) 
{
	CRect rect;
	GetClientRect(&rect);
	if(m_pExplorer && m_pExplorer->m_hWnd && ::IsWindow(m_pExplorer->m_hWnd))
		rect.bottom-=AD_HEIGHT;
	pDC->FillSolidRect (&rect,m_BackColor);
	return TRUE;
}

void CTxtView::OnRButtonUp(UINT nFlags, CPoint point) 
{
	CView::OnRButtonUp(nFlags, point);
}

short CTxtView::GetCurrentGPIndex()
{
	return g_pStockIO->GetIndex(m_TxtCurGPCode,m_TxtCurSetCode);
}

void CTxtView::OnInBlock()
{
	int tempCurrentNo = GetCurrentGPIndex();
	if(tempCurrentNo == -1)	return;
	AfxGetMainWnd()->SendMessage(UM_SEARCH_BLOCK,tempCurrentNo);
}

void CTxtView::OnRightAddblock() 
{
	Func_RightAddblock(GetCurrentGPIndex(),TRUE);
}

void CTxtView::OnRightAddZxg()
{
	Func_RightAddZxg(GetCurrentGPIndex(),TRUE);
}

void CTxtView::OnDelFromBlock()
{
	Func_DelFromBlock(GetCurrentGPIndex(),TRUE);
}

void CTxtView::OnOtherjbm() 
{
	if(g_TdxCfg.JbmNum == 1)
		OnSelectJBM(ID_JBM_MENU1);
}

void CTxtView::OnUpdateOtherjbm(CCmdUI* pCmdUI) 
{
	if(g_TdxCfg.JbmNum == 1)
		pCmdUI->SetCheck(CurrentJBM==0);
	else if(g_TdxCfg.JbmNum == 0)
		pCmdUI->Enable(FALSE);
	else
		pCmdUI->Enable(TRUE);		
}

void CTxtView::SetFindFlag(int pos,int len,COLORREF clr)
{
	cfm.cbSize=sizeof(cfm);
	cfm.crTextColor=clr;
	cfm.dwMask=CFM_COLOR;

	m_RichEdit.SetSel(pos,len+pos);
	m_RichEdit.SetSelectionCharFormat(cfm);
	m_RichEdit.SetSel(len+pos,len+pos);
}

void CTxtView::ProcessFind()
{
	if(g_nTxtFindNum < 1) return;
	int TotalLen = m_RichEdit.GetWindowTextLength();
	char *p,*tmpStr = new char[TotalLen+1];
	CString csString;
	m_RichEdit.GetWindowText(csString);
	strncpy(tmpStr,csString,TotalLen);
	tmpStr[TotalLen] = 0;
	int pos,SubLen,firstpos = 0;
	for(int i=0;i < g_nTxtFindNum;i++)
	{
		SubLen = g_TxtFindStr[i].GetLength();
		pos = 0;
		while(p = strstr(tmpStr+pos,g_TxtFindStr[i]))
		{
			pos = p-tmpStr;
			if(firstpos == 0) firstpos = pos;
			SetFindFlag(pos,SubLen,RGB(255,0,0));
			pos+=SubLen;
		}
	}
	delete tmpStr;
	//不选任何内容，否则SetSelectionCharFormat会起作用
	m_RichEdit.SetSel(firstpos,firstpos);
}

#include "TxtFindDlg.h"
void CTxtView::OnTxtfind()
{
	CTxtFindDlg	dlg;
	dlg.m_bEnable1 = TRUE;
	dlg.m_bEnable2 = TRUE;
	dlg.m_nRadio = g_nFindRadio;
	dlg.m_Str1 = "当前文本资料";
	dlg.m_Str2 = "所有下载的基本资料";
	if(dlg.DoModal() == IDOK)
	{
		g_nTxtFindNum = dlg.m_nFindNum;
		for(int i=0;i<g_nTxtFindNum;i++)
			g_TxtFindStr[i] = dlg.m_FindStr[i];
		int  nFileCount = 0;
		if(dlg.m_nRadio == 1)   //暂时仅支持本地资料文件
		{
			CFile theFile;
			if(!theFile.Open(g_WSXHStr+"tmp\\TxtFind.dat",CFile::modeCreate|CFile::modeWrite|CFile::shareDenyNone))
				return;
			//设定模态进度条对话框
			CProgressWnd wndProgress(this,_F("正在检索，请稍等..."),TRUE);
			wndProgress.GoModal();
			wndProgress.SetRange(0,4000);
			wndProgress.SetStep(1);
			g_bDonotClose = TRUE;
			long handle;
			int  done;
			char string[_MAX_PATH],filename[_MAX_PATH];
			//写文件头
			strcpy(filename,dlg.m_strFindTxt);
			theFile.Write(filename,255);
			struct tag_FindResult findresult;
			struct _finddata_t ffblk;
			for (int n = 0;n < g_TdxCfg.JbmNum;n++)
			{
				for(int k = 0; k < 2;k++)
				{
					sprintf(string,"%s*.txt",(k==0)?g_TdxCfg.SZBASE[n]:g_TdxCfg.SHBASE[n]);
					handle = _findfirst(string,&ffblk);
					if (handle==-1) done=-1;
					else            done=0;
					if(done) continue;
					while (!done)
					{
						//处理暂停事件
						sprintf(filename,"检索:%s %s \n共找到%d个文件",g_TdxCfg.Jbmname[n],ffblk.name,nFileCount);
						wndProgress.SetText(_F(filename));
						wndProgress.StepIt();
						wndProgress.PeekAndPump();
						if(wndProgress.Cancelled())
							goto aftercancel;
						if (ffblk.size != 0 && ffblk.name[0] != '.' && !(ffblk.attrib&_A_SUBDIR))
						{
							sprintf(filename,"%s%s",(k==0)?g_TdxCfg.SZBASE[n]:g_TdxCfg.SHBASE[n],ffblk.name);
							if(TestFound(filename,0))
							{
								findresult.which = n;
								strcpy(findresult.path,filename);
								theFile.Write(&findresult,sizeof(tag_FindResult));
								nFileCount++;
							}
						}
						done = _findnext(handle,&ffblk);
					}
					if(handle != -1)
						_findclose(handle);
				}
			}
		aftercancel:;
			g_bDonotClose = FALSE;
			theFile.Close();
		}
		else
			unlink(g_WSXHStr+"tmp\\TxtFind.dat");//删除检索结果
		CString TextStr;
		m_RichEdit.GetWindowText(TextStr);
		m_RichEdit.SetWindowText(TextStr);
		ProcessFind();	//总是处理当前文本
		g_nTxtFindNo = -1;
		if(nFileCount > 0) OnFindresult();
	}
}

#include "FindResultDlg.h"
void CTxtView::OnFindresult() 
{
	CFindResultDlg dlg;
	dlg.m_nStyle = 0;
	if(dlg.DoModal() == IDOK)
	{
		int which = (dlg.m_FindResult)[dlg.m_nCurSel].which;
		CurrentJBM = which;
		if(which < 0 || which >= g_TdxCfg.JbmNum)
			CurrentJBM = 0;
		CString tmpStr=(dlg.m_FindResult[dlg.m_nCurSel]).path;
		int pos = tmpStr.ReverseFind('\\');
		if(pos != -1) tmpStr = tmpStr.Mid(pos+1);
		pos = tmpStr.Find(".");
		TxtCurGPCode = tmpStr.Left(pos);
		TxtCurSetCode = -1;
		SendMessage(WM_RICHEDIT_MESSAGE);
	}
}

void CTxtView::GetRemoteFile(const char *Code,short setcode)
{
	m_bDS = FALSE;
	if(setcode!=SZ && setcode!=SH) 
		m_bDS = TRUE;
	if(m_bSegmentTxt&&g_bHasF10Cfg&&OnLine)
	{
		m_bCanClickFL = FALSE;
		g_nMainID = MainIDPlus(m_nMainID,TXTVIEW_WIN);
		g_nAssisID = NEW_JBMCFG_ASSISID;
		if(m_bDS)
			g_pGetData->DSGetNewJbmCfg(Code,setcode,CurrentJBM);
		else
			g_pGetData->GetNewJbmCfg(Code,setcode,CurrentJBM);
		memset(m_pTmpBuf,0,MAX_TXT_SIZE*sizeof(char));		
		return;
	}
	if(bRemoteFileOpen)
		theRemoteFile.Close();
	bRemoteFileOpen = FALSE;
	nCurRemotePos	= 0;

	if(!OnLine) //如果在脱机状态
	{
		CString tempStr,tempFileName;
		if(setcode == SZ) tempStr = "RemoteSZ\\";
		else tempStr = "RemoteSH\\";
		tempFileName.Format("%s%s%s.Txt", HomePath,tempStr,Code);
		CFile theFile;
		if( theFile.Open(tempFileName,CFile::modeRead|CFile::shareDenyNone))
		{
			DWORD len = theFile.GetLength(); //得到本地消息文件的长度
			//为buffer申请空间
			char *buffer = new char[len+1];
			theFile.Read(buffer,len);
			for(int k = 0;k < len;k++) //将'\0'变为空格
				if(buffer[k] == 0) buffer[k] = 0x20;
			buffer[len] = 0;
			m_RichEdit.SetRedraw(FALSE);
			m_RichEdit.SetWindowText (buffer);
			ProcessFind();
			m_RichEdit.SetRedraw(TRUE);
			m_RichEdit.Invalidate(FALSE);
			delete buffer;
			theFile.Close();
		}
		return;
	}
	m_bCanClickFL = FALSE;
	g_nMainID = MainIDPlus(m_nMainID,TXTVIEW_WIN);
	g_nAssisID = JBMLEN_ASSISID;
	g_pGetData->GetJbmFileLen(Code,setcode);

	memset(m_pTmpBuf,0,MAX_TXT_SIZE*sizeof(char));
}

void CTxtView::OnGetDataAck(WPARAM wParam,LPARAM lParam)
{
	ANSHEADER *		pHeader = &g_AnsHeader;
	if(wParam==1)	pHeader = &g_AnsHeader3;
	if(m_nMainID!=pHeader->MainID)
		return;
	switch(pHeader->AssisID)
	{
	case NEW_JBMCFG_ASSISID:
		{
			int titlenum = 0;
			if(m_bDS)
				titlenum = g_pGetData->DSGetNewJbmCfgACK();
			else
				titlenum = g_pGetData->GetNewJbmCfgACK();
			if(titlenum < 0)  titlenum = 0;
			m_bCanClickFL = TRUE;
			CRect rc;
			GetClientRect(&rc);
			if(m_pExplorer && m_pExplorer->m_hWnd && ::IsWindow(m_pExplorer->m_hWnd))
				rc.bottom-=AD_HEIGHT;
			JustifyButton(rc);
			Invalidate(TRUE);
			if(titlenum>0)
				PostMessage(WM_COMMAND,IDC_BASEF10ID+m_whichfl);
		}
		break;
	case NEW_JBMTXT_ASSISID:
		{
			int len = 0;
			if(m_bDS)
				len = g_pGetData->DSGetNewJbmTextACK(FileBuffer);
			else
				len = g_pGetData->GetNewJbmTextACK(FileBuffer);
			if(len>0)
			{
				for(int k = 0;k < len;k++) //将'\0'变为空格
					if(FileBuffer[k] == 0) FileBuffer[k] = 0x20;
				FileBuffer[len] = 0;
				if(strlen(g_AdvertInfo.F10AdvertStr1)>0)
				{
					strcat(FileBuffer,"\r\n─────────────────────────────────────\r\n");
					strcat(FileBuffer,_F(g_AdvertInfo.F10AdvertStr1));
					if(strlen(g_AdvertInfo.F10AdvertStr2)>0)
					{
						strcat(FileBuffer,"\r\n");
						strcat(FileBuffer,_F(g_AdvertInfo.F10AdvertStr2));
					}
				}
				strcat(FileBuffer,_F("\r\n\r\n提示:空格键到下一分类,退格键到上一分类,Ctrl+PageUp/PageDown到上/下股票"));
				m_RichEdit.SetWindowText (FileBuffer);
			}
			else
				m_RichEdit.SetWindowText (_F("尚未有相关资料!"));
			//
			ProcessFind();
			m_RichEdit.SetSel(0,0);
		}
		break;
	}
}

void CTxtView::OnLButtonDown(UINT nFlags, CPoint point) 
{
	if(!(m_bLayoutFlag && OnLine && g_bHasF10Cfg && CurrentJBM >= 0))
	{
		CView::OnLButtonDown(nFlags, point);
		return;
	}
	if(m_PreNext_Rect[0].PtInRect(point) && m_PreNowNext_Index[0]>=0)
	{
		TxtCurGPCode = (*g_pStockIO)[m_PreNowNext_Index[0]]->Code;
		TxtCurSetCode = (*g_pStockIO)[m_PreNowNext_Index[0]]->setcode;
		SendMessage(WM_RICHEDIT_MESSAGE);
	}
	else if(m_PreNext_Rect[1].PtInRect(point) && m_PreNowNext_Index[2]>=0)
	{
		TxtCurGPCode = (*g_pStockIO)[m_PreNowNext_Index[2]]->Code;
		TxtCurSetCode = (*g_pStockIO)[m_PreNowNext_Index[2]]->setcode;
		SendMessage(WM_RICHEDIT_MESSAGE);
	}
	else if(m_PreNext_Rect[2].PtInRect(point))
		AfxGetMainWnd()->SendMessage(WM_COMMAND,ID_JBM_MENU1);
	else if(g_TdxCfg.JbmNum>1 && m_PreNext_Rect[3].PtInRect(point))
		AfxGetMainWnd()->SendMessage(WM_COMMAND,ID_JBM_MENU1+1);
	else if(g_TdxCfg.JbmNum>2 && m_PreNext_Rect[4].PtInRect(point))
		AfxGetMainWnd()->SendMessage(WM_COMMAND,ID_JBM_MENU1+2);
	
	CView::OnLButtonDown(nFlags, point);
}

void CTxtView::OnCancelMark()
{
	int nCurGpIndex = GetCurrentGPIndex();
	if(nCurGpIndex<0) return;
	MemStkInfo *tempStkInfo = (*g_pStockIO)[nCurGpIndex];
	if(!tempStkInfo) return;
	MarkStockFlag(tempStkInfo, 0);
}

#include "TipMarkDlg.h"
void CTxtView::OnMarkStockFlag(UINT nID)
{
	int nFlag = nID - ID_MARK_ADD1+1;
	if(nFlag<=0||nFlag>10) return;
	int nCurGpIndex = GetCurrentGPIndex();
	if(nCurGpIndex<0) return;
	MemStkInfo *tempStkInfo = (*g_pStockIO)[nCurGpIndex];
	if(!tempStkInfo) return;
	if(nFlag==MAX_GROUPMARK)
	{
		CTipMarkDlg dlg;
		dlg.m_pStock = tempStkInfo;
		if(dlg.DoModal()==IDOK)
			MarkStockTipFlag(tempStkInfo, dlg.m_strTip);
	}
	else MarkStockFlag(tempStkInfo, (char)nFlag);
	Invalidate(FALSE);
}

void CTxtView::OnUpdateMarkStockFlag(CCmdUI* pCmdUI)
{
	char nFlag = pCmdUI->m_nID - ID_MARK_ADD1+1;
	if(nFlag<=0||nFlag>10) return;
	int nCurGpIndex = GetCurrentGPIndex();
	if(nCurGpIndex<0) return;
	MemStkInfo *tempStkInfo = (*g_pStockIO)[nCurGpIndex];
	if(!tempStkInfo) return;
	pCmdUI->SetCheck(nFlag==tempStkInfo->fl_flag);
}

void ProcessRichEditExLink(CRichEditCtrl *pCtrl,NMHDR*  in_pNotifyHeader,LRESULT* out_pResult)
{
	ENLINK* l_pENLink = ( ENLINK* )in_pNotifyHeader ;
	*out_pResult = 0 ;
	switch( l_pENLink->msg )
	{
		case WM_SETCURSOR:
			::SetCursor( m_hCanPress ) ;
			*out_pResult = 1 ;
			break;
		case WM_LBUTTONDOWN:
			{
				CHARRANGE l_CharRange ;
				pCtrl->GetSel( l_CharRange ) ;
				pCtrl->SetSel( l_pENLink->chrg ) ;
				CString l_URL = pCtrl->GetSelText() ;
				pCtrl->SetSel( l_CharRange ) ;

				CWaitCursor l_WaitCursor ;
				ShellExecute( NULL, _T( "open" ), l_URL, NULL, NULL, SW_SHOWNORMAL ) ;
				*out_pResult = 1 ;
			}
			break;
		case WM_LBUTTONUP:
			*out_pResult = 1 ;
			break ;
	}
}

void CTxtView::OnRichEditExLink(NMHDR*  in_pNotifyHeader, LRESULT* out_pResult)
{
	ProcessRichEditExLink(&m_RichEdit,in_pNotifyHeader,out_pResult);
}
