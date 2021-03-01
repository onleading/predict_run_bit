#include "stdafx.h"
#include "URadar.h"
#include "KeyGuy.h"
#include "WarnManager.h"

URadar::URadar(LPCOMINFO pComInfo):UBase(pComInfo)
{
	m_pTick = NULL;
	m_nTickNum = 0;

	m_nPos = m_nStart = m_nMaxItem = 0;
	m_nDrawTick = 0;

	m_nCurSel = -1;
	m_nLaceLen = 0;
	TextH = 18;

	Radar_SetCode = -1;
	Radar_Code[0] = 0;

	m_nLastPos = -1;

	++g_nUnitRadarFlag;
}

URadar::~URadar()
{
	--g_nUnitRadarFlag;
}

int  URadar::ProcessMsg(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{
	case UM_GET_STOCK:
		return g_pStockIO->GetIndex(Radar_Code, Radar_SetCode);
	case UM_READDATA:
		ReadData(FALSE,wParam==0);
		ProcessMsg(WM_PAINT);
		return 1;
	case UPDATE_WARNGP:
		ProcessMsg(WM_PAINT, 1);
		return 1;
	case WM_PAINT:
		{
			if(wParam == 1) 				
			{
				ProcessMsg(UM_READDATA,lParam);
				break;
			}
			CREATE_TMPDC
			g_d.Bar(&m_MyDC,&m_rcIn,BACK_BRUSH);
			m_MyDC.SetBkMode(TRANSPARENT);

			CPen *oldpen = g_d.SelectPen(&m_MyDC,AXIS_PEN);
			g_d.DrawLine(&m_MyDC,0,0,0,m_rcIn.bottom);
			if(g_GH.bLeftHQInfo && !m_nInCom2)
				g_d.DrawLine(&m_MyDC,m_rcIn.right-1,0,m_rcIn.right-1,m_rcIn.bottom);
			g_d.DrawLine(&m_MyDC,0,m_rcIn.bottom,m_rcIn.right,m_rcIn.bottom);
			DrawIt(&m_MyDC);
			g_d.RestorePen(&m_MyDC,oldpen);

			pDC->BitBlt (left,top,Width(),Height(),&m_MyDC,0,0,SRCCOPY); //从内存DC拷贝到屏幕DC
			DESTORY_TMPDC
		}
		return 1;
	case WM_KEYDOWN:
		{
			UINT nChar = (UINT)wParam;
			switch(nChar)
			{
			case VK_UP:
				if(m_nStart > 0)
				{
					m_nPos = max(max(m_nStart,m_nMaxItem)-1,0);
					ProcessMsg(WM_PAINT);
				}
				return 1;
			case VK_DOWN:
				if(m_nPos < m_nDrawTick-1)
				{
					m_nPos = min(m_nDrawTick-1,m_nPos+m_nMaxItem);
					ProcessMsg(WM_PAINT);
				}
				return 1;
			case VK_SPACE: //回车与走势图中的键冲突，故用SPACE
				Radar_SetCode = -1;
				Radar_Code[0] = 0;
				ProcessMsg(WM_PAINT);
				if(Radar_SetCode != -1)
				{
					char tmpGPValue[80];
					tmpGPValue[0] = '0'+Radar_SetCode;
					strcpy(tmpGPValue+1,Radar_Code);
					CKeyGuy::OnCommonProcess(0,tmpGPValue);
				}
				return 1;
			default:				
				break;
			}
		}
		break;
	case WM_LBUTTONDOWN:
		{
			CPoint pt;
			pt.x = LOWORD(lParam)-left;
			pt.y = HIWORD(lParam)-top;
			if(pt.y>4&&pt.y<4+13&&pt.x>nOptBW&&pt.x<nOptBW+13)
			{
				AfxGetMainWnd()->SendMessage(WM_COMMAND,ID_WARNCFG);
			}
			if(pt.y > m_nLaceLen && pt.y < m_rcIn.bottom)
			{
				Radar_SetCode = -1;
				Radar_Code[0] = 0;
				m_nCurSel = (pt.y-m_nLaceLen)/(TextH+1);
				ProcessMsg(WM_PAINT);
			}
		}
		break;
	case WM_LBUTTONDBLCLK:
		{
			CPoint pt;
			pt.x = LOWORD(lParam)-left;
			pt.y = HIWORD(lParam)-top;
			if(pt.y > m_nLaceLen && pt.y < m_rcIn.bottom)
			{
				Radar_SetCode = -1;
				Radar_Code[0] = 0;
				m_nCurSel = (pt.y-m_nLaceLen)/(TextH+1);
				ProcessMsg(WM_PAINT);
				if(Radar_SetCode != -1)
				{
					char tmpGPValue[80];
					tmpGPValue[0] = '0'+Radar_SetCode;
					strcpy(tmpGPValue+1,Radar_Code);
					CKeyGuy::OnCommonProcess(0,tmpGPValue);
				}
			}
		}
		break;
	default:
		break;
	}
	return UBase::ProcessMsg(message,wParam,lParam);
}

void URadar::ReadData(BOOL bDataReady,BOOL bReqData)
{
	m_nStart = m_nMaxItem = 0;

	m_pTick = g_WarnBuffer;
	m_nTickNum = g_nWarnNum;

	m_nPos = m_nTickNum-1;
	m_nDrawTick = m_nTickNum;
}

void URadar::DrawIt(CDC *pDC)
{
	if(Height() < 20) return;	//如果空间太小，不画	
	int nFixedWidth = m_rcIn.right;

	CFont *pOld = g_d.SelectFont(pDC,SMALL_FONT);
	CSize size = g_d.DisplayText(pDC,75,3,VipColor.BIGTITLECOLOR,_F("市场雷达"));
	pDC->DrawIcon(nFixedWidth-20,3,m_hIcon_Pu[3]);
	nOptBW = nFixedWidth-20;
	int YFrom=2,XFrom1=2,XFrom2=nFixedWidth/6+10,XFrom3=3*nFixedWidth/6+8,XFrom4=4*nFixedWidth/6+10,i;
	int Wid3=nFixedWidth/6,Wid4=nFixedWidth/6;
	 
	YFrom+=20;
	g_d.DrawLine(pDC,0,YFrom,m_rcIn.right,YFrom);
	YFrom+=4;
	g_d.DisplayText(pDC,XFrom1+2,YFrom,VipColor.TXTCOLOR,0,_F("时间"));
	g_d.DisplayText(pDC,XFrom2+6,YFrom,VipColor.TXTCOLOR,0,_F("名称"));
	g_d.DisplayText(pDC,XFrom3,YFrom,VipColor.TXTCOLOR,0,_F("价格"));
	g_d.DisplayText(pDC,XFrom4+8,YFrom,VipColor.TXTCOLOR,0,_F("信号"));
	YFrom+=18;
	g_d.DrawLine(pDC,0,YFrom,m_rcIn.right,YFrom);
	YFrom+=4;
	m_nLaceLen = 23+2+20+4;

	if(m_nDrawTick <= 0) 
	{
		g_d.RestoreFont(pDC,pOld);
		return;
	}

	int rownum = (m_rcIn.Height()-m_nLaceLen)/(TextH+1);
	m_nMaxItem = rownum;
	m_nStart = max(0,m_nPos-rownum+1);

	MemStkInfo *tmpInfo;
	COLORREF tmpColorRef;
	int iImage;
	BOOL HasSel = FALSE;
	for(i = m_nStart; i <= m_nPos;i++)
	{
		tmpInfo = (*g_pStockIO)[m_pTick[i].nStockNo];
		if(tmpInfo)
		{
			iImage=CWarnManager::GetColorFromReason(m_pTick[i].nReason,m_pTick[i].InOutFlag);
			if(iImage==0)
				tmpColorRef=VipColor.KPCOLOR;
			else if(iImage==1)
				tmpColorRef=VipColor.DOWNCOLOR;
			else
				tmpColorRef=VipColor.LEVELCOLOR;
			//开始写一栏
			g_d.DisplayText(pDC,XFrom1,YFrom,VipColor.TXTCOLOR,0,"%02d:%02d",m_pTick[i].time.wHour,m_pTick[i].time.wMinute);
			g_d.BjPrintf(pDC,XFrom3,YFrom,m_pTick[i].fPrice,tmpInfo->Close,GetXSFlag(tmpInfo),Wid3,0);
			g_d.DisplaySameWidtchText(pDC,XFrom2,YFrom,GetBlockGPColor(tmpInfo,GridColor.Grid_CodeName_Color),tmpInfo->Name);
			CString tempStr;
			if(m_pTick[i].nReason>=TJWARNBASE)
				tempStr = (CString)(LPCSTR)(*g_pCalc)[TJ_TYPE][m_pTick[i].nReason-TJWARNBASE].acName;
			else 
				tempStr = CWarnManager::GetStrFromReason(m_pTick[i].nReason);
			g_d.DisplayText(pDC,XFrom4,YFrom,tmpColorRef,0,_F(tempStr));

			if(m_nCurSel==i-m_nStart)
			{
				Radar_SetCode = tmpInfo->setcode;
				memcpy(Radar_Code,tmpInfo->Code,SH_CODE_LEN);
				Radar_Code[SH_CODE_LEN]=0;
				int tempMode = pDC->SetROP2 (R2_XORPEN);
				if(g_bLongHighLine)
					g_d.DrawLine(pDC,2,YFrom+TextH-3,nFixedWidth-2,YFrom+TextH-3,GridColor.GridHighLineColor^VipColor.BACKCOLOR,PS_SOLID,1);
				else
				{
					CRect rc;
					rc.SetRect(2,YFrom,nFixedWidth-2,YFrom+TextH-4);

					CRect   XRc(0,0,rc.Width(),rc.Height());
					CREATE_MEMDC(XRc)
					MemDC.FillSolidRect(&XRc,GridColor.GridBackColor^VipColor.FLOATCURSORCOLOR);
					pDC->BitBlt(2,YFrom,XRc.Width(),XRc.Height(),&MemDC,0,0,SRCINVERT);
					DELETE_MEMDC	
				}
				pDC->SetROP2 (tempMode); 

				HasSel = TRUE;
			}
		}
		YFrom+=TextH+1;
	}
	if(!HasSel) m_nCurSel=-1;
	g_d.RestoreFont(pDC,pOld);
}

short URadar::GetCurrentGPIndex()
{
	return g_pStockIO->GetIndex(Radar_Code, Radar_SetCode);
}