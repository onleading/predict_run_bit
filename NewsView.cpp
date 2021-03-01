// NewsView.cpp : implementation of the CNewsView class
#include "stdafx.h"
#include "TdxW.h"
#include "NewsView.h"
#include "TxtView.h"
#include "ProgressWnd.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CNewsView

IMPLEMENT_DYNCREATE(CNewsView, CView)

BEGIN_MESSAGE_MAP(CNewsView, CView)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_COMMAND_RANGE(ID_RIGHT_ALLSELECT,ID_RIGHT_ALLWINDOW,OnRightClick)
	ON_UPDATE_COMMAND_UI(ID_RIGHT_COPY,OnUpdateCopy)
	ON_UPDATE_COMMAND_UI(ID_RIGHT_ADDNOTE,OnUpdateAddNote) 
	ON_UPDATE_COMMAND_UI(ID_RIGHT_ALLWINDOW,OnUpdateAllWindow)
	ON_MESSAGE(WM_KEYGUY_MESSAGE,OnKeyGuyMessage)
	ON_MESSAGE(UM_GETDATA_ACK,OnGetDataAck)
	ON_WM_ERASEBKGND()
	//{{AFX_MSG_MAP(CNewsView)
	ON_NOTIFY(LVN_ITEMCHANGED, NEWS_TITLE_LIST, OnItemchangedTitleList)
	ON_NOTIFY(LVN_ITEMCHANGED, NEWS_TYPE_LIST,  OnItemchangedTypeList)
	ON_WM_DESTROY()
	ON_COMMAND(ID_TXTFIND, OnTxtfind)
	ON_COMMAND(ID_FINDRESULT, OnFindresult)
	ON_NOTIFY( EN_LINK, NEWS_RICHEDIT, OnRichEditExLink )
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

CNewsView::CNewsView()
{
	KeyGuyFlag = FALSE;
	m_bQuitting = FALSE;

	m_nXXGType = -1;
	m_nXXGItemNum = 0;
	memset(m_XXGTitleStr, 0, 256*100);
	
	//远程使用
	bRemoteFileOpen = FALSE;
	m_nTitleCount	= 0;
	nCurRemotePos	= 0;
	m_nMainID		= 0;
}

CNewsView::~CNewsView()
{
	TDEL(m_pFont);
	theViewManager.RemoveView(this);
}

BOOL CNewsView::PreCreateWindow(CREATESTRUCT& cs)
{
	return CView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CNewsView drawing

void CNewsView::OnDraw(CDC* pDC)
{
	if(!m_bFullWindow)
	{
		CPen thePen; //画间隔线
		thePen.CreatePen(PS_SOLID,2,RGB(192,192,192));
		CPen *pOldPen = pDC->SelectObject(&thePen);
		pDC->MoveTo(151,0);
		pDC->LineTo(151,138);
		pDC->MoveTo(0,139);
		pDC->LineTo(1280*2,139);
		pDC->SelectObject(pOldPen);
		thePen.DeleteObject();
	}
}

/////////////////////////////////////////////////////////////////////////////
// CNewsView printing

BOOL CNewsView::OnPreparePrinting(CPrintInfo* pInfo)
{
	return DoPreparePrinting(pInfo);
}

void CNewsView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
}

void CNewsView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
}

#ifdef _DEBUG
void CNewsView::AssertValid() const
{
	CView::AssertValid();
}

void CNewsView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CNewsView message handlers

void CNewsView::OnDestroy() 
{
	m_bQuitting = TRUE;
	CView::OnDestroy();	
}

void CNewsView::OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView) 
{
	theViewManager.OnActivateView(bActivate, this);
	g_bSepEsc = FALSE;
	CView::OnActivateView(bActivate, pActivateView, pDeactiveView);
	theViewManager.pOldActiveView=NULL;
	if(!m_bQuitting && bActivate)
	{
		AfxGetMainWnd()->SendMessage(UM_HIDESHOWBUTTON,HideShowBtn_Xxm);
	}
}

void CNewsView::OnInitialUpdate()
{
	CView::OnInitialUpdate();
	Modify_CorpWebMenu();
		
	GetParent()->SetWindowText (_F("公告消息"));
	AfxGetMainWnd()->SetWindowText (_F("公告消息"));
	theViewManager.AddView("公告消息", this);

	for(int i = 0;i < XXMNum;i++)
	{
		CString tempStr = XXMTopic[i].Left(18);
		tempStr.TrimRight();
		m_TypeList.InsertItem(i,_F(tempStr),0);
	}
//	m_TypeList.InsertItem(i,_F("信息港资讯"),0);
//	m_nXXGType=i;
//	i++;
//	m_TypeList.InsertItem(i,_F("检索结果"),0);	
	CurrentType = CurrentTitle = -1;
}

int CNewsView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	m_RichEdit.Create(WS_VISIBLE|WS_CHILD|ES_AUTOVSCROLL|ES_READONLY|WS_VSCROLL|WS_TABSTOP|ES_MULTILINE,CRect(0,0,0,0),this,NEWS_RICHEDIT);
	m_RichEdit.SetMenuIndex(6);
	m_RichEdit.ApplyTxtSize ();
	m_RichEdit.GetDefaultCharFormat(cfm);
	PARAFORMAT pf;
	m_RichEdit.GetParaFormat(pf);
	pf.cTabCount = 4;
	m_RichEdit.SetParaFormat(pf);

	unsigned mask = ::SendMessage(m_RichEdit.m_hWnd, EM_GETEVENTMASK, 0, 0);
	::SendMessage(m_RichEdit.m_hWnd, EM_SETEVENTMASK, 0, mask | ENM_LINK);
	::SendMessage(m_RichEdit.m_hWnd, EM_AUTOURLDETECT, true, 0);  //自动检测URL

	//股票列表框
	m_ImageList.Create(16,16,ILC_COLOR32|ILC_MASK,1,1);
	HICON hIcon = ::LoadIcon(AfxGetResourceHandle(),MAKEINTRESOURCE(IDI_FOLDER));
	m_ImageList.Add(hIcon);
	hIcon = ::LoadIcon(AfxGetResourceHandle(),MAKEINTRESOURCE(IDI_ZX2));
	m_ImageList.Add(hIcon);

	m_TypeList.Create(WS_VISIBLE|WS_CHILD|LVS_REPORT|LVS_NOCOLUMNHEADER| LVS_SINGLESEL | /*LVS_SHOWSELALWAYS |*/ WS_TABSTOP ,CRect(0,0,150,138),this,NEWS_TYPE_LIST);
	m_TitleList.Create(WS_VISIBLE|WS_CHILD|LVS_REPORT|LVS_NOCOLUMNHEADER| LVS_SINGLESEL | /*LVS_SHOWSELALWAYS | */WS_TABSTOP,CRect(0,0,0,0),this,NEWS_TITLE_LIST);
	m_TypeList.SetBkColor(TxtColor.TxtBackColor);
	m_TypeList.SetTextBkColor (TxtColor.TxtBackColor);
	m_TypeList.SetTextColor(TreeColor.TreeForeColor);
	m_TypeList.SetImageList(&m_ImageList,LVSIL_SMALL);

	m_TitleList.SetBkColor(TxtColor.TxtBackColor);
	m_TitleList.SetTextBkColor (TxtColor.TxtBackColor);
	m_TitleList.SetTextColor(TxtColor.TxtForeColor);
	m_TitleList.SetExtendedStyle(LVS_EX_FULLROWSELECT);
	m_TitleList.SetImageList(&m_ImageList,LVSIL_SMALL);

	m_pFont = new CFont;
	m_pFont->CreateFont(g_nFontSize[2],0,0,0,g_nFontWeigth[2],0,0,0,(g_b2000XPSys&&stricmp(g_strFontName[2],"System")==0)?ANSI_CHARSET:DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,g_b2000XPSys?ANTIALIASED_QUALITY:DEFAULT_QUALITY,DEFAULT_PITCH|FF_DONTCARE,g_strFontName[2]);
	m_TypeList.SetFont(m_pFont);
	m_TitleList.SetFont(m_pFont);

	LV_COLUMN lvColumn;
	lvColumn.mask = LVCF_WIDTH|LVCF_TEXT|LVCF_FMT|LVCF_SUBITEM;
	lvColumn.fmt = LVCFMT_LEFT;
	lvColumn.cx = 150;  //宽度可调,主要防止出现水平滚动条
	lvColumn.iSubItem = 0;
	lvColumn.pszText  = "F0514M0824";
	m_TypeList.InsertColumn (0,&lvColumn);
	lvColumn.cx = 620;//450;  //宽度可调,主要防止出现水平滚动条
	m_TitleList.InsertColumn (0,&lvColumn);

	m_bFullWindow = FALSE;
	return 0;
}

void CNewsView::OnSize(UINT nType, int cx, int cy) 
{
	CView::OnSize(nType, cx, cy);

	if(m_bFullWindow == FALSE)
		m_RichEdit.SetWindowPos(NULL,16,145,cx-16,cy-145,SWP_NOZORDER);
	else
	{
		CRect rect;
		GetClientRect(&rect);
		m_RichEdit.SetWindowPos(NULL,rect.left+16,rect.top,rect.Width()-16,rect.Height(),SWP_NOZORDER);
	}

	m_TitleList.SetWindowPos(NULL,152,0,cx-152,138,SWP_NOZORDER);
	m_TitleList.DeleteColumn(0);
	LV_COLUMN lvColumn;
	lvColumn.mask = LVCF_WIDTH|LVCF_TEXT|LVCF_FMT|LVCF_SUBITEM;
	lvColumn.fmt = LVCFMT_LEFT;
	lvColumn.iSubItem = 0;
	lvColumn.pszText  = "F0514M0824";
	lvColumn.cx = cx-152-28;  //宽度可调,主要防止出现水平滚动条
	m_TitleList.InsertColumn (0,&lvColumn);
}

void CNewsView::OnTypeChange()
{
	//先删除标题列表
	m_TitleList.SetRedraw(FALSE);
	m_TitleList.DeleteAllItems(); //将标题列表清空
	m_TitleList.SetRedraw(TRUE);
	if(CurrentType >= 0)
	{
		MainIDPlus(m_nMainID,NEWSVIEW_WIN); //即使是本地，也应将MainID变一下，否则ACK会出问题
		if(CurrentType == XXMNum+1) //检索结果
		{
			TitleNum = 0;
			char  tmpLine[255];
			CFile theFile;
			struct tag_FindResult2 findresult2;
			if(theFile.Open(g_WSXHStr+"tmp\\newsfind.dat",CFile::modeRead|CFile::shareDenyNone))
			{
				if(theFile.Read(tmpLine,255) == 255) //正确的头
				{
					while(theFile.Read(&findresult2,sizeof(tag_FindResult2))==sizeof(tag_FindResult2))
					{
						findresult2.whichtype = 0;
						sprintf(tmpLine,"本地缓存: %s",findresult2.title);
						m_FindResult2[TitleNum] = findresult2;
						m_TitleList.InsertItem(TitleNum,_F(tmpLine),1);
						TitleNum++;
						if(TitleNum >= min(500,MAX_FINDRESULT)) break;
					}
				}
				theFile.Close();
			}
			m_nFindNum = TitleNum;
		}
		else if(CurrentType == m_nXXGType)
			GetXXGTitle();
		else	//远程消息面
			GetRemoteTitle(CurrentType);
	}
}

void CNewsView::ShowOneFile(int whichtype,const char *filename,int which)
{
	CFile theFile;
	if( theFile.Open(filename,CFile::modeRead|CFile::shareDenyNone) )
	{
		DWORD len = theFile.GetLength();
		//为buffer申请空间
		char *buffer = new char[len+1];
		theFile.Read(buffer,len);
		for(int k = 0;k < len;k++) //将'\0'变为空格
			if(buffer[k] == 0) buffer[k] = 0x20;
		buffer[len] = 0;
		m_RichEdit.SetRedraw(FALSE);
		m_RichEdit.SetWindowText ((CString)"\r\n"+buffer);
		ProcessFind();
		m_RichEdit.SetRedraw(TRUE);
		m_RichEdit.Invalidate(FALSE);
		delete buffer; //释放空间
		theFile.Close();
	}
	else
		m_RichEdit.SetWindowText (_F("该报道仅为标题新闻!"));
}

void CNewsView::OnTitleChange()
{
	if(CurrentType < 0 || CurrentTitle < 0) return;
	if(CurrentType==XXMNum+1)	//检索结果
		ShowOneFile(m_FindResult2[CurrentTitle].whichtype,m_FindResult2[CurrentTitle].title,m_FindResult2[CurrentTitle].whichindex);
	else if(CurrentType == m_nXXGType)
	{
		int nxh = m_TitleList.GetItemData(CurrentTitle);
		if(nxh>=0&&nxh<m_nXXGItemNum)
		{
			char reqTitle[256]={0}, fullTitle[256]={0};
			strncpy(reqTitle, &m_XXGTitleStr[nxh*256], 255);
			strncpy(fullTitle, &m_XXGTitleStr[nxh*256], 255);
			char *argv[10];
			short argc = GetCmdLine(reqTitle, argv, 10, "|");
			if(argc>1) GetXXGTxt(argv[1], fullTitle);
		}
	}
	else								//远程消息
		GetRemoteFile(CurrentTitle);
}

void CNewsView::JustifyPos()
{
	CRect rect;
	GetClientRect(&rect);
	if(m_bFullWindow)
	{
		m_TypeList.ShowWindow(SW_HIDE);
		m_TitleList.ShowWindow(SW_HIDE);
		m_RichEdit.MoveWindow(&rect,TRUE);
		m_RichEdit.SetFocus();
	}
	else
	{ 
		m_RichEdit.SetWindowPos(NULL,16,145,rect.Width()-16,rect.Height()-145,SWP_NOZORDER);
		m_TypeList.ShowWindow(SW_SHOW);
		m_TitleList.ShowWindow(SW_SHOW);
	}
}

void CNewsView::OnRightClick(UINT nID)
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
			if( theFile.Open(g_WSXHStr+"Coolinfo.txt",CFile::modeCreate|CFile::modeWrite|CFile::modeNoTruncate) )
			{
				theFile.SeekToEnd();
				theFile.Write(Separator,tempLen);
				theFile.Write(pWriteBuf,lLen);
				theFile.Close();
			}
			delete pWriteBuf;
			break;
		}
	case ID_RIGHT_ALLWINDOW:
		{
			m_bFullWindow = !m_bFullWindow;
			JustifyPos();
			break;
		}
	}
}

void CNewsView::OnUpdateCopy(CCmdUI* pCmdUI)
{
	long StartChar,EndChar;
	m_RichEdit.GetSel(StartChar,EndChar);
	if(StartChar == EndChar)
		pCmdUI->Enable(FALSE);
	else
		pCmdUI->Enable(TRUE);
}

void CNewsView::OnUpdateAddNote(CCmdUI* pCmdUI)
{
	long StartChar,EndChar;
	m_RichEdit.GetSel(StartChar,EndChar);
	if(StartChar == EndChar)
		pCmdUI->Enable(FALSE);
	else
		pCmdUI->Enable(TRUE);
}


void CNewsView::OnUpdateAllWindow(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_bFullWindow);
}

BOOL CNewsView::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_CHAR && KeyGuyFlag == FALSE )
	{
		if(ProcessKeyGuy(pMsg,this,KeyGuyFlag))
			return TRUE;
		if(pMsg->wParam == VK_RETURN || pMsg->wParam == VK_TAB)
		{
			if(pMsg->wParam == VK_TAB&&FiltTabKeyCom()) return TRUE;

			CWnd * SubWnd = GetFocus();
			CWnd *NextWnd = GetNextDlgTabItem(SubWnd);
			NextWnd->SetFocus();
			if(NextWnd->IsKindOf(RUNTIME_CLASS(CListCtrl)))
			{
				int tmpIndex;
				if(SubWnd->IsKindOf(RUNTIME_CLASS(CRichEditCtrl)))
					tmpIndex = CurrentType;
				else
					tmpIndex = CurrentTitle;
				tmpIndex = max(0,tmpIndex);
				((CListCtrl *)NextWnd)->SetItemState(tmpIndex,LVIS_SELECTED,LVIS_SELECTED);
				((CListCtrl *)NextWnd)->SetItemState(tmpIndex,LVIS_FOCUSED,LVIS_FOCUSED);
			}
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
	//Ctrl+C时不处理控件的默认处理
	if(pMsg->message == WM_KEYDOWN && (pMsg->wParam == 'c' || pMsg->wParam == 'C' ))		
	{
		short sRet=::GetKeyState(VK_CONTROL);
		if(sRet < 0)
		{
			OnRightClick(ID_RIGHT_COPY);
			return TRUE;
		}
	}

	return CView::PreTranslateMessage(pMsg);
}

void CNewsView::OnKeyGuyMessage(WPARAM wParam, LPARAM lParam)
{
	SendMessage(WM_PAINT);
	m_TypeList.SetFocus();
	m_TypeList.SetItemState(wParam-701, LVIS_SELECTED,LVIS_SELECTED);
	m_TypeList.SetItemState(wParam-701, LVIS_FOCUSED,LVIS_FOCUSED);

	if(wParam-701 > XXMNum) return; //如果无此消息面，则返回
	if((wParam-701) != CurrentType)
	{
		CurrentType = wParam - 701;
		OnTypeChange();
	}
}

BOOL CNewsView::OnEraseBkgnd(CDC* pDC) 
{
	CRect rect;
	GetClientRect(&rect);
	pDC->FillSolidRect (&rect,TxtColor.TxtBackColor);
	return TRUE;
}

void CNewsView::OnItemchangedTitleList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	POSITION pos;
	pos = m_TitleList.GetFirstSelectedItemPosition();
	if(pos != NULL)
	{
		int m_index = m_TitleList.GetNextSelectedItem(pos);
		if(CurrentTitle != m_index)
		{
			CurrentTitle = m_index;
			OnTitleChange();
		}
	}
	*pResult = 0;
}

void CNewsView::OnItemchangedTypeList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	POSITION pos;
	pos = m_TypeList.GetFirstSelectedItemPosition();
	if(pos != NULL)
	{
		int m_index = m_TypeList.GetNextSelectedItem(pos);
		if(CurrentType != m_index)
		{
			CurrentType = m_index;
			OnTypeChange();
		}
	}	
	*pResult = 0;
}

void CNewsView::SetFindFlag(int pos,int len,COLORREF clr)
{
	cfm.cbSize=sizeof(cfm);
	cfm.crTextColor=clr;
	cfm.dwMask=CFM_COLOR;

	m_RichEdit.SetSel(pos,len+pos);
	m_RichEdit.SetSelectionCharFormat(cfm);
	m_RichEdit.SetSel(len+pos,len+pos);
}

void CNewsView::ProcessFind()
{
	if(g_nNewsFindNum < 1) return;
	int TotalLen = m_RichEdit.GetWindowTextLength();
	char *p,*tmpStr = new char[TotalLen+1];
	CString csString;
	m_RichEdit.GetWindowText(csString);
	strncpy(tmpStr,csString,TotalLen);
	tmpStr[TotalLen] = 0;
	int pos,SubLen,firstpos = 0;
	for(int i=0;i < g_nNewsFindNum;i++)
	{
		SubLen = g_NewsFindStr[i].GetLength();
		pos = 0;
		while(p = strstr(tmpStr+pos,g_NewsFindStr[i]))
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
void CNewsView::OnTxtfind()
{
	CTxtFindDlg	dlg;
	dlg.m_bEnable1 = TRUE;
	dlg.m_bEnable2 = TRUE;
	dlg.m_nRadio = g_nFindRadio;
	dlg.m_Str1 = "当前文本消息";
	dlg.m_Str2 = "所有类型的本地消息";
	if(dlg.DoModal() == IDOK)
	{
		g_nNewsFindNum = dlg.m_nFindNum;
		for(int i=0;i<g_nNewsFindNum;i++)
			g_NewsFindStr[i] = dlg.m_FindStr[i];
		int  nFileCount = 0;
		if(dlg.m_nRadio == 1)   //暂时仅支持本地资料文件
		{
			CFile theFile;
			if(!theFile.Open(g_WSXHStr+"tmp\\NewsFind.dat",CFile::modeCreate|CFile::modeWrite|CFile::shareDenyNone))
				return;
			//设定模态进度条对话框
			CProgressWnd wndProgress(this,_F("正在检索，请稍等..."),TRUE);
			wndProgress.GoModal();
			wndProgress.SetRange(0,500);
			wndProgress.SetStep(1);
			g_bDonotClose = TRUE;

			long handle;
			int  done;
			char string[_MAX_PATH],filename[_MAX_PATH];
			//写文件头
			strcpy(filename,dlg.m_strFindTxt);
			theFile.Write(filename,255);
			struct tag_FindResult2 findresult;
			struct _finddata_t ffblk;
			CString tmpStr;
			CTime theTime = CTime::GetCurrentTime();	//得到当前时间
			for(int n=0;n<XXMNum;n++)
			{
				tmpStr = g_WSXHStr+"LData\\cache\\";
				sprintf(string,"%s*.*",tmpStr);
				handle = _findfirst(string,&ffblk);
				if (handle==-1) done=-1;
				else            done=0;
				if(done) continue;
				while (!done)
				{
					//处理暂停事件
					sprintf(filename,"检索:%s %s \n共找到%d个文件",XXMTopic[n],ffblk.name,nFileCount);
					wndProgress.SetText(filename);
					wndProgress.StepIt();
					wndProgress.PeekAndPump();
					if(wndProgress.Cancelled())
						goto aftercancel;
					if (ffblk.size != 0 && ffblk.name[0] != '.' && !(ffblk.attrib&_A_SUBDIR))
					{
						sprintf(filename,"%s%s",tmpStr,ffblk.name);
						if(TestFound(filename,1))
						{
							findresult.whichtype = n;
							strcpy(findresult.title,filename);
							findresult.whichindex = 0;
							theFile.Write(&findresult,sizeof(tag_FindResult2));
							nFileCount++;
						}
					}
					done = _findnext(handle,&ffblk);
				}
				if(handle != -1)
					_findclose(handle);
			}
		aftercancel:;
			g_bDonotClose = FALSE;
			theFile.Close();
		}
		else
			unlink(g_WSXHStr+"tmp\\NewsFind.dat");	//删除检索结果
		CString TextStr;
		m_RichEdit.GetWindowText(TextStr);
		m_RichEdit.SetWindowText(TextStr);
		ProcessFind();	//总是处理当前文本
		g_nNewsFindNo = -1;
		if(nFileCount > 0) OnFindresult();
	}
}

#include "FindResultDlg.h"
void CNewsView::OnFindresult() 
{
	CFindResultDlg dlg;
	dlg.m_nStyle = 1;
	if(dlg.DoModal() == IDOK)
	{
		ShowOneFile(dlg.m_FindResult2[dlg.m_nCurSel].whichtype,dlg.m_FindResult2[dlg.m_nCurSel].title,dlg.m_FindResult2[dlg.m_nCurSel].whichindex);
		//移到相应的项目上去
		CurrentType = XXMNum+1; 
		OnTypeChange();
		CurrentTitle = dlg.m_nCurSel;
		OnTitleChange();
		m_RichEdit.SetFocus();
	}
}

//////////////////////////////////////////////////////////
//				远程数据请求
//////////////////////////////////////////////////////////

void CNewsView::GetRemoteTitle(int Which)
{
	if(XXMType[Which] == TEXTXX)	//如果是文本文件
		GetRemoteFile(0);
	else							//如果是info_cache
	{
		TitleNum	 = 0;
		CurrentTitle = -1;
		m_nTitleCount = 0;

		if(!OnLine)
		{
			CTime theTime = CTime::GetCurrentTime();
			char tempFileName[256];
			sprintf( tempFileName,"%sLData\\cache\\%02u.lst",g_WSXHStr,Which+1);
			CStdioFile theFile;
			if( theFile.Open(tempFileName,CFile::modeRead|CFile::shareDenyNone) )
			{
				CString tempLine;
				while(theFile.ReadString(tempLine))
				{
					TrimString(tempLine);
					if( tempLine.IsEmpty() ) 
						continue;
					if( tempLine.GetLength() < 8)  continue;
					m_TitleList.InsertItem(TitleNum,_F(tempLine.Mid(8)),1);
					TitleNum++;
					if(TitleNum >= 256) //远程只允许256条Title
						break;
				}
				theFile.Close();
			}
			m_TitleList.SetItemState(0,LVIS_SELECTED,LVIS_SELECTED);
			m_TitleList.SetItemState(0,LVIS_FOCUSED,LVIS_FOCUSED);
			return;
		}
		g_nMainID = MainIDPlus(m_nMainID,NEWSVIEW_WIN);
		g_nAssisID = XXMTITLE_ASSISID;
		g_pGetData->GetXxmItem(Which,0);
	}
}

void CNewsView::GetRemoteFile(int CurTitleNo)
{
	m_RichEdit.SetWindowText("");

	if(bRemoteFileOpen)
		theRemoteFile.Close();
	bRemoteFileOpen = FALSE;

	nCurRemotePos	= 0;
	m_nCurTitleNo = CurTitleNo;

	if(!OnLine)
	{
		CString tempFileName;
		CTime theTime = CTime::GetCurrentTime ();
		tempFileName.Format("%sLData\\cache\\%02d%02d.txt",g_WSXHStr,CurrentType+1,m_nCurTitleNo+1);
		CFile theFile;
		if( theFile.Open(tempFileName,CFile::modeRead|CFile::shareDenyNone))
		{
			DWORD len = theFile.GetLength();	//得到本地消息文件的长度
			//为buffer申请空间
			char *buffer = new char[len+1]; 
			theFile.Read(buffer,len);
			for(int k = 0;k < len;k++) //将'\0'变为空格
				if(buffer[k] == 0) buffer[k] = 0x20;
			buffer[len] = 0;
			m_RichEdit.SetRedraw(FALSE);
			m_RichEdit.SetWindowText ((CString)"\r\n"+buffer);
			ProcessFind();
			m_RichEdit.SetRedraw(TRUE);
			m_RichEdit.Invalidate(FALSE);
			delete buffer;
			theFile.Close();
		}
		return;
	}
	g_nMainID = MainIDPlus(m_nMainID,NEWSVIEW_WIN);
	g_nAssisID = XXMLEN_ASSISID;
	g_pGetData->GetXxmFileLen(CurrentType,CurTitleNo);
}

void	CNewsView::GetXXGTitle()
{
	if(OnLine)
	{
		g_nMainID = MainIDPlus(m_nMainID,NEWSVIEW_WIN);
		g_nAssisID = XXGTITLE_ASSISID;
		g_pGetData->GetPathFile("infoharbor.lst", 0);
	}
}

void	CNewsView::GetXXGTxt(char *title, char *fullTitle)
{
	std::map<CString, CString, StringComp>::iterator iter = m_mXxgTxt.find(fullTitle);
	if(iter!=m_mXxgTxt.end())
	{
		CString strContent = (*iter).second;
		m_RichEdit.SetRedraw(FALSE);
		m_RichEdit.SetWindowText("");
		m_RichEdit.SetWindowText(strContent);
		ProcessFind();
		m_RichEdit.SetRedraw(TRUE);
		m_RichEdit.Invalidate(FALSE);
		m_RichEdit.SetSel(0,0);
		return;
	}
	if(OnLine)
	{
		g_nMainID = MainIDPlus(m_nMainID,NEWSVIEW_WIN);
		g_nAssisID = XXGTXT_ASSISID;
		g_pGetData->GetPathFile(title, 0);
	}
}

void CNewsView::OnGetDataAck(WPARAM wParam,LPARAM lParam)
{
	if(m_nMainID != g_AnsHeader.MainID)	return;
	switch(g_AnsHeader.AssisID)
	{
	case XXMTITLE_ASSISID:
		{
			CTime theTime = CTime::GetCurrentTime();
			int TotalTitleNum,ReadNum;
			TotalTitleNum = g_pGetData->GetXxmItemACK(m_TitleStr+m_nTitleCount*18*74);
			m_nTitleCount++;
			ReadNum = (m_nTitleCount < (TotalTitleNum+17)/18)?18:TotalTitleNum-(m_nTitleCount-1)*18;
			for(int i=0;i < ReadNum;i++)
			{
				m_TitleList.InsertItem(TitleNum,_F(m_TitleStr+TitleNum*74),1);
				TitleNum++;
				if(TitleNum >= 256) break;
			}
			if( m_nTitleCount < (TotalTitleNum+17)/18 && TitleNum < 256)
			{
				g_nMainID = MainIDPlus(m_nMainID,NEWSVIEW_WIN);
				g_nAssisID = XXMTITLE_ASSISID;
				g_pGetData->GetXxmItem(CurrentType,18*m_nTitleCount);
			}
			else
			{
				m_TitleList.SetItemState(0,LVIS_SELECTED,LVIS_SELECTED);
				m_TitleList.SetItemState(0,LVIS_FOCUSED,LVIS_FOCUSED);

				CString tempFileName,tempStr;
				tempFileName.Format("%sLData\\cache\\%02d.lst",g_WSXHStr,CurrentType+1);
				CStdioFile theFile;
				if( theFile.Open(tempFileName,CFile::modeCreate|CFile::modeWrite|CFile::shareDenyNone) )
				{
					for(i = 0;i < TitleNum;i++)
					{
						m_TitleStr[i*74+73] = '\0';
						tempStr.Format("%02d%02d.txt%s\n\n",CurrentType+1,i+1, m_TitleStr+i*74 );
						//将消息标题写盘，并将其填充标题列表
						theFile.WriteString(tempStr); 
					}
					theFile.Close();
				}
				else
					TDX_MessageBox(m_hWnd,"不能将远程公告新闻存盘!",MB_OK|MB_ICONEXCLAMATION);
			}
		}
		break;
	case XXMLEN_ASSISID:
		{
			DWORD RemoteFileLen = g_pGetData->GetFileLenACK();
			if(RemoteFileLen <= 0 || RemoteFileLen > 10*1024*1024)	break; //如果长度为0或大于10M，不请求之
			BOOL bIsSame = FALSE;
			CString tempFileName;
			CTime theTime = CTime::GetCurrentTime ();
			tempFileName.Format("%sLData\\cache\\%02d%02d.txt",g_WSXHStr,CurrentType+1,m_nCurTitleNo+1);
			CFile theFile;
			if( theFile.Open(tempFileName,CFile::modeRead|CFile::shareDenyNone))
			{
				DWORD len = theFile.GetLength();	//得到本地消息文件的长度
				if(RemoteFileLen == len)
				{
					bIsSame = TRUE;//如果长度相同，则直接从本地读取
					//为buffer申请空间
					char *buffer = new char[len+1]; 
					theFile.Read(buffer,len);
					for(int k = 0;k < len;k++) //将'\0'变为空格
						if(buffer[k] == 0) buffer[k] = 0x20;
					buffer[len] = 0;
					m_RichEdit.SetRedraw(FALSE);
					m_RichEdit.SetWindowText ((CString)"\r\n"+buffer);
					ProcessFind();
					m_RichEdit.SetRedraw(TRUE);
					m_RichEdit.Invalidate(FALSE);
					delete buffer;
				}
				theFile.Close();
			}
			if(!bIsSame)
			{
				//先写盘，再从文件中读取
				if( theRemoteFile.Open(tempFileName,CFile::modeCreate|CFile::modeReadWrite|CFile::shareDenyNone) )
				{
					bRemoteFileOpen = TRUE;

					g_nMainID = MainIDPlus(m_nMainID,NEWSVIEW_WIN);
					g_nAssisID = XXM_ASSISID;
					//抛砖引玉
					g_pGetData->GetText(CurrentType,m_nCurTitleNo,nCurRemotePos);
				}
				else	
					TDX_MessageBox(m_hWnd,"不能将远程公告新闻存盘!",MB_OK|MB_ICONEXCLAMATION);
			}
		}
		break;
	case XXM_ASSISID:
		{
			char	FileBuffer[2100];
			int	len = g_pGetData->GetTextACK(FileBuffer);
			theRemoteFile.Write(FileBuffer,len);
			for(int k = 0;k < len;k++) //将'\0'变为空格
				if(FileBuffer[k] == 0) FileBuffer[k] = 0x20;
			FileBuffer[len] = 0;
			CString tmpStr;
			m_RichEdit.GetWindowText(tmpStr);
			tmpStr+=FileBuffer;
			m_RichEdit.SetRedraw(FALSE);
			m_RichEdit.SetWindowText(tmpStr);
			ProcessFind();
			m_RichEdit.SetRedraw(TRUE);
			m_RichEdit.Invalidate(FALSE);
			nCurRemotePos += len;
			if(len == 2048)
			{
				g_nMainID = MainIDPlus(m_nMainID,NEWSVIEW_WIN);
				g_nAssisID = XXM_ASSISID;
				g_pGetData->GetText(CurrentType,m_nCurTitleNo,nCurRemotePos);
			}
			else
			{
				m_RichEdit.SetSel(0,0);
				theRemoteFile.Close();
				bRemoteFileOpen = FALSE;
			}
		}
		break;
	case XXGTITLE_ASSISID:
		{
			char strRecTitle[30000]={0};
			short 	TotalTitleSize = g_pGetData->GetPathFileACK(strRecTitle);
			if(TotalTitleSize>0)
			{
				m_TitleList.DeleteAllItems();
				char *argv[100]; m_nXXGItemNum = 0;
				short argc0 = GetCmdLine(strRecTitle, argv, 100,"\r");
				for(int k=0;k<argc0;k++)
				{
					strncpy(&m_XXGTitleStr[k*256], argv[k], 256);
					char *argv1[10];
					short argc = GetCmdLine(argv[k], argv1, 10, "|");
					if(argc>1) 
					{
						m_TitleList.InsertItem(m_nXXGItemNum,_F(argv1[0]),1);
						m_TitleList.SetItemData(m_nXXGItemNum, k);
						++m_nXXGItemNum;
					}
				}
				m_TitleList.SetItemState(0,LVIS_SELECTED,LVIS_SELECTED);
				m_TitleList.SetItemState(0,LVIS_FOCUSED,LVIS_FOCUSED);
			}
		}
		break;
	case XXGTXT_ASSISID:
		{
			char	FileBuffer[32*1024+200]={0};
			int	len = g_pGetData->GetPathFileACK(FileBuffer);
			m_RichEdit.SetRedraw(FALSE);
			if(len>0) m_RichEdit.SetWindowText("");
			for(int k = 0;k < len;k++) //将'\0'变为空格
				if(FileBuffer[k] == 0) FileBuffer[k] = 0x20;
			FileBuffer[len] = 0;
			_FL(FileBuffer);
			m_RichEdit.SetWindowText(FileBuffer);
			ProcessFind();
			m_RichEdit.SetRedraw(TRUE);
			m_RichEdit.Invalidate(FALSE);
			m_RichEdit.SetSel(0,0);
			//缓存
			POSITION pos = m_TitleList.GetFirstSelectedItemPosition();
			if(pos != NULL)
				CurrentTitle = m_TitleList.GetNextSelectedItem(pos);
			int nxh = m_TitleList.GetItemData(CurrentTitle);
			if(nxh>=0&&nxh<m_nXXGItemNum)
			{
				char reqTitle[256]={0};
				strncpy(reqTitle, &m_XXGTitleStr[nxh*256], 255);
				m_mXxgTxt[(CString)reqTitle] = (CString)FileBuffer;
			}
		}
		break;
	default:
		break;
	}
}

extern void ProcessRichEditExLink(CRichEditCtrl *pCtrl,NMHDR*  in_pNotifyHeader,LRESULT* out_pResult);
void CNewsView::OnRichEditExLink(NMHDR*  in_pNotifyHeader, LRESULT* out_pResult)
{
	ProcessRichEditExLink(&m_RichEdit,in_pNotifyHeader,out_pResult);
}
