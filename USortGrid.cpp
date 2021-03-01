// USortGrid.cpp: implementation of the USortGrid class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "tdxw.h"
#include "USortGrid.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
USortGrid::USortGrid(LPCOMINFO pComInfo):UBase(pComInfo)
{
	WhichType = 6;
	m_nCurSel = -1;
	m_bHasData = TRUE;
	m_nMainID = 0;
	m_bRefreshDraw = FALSE;
	m_bAscendSort = FALSE;
	memset(TypeStr,0,sizeof(char)*MAX_ROW_NUM*25);

	for(int i=0;i<MAX_ROW_NUM;i++)
	{
		TypeFlag[i] = 0;
		SortDate[i] = -1;
	}
	m_CalcCode = "100*(CLOSE-REF(CLOSE,1))/REF(CLOSE,1);";

	CalcSomeInfo();

	memset(m_ZHBuf,0,sizeof(SORTRESULT)*MAX_ROW_NUM);
	memset(&m_ZHBuf[MAX_ROW_NUM-1],0,sizeof(SORTRESULT));
	m_bNeedHZ = TRUE;

	m_nItems = MAX_ROW_ITEM;

	m_pVScrollBar = NULL;
	m_pVScrollBar=new CMyScrollBar;
	if(!m_pVScrollBar || !m_pVScrollBar->Create(_T("MyScrollBar"),"",WS_CHILD|WS_VISIBLE,CRect(0,0,0,0),m_pView,ID_QJ_VSCROLL))
		return;
	m_pVScrollBar->Init(X_VERTICAL,m_pView->m_hWnd,UM_VSCROLLBAR,pComInfo->nWhichUnit);
	nStartRow = 0;
}

USortGrid::~USortGrid()
{
	TDEL(m_pVScrollBar);
}
//////////////////////////////////////////////////////////////////////////
void	USortGrid::SetControlPos(CWnd *pWnd, CRect &rect)
{
	if(!m_pVScrollBar) return;
	m_pVScrollBar->SetWindowPos(pWnd,left+rect.right-16,top+rect.top,16,rect.Height(),SWP_SHOWWINDOW);
	m_pVScrollBar->SetScrollPos(m_pVScrollBar->GetScrollPos());
}

//重新组织滚动条
void USortGrid::RefreshScrollBar()
{
	m_pVScrollBar->SetScrollColor(VipColor.AXISCOLOR,1,VipColor.BACKCOLOR,VipColor.AXISCOLOR);
	m_pVScrollBar->SetScrollRange (0,MAX_ROW_ITEM-1);
	m_pVScrollBar->SetScrollPos(0);
	
	SetControlPos(NULL,m_rcIn);
}

BOOL	USortGrid::InitDPDate()
{
	long lReadNum = g_pCalc->_CallDTopViewCalc_4(SH_DPCODE,SH,PER_DAY, ASK_ALL,1);
	LPHISDAT pAnalyData = (LPHISDAT)g_pCalc->_CallDTopViewCalc_15();
	if (pAnalyData == NULL||lReadNum < MAX_ROW_NUM) return FALSE;
	for(int i=MAX_ROW_NUM-2;i>=0;i--)
		SortDate[(MAX_ROW_NUM-2)-i] = 10000*pAnalyData[lReadNum-1-i].Time.year+100*pAnalyData[lReadNum-1-i].Time.month+pAnalyData[lReadNum-1-i].Time.day;

	for(i=0;i<MAX_ROW_NUM-1;i++)
		sprintf(TypeStr[i], "%04d/%02d/%02d", SortDate[i]/10000, (SortDate[i]/100)%100, SortDate[i]%100);
	strcpy(TypeStr[MAX_ROW_NUM-1], "排行汇总");

	return TRUE;
}

void	USortGrid::CalcSomeInfo()
{
	for(int i=0;i<MAX_ROW_NUM;i++)
		TypeFlag[i] = 1;
	
	m_nUnitCol = MAX_ROW_NUM;
	m_nUnitRow = 1;
	m_nRowNum = MAX_ROW_ITEM;
	m_nItemHeight = 21;
	m_nCurSel = 0;
}

void USortGrid::DrawOneCell(CDC *pDC,int type)
{
	int i;
	CRect rc = CellRect[type];
	//先写标题
	CSize sz = g_d.DisplayText(pDC,rc.left,rc.top,VipColor.BACKCOLOR,0,_F(TypeStr[type]));
	CRect	rcTitle(rc.left, rc.top, rc.right, rc.top+m_nItemHeight);
	g_d.Bar(pDC,rcTitle,BACK_BRUSH);
	g_d.Draw3DBar3(pDC, rcTitle, RGB(64,0,0), FALSE);
	int nLLMore = max((rc.Width()-sz.cx)/2,1);
	int nTopMore = max((m_nItemHeight-sz.cy)/2, 2);
	g_d.DisplayText(pDC,rc.left+nLLMore,rc.top+nTopMore,VipColor.BIGTITLECOLOR,0,_F(TypeStr[type]));

	MemStkInfo *tempStkInfo;
	int xsflag;
	CSize size;

	float fClose;
	for(i = 0;i < m_nRowNum;i++)
	{
		int nRealItemRow = i+nStartRow;
		tempStkInfo = g_pStockIO->Get_MemStkInfo(m_ZHBuf[type].Code[nRealItemRow],m_ZHBuf[type].SetCode[nRealItemRow]);
		if(!tempStkInfo) break;
		fClose = g_pStockIO->GetStockClose(tempStkInfo);
		xsflag = GetXSFlag(tempStkInfo);
		BOOL bSel = (strcmp(m_ZHBuf[m_nCurSel/m_nItems].Code[m_nCurSel%m_nItems], m_ZHBuf[type].Code[nRealItemRow])==0);
		if(type==MAX_ROW_NUM-1)
			size = g_d.DisplaySameWidtchText(pDC,rc.left+nLLMore/3,rc.top+m_nItemHeight+m_nItemHeight*i+nTopMore,bSel?GridColor.Grid_CodeName_Color:GetBlockGPColor(tempStkInfo,RGB(128,255,255)),"%d:%s",(int)m_ZHBuf[type].Other[nRealItemRow],tempStkInfo->Name);
		else size = g_d.DisplaySameWidtchText(pDC,rc.left+nLLMore/3,rc.top+m_nItemHeight+m_nItemHeight*i+nTopMore,bSel?GridColor.Grid_CodeName_Color:GetBlockGPColor(tempStkInfo,RGB(128,255,255)),tempStkInfo->Name);
		CSize zs=DrawMarkFlag(tempStkInfo,pDC,rc.left+nLLMore/3+size.cx, rc.top+m_nItemHeight+m_nItemHeight*i-1,0,0,m_pView);
		if(tempStkInfo->fl_flag>0)
			size.cx += zs.cx;
		if(m_nCurSel == type*MAX_ROW_ITEM+nRealItemRow)
			g_d.DrawRect(pDC,rc.left,rc.top+m_nItemHeight+m_nItemHeight*i,rc.left+nLLMore+2+size.cx+4,rc.top+m_nItemHeight-1+m_nItemHeight*i+m_nItemHeight);
	}

	if(!m_bNeedHZ||type==MAX_ROW_NUM-1) return;
	//排序最出风头的
	for(i = 0;i < MAX_ROW_ITEM;i++)
	{
		tempStkInfo = g_pStockIO->Get_MemStkInfo(m_ZHBuf[type].Code[i],m_ZHBuf[type].SetCode[i]);
		if(!tempStkInfo) break;
		
		BOOL bSettle = FALSE;
		for(int j=0;j<MAX_ROW_ITEM*(MAX_ROW_NUM-1);j++)
		{
			if(m_ZHBuf[MAX_ROW_NUM-1].SetCode[j]==tempStkInfo->setcode&&strcmp(m_ZHBuf[MAX_ROW_NUM-1].Code[j],tempStkInfo->Code)==0)		//找到了
			{
				m_ZHBuf[MAX_ROW_NUM-1].Other[j] += 1.0f;
				if(j<=0) break;
				for(int k=j;k>0;k--)
				{
					if(k<=0)
					{
						bSettle = TRUE;
						break;
					}
					if(!m_bAscendSort&&m_ZHBuf[MAX_ROW_NUM-1].Other[k]>m_ZHBuf[MAX_ROW_NUM-1].Other[k-1]+0.5f)					//交换
					{
						char SetCode = m_ZHBuf[MAX_ROW_NUM-1].SetCode[k];
						m_ZHBuf[MAX_ROW_NUM-1].SetCode[k] = m_ZHBuf[MAX_ROW_NUM-1].SetCode[k-1];
						m_ZHBuf[MAX_ROW_NUM-1].SetCode[k-1] = SetCode;
						
						char Code[CODE_SIZE-1]={0};
						strncpy(Code, m_ZHBuf[MAX_ROW_NUM-1].Code[k],CODE_SIZE-1);
						strncpy(m_ZHBuf[MAX_ROW_NUM-1].Code[k],m_ZHBuf[MAX_ROW_NUM-1].Code[k-1],CODE_SIZE-1);
						strncpy(m_ZHBuf[MAX_ROW_NUM-1].Code[k-1],Code,CODE_SIZE-1);
						
						float Now = m_ZHBuf[MAX_ROW_NUM-1].Now[k];
						m_ZHBuf[MAX_ROW_NUM-1].Now[k] = m_ZHBuf[MAX_ROW_NUM-1].Now[k-1];
						m_ZHBuf[MAX_ROW_NUM-1].Now[k-1] = Now;
						
						float Other = m_ZHBuf[MAX_ROW_NUM-1].Other[k];
						m_ZHBuf[MAX_ROW_NUM-1].Other[k] = m_ZHBuf[MAX_ROW_NUM-1].Other[k-1];
						m_ZHBuf[MAX_ROW_NUM-1].Other[k-1] = Other;
					}
					else if(!m_bAscendSort)
					{
						bSettle = TRUE;
						break;
					}
				}
				if(bSettle) 
					break;
			}
			else if(m_ZHBuf[MAX_ROW_NUM-1].Code[j][0]==0)			//没有，空的
			{
				m_ZHBuf[MAX_ROW_NUM-1].SetCode[j]=tempStkInfo->setcode;
				strncpy(m_ZHBuf[MAX_ROW_NUM-1].Code[j], tempStkInfo->Code, CODE_SIZE-1);
				m_ZHBuf[MAX_ROW_NUM-1].Now[j] = fClose;
				m_ZHBuf[MAX_ROW_NUM-1].Other[j]=1.0f;
				break;
			}
		}
	}
	int nRealNum = min(MAX_ROW_ITEM, nGpNum);
	for(i=nRealNum;i<MAX_ROW_ITEM*(MAX_ROW_NUM-1);i++)
	{
		m_ZHBuf[MAX_ROW_NUM-1].SetCode[i] = -1;
		memset(m_ZHBuf[MAX_ROW_NUM-1].Code[i], 0, sizeof(m_ZHBuf[MAX_ROW_NUM-1].Code[i]));
		m_ZHBuf[MAX_ROW_NUM-1].Now[i] = 0.0f;
		m_ZHBuf[MAX_ROW_NUM-1].Other[i] = 0.0f;
	}
}

#include "QuickFlushTjDlg.h"
void USortGrid::Sort()
{
	InitDPDate();
//	RefreshAllHq(60);

	if(WhichType==TYPE_LC)
	{
	//	nGpNum = FiltLCCode(lpnUsedStkIndex);
		nGpNum = g_pConstMng->GetZJHHYBlockNum();
		int nRet = 0;
		for(int i = 0;i < nGpNum;i++)
		{
			ZJHHY_Cfg* pHyCfg = g_pConstMng->GetHyFromHYXH(i);
			long nIDIndex = g_pStockIO->GetIndex(pHyCfg->HYCode, SZ);
			if(nIDIndex>=0) 
				lpnUsedStkIndex[nRet++] = nIDIndex;
		}
		nGpNum = nRet;
	}
	else if (WhichType<100)
	{
		int		nSetDomain;          //SZ/SH/SZ_SH
		union   flunion ftype;       //股票过滤条件
		nSetDomain = lpnSysDomainType[WhichType];
		ftype.fshort = lpsSysClassType_new[WhichType];
		//取使用股票
		nGpNum = g_pStockIO->FiltCode(nSetDomain, ftype, lpnUsedStkIndex);
	}
	if(nGpNum > 0) m_nCurSel = 0;

	memset(m_ZHBuf,0,sizeof(SORTRESULT)*MAX_ROW_NUM);
	memset(&m_ZHBuf[MAX_ROW_NUM-1],0,sizeof(SORTRESULT));
	m_bNeedHZ = TRUE;

	CQuickFlushTjDlg *pDlg = new CQuickFlushTjDlg(m_pView);
	pDlg->Create(IDD_FLUSHTJBLOCK_DIALOG);
	pDlg->CenterWindow();
	pDlg->ShowWindow (SW_SHOW);
	pDlg->m_ctlProgress.SetRange32(0,nGpNum);
	pDlg->m_ctlProgress.SetStep(1);
	pDlg->m_ctlProgress.SetPos(0);
	pDlg->GetDlgItem(IDC_STATIC1)->SetWindowText("正在统计多路排行数据...");

	for (int i = 0;i < nGpNum;i++) 
	{
		pDlg->m_ctlProgress.StepIt();
		MemStkInfo *pStock = (*g_pStockIO)[lpnUsedStkIndex[i]];
		if(!pStock) continue;
		float *fPut[MAX_LINEOUT]={0};

		short nResNum = -1;
		LPHISDAT pAnalyData = NULL;
		//行业指数
		long lIdxNo = -1;
		if(g_pLcIdxMng&&g_pLcIdxMng->TestLCIndex(pStock->Code, lIdxNo))
		{
			nResNum = g_pCalc->_CallDTopViewCalc_5((char*)(LPCSTR)m_CalcCode,pStock->setcode,pStock->Code,fPut);
			pAnalyData = (LPHISDAT)g_pCalc->_CallDTopViewCalc_15();
			if (pAnalyData == NULL||nResNum < 20) 
			{
				if(g_pLcIdxMng->ReCalc(lIdxNo))
				{
					nResNum = g_pCalc->_CallDTopViewCalc_5((char*)(LPCSTR)m_CalcCode,pStock->setcode,pStock->Code,fPut);
					pAnalyData = (LPHISDAT)g_pCalc->_CallDTopViewCalc_15();
				}
			}
			if (pAnalyData == NULL||nResNum < 20)
				continue;
		}
		else
		{
			nResNum = g_pCalc->_CallDTopViewCalc_5((char*)(LPCSTR)m_CalcCode,pStock->setcode,pStock->Code,fPut);
			pAnalyData = (LPHISDAT)g_pCalc->_CallDTopViewCalc_15();
			if (pAnalyData == NULL||nResNum < 1) 
				continue;
		}

		PeekMsg();
		
		BOOL bSettleOK = FALSE;
		for(int j=MAX_ROW_NUM-1;j>=0;j--)
		{
			bSettleOK = FALSE;
			long nDate = SortDate[j];
			for(int k=nResNum-1;k>=0;k--)
			{
				if(bSettleOK) break;
				long nKDate = 10000*pAnalyData[k].Time.year+100*pAnalyData[k].Time.month+pAnalyData[k].Time.day;
				if(nKDate==nDate)			//找到了
				{
					float fData = fPut[0][k];
					for(int m=0;m<MAX_ROW_ITEM;m++)
					{
						if(m_ZHBuf[j].Code[m][0]==0)				//空的
						{
							m_ZHBuf[j].SetCode[m] = pStock->setcode;
							strncpy(m_ZHBuf[j].Code[m], pStock->Code, CODE_SIZE-1);
							m_ZHBuf[j].Now[m] = pAnalyData[k].Close;
							m_ZHBuf[j].Other[m] = fData;
							bSettleOK = TRUE;
							break;
						}
						if(m_bAscendSort)				//要小的
						{
							if(fData<m_ZHBuf[j].Other[m])
							{
								for(int n=MAX_ROW_ITEM-1;n>m;n--)
								{
									m_ZHBuf[j].SetCode[n] = m_ZHBuf[j].SetCode[n-1];
									strncpy(m_ZHBuf[j].Code[n], m_ZHBuf[j].Code[n-1], CODE_SIZE-1);
									m_ZHBuf[j].Now[n] = m_ZHBuf[j].Now[n-1];
									m_ZHBuf[j].Other[n] = m_ZHBuf[j].Other[n-1];
								}
								m_ZHBuf[j].SetCode[m] = pStock->setcode;
								strncpy(m_ZHBuf[j].Code[m], pStock->Code, CODE_SIZE-1);
								m_ZHBuf[j].Now[m] = pAnalyData[k].Close;
								m_ZHBuf[j].Other[m] = fData;
								bSettleOK = TRUE;
								break;
							}
						}
						else						//要大的
						{
							if(fData>m_ZHBuf[j].Other[m])
							{
								for(int n=MAX_ROW_ITEM-1;n>m;n--)
								{
									m_ZHBuf[j].SetCode[n] = m_ZHBuf[j].SetCode[n-1];
									strncpy(m_ZHBuf[j].Code[n], m_ZHBuf[j].Code[n-1], CODE_SIZE-1);
									m_ZHBuf[j].Now[n] = m_ZHBuf[j].Now[n-1];
									m_ZHBuf[j].Other[n] = m_ZHBuf[j].Other[n-1];
								}
								m_ZHBuf[j].SetCode[m] = pStock->setcode;
								strncpy(m_ZHBuf[j].Code[m], pStock->Code, CODE_SIZE-1);
								m_ZHBuf[j].Now[m] = pAnalyData[k].Close;
								m_ZHBuf[j].Other[m] = fData;
								bSettleOK = TRUE;
								break;
							}
						}
					}
				}
				else if(nKDate<nDate)
					break;
			}
		}
	}
	pDlg->DestroyWindow();
	TDEL(pDlg);
}

int USortGrid::HitTest(CPoint point)
{
	CRect rc,rc2;
	int i,j;
	for(i = 0;i<MAX_ROW_NUM;i++)
	{
		rc = CellRect[i];
		if(rc.PtInRect(point)) break;
	}
	if(i >= MAX_ROW_NUM) return -1;
	rc2.CopyRect(&rc);
	rc2.left += 5;
	for(j = 0;j < min(m_nRowNum,nGpNum);j++)
	{
		rc2.top = rc.top+21+m_nItemHeight*j;
		rc2.bottom = rc.top+21+m_nItemHeight*(j+1);
		if(rc2.PtInRect(point))
			return i*MAX_ROW_ITEM+nStartRow+j;
	}
	return -1;
}

static long s_scrolltick=GetTickCount();
void USortGrid::OnVScrollMessage(WPARAM wParam,LPARAM lParam)
{
	if(GetTickCount() - s_scrolltick < 30) return;
	s_scrolltick=GetTickCount();
	
	int nPos = (int)wParam;
	if(nPos != nStartRow)
	{
		nStartRow=min(nPos, MAX_ROW_ITEM-m_nUnitRow);
		ProcessMsg(WM_PAINT);
	}
	m_pVScrollBar->SetScrollPos(nPos);	
}

int  USortGrid::ProcessMsg(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{
	case UM_GET_STOCK:
		return g_pStockIO->GetIndex(m_ZHBuf[m_nCurSel/m_nItems].Code[m_nCurSel%m_nItems]);
	case WM_PAINT:
		{
			if(wParam == 1)
				ProcessMsg(UM_ZH_READSORT);
			
			CDC *pDC = m_pView->GetDC();   
			CBitmap		tempBitmap;				//作出临时位图
			CDC memDC;							//作出内存DC
			memDC.CreateCompatibleDC (pDC);
			tempBitmap.CreateCompatibleBitmap (pDC,m_rcIn.Width (),m_rcIn.Height());
			CBitmap *pOldBitmap = memDC.SelectObject (&tempBitmap);				
			g_d.Bar(&memDC,&m_rcIn,BACK_BRUSH);
			memDC.SetBkMode (TRANSPARENT);
			CFont *oldfont = g_d.SelectFont(&memDC,SMALL2_FONT);
			CPen *oldpen = g_d.SelectPen(&memDC,AXIS_PEN);

			int nTitleHeight = 0;
			int nXhWidth = 20;
			//计算每个排名区的Rect
			int i,j,tmpWidth = (m_rcIn.Width()-nXhWidth-16)/m_nUnitCol,tmpHeight = (m_rcIn.Height()-nTitleHeight)/m_nUnitRow;
			CRect rc;
			for(i=0;i<MAX_ROW_NUM;i++) CellRect[i].SetRect(0, 0, 0, 0);
			int pValidFlag[MAX_ROW_NUM] = {-1};
			int nXH = 0;
			for(i=0;i<MAX_ROW_NUM;i++)
			{
				if(TypeFlag[i]==1)
					pValidFlag[nXH++] = i;
			}
			for(i = 0;i < m_nUnitCol;i++) //列
			{
				rc.left = nXhWidth+i*tmpWidth;
				for(j = 0;j < m_nUnitRow;j++) //行
				{
					rc.top = j*tmpHeight+nTitleHeight;
					rc.right = rc.left + tmpWidth;
					rc.bottom = rc.top + tmpHeight;
					if(pValidFlag[i*m_nUnitRow+j]>=0) 
						CellRect[pValidFlag[i*m_nUnitRow+j] ] = rc;
				}
			}

			m_nRowNum = min((tmpHeight-m_nItemHeight)/m_nItemHeight, MAX_ROW_ITEM);
			for(i = 0;i <= m_nRowNum;i++)	//画行线
			{
				if(i%2==0) continue;
				g_d.Bar(&memDC,0,nTitleHeight+i*m_nItemHeight,m_rcIn.right,nTitleHeight+(i+1)*m_nItemHeight,GridColor.Grid2GridColor);
			}
			
			for(i = 0;i < m_nUnitRow-1;i++)
				g_d.DrawLine(&memDC,0,(i+1)*tmpHeight+nTitleHeight,m_rcIn.Width(),(i+1)*tmpHeight+nTitleHeight);
			for(i = 0;i < m_nUnitCol-1;i++)
				g_d.DrawLine(&memDC,nXhWidth+(i+1)*tmpWidth,nTitleHeight,nXhWidth+(i+1)*tmpWidth,m_rcIn.Height());
			g_d.DrawLine(&memDC, nXhWidth, 0, nXhWidth, m_rcIn.Height());
			for(i = 0;i < m_nRowNum;i++)
				g_d.DisplayText(&memDC, 4, (i+1)*m_nItemHeight+nTitleHeight+4, GridColor.Grid_CodeName_Color, "%d", nStartRow+i+1);
			
			if(m_bHasData) //如果有数据
			{
				//画具体的每一个排名
				g_d.SelectPen(&memDC,GRIDSEL_PEN);
				for(i = MAX_ROW_NUM-2;i >=0; i--)
				{
					if(CellRect[i].Width()<3||CellRect[i].Height()<3||TypeFlag[i]==0) continue;
					DrawOneCell(&memDC,i);
				}
				DrawOneCell(&memDC,MAX_ROW_NUM-1);
			}
			m_bNeedHZ = FALSE;

			pDC->BitBlt (left,top,m_rcIn.Width()-16,m_rcIn.Height(),&memDC,0,0,SRCCOPY);//从内存DC拷贝到屏幕DC
			
			g_d.RestorePen(&memDC,oldpen);
			memDC.SelectObject(pOldBitmap);
			tempBitmap.DeleteObject();
			memDC.DeleteDC();
		}
		break;
	case WM_TIMER:
		{
			
		}
		break;
	case WM_CHANGESCTYPE:
		{
			if(WhichType==TYPE_LC)
				nGpNum = FiltLCCode(lpnUsedStkIndex);
			else if (WhichType<100)
			{
				int		nSetDomain;          //SZ/SH/SZ_SH
				union   flunion ftype;       //股票过滤条件
				nSetDomain = lpnSysDomainType[WhichType];
				ftype.fshort = lpsSysClassType_new[WhichType];
				//取使用股票
				nGpNum = g_pStockIO->FiltCode(nSetDomain, ftype, lpnUsedStkIndex);
			}
			RefreshScrollBar();
			ProcessMsg(WM_PAINT,1);
		}
		break;
	case UM_VSCROLLBAR:
		OnVScrollMessage(wParam,lParam);
		return 1;
	case WM_ERASEBKGND:
		return TRUE;
	case WM_KEYDOWN:
		if(m_nCurSel >= 0)
		{
			int index = m_nCurSel%m_nItems;
			if(wParam == VK_LEFT)
			{
				int nCurrUnit = m_nCurSel/MAX_ROW_ITEM, nCurrItem = m_nCurSel%MAX_ROW_ITEM;
				nCurrUnit = ToNextUnit(nCurrUnit, m_nUnitRow, FALSE);
				m_nCurSel = MAX_ROW_ITEM*nCurrUnit+nCurrItem;
				ProcessMsg(WM_PAINT);
				return TRUE;
			}
			if(wParam == VK_RIGHT)
			{
				int nCurrUnit = m_nCurSel/MAX_ROW_ITEM, nCurrItem = m_nCurSel%MAX_ROW_ITEM;
				nCurrUnit = ToNextUnit(nCurrUnit, m_nUnitRow, TRUE);
				m_nCurSel = MAX_ROW_ITEM*nCurrUnit+nCurrItem;
				ProcessMsg(WM_PAINT);
				return TRUE;
			}
			if(wParam == VK_UP)
			{
				int nCurrUnit = m_nCurSel/MAX_ROW_ITEM, nCurrItem = m_nCurSel%MAX_ROW_ITEM;
				if(nCurrItem==0) 
				{
					nCurrItem = m_nRowNum-1;
					nCurrUnit = ToNextUnit(nCurrUnit, 1, FALSE);
				}
				else nCurrItem--;
				m_nCurSel = MAX_ROW_ITEM*nCurrUnit+nCurrItem;
				ProcessMsg(WM_PAINT);
				return TRUE;
			}
			if(wParam == VK_DOWN)
			{
				int nCurrUnit = m_nCurSel/MAX_ROW_ITEM, nCurrItem = m_nCurSel%MAX_ROW_ITEM;
				if(nCurrItem==m_nRowNum-1) 
				{
					nCurrItem = 0;
					nCurrUnit = ToNextUnit(nCurrUnit, 1, TRUE);
				}
				else nCurrItem++;
				m_nCurSel = MAX_ROW_ITEM*nCurrUnit+nCurrItem;
				ProcessMsg(WM_PAINT);
				return TRUE;
			}
			if(wParam == VK_RETURN)
			{
				int iIndex = g_pStockIO->GetIndex(m_ZHBuf[m_nCurSel/MAX_ROW_ITEM].Code[m_nCurSel%MAX_ROW_ITEM]);
				return TRUE;
			}
		}
		break;
	case WM_LBUTTONDOWN:
		{
			CPoint point(LOWORD(lParam),HIWORD(lParam));
			CSize off(left, top);
			point -= off;
			int iNo = HitTest(point);
			if(iNo != -1)
			{
				m_nCurSel = iNo;
				ProcessMsg(WM_PAINT);
			}
		}
		break;
	case WM_LBUTTONDBLCLK:
		{
			CPoint point(LOWORD(lParam),HIWORD(lParam));
			int iNo = HitTest(point);
			if(iNo != -1)
				m_nCurSel = iNo;

			short nGpIndex = ProcessMsg(UM_GET_STOCK);
			MemStkInfo *pInfo = (*g_pStockIO)[nGpIndex];
			if(pInfo)
			{
				if(!m_nInCom2)
					ToZst(pInfo->Code,pInfo->setcode,FALSE);
			}
		}
		break;
	case WM_RBUTTONDOWN:
		{
			CPoint point(LOWORD(lParam),HIWORD(lParam));
			CSize off(left, top);
			point -= off;
			//设置菜单
			CNewMenu SCPopMenu;
			SCPopMenu.CreatePopupMenu();
			//添加市场选择的菜单项
			for(int i=0;i<8;i++)
				SCPopMenu.AppendMenu(WhichType==i?MF_CHECKED:MF_UNCHECKED, ID_SCSTART+i, _F(GPType[i]));
			SCPopMenu.AppendMenu(WhichType==TYPE_ZXB?MF_CHECKED:MF_UNCHECKED, ID_SCSTART+12, _F(GPType[TYPE_ZXB]));
			SCPopMenu.AppendMenu(WhichType==TYPE_QZ?MF_CHECKED:MF_UNCHECKED, ID_SCSTART+13, _F(GPType[TYPE_QZ]));
			SCPopMenu.AppendMenu(WhichType==TYPE_CYB?MF_CHECKED:MF_UNCHECKED, ID_SCSTART+14, _F(GPType[TYPE_CYB]));
			SCPopMenu.AppendMenu(WhichType==TYPE_LC?MF_CHECKED:MF_UNCHECKED, ID_SCSTART+20, _F(GPType[TYPE_LC]));
			
			point += off;
			m_pView->ClientToScreen (&point);
			SCPopMenu.TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON,point.x,point.y,m_pView);
		}
		break;
	case WM_MOUSEMOVE:
		{
			CPoint pt(LOWORD(lParam),HIWORD(lParam));			
			g_pToolTipMark->ShowTool(pt, m_pView);
		}
		break;
	case UM_ZH_READSORT:
		{
			RefreshScrollBar();
			Sort();
		}
		break;
	default:
		break;
	}
	return UBase::ProcessMsg(message,wParam,lParam);
}

int	 USortGrid::ToNextUnit(int nCurrUnit, int nSkipVal, BOOL bNext)
{
	if(nCurrUnit<0||nCurrUnit>=MAX_ROW_NUM) return 0;
	int res = nCurrUnit;
	int nSkipped = 0;
	if(bNext)
	{
		while (nSkipped < nSkipVal)
		{
			if(res<MAX_ROW_NUM-1)
				++res;
			else res = 0;
			if(TypeFlag[res]==1)
				nSkipped++;
		}
	}
	else
	{
		while (nSkipped < nSkipVal)
		{
			if(res>0)
				--res;
			else res = MAX_ROW_NUM-1;
			if(TypeFlag[res]==1)
				nSkipped++;
		}
	}
	return res;
}

short USortGrid::GetCurrentGPIndex()
{
	if(m_nCurSel<0) return -1;
	return g_pStockIO->GetIndex(m_ZHBuf[m_nCurSel/m_nItems].Code[m_nCurSel%m_nItems]);
}

