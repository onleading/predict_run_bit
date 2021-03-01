// MyCaptionBar.cpp : implementation file
//

#include "stdafx.h"
#include "MyCaptionBar.h"
#include "resource.h"
#include "MyInterface.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMyCaptionBar

CMyCaptionBar::CMyCaptionBar()
{
	m_hmenu=NULL;
	m_bt=NULL;
	m_bt_rect=NULL;
	m_leftbuttondown=FALSE;
	m_nButton=0;
	m_pstr=NULL;
	m_bFirstShow=FALSE;
	m_nID_Menu=0;
	memset(m_sysbt,0,sizeof(MyBitmapBt*)*4);
	memset(m_rbt,0,sizeof(MyBitmapBt*)*5);
	m_titlestr=_F("标题");

	HINSTANCE tmphi=AfxGetResourceHandle();
	AfxSetResourceHandle(g_resModule);
	CSize sz=g_GUI.GetBitmapSize("IDB_CAPTIONBAR_SYS_CLOSEBT_N");
	m_sysbt_rect[0].SetRect(0,0,sz.cx,sz.cy);
	sz=g_GUI.GetBitmapSize("IDB_CAPTIONBAR_SYS_STOREBT_N");
	m_sysbt_rect[1].SetRect(m_sysbt_rect[0].left-sz.cx-2,0,m_sysbt_rect[0].left-2,sz.cy);
	sz=g_GUI.GetBitmapSize("IDB_CAPTIONBAR_SYS_MINBT_N");
	m_sysbt_rect[2].SetRect(m_sysbt_rect[1].left-sz.cx-2,0,m_sysbt_rect[1].left-2,sz.cy);

	if(!g_OEM.bWebButtonInCaption)
		m_rbt_rect[4].SetRectEmpty();
	if(g_OEM.bWebButtonInCaption && g_OEM.bRightWebBtn)
	{
		sz=g_GUI.GetBitmapSize("IDB_CAPTIONBAR_BT4_N");
		m_rbt_rect[4].SetRect(m_sysbt_rect[2].left-sz.cx-10,0,m_sysbt_rect[2].left-10,sz.cy);
		sz=g_GUI.GetBitmapSize("IDB_CAPTIONBAR_BT0_N");
		m_rbt_rect[0].SetRect(m_rbt_rect[4].left-sz.cx-1,0,m_rbt_rect[4].left-1,sz.cy);
	}
	else
	{
		sz=g_GUI.GetBitmapSize("IDB_CAPTIONBAR_BT0_N");
		m_rbt_rect[0].SetRect(m_sysbt_rect[2].left-sz.cx-10,0,m_sysbt_rect[2].left-10,sz.cy);
	}
	sz=g_GUI.GetBitmapSize("IDB_CAPTIONBAR_BT1_N");
	m_rbt_rect[1].SetRect(m_rbt_rect[0].left-sz.cx,0,m_rbt_rect[0].left,sz.cy);	
	sz=g_GUI.GetBitmapSize("IDB_CAPTIONBAR_BT2_N");
	m_rbt_rect[2].SetRect(m_rbt_rect[1].left-sz.cx,0,m_rbt_rect[1].left,sz.cy);
	sz=g_GUI.GetBitmapSize("IDB_CAPTIONBAR_BT3_N");
	m_rbt_rect[3].SetRect(m_rbt_rect[2].left-sz.cx,0,m_rbt_rect[2].left,sz.cy);
	if(g_OEM.bWebButtonInCaption && !g_OEM.bRightWebBtn)
	{
		sz=g_GUI.GetBitmapSize("IDB_CAPTIONBAR_BT4_N");
		m_rbt_rect[4].SetRect(m_rbt_rect[3].left-sz.cx,0,m_rbt_rect[3].left,sz.cy);
	}

	AfxSetResourceHandle(tmphi);

	m_nHeadIconWid = 18;
	if(g_OEM.bGdzqEmbedWTVersion)
		m_nHeadIconWid = 70;
	else if(g_OEM.bZxzqEmbedWTVersion)
		m_nHeadIconWid = 62;
}

CMyCaptionBar::~CMyCaptionBar()
{
	int i;
	if(m_bt)
	{
		for(UINT i=0;i<m_nButton;i++)
			TDEL(m_bt[i]);
		delete[] m_bt;
	}
	if(m_bt_rect) delete[] m_bt_rect;
	if(m_pstr)	delete[] m_pstr;
	for(i=0;i<4;i++)
	{
		TDEL(m_sysbt[i]);
	}
	for(i=0;i<5;i++)
	{
		TDEL(m_rbt[i]);
	}
}

float GetXSRatio(int Wid)
{
	float xs=7.0;
	if(Wid>=900)
	{
		if(g_OEM.bGdzqEmbedWTVersion)
			xs=7.5;
		else
			xs=7.85;
	}
	return xs;
}

BEGIN_MESSAGE_MAP(CMyCaptionBar, CDialogBar)
	//{{AFX_MSG_MAP(CMyCaptionBar)
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_CREATE()
	ON_WM_ERASEBKGND()
	ON_WM_SIZE()
	ON_WM_LBUTTONDBLCLK()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CMyCaptionBar message handlers

void CMyCaptionBar::OnMouseMove(UINT nFlags, CPoint point) 
{
	if(nFlags==MK_LBUTTON && m_leftbuttondown)
	{
		CPoint pt=point;
		ClientToScreen(&pt);
		if(pt!=m_pt)
		{
			CWnd *pwnd=GetParent();
			if(pwnd)
			{
				CRect rc;
				pwnd->GetWindowRect(&rc);
				pwnd->SetWindowPos(NULL,rc.left+pt.x-m_pt.x,rc.top+pt.y-m_pt.y,0,0,SWP_NOSIZE|SWP_NOZORDER);
				m_pt=pt;
			}
		}
	}
	CDialogBar::OnMouseMove(nFlags, point);
}

void CMyCaptionBar::OnLButtonDown(UINT nFlags, CPoint point) 
{
	if(!m_leftbuttondown && !GetParent()->IsZoomed())
	{
		m_leftbuttondown=TRUE;
		m_pt=point;
		ClientToScreen(&m_pt);
		SetCapture();
	}
	//点图片区弹出公司网址
	else if(m_nHeadIconWid>20 && point.x<m_nHeadIconWid)
		ShellExecute(NULL, _T("open"), (CString)"http://"+g_Url.CorpWeb, NULL, NULL, SW_SHOW);
	CDialogBar::OnLButtonDown(nFlags, point);
}

void CMyCaptionBar::OnLButtonUp(UINT nFlags, CPoint point) 
{
	ReleaseCapture();
	m_leftbuttondown=FALSE;
	CDialogBar::OnLButtonUp(nFlags, point);
}

void CMyCaptionBar::NextBtRect(CRect &rc,int offset,int xLimit)
{
	rc.OffsetRect(offset,0);
	if(rc.right>xLimit)
		rc.SetRect(0,0,0,0);
}

int CMyCaptionBar::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CDialogBar::OnCreate(lpCreateStruct) == -1)
		return -1;
	if(!g_OEM.bZxzqEmbedWTVersion && !g_OEM.bGdzqEmbedWTVersion)
	{
		if(g_OEM.bShzqEmbedWTVersion)
			m_hIcon = LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_SHZQ));
		else if(g_OEM.bYhzqEmbedWTVersion)
			m_hIcon = LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_YHZQ));
		else if(g_OEM.bZxjtEmbedWTVersion)
			m_hIcon = LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_ZXJT));
		else
			m_hIcon = LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_MAINFRAME));
	}
	else
	{
		CBitmap tmpbm;
		m_imagelist.Create(m_nHeadIconWid,24,ILC_COLOR24|ILC_MASK,1,0);
		if(g_OEM.bGdzqEmbedWTVersion)
			tmpbm.LoadBitmap(IDB_BITMAP_GDZQ);
		else
			tmpbm.LoadBitmap(IDB_BITMAP_ZXZQ);
		m_imagelist.Add(&tmpbm,RGB(0,0,0));
	}
	CRect rc;
	GetWindowRect(&rc);
	GetParent()->ScreenToClient(&rc);
	rc.bottom=rc.top+24;
	MoveWindow(rc);
	
	HINSTANCE tmphi=AfxGetResourceHandle();
	AfxSetResourceHandle(g_resModule);
	LPCTSTR temp0[9]={0,0,"IDB_CAPTIONBAR_SYS_CLOSEBT_N",
		0,0,"IDB_CAPTIONBAR_SYS_CLOSEBT_D",
		0,0,"IDB_CAPTIONBAR_SYS_CLOSEBT_O"};
	LPCTSTR temp1[9]={0,0,"IDB_CAPTIONBAR_SYS_STOREBT_N",
		0,0,"IDB_CAPTIONBAR_SYS_STOREBT_D",
		0,0,"IDB_CAPTIONBAR_SYS_STOREBT_O"};
	LPCTSTR temp2[9]={0,0,"IDB_CAPTIONBAR_SYS_MINBT_N",
		0,0,"IDB_CAPTIONBAR_SYS_MINBT_D",
		0,0,"IDB_CAPTIONBAR_SYS_MINBT_O"};
	LPCTSTR temp3[9]={0,0,"IDB_CAPTIONBAR_SYS_MAXBT_N",
		0,0,"IDB_CAPTIONBAR_SYS_MAXBT_D",
		0,0,"IDB_CAPTIONBAR_SYS_MAXBT_O"};
	
	m_sysbt[0]=new MyBitmapBt(ID_CAPTIONBAR_SYS_CLOSE,"",NormalBt);
	m_sysbt[0]->SetBK(temp0);
	m_sysbt[0]->Create(NULL,"",WS_VISIBLE|WS_CHILD,rc,this,ID_CAPTIONBAR_SYS_CLOSE);
	m_sysbt[1]=new MyBitmapBt(ID_CAPTIONBAR_SYS_STORE,"",NormalBt);
	m_sysbt[1]->SetBK(temp1);
	m_sysbt[1]->Create(NULL,"",WS_VISIBLE|WS_CHILD,rc,this,ID_CAPTIONBAR_SYS_STORE);
	m_sysbt[2]=new MyBitmapBt(ID_CAPTIONBAR_SYS_MIN,"",NormalBt);
	m_sysbt[2]->SetBK(temp2);
	m_sysbt[2]->Create(NULL,"",WS_VISIBLE|WS_CHILD,rc,this,ID_CAPTIONBAR_SYS_MIN);
	m_sysbt[3]=new MyBitmapBt(ID_CAPTIONBAR_SYS_MAX,"",NormalBt);
	m_sysbt[3]->SetBK(temp3);
	m_sysbt[3]->Create(NULL,"",/*WS_VISIBLE|*/WS_CHILD,rc,this,ID_CAPTIONBAR_SYS_MAX);
	

	LPCTSTR temp4[9]={0,0,"IDB_CAPTIONBAR_BT0_N",
		0,0,"IDB_CAPTIONBAR_BT0_D",
		0,0,"IDB_CAPTIONBAR_BT0_N"};

	LPCTSTR temp5[9]={0,0,"IDB_CAPTIONBAR_BT1_N",
		0,0,"IDB_CAPTIONBAR_BT1_D",
		0,0,"IDB_CAPTIONBAR_BT1_N"};

	LPCTSTR temp6[9]={0,0,"IDB_CAPTIONBAR_BT2_N",
		0,0,"IDB_CAPTIONBAR_BT2_D",
		0,0,"IDB_CAPTIONBAR_BT2_N"};

	LPCTSTR temp7[9]={0,0,"IDB_CAPTIONBAR_BT3_N",
		0,0,"IDB_CAPTIONBAR_BT3_D",
		0,0,"IDB_CAPTIONBAR_BT3_N"};

	LPCTSTR temp8[9]={0,0,"IDB_CAPTIONBAR_BT4_N",
		0,0,"IDB_CAPTIONBAR_BT4_D",
		0,0,"IDB_CAPTIONBAR_BT4_N"};

	m_rbt[0]=new MyBitmapBt(ID_CAPTIONBAR_BT0,"",NormalBt);//网站
	m_rbt[0]->SetBK(temp4);
	m_rbt[0]->Create(NULL,"",WS_VISIBLE|WS_CHILD,rc,this,ID_CAPTIONBAR_BT0);
	
	m_rbt[1]=new MyBitmapBt(ID_CAPTIONBAR_BT1,"",NormalBt);//交易
	m_rbt[1]->SetBK(temp5);
	m_rbt[1]->Create(NULL,"",WS_VISIBLE|WS_CHILD,rc,this,ID_CAPTIONBAR_BT1);
	
	m_rbt[2]=new MyBitmapBt(ID_CAPTIONBAR_BT2,"",NormalBt);//资讯
	m_rbt[2]->SetBK(temp6);
	m_rbt[2]->Create(NULL,"",WS_VISIBLE|WS_CHILD,rc,this,ID_CAPTIONBAR_BT2);

	m_rbt[3]=new MyBitmapBt(ID_CAPTIONBAR_BT3,"",NormalBt);//行情
	m_rbt[3]->SetBK(temp7);
	m_rbt[3]->Create(NULL,"",WS_VISIBLE|WS_CHILD,rc,this,ID_CAPTIONBAR_BT3);

	if(g_OEM.bWebButtonInCaption)
	{
		m_rbt[4]=new MyBitmapBt(ID_CAPTIONBAR_BT4,"",NormalBt);//其它
		m_rbt[4]->SetBK(temp8);
		m_rbt[4]->Create(NULL,"",WS_VISIBLE|WS_CHILD,rc,this,ID_CAPTIONBAR_BT4);
	}
	AfxSetResourceHandle(tmphi);

	return 0;
}

BOOL CMyCaptionBar::OnEraseBkgnd(CDC* pDC) 
{
	if(!m_bFirstShow)
	{
		m_bFirstShow=TRUE;
		if(m_nID_Menu!=0)
			LoadMenu(m_nID_Menu);
	}

	ModifyDC(pDC);

	CRect rc;
	CDC memdc;
	memdc.CreateCompatibleDC(pDC);
	GetClientRect(&rc);
	CBitmap membm;
	membm.CreateCompatibleBitmap(pDC,rc.Width(),rc.Height());
	CBitmap *poldbm=memdc.SelectObject(&membm);
	if(g_resModule)
	{
		HINSTANCE tmphi= AfxGetResourceHandle();
		AfxSetResourceHandle(g_resModule);
		CBitmap bm;
		if(bm.LoadBitmap("IDB_CAPTIONBAR_BK"))
			g_GUI.DrawXPBmp(&memdc,&bm,CPoint(0,0),CPoint(rc.Width(),rc.Height()));
		AfxSetResourceHandle(tmphi);
	}

	int y=2;
	int cx=m_titlestr.GetLength()*6;
	int first_rbt_left = m_rbt_rect[3].left;
	int first_rbt_height = m_rbt_rect[3].Height();	
	if(g_OEM.bWebButtonInCaption && !g_OEM.bRightWebBtn)
	{
		first_rbt_left = m_rbt_rect[4].left;
		first_rbt_height = m_rbt_rect[4].Height();
	}
	int x1=0,x2=0,x3=first_rbt_left-5;
	for(UINT i=m_nButton-1;i>=0;i--)
	{
		if(m_bt_rect[i].right!=0)
		{
			x1=m_bt_rect[i].right+5;
			break;
		}
	}
	if(x1!=0 && x3-x1>=cx) 
	{
		x2=x1+(x3-x1-cx)/2;
		m_titlestr_rect.SetRect(x2,y,x2+cx,y+first_rbt_height);
	}
	else
		m_titlestr_rect.SetRect(0,0,0,0);

	if(g_OEM.bZxzqEmbedWTVersion || g_OEM.bGdzqEmbedWTVersion)
		m_imagelist.Draw(&memdc,0,m_icon_point,ILD_NORMAL);
	else
		::DrawIconEx(memdc.m_hDC,m_icon_point.x,m_icon_point.y,m_hIcon,16,16,0,NULL,DI_NORMAL);
	memdc.SetBkMode(TRANSPARENT);
	CFont titleFont,*pOldFont;
	titleFont.CreateFont(12,0,0,0,0,0,0,0,DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,DEFAULT_PITCH, SONG_FONT );
	pOldFont=memdc.SelectObject(&titleFont);
	memdc.SetTextColor(RGB(255,0,0));
	memdc.DrawText(m_titlestr,m_titlestr_rect,DT_RIGHT|DT_VCENTER|DT_SINGLELINE);
	pDC->BitBlt(0,0,rc.Width(),rc.Height(),&memdc,0,0,SRCCOPY);
	memdc.SelectObject(poldbm);
	membm.DeleteObject();
	memdc.SelectObject(pOldFont);
	titleFont.DeleteObject();
	memdc.DeleteDC();
	return TRUE;
}

LRESULT CMyCaptionBar::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	if(message==UM_MYBITMAPBUTTON_ONMOUSEMOVE)
	{
		UINT nID=(UINT)lParam;
		UINT nIDNow=nID;
		CPoint point;
		GetCursorPos(&point);
		ScreenToClient(&point);
		for(UINT i=0;i<m_nButton;i++)
		{
			if(nID==i) continue;
			if(m_bt_rect[i].PtInRect(point))
			{
				nIDNow=i+ID_CAPTIONBAR_MENU0;
				break;
			}
		}
		if(nIDNow!=nID)
			OnMenuButton(nIDNow-ID_CAPTIONBAR_MENU0);
		return 0;
	}
	if(message==UM_MYBITMAPBUTTON)
	{
		switch((UINT)lParam)
		{
		case ID_CAPTIONBAR_SYS_MIN:
			{
				ChangeParentWndSize(SC_MINIMIZE);
				return 0;
			}
			break;
		case ID_CAPTIONBAR_SYS_MAX:
			{
				ChangeParentWndSize(SC_MAXIMIZE);
				return 0;
			}
			break;
		case ID_CAPTIONBAR_SYS_STORE:
			{
				ChangeParentWndSize(SC_RESTORE);
				return 0;
			}
			break;
		case ID_CAPTIONBAR_SYS_CLOSE:
			{
				ChangeParentWndSize(SC_CLOSE);
				return 0;
			}
			break;
		case ID_CAPTIONBAR_BT0:	//网站按钮
			if(strlen(g_OEM.strCaptionQSWebURL)>0)
				OpenCusWebPage(g_pMainWin,g_OEM.strCaptionQSWebURL,g_OEM.bMainWebPageInsideOpen);
			else if(g_OEM.bGtjaEmbedWTVersion)
				AfxGetMainWnd()->SendMessage(WM_COMMAND,ID_FUNC_WT);
			else
				AfxGetMainWnd()->SendMessage(WM_COMMAND, ID_CORP_WEB);
			return 0;
		case ID_CAPTIONBAR_BT1:	//交易按钮
			if(g_OEM.bGtjaEmbedWTVersion)
				AfxGetMainWnd()->SendMessage(WM_COMMAND, ID_F9);
			else if(strlen(g_NewJyCfg.AddedWT1_Name)>0 || strlen(g_NewJyCfg.AddedWT2_Name)>0)
			{
				CNewMenu showMore;
				showMore.CreatePopupMenu();
				
				if(!g_OEM.bF12SelectWT)
					showMore.AppendMenu(0, ID_FUNC_EMBWT, _F(g_NewJyCfg.AddedWT_Name));				
				CString tmpTitle;
				if(strlen(g_NewJyCfg.AddedWT1_Name)>0)
					showMore.AppendMenu(0, ID_FUNC_ADDEDWT1, _F(g_NewJyCfg.AddedWT1_Name));
				if(strlen(g_NewJyCfg.AddedWT2_Name)>0)
					showMore.AppendMenu(0, ID_FUNC_ADDEDWT2, _F(g_NewJyCfg.AddedWT2_Name));
				if(g_OEM.bF12SelectWT)
					showMore.AppendMenu(0, ID_FUNC_EMBWT, _F(g_NewJyCfg.AddedWT_Name));
				CPoint point(m_rbt_rect[1].left, m_rbt_rect[1].bottom);
				ClientToScreen(&point);
				showMore.TrackPopupMenu(TPM_LEFTALIGN |TPM_RIGHTBUTTON, point.x, point.y, AfxGetMainWnd());				
			}
			else
				AfxGetMainWnd()->SendMessage(WM_COMMAND, ID_FUNC_WT);
			return 0;
		case ID_CAPTIONBAR_BT2: //资讯按钮
			AfxGetMainWnd()->SendMessage(WM_COMMAND, IDC_ZXTREE);
			return 0;
		case ID_CAPTIONBAR_BT3: //行情按钮
			if(strlen(g_OEM.strCaptionHQURL)>0)
				OpenCusWebPage(g_pMainWin,g_OEM.strCaptionHQURL,g_OEM.bMainWebPageInsideOpen);
			else
				AfxGetMainWnd()->SendMessage(WM_COMMAND,IDC_CONNECTHQ);
			return 0;
		//如果点击了WebBtn的话,闪电手功能的话直接打开闪电手,如果是网址则直接打开网址,非网址则打开应用程序
		case ID_CAPTIONBAR_BT4:	//额外按钮	
			if(g_OEM.bHlzqEmbedWTVersion)
			{
				char tmpPath[MAX_PATH];
				sprintf(tmpPath,"%sHLIQ\\TQ.exe",HomePath);
				ExecExtern(tmpPath,m_hWnd,"TYPE:10");
			}
			else if(strnicmp(g_OEM.strWebButtonURL,"http",4)==0)
				OpenCusWebPage(g_pMainWin,g_OEM.strWebButtonURL,g_OEM.bMainWebPageInsideOpen);
 			else if(strlen(g_OEM.strWebButtonURL)>0)
				ExecExtern(g_OEM.strWebButtonURL,NULL);
			else if(g_GC.bHasQzSDS)
				AfxGetMainWnd()->SendMessage(WM_COMMAND,IDC_QZSDS);
			return 0;
		}
		UINT n=(UINT)lParam-ID_CAPTIONBAR_MENU0;
		if(n>=0 && n<m_nButton)
			OnMenuButton(n);
		return 0;
	}
	if(message==UM_EXITMENULOOP)
		CancelAndNewMenu();
	return CDialogBar::DefWindowProc(message, wParam, lParam);
}

void CMyCaptionBar::CancelAndNewMenu()
{
	m_hmenu=NULL;
	BOOL found=FALSE;
	UINT nIndex=0;
	for(UINT i=0;i<m_nButton;i++)
	{
		if(m_bt[i]->GetSel())
		{
			found=TRUE;
			nIndex=i;
		}
	}
	if(found)
	{
		CPoint curPos;
		CRect rc=m_bt_rect[nIndex];
		ClientToScreen(&rc);
		curPos.y=rc.bottom;
		curPos.x=rc.left;

		CNewMenu *pmenu=(CNewMenu*)m_menu.GetSubMenu(nIndex);
		ProcessMenu(pmenu,nIndex);
		m_hmenu=pmenu->GetSafeHmenu();
		pmenu->TrackPopupMenu(TPM_LEFTALIGN, curPos.x,curPos.y, AfxGetMainWnd());
		m_bt[nIndex]->SetSel(FALSE);
	}
}

void CMyCaptionBar::OnMenuButton(UINT nIndex)
{
	m_bt[nIndex]->SetSel(TRUE);
	if(m_hmenu)
		::SendMessage(AfxGetMainWnd()->m_hWnd,WM_CANCELMODE,0,0);
	
	if(!m_hmenu)
	{
		CPoint curPos;
		CRect rc=m_bt_rect[nIndex];
		ClientToScreen(&rc);
		curPos.y=rc.bottom;
		curPos.x=rc.left;
		CNewMenu *pmenu=(CNewMenu*)m_menu.GetSubMenu(nIndex);
		ProcessMenu(pmenu,nIndex);
		m_hmenu=pmenu->GetSafeHmenu();
		pmenu->TrackPopupMenu(TPM_LEFTALIGN, curPos.x,curPos.y, AfxGetMainWnd());
		m_bt[nIndex]->SetSel(FALSE);
	}
}

extern void AppendCustomPadMenu(CNewMenu* pPadMenu);
extern void AppendDSMenu(CNewMenu* pDSMenu);
BOOL CMyCaptionBar::ProcessMenu(LPVOID pNMenu,UINT nIndex)
{
	CNewMenu *pMenu=(CNewMenu*)pNMenu;
	if(!pMenu) return FALSE;
	if(nIndex==0)
	{
		pMenu->RemoveMenu(IDC_WEBUSER_1001,MF_BYCOMMAND);
		pMenu->RemoveMenu(IDC_WEBUSER_1002,MF_BYCOMMAND);
		pMenu->RemoveMenu(IDC_WEBUSER_1003,MF_BYCOMMAND);
		if(g_bUserHostEverConnect && OnLine)
		{
		#ifdef PERSONAL
			pMenu->RemoveMenu(ID_WRITEURGENT,MF_BYCOMMAND);
			if(g_MngRight==5)
				pMenu->InsertMenu(ID_CORP_WEB,MF_BYCOMMAND,ID_WRITEURGENT,_F("发布通告"));	
		#endif
			pMenu->RemoveMenu(ID_SEND_FILE,MF_BYCOMMAND);
			pMenu->RemoveMenu(ID_REC_FILE,MF_BYCOMMAND);
			pMenu->InsertMenu(ID_CORP_WEB,MF_BYCOMMAND,ID_SEND_FILE,_F("上传个性数据"));	
			pMenu->InsertMenu(ID_CORP_WEB,MF_BYCOMMAND,ID_REC_FILE,_F("下载个性数据"));	
		}		
		if(g_bHasUserHost)
		{
			if(!g_bIsGuest && g_LoginInfo.bDialogRegister && g_nLastLoginType!=JY_LOGIN) //如果是交易登录,不能修改密码
			{
				pMenu->InsertMenu(ID_CORP_WEB, MF_BYCOMMAND,IDC_WEBUSER_1002, _F("修改密码"));	
				if(g_LoginInfo.bCanDelMySelf)
					pMenu->InsertMenu(ID_CORP_WEB, MF_BYCOMMAND,IDC_WEBUSER_1003, _F("删除用户"));	
			}
		}
		if(g_bHasTICQ)
			pMenu->InsertMenu(ID_CORP_WEB, MF_BYCOMMAND,IDC_WEBUSER_1001, _F("TQ"));			
	}
	if(nIndex==1)
	{	//如果功能菜单
		CNewMenu *pPadMenu=(CNewMenu*)pMenu->GetSubMenu(8);
		GetDynaCusPadInfo();
		int iPos,iFix=8;
		for (iPos = pPadMenu->GetMenuItemCount()-1; iPos > iFix; iPos--)
			pPadMenu->DeleteMenu(iPos, MF_BYPOSITION);
		AppendCustomPadMenu(pPadMenu);
	}
	if(nIndex==4)
	{	//如果是资讯菜单
		CNewMenu* pZxMenu = pMenu;
		//F11资讯项
		if(strlen(g_GC.HKey_Path) && strlen(g_GC.HKey_KeyString))
		{
			pZxMenu->RemoveMenu(IDC_HKEYPROGRAM,MF_BYCOMMAND);
			pZxMenu->InsertMenu(ID_TDX_JBZL, MF_BYCOMMAND,IDC_HKEYPROGRAM, _F(g_GC.HKey_MenuName+CString("\tF11")));
		}		
		else if(strlen(g_F11WebPage.PageName) != 0)
		{
			pZxMenu->RemoveMenu(IDC_F11WEBPAGE,MF_BYCOMMAND);
			pZxMenu->InsertMenu(ID_TDX_JBZL, MF_BYCOMMAND,IDC_F11WEBPAGE, _F(g_F11WebPage.PageName+CString("\tF11")));	
		}
		//
		if(strlen(g_Url.AddZxStr) != 0)
		{
			pZxMenu->RemoveMenu(ID_RIGHT_ADDZX,MF_BYCOMMAND);
			pZxMenu->InsertMenu(ID_NEWS, MF_BYCOMMAND,ID_RIGHT_ADDZX, _F(g_Url.AddZxStr));	
		}
		if(!g_bHasWebInfo && strlen(g_Url.WebInfoURL)==0)
			pZxMenu->RemoveMenu(ID_WEBINFO,MF_BYCOMMAND);
		if(g_GC.Video.nVideoFlag==0)
			pZxMenu->RemoveMenu(ID_VIDEO,MF_BYCOMMAND);
	}
	if(g_bHasDS && nIndex==5)
	{	
		CNewMenu* pDSMenu = pMenu;
		if(pDSMenu)
			AppendDSMenu(pDSMenu);
	}
	int nGap=0;
	if(g_bHasDS)
		nGap++;
	if(CanAdvLevel2() || g_GC.bHasGPGrade)
		nGap++;
	if(g_nWebPageNum > 0)
		nGap++;
	if(g_nWebPageNum2 > 0)
		nGap++;
	if(nIndex==5+nGap)
	{	//如果是显示菜单,将"工具栏"和"全屏显示"菜单删除
		CNewMenu* pShowMenu=pMenu;
		pShowMenu->DeleteMenu(ID_FULLSCREEN,MF_BYCOMMAND);
	}
	if(nIndex==6+nGap)
	{	//如果是帮助菜单
		CNewMenu* pHelpMenu = pMenu;
		if(strlen(g_Url.EmailStr) != 0)
		{
			pHelpMenu->RemoveMenu(ID_RIGHT_EMAIL,MF_BYCOMMAND);
			pHelpMenu->InsertMenu(ID_FORUM, MF_BYCOMMAND,ID_RIGHT_EMAIL, _F(g_Url.EmailStr));	
		}
		if(strlen(g_Url.HelpWebStr) != 0)
		{
			pHelpMenu->RemoveMenu(ID_RIGHT_HELPWEB,MF_BYCOMMAND);
			pHelpMenu->InsertMenu(ID_FORUM, MF_BYCOMMAND,ID_RIGHT_HELPWEB, _F(g_Url.HelpWebStr));	
		}		
	}
	return TRUE;
}

void CMyCaptionBar::ModifyDC(CDC *pDC)
{
	for(UINT i=0;i<3;i++)
	{
		pDC->ExcludeClipRect(m_sysbt_rect[i]);
	}
	for(i=0;i<5;i++)
	{
		//注意:bWebButtonInCaption没有的话,m_rbt_rect[4]应该为0
		pDC->ExcludeClipRect(m_rbt_rect[i]);
	}
	for(i=0;i<m_nButton;i++)
	{
		if(m_bt_rect[i].Width()!=0)
			pDC->ExcludeClipRect(m_bt_rect[i]);
	}
}

BOOL CMyCaptionBar::LoadMenu(UINT nIDMenu)
{
	m_nID_Menu=nIDMenu;
	if(!m_bFirstShow)
		return FALSE;

	if(m_nButton!=0)
	{
		delete[] m_pstr;
		m_menu.DestroyMenu();
		for(int n=0;n<(int)m_nButton;n++)
		{
			m_bt[n]->DestroyWindow();
			TDEL(m_bt[n]);
		}
		delete[] m_bt;
		delete[] m_bt_rect;
		m_bt=NULL;
		m_bt_rect=NULL;
		m_nButton=0;
	}
	
	m_menu.LoadMenu(nIDMenu);
//	m_menu.LoadToolBar(IDR_MAINFRAME);
	if(g_nWebPageNum > 0) //构建Web菜单
	{		
		m_WebMenu.CreatePopupMenu();
		UINT StartID = ID_WEB_MENU_1;
		for(int i = 0;i < g_nWebPageNum;i++)
		{
			if(g_strWebPageURL[i][0]==0)
				m_WebMenu.AppendMenu( MF_SEPARATOR,StartID+i,g_strWebPageStr[i]);
			else
				m_WebMenu.AppendMenu( MF_STRING,StartID+i,g_strWebPageStr[i]);
		}		
	}
	if(g_nWebPageNum2 > 0) //构建Web菜单
	{		
		m_WebMenu2.CreatePopupMenu();
		UINT StartID = ID_WEB2_MENU_1;
		for(int i = 0;i < g_nWebPageNum2;i++)
		{
			if(g_strWebPageURL2[i][0]==0)
				m_WebMenu2.AppendMenu( MF_SEPARATOR,StartID+i,g_strWebPageStr2[i]);
			else
				m_WebMenu2.AppendMenu( MF_STRING,StartID+i,g_strWebPageStr2[i]);
		}		
	}
	int nPos=5;
	if(g_bHasDS)
	{
		m_DSMenu.CreatePopupMenu();
		CString tmpMenuStr;
		tmpMenuStr.Format("连接%s",g_DS.DSOEMName);
		m_DSMenu.AppendMenu( MF_STRING,ID_DSINIT,_F(tmpMenuStr));
		tmpMenuStr.Format("断开%s",g_DS.DSOEMName);
		m_DSMenu.AppendMenu( MF_STRING,ID_UNINITDS,_F(tmpMenuStr));
		m_DSMenu.AppendMenu(MF_SEPARATOR);
		m_menu.InsertMenu(nPos,MF_BYPOSITION|MF_POPUP,(UINT)(m_DSMenu.m_hMenu),_F(g_DS.DSOEMName));
		nPos++;
	}
	if(CanAdvLevel2() || g_GC.bHasGPGrade)
	{
		m_AdvFuncMenu.CreatePopupMenu();
		if(g_GC.bHasGPGrade)
		{
			m_AdvFuncMenu.AppendMenu( MF_STRING,ID_GPGRADE_DLG,_F("机构评测和诊断\t18"));
			m_AdvFuncMenu.AppendMenu( MF_STRING,ID_TDXREPORT,_F("研究报告平台\t19"));
			m_AdvFuncMenu.AppendMenu(MF_SEPARATOR);
		}
		if(CanAdvLevel2())
		{			
			m_AdvFuncMenu.AppendMenu( MF_STRING,ID_KQS,_F("权证盘口王"));
			m_AdvFuncMenu.AppendMenu( MF_STRING,ID_KQSCFG,_F("权证盘口王设置"));
			if(g_GC.bHasQzSDS)
			{
				m_AdvFuncMenu.AppendMenu(MF_SEPARATOR);
				m_AdvFuncMenu.AppendMenu( MF_STRING,IDC_QZSDS,_F("权证闪电手\t25"));
			}
		}
	#ifndef CITY10		
		m_AdvFuncMenu.AppendMenu(MF_SEPARATOR);
		m_AdvFuncMenu.AppendMenu( MF_STRING,IDC_USERCENTER,_F("会员服务中心"));
	#endif
		m_menu.InsertMenu(nPos,MF_BYPOSITION|MF_POPUP,(UINT)(m_AdvFuncMenu.m_hMenu),_F("深度(&G)"));
		nPos++;
	}
	if(g_nWebPageNum > 0)
		m_menu.InsertMenu(nPos,MF_BYPOSITION|MF_POPUP,(UINT)(m_WebMenu.m_hMenu),g_strMainWebTitle);
	if(g_nWebPageNum2 > 0)
		m_menu.InsertMenu(nPos+1,MF_BYPOSITION|MF_POPUP,(UINT)(m_WebMenu2.m_hMenu),g_strMainWebTitle2);

	m_nButton=m_menu.GetMenuItemCount();
	m_pstr=new CString[m_nButton];
	for(UINT i=0;i<m_nButton;i++)
		m_menu.GetMenuString(i,m_pstr[i],MF_BYPOSITION);
	nPos=5;
	if(g_bHasDS)
	{
		m_pstr[nPos]=_F(g_DS.DSOEMName);
		nPos++;
	}
	if(CanAdvLevel2() || g_GC.bHasGPGrade)
	{
		m_pstr[nPos]=_F("深度(&G)");
		nPos++;
	}
	if(g_nWebPageNum>0)
		m_pstr[nPos]=g_strMainWebTitle;
	if(g_nWebPageNum2>0)
		m_pstr[nPos+1]=g_strMainWebTitle2;

	HINSTANCE tmphi=AfxGetResourceHandle();
	AfxSetResourceHandle(g_resModule);
	LPCTSTR temp[9]={0,0,"IDB_CAPTIONBAR_MENUBK_N",
		0,0,"IDB_CAPTIONBAR_MENUBK_D",
		0,0,"IDB_CAPTIONBAR_MENUBK_N"};

	CRect rc1,rc2;
	m_bt=new MyBitmapBt*[m_nButton];
	m_bt_rect=new CRect[m_nButton];
	for(UINT n=0;n<m_nButton;n++)
	{
		m_bt[n]=new MyBitmapBt(ID_CAPTIONBAR_MENU0+n,m_pstr[n],MenuBt);
		m_bt[n]->SetBK(temp);
		COLORREF cor0,cor1,cor2;
		cor0=g_GUI.GetColorCfgFromResource(g_resModule,"IDR_COLOR","MyCaptionBar_MenuBt",0);
		cor1=g_GUI.GetColorCfgFromResource(g_resModule,"IDR_COLOR","MyCaptionBar_MenuBt",1);
		cor2=g_GUI.GetColorCfgFromResource(g_resModule,"IDR_COLOR","MyCaptionBar_MenuBt",2);
		m_bt[n]->SetFrameCor_MenuStyle(cor0,cor1);
		m_bt[n]->SetTextCor(cor2,cor2,cor2);
		m_bt[n]->SetTextOffset(0,0,1,1,0,0);
		m_bt[n]->Create(NULL,"",WS_VISIBLE|WS_CHILD,CRect(0,0,0,0),this,ID_CAPTIONBAR_MENU0+n);
	}
	AfxSetResourceHandle(tmphi);
	GetClientRect(&rc1);
	rc2=rc1;
	rc2.top=2;
	rc2.bottom=rc2.top+20;
	rc2.left+=m_nHeadIconWid+2;
	int nLast=-1;
	float xs=GetXSRatio(rc1.Width());
	int first_rbt_left = m_rbt_rect[3].left;
	if(g_OEM.bWebButtonInCaption && !g_OEM.bRightWebBtn)
		first_rbt_left = m_rbt_rect[4].left;
	for(n=0;n<m_nButton;n++)
	{
		rc2.right=rc2.left+m_pstr[n].GetLength()*xs-4;
		m_bt_rect[n]=rc2;
		NextBtRect(rc2,rc2.Width()+1,first_rbt_left-5);
		if(rc2.Width()==0)
		{
			nLast=n;
			break;
		}
		m_bt[n]->MoveWindow(m_bt_rect[n]);
	}
	if(nLast!=-1 && nLast!=m_nButton-1)
	{
		for(n=nLast+1;n<m_nButton;n++)
		{
			m_bt[n]->ShowWindow(SW_HIDE);
			m_bt_rect[n].SetRect(0,0,0,0);
		}
	}	
	Invalidate();
	return TRUE;
}

void CMyCaptionBar::OnSize(UINT nType, int cx, int cy) 
{
	CDialogBar::OnSize(nType, cx, cy);

	int ox=m_sysbt_rect[0].left;
	int x=cx-m_sysbt_rect[0].Width()-5;
	int oy=m_sysbt_rect[0].top-1;
	int y=1;
	m_sysbt_rect[0].OffsetRect(-ox+x,-oy+y);
	m_sysbt_rect[1].OffsetRect(-ox+x,-oy+y);
	m_sysbt_rect[2].OffsetRect(-ox+x,-oy+y);
	oy=m_rbt_rect[0].top-1;
	y=0;
	m_rbt_rect[0].OffsetRect(-ox+x,-oy+y);
	m_rbt_rect[1].OffsetRect(-ox+x,-oy+y);
	m_rbt_rect[2].OffsetRect(-ox+x,-oy+y);
	m_rbt_rect[3].OffsetRect(-ox+x,-oy+y);
	m_rbt_rect[4].OffsetRect(-ox+x,-oy+y);

	if(m_sysbt[0] && m_sysbt[1] && m_sysbt[2] && m_sysbt[3])
	{
		m_sysbt[0]->MoveWindow(m_sysbt_rect[0]);
		m_sysbt[1]->MoveWindow(m_sysbt_rect[1]);
		m_sysbt[2]->MoveWindow(m_sysbt_rect[2]);
		m_sysbt[3]->MoveWindow(m_sysbt_rect[1]);
		if(GetParent()->IsZoomed())
		{
			m_sysbt[1]->ShowWindow(SW_SHOW);
			m_sysbt[3]->ShowWindow(SW_HIDE);
		}
		else
		{
			m_sysbt[3]->ShowWindow(SW_SHOW);
			m_sysbt[1]->ShowWindow(SW_HIDE);
		}
	}
	if(m_rbt[0] && m_rbt[1] && m_rbt[2] && m_rbt[3])
	{
		m_rbt[0]->MoveWindow(m_rbt_rect[0]);
		m_rbt[1]->MoveWindow(m_rbt_rect[1]);
		m_rbt[2]->MoveWindow(m_rbt_rect[2]);
		m_rbt[3]->MoveWindow(m_rbt_rect[3]);		
	}
	if(m_rbt[4])
		m_rbt[4]->MoveWindow(m_rbt_rect[4]);
	CRect rc2;
	GetClientRect(&rc2);
	rc2.top=2;
	rc2.bottom=rc2.top+20;
	rc2.left+=m_nHeadIconWid;
	m_icon_point.x=1;
	if(!g_OEM.bZxzqEmbedWTVersion && !g_OEM.bGdzqEmbedWTVersion)
		m_icon_point.y=5;
	else
		m_icon_point.y=0;
	int nLast=-1;
	float xs=GetXSRatio(cx);
	int first_rbt_left = m_rbt_rect[3].left;
	if(g_OEM.bWebButtonInCaption && !g_OEM.bRightWebBtn)
		first_rbt_left = m_rbt_rect[4].left;
	for(UINT n=0;n<m_nButton;n++)
	{
		rc2.right=rc2.left+m_pstr[n].GetLength()*xs;
		m_bt_rect[n]=rc2;
		NextBtRect(rc2,rc2.Width()+1,first_rbt_left-5);
		m_bt[n]->MoveWindow(m_bt_rect[n]);
		if(!m_bt[n]->IsWindowVisible())
			m_bt[n]->ShowWindow(SW_SHOW);
		if(rc2.Width()==0)
		{
			nLast=n;
			break;
		}
	}
	if(nLast!=-1 && nLast!=m_nButton-1)
	{
		for(n=nLast+1;n<m_nButton;n++)
		{
			if(m_bt[n]->IsWindowVisible())
				m_bt[n]->ShowWindow(SW_HIDE);
			m_bt_rect[n].SetRect(0,0,0,0);
		}
	}
}

void CMyCaptionBar::ChangeParentWndSize(UINT sc)
{
	switch(sc)  
	{
	case SC_RESTORE:
		{
			::PostMessage(GetParent()->m_hWnd,WM_SYSCOMMAND, SC_RESTORE, 0);
		}
		break;
	case SC_MAXIMIZE:
		{
			::PostMessage(GetParent()->m_hWnd,WM_SYSCOMMAND, SC_MAXIMIZE, 0);
		}
		break;
	case SC_MINIMIZE:
		{
			::PostMessage(GetParent()->m_hWnd,WM_SYSCOMMAND, SC_MINIMIZE, 0);
		}
		break;
	case SC_CLOSE:
		{
			::PostMessage(GetParent()->m_hWnd,WM_SYSCOMMAND, SC_CLOSE, 0);
		}
		break;
	}
}

void CMyCaptionBar::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	if (GetParent()->IsZoomed())
	{
		ChangeParentWndSize(SC_RESTORE);
	}
	else
	{
		ChangeParentWndSize(SC_MAXIMIZE);
	}

	CDialogBar::OnLButtonDblClk(nFlags, point);
}

void CMyCaptionBar::SetTitleStr(LPCSTR str)
{
	m_titlestr=str;
	Invalidate();
}

void CMyCaptionBar::GetTitleStr(CString &str)
{
	str=m_titlestr;
}
