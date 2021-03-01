// UF10.cpp: implementation of the UF10 class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "tdxw.h"
#include "UF10.h"
#include "UF10Ctrl.h"
#include "UF10CtrlWnd.h"

#define MAX_TXT_SIZE	260*1000

UF10::UF10(LPCOMINFO pComInfo, UF10Ctrl *pF10Ctrl)
{
	m_pUF10Ctrl = pF10Ctrl;
	m_pUF10CtrlWnd = NULL;
	memcpy(&m_ComInfo, pComInfo, sizeof(m_ComInfo));

	CurrentJBM = g_CurrentJBM;
	if(CurrentJBM < 0)
		CurrentJBM = 0;
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

	m_nMostLeft = 0;
	for(int i=0;i<5;i++)
		m_PreNext_Rect[i].SetRect(0, 0, 0, 0);
	m_nRichEditTop=0;
	m_SelColor=VipColor.KPCOLOR;
	m_ForeColor=TreeColor.TreeForeColor;
	m_BackColor=TxtColor.TxtBackColor;

	m_bDS = FALSE;
}

UF10::~UF10()
{
	if(m_pUF10CtrlWnd)
	{
		m_pUF10CtrlWnd->DestroyWindow();
		delete m_pUF10CtrlWnd;
	}

	g_CurrentJBM = CurrentJBM;

	TDEL(m_pFont);
	TDEL(m_pTmpBuf);
}

void	UF10::OnCreate()
{
	m_RichEdit.Create(WS_VISIBLE|WS_CHILD|ES_AUTOVSCROLL|ES_READONLY|WS_VSCROLL|WS_TABSTOP|ES_MULTILINE,CRect(0,0,0,0),m_pUF10CtrlWnd,TXT_RICHEDIT);
	m_RichEdit.SetMenuIndex(-1);
	if(g_OEM.bEmbedWTVersion)
	{
		m_ForeColor=VipColor.TXTCOLOR;
		m_BackColor=VipColor.BACKCOLOR;
		m_RichEdit.m_bUseTxtColor=FALSE;
	}
	//使用从INI中读入的大小	
	m_RichEdit.ApplyTxtSize();
	m_pFont = new CFont;
	m_pFont->CreateFont(g_nFontSize[2],0,0,0,g_nFontWeigth[2],0,0,0,(g_b2000XPSys&&stricmp(g_strFontName[2],"System")==0)?ANSI_CHARSET:DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,g_b2000XPSys?ANTIALIASED_QUALITY:DEFAULT_QUALITY,DEFAULT_PITCH|FF_DONTCARE,g_strFontName[2]);

	//创建动态F10分类按钮
 	for(int i=0;i<MAX_F10CAPTION;i++)
		m_btnF10[i].Create("",WS_CHILD|BS_OWNERDRAW,CRect(0,0,0,0),m_pUF10CtrlWnd,IDC_BASEF10ID+i);
 	for(i=0;i<MAX_F10CAPTION;i++)
	{
		m_btnF10[i].SetTextColor(VipColor.KNCOLOR);
		m_btnF10[i].SetBkColor(VipColor.BACKCOLOR);
		m_btnF10[i].SetHoverColor(VipColor.BACKCOLOR);
		m_btnF10[i].SetFont(m_pFont);
	}
	SetSelColor();
	m_bLayoutFlag = TRUE;

	OnShowTxt();
}

BOOL	UF10::InitShow(HWND hVessel)
{
	m_hVessel = hVessel;
	if(!m_pUF10CtrlWnd||!m_pUF10CtrlWnd->m_hWnd)
	{
		delete m_pUF10CtrlWnd;
		m_pUF10CtrlWnd = new CUF10CtrlWnd(this);
		m_pUF10CtrlWnd->Create(NULL,"hidef10wnd",WS_CHILD|WS_VISIBLE,CRect(0,0,0,0),CWnd::FromHandle(m_hVessel),IDC_HIDENETWND);
		if(!m_pUF10CtrlWnd||!m_pUF10CtrlWnd->m_hWnd) return FALSE;
	}

	return TRUE;
}

void	UF10::ArrangeWnd(CRect &rc)
{
	if(!m_pUF10CtrlWnd) return;

	if(m_pUF10CtrlWnd&&m_pUF10CtrlWnd->m_hWnd)
		m_pUF10CtrlWnd->MoveWindow(&rc, FALSE);
}

void UF10::SetSelColor()
{
	m_SelColor=VipColor.KPCOLOR;
	if(CurrentJBM>0)
		m_SelColor=VipColor.VOLCOLOR;
	else if(CurrentJBM>1)
		m_SelColor=RGB(192,0,192);
 	for(int i=0;i<MAX_F10CAPTION;i++)
		m_btnF10[i].SetSelColor(m_SelColor);
}

//////////////////////////////////////////////////////////////////////////
void UF10::OnShowTxt()
{
	int tempCurrentGPNo = GetSYSCurrentGPIndex();
	if(tempCurrentGPNo >= 0)
	{
		//如果是实时港股，则打开财华(或其它)相应的股票和权证网页
		MemStkInfo * pInfo = (*g_pStockIO)[tempCurrentGPNo];
		if(pInfo)
		{
			if(pInfo->bigmarket==HKGP_TYPE && g_GC.bIsSSDS)
			{
				CString tmpInfoURL=HK_GP_F10_WEBPAGE;
				if(testhkwarrant(pInfo))
					tmpInfoURL=HK_QZ_F10_WEBPAGE;
				OpenCusWebPage(m_pUF10CtrlWnd,tmpInfoURL,FALSE);
				return;
			}
			else
			{
				TxtCurGPCode = pInfo->Code;
				TxtCurSetCode = pInfo->setcode;
			}
		}
	}
	else
	{
		TxtCurGPCode = ComCurGPCode;
		TxtCurSetCode = ComCurSetCode;
	}

	m_pUF10CtrlWnd->SendMessage(WM_RICHEDIT_MESSAGE);
}

//////////////////////////////////////////////////////////////////////////
void UF10::UpdateButton()
{
	for(int i=0;i<MAX_F10CAPTION;i++)
		m_btnF10[i].ShowWindow(SW_HIDE);
 	for(i=0;i<min(g_F10CaptionNum,MAX_F10CAPTION);i++)
	{
	   m_btnF10[i].SetWindowText(g_NewF10Caption[i].sTitle);
	   m_btnF10[i].ShowWindow(SW_SHOW);
	}
}

void UF10::UncheckAllButton()
{
	int i;
	for(i=0;i<MAX_F10CAPTION;i++) //先全部取消选中
	{
		if(m_btnF10[i].IsBtnChecked())
		   m_btnF10[i].CheckButton(FALSE);
	}
}

int UF10::GetCurSelBtn()
{
	int count=min(g_F10CaptionNum,MAX_F10CAPTION);
	for(int i=0;i<count;i++)
	{
		if(m_btnF10[i].IsBtnChecked())
			return i;
	}
	return -1;
}

#define  F10_BTN_HEIGHT 25

void UF10::JustifyButton(CRect rcdlg)
{
	int nBtnWidth=65;
	int k=(rcdlg.Width()-70)/nBtnWidth,left=0;	
	if(k<0) k=0;
	m_nRichEditTop=2*F10_BTN_HEIGHT+10;
	m_RichEdit.SetWindowPos(NULL,16,m_nRichEditTop,rcdlg.Width()+1-16,rcdlg.Height()-m_nRichEditTop,SWP_NOZORDER);
	if(g_F10CaptionNum < k)
	{
		int top = 1,left = min(70,rcdlg.Width()-g_F10CaptionNum*nBtnWidth);
		for(int i=0;i<g_F10CaptionNum;i++)
		{
			m_btnF10[i].SetWindowPos(NULL,left,top,nBtnWidth,F10_BTN_HEIGHT,SWP_SHOWWINDOW);
 			left+=(nBtnWidth-1);
		}		
	}
	else
	{
		int visiblenum = min(g_F10CaptionNum,2*k);
		int singlenum = (visiblenum+1)/2;
		int top = 1;
		left = min(70,rcdlg.Width()-singlenum*nBtnWidth);
		for(int i=0;i<singlenum;i++)
		{
			m_btnF10[i].SetWindowPos(NULL,left,top,nBtnWidth,F10_BTN_HEIGHT,SWP_SHOWWINDOW);
 			left+=(nBtnWidth-1);
		}
		top+=(F10_BTN_HEIGHT-1);
		left = min(70,rcdlg.Width()-singlenum*nBtnWidth);	
		for(i=singlenum;i<visiblenum;i++)
		{
			m_btnF10[i].SetWindowPos(NULL,left,top,nBtnWidth,F10_BTN_HEIGHT,SWP_SHOWWINDOW);
 			left+=(nBtnWidth-1);
		}
		for(;i<g_F10CaptionNum;i++)
			m_btnF10[i].SetWindowPos(NULL,0,0,0,0,SWP_SHOWWINDOW);
		m_nMostLeft = left;
	}
	UpdateButton();
}

short UF10::GetCurrentGPIndex()
{
	return g_pStockIO->GetIndex(m_TxtCurGPCode,m_TxtCurSetCode);
}

void UF10::GetRemoteFile(const char *Code,short setcode)
{
	m_bDS = FALSE;
	if(setcode!=SZ && setcode!=SH) 
		m_bDS = TRUE;
	if(OnLine)
	{
		m_bCanClickFL = FALSE;
		m_ComInfo.pWnd->SendMessage(UM_GETMAINID); //直接让父窗口填写g_nMainID
		m_nMainID = g_nMainID;
		g_nAssisID = SetComAssisID(m_ComInfo.nWhichUnit,NEW_JBMCFG_ASSISID);
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

	if(!OnLine)
		return;
	
	m_bCanClickFL = FALSE;
	m_ComInfo.pWnd->SendMessage(UM_GETMAINID); //直接让父窗口填写g_nMainID
	m_nMainID = g_nMainID;
	g_nAssisID = SetComAssisID(m_ComInfo.nWhichUnit,JBMLEN_ASSISID);
	g_pGetData->GetJbmFileLen(Code,setcode);

	memset(m_pTmpBuf,0,MAX_TXT_SIZE*sizeof(char));
}

//////////////////////////////////////////////////////////////////////////
void UF10::SetStock(MemStkInfo *pStock)
{
	TxtCurGPCode = pStock->Code;
	TxtCurSetCode = pStock->setcode;
	m_pUF10CtrlWnd->SendMessage(WM_RICHEDIT_MESSAGE);
}

//响应"股票资料"的消息
void UF10::OnRichEditMessage()
{
	//重新得到CurrentJBM的值
	if(CurrentJBM!=g_CurrentJBM)
	{
		CurrentJBM = g_CurrentJBM;
		Resize();
	}	
	if(CurrentJBM < 0)
		CurrentJBM = 0;
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
		m_ComInfo.pWnd->SendMessage(UM_GETMAINID); //直接让父窗口填写g_nMainID
		g_nAssisID = SetComAssisID(m_ComInfo.nWhichUnit,0);
		GetRemoteFile(m_TxtCurGPCode,m_TxtCurSetCode);
		MemStkInfo *tempMemStkInfo=g_pStockIO->Get_MemStkInfo(m_TxtCurGPCode,m_TxtCurSetCode);
	}
	else
		m_RichEdit.SetWindowText(_F("请用键盘精灵选择您要查看资料的股票.按TAB键或点右键切换其它类型的基本资料."));
}

void UF10::OnClickFLButton(UINT nID)
{
	if(!m_bCanClickFL || g_F10CaptionNum<1) return;

	m_whichfl = nID-IDC_BASEF10ID;
	if(m_whichfl<0 || m_whichfl>=g_F10CaptionNum) 
		m_whichfl=0;
	UncheckAllButton();
	m_btnF10[m_whichfl].CheckButton(TRUE);
	m_ComInfo.pWnd->SendMessage(UM_GETMAINID); //直接让父窗口填写g_nMainID
	m_nMainID = g_nMainID;
	g_nAssisID = SetComAssisID(m_ComInfo.nWhichUnit,NEW_JBMTXT_ASSISID);
	//抛砖引玉
	if(m_bDS)
		g_pGetData->DSGetNewJbmText(m_TxtCurGPCode,m_TxtCurSetCode,m_whichfl,g_NewF10Caption[m_whichfl].sFilePath,g_NewF10Caption[m_whichfl].nOffset,g_NewF10Caption[m_whichfl].nLength,CurrentJBM);		
	else
		g_pGetData->GetNewJbmText(m_TxtCurGPCode,m_TxtCurSetCode,m_whichfl,g_NewF10Caption[m_whichfl].sFilePath,g_NewF10Caption[m_whichfl].nOffset,g_NewF10Caption[m_whichfl].nLength,CurrentJBM);		
}

//改变JBM配置
void UF10::OnSelectJBM(UINT nID)
{
	CurrentJBM = nID - ID_JBM_MENU1;
	g_CurrentJBM = CurrentJBM;
}

void UF10::Resize()
{
	CRect rcdlg;
	m_pUF10CtrlWnd->GetClientRect(&rcdlg);
	if(!OnLine)
	{
		for(int i=0;i<MAX_F10CAPTION;i++)
			m_btnF10[i].ShowWindow(SW_HIDE);
		m_nRichEditTop=rcdlg.top;
		m_RichEdit.SetWindowPos(NULL,rcdlg.left+16,m_nRichEditTop,rcdlg.Width()-16,rcdlg.Height(),SWP_NOZORDER);
	}
	else
		JustifyButton(rcdlg);
}

void UF10::OnDraw(CDC* pDC) 
{	
	CRect rect;
	m_pUF10CtrlWnd->GetClientRect(&rect);
	if(m_bLayoutFlag && OnLine && CurrentJBM >= 0)
	{
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
			rect.bottom=rect.top+F10_BTN_HEIGHT+3;
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
}

BOOL UF10::OnEraseBkgnd(CDC* pDC) 
{
	CRect rect;
	m_pUF10CtrlWnd->GetClientRect(&rect);
	pDC->FillSolidRect (&rect,m_BackColor);
	return TRUE;
}

void UF10::OnGetDataAck(WPARAM wParam,LPARAM lParam)
{
	ANSHEADER *		pHeader = &g_AnsHeader;
	if(lParam==1)	pHeader = &g_AnsHeader3;
	if(m_nMainID!=pHeader->MainID)
		return;
	switch(wParam)
	{
	case JBMLEN_ASSISID:
		{
			//得到远程消息文件的长度
			DWORD RemoteFileLen = g_pGetData->GetFileLenACK();
			if(RemoteFileLen <= 0 || RemoteFileLen > 10*1024*1024) 
			{
				m_RichEdit.SetWindowText(_F("该股票无相应的基本资料."));
				break;
			}
			BOOL bIsSame = FALSE;
			CString tempFileName,tempStr;
			CTime theTime = CTime::GetCurrentTime ();
			if(m_TxtCurSetCode == SZ) tempStr = "RemoteSZ\\";
			else tempStr = "RemoteSH\\";
			tempFileName.Format("%s%s%s.Txt", HomePath,tempStr,m_TxtCurGPCode);
			CFile theFile;
			if( theFile.Open(tempFileName,CFile::modeRead|CFile::shareDenyNone))
			{
				DWORD len = theFile.GetLength(); //得到本地消息文件的长度
				if(RemoteFileLen == len)	//如果长度相同，则直接从本地读取
				{
					bIsSame = TRUE;
					//为buffer申请空间
					char *buffer = new char[len+1]; 
					theFile.Read(buffer,len);
					for(int k = 0;k < len;k++) //将'\0'变为空格
						if(buffer[k] == 0) buffer[k] = 0x20;
					buffer[len] = 0;
					m_RichEdit.SetRedraw(FALSE);
					m_RichEdit.SetWindowText (buffer);
					m_RichEdit.SetRedraw(TRUE);
					m_RichEdit.Invalidate(FALSE);
					delete buffer;
				}
				theFile.Close();
			}
			if(bIsSame) //只缓存一个小时
			{
				CFileStatus FileStatus;
				if( CFile::GetStatus(tempFileName,FileStatus) )
				{
					CTime curTime = CTime::GetCurrentTime();
					CTime time = FileStatus.m_mtime;
					CTimeSpan span=curTime-time;
					int ltt=span.GetHours();
					if(span.GetHours()>0)
						bIsSame=FALSE;
				}
			}
			if(!bIsSame)
			{
				//先写盘，再从文件中读取
				if( theRemoteFile.Open(tempFileName,CFile::modeCreate|CFile::modeReadWrite|CFile::shareDenyNone) )
				{
					bRemoteFileOpen = TRUE;

					m_ComInfo.pWnd->SendMessage(UM_GETMAINID); //直接让父窗口填写g_nMainID
					m_nMainID = g_nMainID;
					g_nAssisID = SetComAssisID(m_ComInfo.nWhichUnit,JBM_ASSISID);
					//抛砖引玉
					g_pGetData->GetJbmText(m_TxtCurGPCode,m_TxtCurSetCode,nCurRemotePos,g_bUseBigJBM);
				}
				else
					TDX_MessageBox(m_pUF10CtrlWnd->GetSafeHwnd(),"不能将基本资料存盘!",MB_OK|MB_ICONEXCLAMATION);
			}
			else
			{
				//如果支持分类资料，则继续请求				
				m_ComInfo.pWnd->SendMessage(UM_GETMAINID); //直接让父窗口填写g_nMainID
				m_nMainID = g_nMainID;
				g_nAssisID = SetComAssisID(m_ComInfo.nWhichUnit,JBMCFG_ASSISID);	
				g_pGetData->GetJbmCfg(m_TxtCurGPCode,m_TxtCurSetCode);				
			}
		}
		break;
	case JBM_ASSISID:
		{
			int len = g_pGetData->GetTextACK(FileBuffer);
			theRemoteFile.Write(FileBuffer,len);
			for(int k = 0;k < len;k++) //将'\0'变为空格
				if(FileBuffer[k] == 0) FileBuffer[k] = 0x20;
			FileBuffer[len] = 0;
			//附加到m_pTmpBuf中去
			if(strlen(m_pTmpBuf) < MAX_TXT_SIZE-len-1)
				strcat(m_pTmpBuf,FileBuffer);
			m_RichEdit.SetWindowText (m_pTmpBuf);
			//
			nCurRemotePos += len;
			if(len == (g_bUseBigJBM?10240:2048)) //非要用括号括起来
			{
				m_ComInfo.pWnd->SendMessage(UM_GETMAINID); //直接让父窗口填写g_nMainID
				m_nMainID = g_nMainID;
				g_nAssisID = SetComAssisID(m_ComInfo.nWhichUnit,JBM_ASSISID);
				g_pGetData->GetJbmText(m_TxtCurGPCode,m_TxtCurSetCode,nCurRemotePos,g_bUseBigJBM);
			}
			else
			{
				m_RichEdit.SetSel(0,0);
				theRemoteFile.Close();
				bRemoteFileOpen = FALSE;
				
				m_ComInfo.pWnd->SendMessage(UM_GETMAINID); //直接让父窗口填写g_nMainID
				m_nMainID = g_nMainID;
				g_nAssisID = SetComAssisID(m_ComInfo.nWhichUnit,JBMCFG_ASSISID);
				g_pGetData->GetJbmCfg(m_TxtCurGPCode,m_TxtCurSetCode);					
			}
		}
		break;
	case JBMCFG_ASSISID:
		{
			int titlenum = g_pGetData->GetJbmCfgACK();
			if(titlenum < 0)  titlenum = 0;
			m_bCanClickFL = TRUE;
			CRect rc;
			m_pUF10CtrlWnd->GetClientRect(&rc);
			JustifyButton(rc);
			m_pUF10CtrlWnd->Invalidate(TRUE);
		}	
		break;
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
			m_pUF10CtrlWnd->GetClientRect(&rc);
			JustifyButton(rc);
			m_pUF10CtrlWnd->Invalidate(TRUE);
			if(titlenum>0)
				m_pUF10CtrlWnd->PostMessage(WM_COMMAND,IDC_BASEF10ID+m_whichfl);
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
				m_RichEdit.SetWindowText (FileBuffer);
			}
			else
				m_RichEdit.SetWindowText (_F("尚未有相关资料!"));
			//
			m_RichEdit.SetSel(0,0);
		}
		break;
	}
}

void UF10::OnLButtonDown(UINT nFlags, CPoint point) 
{
	if(m_PreNext_Rect[2].PtInRect(point))
		m_pUF10CtrlWnd->SendMessage(WM_COMMAND,ID_JBM_MENU1);
	else if(g_TdxCfg.JbmNum>1 && m_PreNext_Rect[3].PtInRect(point))
		m_pUF10CtrlWnd->SendMessage(WM_COMMAND,ID_JBM_MENU1+1);
	else if(g_TdxCfg.JbmNum>2 && m_PreNext_Rect[4].PtInRect(point))
		m_pUF10CtrlWnd->SendMessage(WM_COMMAND,ID_JBM_MENU1+2);
}
