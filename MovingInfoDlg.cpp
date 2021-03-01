// MovingInfoDlg.cpp : implementation file

#include "stdafx.h"
#include "TdxW.h"
#include "MovingInfoDlg.h"
#include "inifile.h"
#include "mainfrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMovingInfoDlg dialog

CMovingInfoDlg::CMovingInfoDlg(CWnd* pParent /*=NULL*/)
: CDialog(CMovingInfoDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CMovingInfoDlg)
	//}}AFX_DATA_INIT
	m_iTextBoxHeight=28;
	m_iDlgWidth=465;
	this->m_iHeight=0;
	this->m_bMaximized=false;
	this->m_bLeftToRight=true;
	this->m_bRestart=true;

	this->m_bEnableStop=false;
	this->m_bTextStopped=false;
	this->m_bShowSysInfo=true;

	m_nMovingSpaceTime = 50; //系数
	m_nMovingStopTime = 50;

	m_bLayoutFlag = FALSE;

	m_posNow.head = 0;
	m_posNow.tail = 0;
	m_vtPos.clear();
	m_vtSysInfo.clear();
	m_vtNowSysInfo.clear();
}

void CMovingInfoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMovingInfoDlg)
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CMovingInfoDlg, CDialog)
//{{AFX_MSG_MAP(CMovingInfoDlg)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_TIMER()
	ON_WM_DESTROY()
	ON_WM_MOUSEMOVE()
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
	ON_NOTIFY_EX(TTN_NEEDTEXT,0,OnShowToolTip)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMovingInfoDlg message handlers

void CMovingInfoDlg::OnCancel() 
{
}

void CMovingInfoDlg::OnOK() 
{
}

BOOL CMovingInfoDlg::OnShowToolTip(UINT id,NMHDR *pTTTStruct ,LRESULT *pResult)
{
	pResult=NULL;
	id=0;
	TOOLTIPTEXT * pTTT=(TOOLTIPTEXT*)pTTTStruct;
	UINT nid=pTTTStruct->idFrom;
    if(pTTT->uFlags&TTF_IDISHWND)
	{
		nid=::GetDlgCtrlID(HWND(nid));
	}
	switch(nid)
	{
	case IDC_QUIT:
		pTTT->lpszText=_F("隐藏跑马灯");
		break;
	case IDC_SETTING:
		pTTT->lpszText=_F("跑马灯设置");
		break;
	case IDC_RESTORE:
		if(this->m_bMaximized)
		{
			pTTT->lpszText=_F("还原为单行模式");
		}
		else
		{
			pTTT->lpszText=_F("还原为多行模式");
		}
		break;
	}
	return TRUE;	
}

BOOL CMovingInfoDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
    //从ini读出相应的配置
	CIniFile inif;
	inif.SetFileName(g_WSXHStr+"user.ini");
	inif.SetAppName("movinginfo");
	
	int temp=inif.GetInt("scrollstyle",0);
    if(temp!=0&&temp!=1) temp=0;		//0,连续滚动，1，滚动一屏时暂停
	if(temp==0)	m_bEnableStop=false;
	else		m_bEnableStop=true;

	temp=inif.GetInt("infotype",0);
	if(temp!=0&&temp!=1)	temp=0;		//0,只显示系统信息，1,只显示自选股
	if(temp==0) m_bShowSysInfo = true;
	else		m_bShowSysInfo = false;	
	
	//滚动间隔时间,越大越慢
	temp=inif.GetInt("MovingSpace",50);
	if(temp < 1 || temp > 500) temp = 50;
	m_nMovingSpaceTime = temp;

	//滚动停止时间,越大停止时间越长
	temp=inif.GetInt("MovingStop",50);
	if(temp < 2 || temp > 50000) temp = 50;
	m_nMovingStopTime = temp;

	int x=inif.GetInt("left",382),y=inif.GetInt("top",-2);
	m_iDlgWidth=inif.GetInt("width",500);
	x = 3*GetSystemMetrics(SM_CXSCREEN)/10;
	m_iDlgWidth = 4*GetSystemMetrics(SM_CXSCREEN)/10;
	if(m_iDlgWidth < 100 || m_iDlgWidth > ScreenCx*1.5)
		m_iDlgWidth = 500;
	EnableToolTips();
			
    //首先确定显示一行文字的区域的高度，每行高应为字体高度+2（上下各留一个像素的间隔）
	CDC *pDC=GetDC();
	CSize sz=pDC->GetTextExtent("好");
	this->m_iTextBoxHeight=sz.cy+2;
	ReleaseDC(pDC);
	
	m_bLayoutFlag=TRUE;
    SetTimer(0x2345,m_nMovingSpaceTime,NULL);

	return TRUE;   
}

void CMovingInfoDlg::ArrangeCtrl()
{
	if(!m_bLayoutFlag) return;
	int nButtomNum = 0;
    //左边有6个像素留给把手，右边留3个按钮的空间，元素间间隔为1 
	if(this->m_bMaximized==false)
		this->m_rcSingleLine.SetRect(2,0,m_iDlgWidth,m_iTextBoxHeight );
	else
	{
		this->m_rcSingleLine.SetRect(2,0,m_iDlgWidth,m_iTextBoxHeight );
		this->m_rcMultiLine.SetRect(0,m_iTextBoxHeight+1,m_iDlgWidth,4*m_iTextBoxHeight+1);
	}
}

extern void DrawBJWord(CDC *pDC,int x1,int y1,float value1,float value2,BOOL bXSFlag);
extern void DrawJEWord(CDC *pDC,int x1,int y1,float ftmp);

void CMovingInfoDlg::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	CRect rc;
    GetClientRect(rc);
	//画左边的把手以便于拖动,左边的把手占6个像素
	CPen *penOld,pen3DShadow(PS_SOLID,1,RGB(128,128,128)),pen3DLight(PS_SOLID,1,RGB(255,255,255));
    penOld=dc.SelectObject(&pen3DLight);
    dc.MoveTo(rc.left,0);
	dc.LineTo(rc.left, m_iTextBoxHeight-1);
	dc.SelectObject(&pen3DShadow);
	dc.MoveTo(rc.left+1,0);
	dc.LineTo(rc.left+1,m_iTextBoxHeight-1);
	dc.SelectObject(penOld);

    //以下显示跑马灯
    CRect rcbmp;
	if(m_bMaximized)	rcbmp=m_rcMultiLine;
	else				rcbmp=m_rcSingleLine;
	CBitmap bmp,*pOldBmp;
	CDC memDC;
	memDC.CreateCompatibleDC(&dc);
	memDC.SetBkMode(TRANSPARENT);
	bmp.CreateCompatibleBitmap(&dc,rcbmp.Width()+5,rcbmp.Height());
	pOldBmp=memDC.SelectObject(&bmp);
	if(OnLine)
		memDC.FillSolidRect(0,0,rcbmp.Width(),rcbmp.Height(),GridColor.GridBackColor);
	else
		memDC.FillSolidRect(0,0,rcbmp.Width(),rcbmp.Height(),RGB(128,128,128));
	if(this->m_bLeftToRight && m_vtPos.size() > 0)//从左到右
	{
        if(this->m_bShowSysInfo)
		{		
			CFont font ,*pOldFont;
			font.CreateFont(16,0,0,0,FW_MEDIUM,0,0,0,g_b2000XPSys?ANSI_CHARSET:DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,DEFAULT_PITCH|FF_DONTCARE,g_strFontName[0]); //在跑马灯中使用列表字体的字体,不用大小
			pOldFont=memDC.SelectObject(&font);
			for(int i=0;i<this->m_vtNowSysInfo.size();i++)
			{            
				//检查是否需要暂停
				if(this->m_bEnableStop)
				{
					if(m_vtPos[i].head==m_posNow.head&&m_bTextStopped==false)
					{
						this->m_bTextStopped=true;
						this->m_iStopCount=0;
					}
				}
				if((this->m_vtPos[i].head>=this->m_posNow.head&&m_vtPos[i].head<=m_posNow.tail )||
					(m_vtPos[i].tail>=m_posNow.head&&m_vtPos[i].tail<=m_posNow.tail)||
					(m_posNow.head>=m_vtPos[i].head&&m_posNow.head<=m_vtPos[i].tail)||
					(m_posNow.tail>=m_vtPos[i].head&&m_posNow.tail<=m_vtPos[i].tail))
				{
					memDC.SetTextColor(TxtColor.TxtForeColor);
					CRect rc(m_vtPos[i].head-m_posNow.head,0,m_vtPos[i].tail-m_posNow.head,rcbmp.Height());
					memDC.DrawText(m_vtNowSysInfo[i].content,rc,DT_LEFT|DT_SINGLELINE|DT_VCENTER);					
				}				
			}
			memDC.SelectObject(pOldFont);
		}
		else
		{
			int i,from=0;
			CString str,strtemp;
			CRect rc;
			MemStkInfo *pStkInfo = NULL;
			struct CurrStockData CurrHq;
			short setcode;
			char  gpcode[CODE_SIZE]={0};
			short m_XsFlag = 2;
			CFont *oldfont = g_d.SelectFont(&memDC,BOLDGRAPH_FONT);
			for(i=0;i<g_nMovingZXG;i++)
			{
				CString tempLine = (CString)(g_strMovingZXG[i]);
				int tmpb=tempLine.Find("#",0);
				int tmp = (tmpb==-1?0:tmpb);
				CString tempLine2 = tempLine.Mid(tmp+1);
				setcode = tmpb>=0?atoi(tempLine):tempLine[0]-'0';

				memcpy(gpcode,tempLine2.GetBuffer(CODE_SIZE-1),CODE_SIZE-1);
				gpcode[CODE_SIZE] = 0;
				pStkInfo = g_pStockIO->Get_MemStkInfo(gpcode,setcode);
				if(!pStkInfo) continue;
				g_pStockIO->ReadCurrStockData(pStkInfo,&CurrHq); //取得行情
				m_XsFlag = GetXSFlag(pStkInfo);
                //检查是否需要暂停
				if(this->m_bEnableStop)
				{
					if(m_vtPos[i].head==m_posNow.head&&m_bTextStopped==false)
					{
						this->m_bTextStopped=true;
						this->m_iStopCount=0;
					}
				}
				if(  (this->m_vtPos[i].head>=this->m_posNow.head&&m_vtPos[i].head<=m_posNow.tail )||
					(m_vtPos[i].tail>=m_posNow.head&&m_vtPos[i].tail<=m_posNow.tail)||
					(m_posNow.head>=m_vtPos[i].head&&m_posNow.head<=m_vtPos[i].tail)||
					(m_posNow.tail>=m_vtPos[i].head&&m_posNow.tail<=m_vtPos[i].tail))
				{
					int tmpInt = m_vtPos[i].head-m_posNow.head;
					g_d.DisplayText(&memDC,tmpInt+1,0,GridColor.Grid_CodeName_Color,"%8s",pStkInfo->Name);
					g_d.BjPrintf(&memDC,tmpInt+60,0,CurrHq.Now,CurrHq.Close,m_XsFlag,60);
					g_d.BjPrintf(&memDC,tmpInt+120,0,CurrHq.Now-CurrHq.Close,0.0,m_XsFlag,45);
					g_d.BjPrintf(&memDC,tmpInt+165,0,(CurrHq.Now-CurrHq.Close)*100/CurrHq.Close,0.0,m_XsFlag,60,1);
					g_d.DisplayText(&memDC,tmpInt+225,0,VipColor.VOLCOLOR,60,MakeVol2(CurrHq.Volume));
					g_d.NoBjPrintf(&memDC,tmpInt+285,0,GridColor.Grid_Level_Color,CurrHq.Close,m_XsFlag,55);
				}
			} 
			g_d.RestoreFont(&memDC,oldfont);
		}
	}
	else if(m_vtPos.size() > 0) //从下到上
	{
		if(this->m_bShowSysInfo)
		{
			CFont font ,*pOldFont;
			font.CreateFont(16,0,0,0,FW_MEDIUM,0,0,0,g_b2000XPSys?ANSI_CHARSET:DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,DEFAULT_PITCH|FF_DONTCARE,g_strFontName[0]); //在跑马灯中使用列表字体的字体,不用大小
			pOldFont=memDC.SelectObject(&font);
			for(int i=0;i<this->m_vtNowSysInfo.size();i++)
			{
                //检查是否需要暂停
				if(this->m_bEnableStop)
				{
					CSize sz;
					sz=memDC.GetTextExtent("好"); //通过计算，使上下滚动的文字刚好在中间就暂停一下
					if(m_vtPos[i].head==m_posNow.head&&m_bTextStopped==false)
					{
						if(this->m_bMaximized)
						{
							if(i%3==0)//此处每三行停一次
							{
								this->m_bTextStopped=true;
								this->m_iStopCount=0;
							}
						}
						else
						{
							this->m_bTextStopped=true;
							this->m_iStopCount=0;
						}
					}
				}
				if(  (this->m_vtPos[i].head>=this->m_posNow.head&&m_vtPos[i].head<=m_posNow.tail )||
					(m_vtPos[i].tail>=m_posNow.head&&m_vtPos[i].tail<=m_posNow.tail)||
					(m_posNow.head>=m_vtPos[i].head&&m_posNow.head<=m_vtPos[i].tail)||
					(m_posNow.tail>=m_vtPos[i].head&&m_posNow.tail<=m_vtPos[i].tail))
				{
					memDC.SetTextColor(TxtColor.TxtForeColor);
					CRect rc(0,m_vtPos[i].head-m_posNow.head,rcbmp.Width(),m_vtPos[i].tail-m_posNow.head );
					memDC.DrawText(m_vtNowSysInfo[i].content,rc,DT_LEFT|DT_SINGLELINE|DT_VCENTER);
				}
			}
			memDC.SelectObject(pOldFont);
		}
		else
		{
			int i,from=0;
			CString str,strtemp;
			CRect rc;
			MemStkInfo *pStkInfo = NULL;
			struct CurrStockData CurrHq;
			short setcode;
			char  gpcode[CODE_SIZE];
			short m_XsFlag = 2;
			CFont *oldfont = g_d.SelectFont(&memDC,BOLDGRAPH_FONT);
			for(i=0;i<g_nMovingZXG;i++)
			{                
				setcode = (g_strMovingZXG[i][0]=='0')?0:1;
				memcpy(gpcode,g_strMovingZXG[i]+1,CODE_SIZE-1);
				gpcode[CODE_SIZE] = 0;
				pStkInfo = g_pStockIO->Get_MemStkInfo(gpcode,setcode);
				if(!pStkInfo) continue;
				g_pStockIO->ReadCurrStockData(pStkInfo,&CurrHq); //取得行情
				m_XsFlag = GetXSFlag(pStkInfo);
				//检查是否需要暂停
				if(this->m_bEnableStop)
				{
					CSize sz;
					sz=memDC.GetTextExtent("好"); //通过计算，使上下滚动的文字刚好在中间就暂停一下,
					if(m_vtPos[i].head==m_posNow.head&&m_bTextStopped==false)
					{
						if(this->m_bMaximized)
						{
							if(i%3==0)//此处每三行停一次
							{
								this->m_bTextStopped=true;
								this->m_iStopCount=0;
							}
						}
						else
						{
							this->m_bTextStopped=true;
							this->m_iStopCount=0;
						}
					}
				}
				if(  (this->m_vtPos[i].head>=this->m_posNow.head&&m_vtPos[i].head<=m_posNow.tail )||
					(m_vtPos[i].tail>=m_posNow.head&&m_vtPos[i].tail<=m_posNow.tail)||
					(m_posNow.head>=m_vtPos[i].head&&m_posNow.head<=m_vtPos[i].tail)||
					(m_posNow.tail>=m_vtPos[i].head&&m_posNow.tail<=m_vtPos[i].tail))
				{
					g_d.DisplayText(&memDC,1,m_vtPos[i].head-m_posNow.head,GridColor.Grid_CodeName_Color,"%6s %8s",pStkInfo->Code,pStkInfo->Name);
					g_d.BjPrintf(&memDC,110,m_vtPos[i].head-m_posNow.head,CurrHq.Now,CurrHq.Close,m_XsFlag,60);
					g_d.BjPrintf(&memDC,170,m_vtPos[i].head-m_posNow.head,CurrHq.Now-CurrHq.Close,0.0,m_XsFlag,45);
					g_d.BjPrintf(&memDC,215,m_vtPos[i].head-m_posNow.head,(CurrHq.Now-CurrHq.Close)*100/CurrHq.Close,0.0,m_XsFlag,60,1);
					g_d.DisplayText(&memDC,275,m_vtPos[i].head-m_posNow.head,VipColor.VOLCOLOR,60,MakeVol2(CurrHq.Volume));
					g_d.NoBjPrintf(&memDC,335,m_vtPos[i].head-m_posNow.head,GridColor.Grid_Level_Color,CurrHq.Close,m_XsFlag,55);
				}
			} 
			g_d.RestoreFont(&memDC,oldfont);
		}
	}
	dc.BitBlt(rcbmp.left,rcbmp.top,rcbmp.Width(),rcbmp.Height(),&memDC,0,0,SRCCOPY);
	memDC.SelectObject(pOldBmp);
	memDC.DeleteDC();
	bmp.DeleteObject();
}

void CMovingInfoDlg::OnLButtonDown(UINT nFlags, CPoint point) 
{
    //如果点在把手上则可以拖动
    CRect rcGrip,rcDlg;
	GetWindowRect(rcDlg);
    rcGrip.SetRect(0,0,5,rcDlg.Height()-1);
	if(rcGrip.PtInRect(point))
	{
		HCURSOR h=::LoadCursor(NULL,IDC_SIZEALL);
		::SetCursor(h);
	}
//	this->SendMessage(WM_NCLBUTTONDOWN,HTCAPTION,0);
	if(this->m_bShowSysInfo)
	{
		CRect rc;
		if(this->m_bMaximized)
			rc=this->m_rcMultiLine;
		else
			rc=this->m_rcSingleLine;
		if(rc.PtInRect(point))
		{
			for(int i=0;i<this->m_vtNowSysInfo.size();i++)
			{
				CRect rcText;
				if(this->m_bLeftToRight)
					rcText.SetRect(m_vtPos[i].head-m_posNow.head,0,m_vtPos[i].tail-m_posNow.head,rc.Height());
				else
				{
                    if(this->m_bMaximized==false)
						rcText.SetRect(0,m_vtPos[i].head-m_posNow.head,rc.Width(),m_vtPos[i].tail-m_posNow.head );
					else
						rcText.SetRect(0,m_vtPos[i].head-m_posNow.head+1+this->m_iTextBoxHeight+1,rc.Width(),m_vtPos[i].tail-m_posNow.head-1+this->m_iTextBoxHeight+1 );
				}				
				if(rcText.PtInRect(point)&&m_vtNowSysInfo[i].url[0]!='\0')
					ShellExecute(NULL, _T("open"), m_vtNowSysInfo[i].url, NULL, NULL, SW_SHOW);				
			}
		}
	}
	CDialog::OnLButtonDown(nFlags, point);
}

void CMovingInfoDlg::OnTimer(UINT nIDEvent) 
{
	static bool InOnTimer=false;//避免重入
	if(nIDEvent==0x2345 && InOnTimer==false)
	{
		InOnTimer=true;
		if(this->m_bRestart)
		{
			m_bRestart=false;
			InitNext();
		}
		if(::IsWindow(GetSafeHwnd()))
		{
			if(this->m_bMaximized)
			{
				InvalidateRect(this->m_rcSingleLine,FALSE);
				InvalidateRect(this->m_rcMultiLine,FALSE);
			}
			else
				InvalidateRect(this->m_rcSingleLine,FALSE);			
			CRect rc(0,0,6,this->m_iTextBoxHeight);
			InvalidateRect(rc,FALSE);
		}
		if(this->m_bEnableStop)
		{
			if(this->m_bTextStopped)
			{
				this->m_iStopCount++;
				if(m_iStopCount==m_nMovingStopTime)
				{
					m_bTextStopped=false;
					this->m_posNow.head++;
					this->m_posNow.tail++;					
				}
			}
			else
			{
                this->m_posNow.head++;
				this->m_posNow.tail++;
			}
		}
		else
		{
			this->m_posNow.head++;
			this->m_posNow.tail++;
		}
		
		//计算何时进行下一次滚动
		if(this->m_vtPos.size())
		{
			if(m_posNow.head > m_vtPos.back().tail)
				m_bRestart=true;
		}
		InOnTimer=false;
	}
	CDialog::OnTimer(nIDEvent);
}

void CMovingInfoDlg::OnDestroy() 
{	
	KillTimer(0x2345);
	CRect rc;
	GetWindowRect(rc);
	CIniFile inif;
	inif.SetFileName(g_WSXHStr+"user.ini");
	inif.SetAppName("movinginfo");
	inif.PutInt(rc.left,"left");
	inif.PutInt(rc.top,"top");
	inif.PutInt(rc.Width()-4,"width");
	CDialog::OnDestroy();
}

void CMovingInfoDlg::InitNext()
{
	this->m_vtPos.clear();
	CPaintDC dc(this);
	CRect rc;
	if(this->m_bMaximized)
		rc=this->m_rcMultiLine;
	else
		rc=this->m_rcSingleLine;
	//为了使从左到右的文字有间隔，每条文字之间都会间隔四个空格
	if(this->m_bShowSysInfo)
	{
		this->m_vtNowSysInfo.clear();
		for(int i=0;i<m_vtSysInfo.size();i++)
			m_vtNowSysInfo.push_back(m_vtSysInfo[i]);
		if(m_vtNowSysInfo.size()<=0)	return;
		if(this->m_bLeftToRight)//从左到右
		{
			this->m_posNow.head=-rc.Width();
			this->m_posNow.tail=0;
			POS pos;
			CString str;
			int i;
			CSize sz;
			int len=0;
			for(i=0;i<m_vtNowSysInfo.size();i++)
			{
				str.Format("    %s                       ",m_vtNowSysInfo[i].content);
				sz=dc.GetTextExtent(str);
				sz.cx+=30;
				pos.head=len;
				pos.tail=pos.head+sz.cx;
				this->m_vtPos.push_back(pos);
				len=pos.tail;
			}
		}
		else//从下到上
		{
			this->m_posNow.head=-rc.Height();
			this->m_posNow.tail=0;
			int len=1;
			POS pos;
			CSize sz=dc.GetTextExtent("好");
			for(i=0;i<m_vtNowSysInfo.size();i++)
			{
				pos.head=len;
				pos.tail=pos.head+sz.cy;
				pos.tail+=2;//每行之间间隔 
				m_vtPos.push_back(pos);
				len=pos.tail;
			}
		}		
	}
	else
	{	
		if(this->m_bLeftToRight)//从左到右，对于股票信息来说，每行长度都一样，所以我们只需要计算第一只股票的长度就可以了
		{
			this->m_posNow.head=-rc.Width();
			this->m_posNow.tail=0;
			POS pos;
			int len=0;
			for(int i=0;i<g_nMovingZXG;i++)
			{
				pos.head=len;
				pos.tail=pos.head+340+10;
				m_vtPos.push_back(pos);
				len=pos.tail;
			}
		}
		else//从下到上
		{
			this->m_posNow.head=-rc.Height();
			this->m_posNow.tail=0;
			int len=1;
			POS pos;
			CSize sz=dc.GetTextExtent("好");
			for(int i=0;i<g_nMovingZXG;i++)
			{
				pos.head=len;
				pos.tail=pos.head+sz.cy;
				pos.tail+=2;//每行之间间隔 
				m_vtPos.push_back(pos);
				len=pos.tail;
			}
		}
	}	
}

void CMovingInfoDlg::SetScrollDirection(bool bLeftToRight)
{
	this->m_bLeftToRight=bLeftToRight;
}

void CMovingInfoDlg::SetScrollInfo(bool bShowSysInfo)
{
	this->m_bShowSysInfo=bShowSysInfo;
}

void CMovingInfoDlg::SetScrollStyle(bool bEnableStop)
{
	this->m_bEnableStop=bEnableStop;
}

void CMovingInfoDlg::OnMouseMove(UINT nFlags, CPoint point) 
{
	//以下检查是否鼠标在一个超链接上
	if(this->m_bShowSysInfo)
	{
		CRect rc;
		if(this->m_bMaximized)
		{
			rc=this->m_rcMultiLine;
		}
		else
		{
			rc=this->m_rcSingleLine;
		}
		if(rc.PtInRect(point))
		{
			int i;
			for(i=0;i<this->m_vtNowSysInfo.size();i++)
			{
				CRect rcText;
				if(this->m_bLeftToRight)
				{
					rcText.SetRect(m_vtPos[i].head-m_posNow.head,0,m_vtPos[i].tail-m_posNow.head,rc.Height());
				}
				else
				{
                    if(this->m_bMaximized==false)
					{
						rcText.SetRect(0,m_vtPos[i].head-m_posNow.head,rc.Width(),m_vtPos[i].tail-m_posNow.head );
					}
					else
					{
						rcText.SetRect(0,m_vtPos[i].head-m_posNow.head+1+this->m_iTextBoxHeight+1,rc.Width(),m_vtPos[i].tail-m_posNow.head-1+this->m_iTextBoxHeight+1 );
					}
				}
				
				if(rcText.PtInRect(point)&&m_vtNowSysInfo[i].url[0]!='\0')
					SetCursor(AfxGetApp()->LoadCursor(IDC_MOVE2));
			}
		}
	}
	CDialog::OnMouseMove(nFlags, point);
}

void CMovingInfoDlg::AddSysInfo(SYS_INFO si)
{
	for(int i=0;i<m_vtSysInfo.size();i++)
	{
		if(memcmp(&si, &m_vtSysInfo[i],sizeof(SYS_INFO))==0)
			return;
	}
	m_vtSysInfo.push_back(si);
}

void CMovingInfoDlg::DeleteAllSysInfo()
{
	m_vtSysInfo.clear();
}

void CMovingInfoDlg::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);
	
	if(cx > 100) m_iDlgWidth=cx;
	ArrangeCtrl();
	
}