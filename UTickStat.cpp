#include "stdafx.h"
#include "UTickStat.h"
#include "ComView.h"

UTickStat::UTickStat(LPCOMINFO pComInfo):UBase(pComInfo)
{
	m_bEmpty = TRUE;
	
	m_bPile		= FALSE;
	m_bVolume	= TRUE;
	m_bAll		= FALSE;
	m_pHQDBFile = NULL;

	memset(&m_TickStat,0,sizeof(TICKSTAT));
	memset(m_dValue,0,6*sizeof(double));
	m_UpColor		= VipColor.KPCOLOR;
	m_DownColor		= VipColor.DOWNCOLOR;
	m_LevelColor	= VipColor.LEVELCOLOR;

	m_BrushColor[0] = VipColor.DOWNCOLOR;
	m_BrushColor[1] = GetColor(VipColor.DOWNCOLOR,0.5);
	m_BrushColor[2] = GetColor(VipColor.DOWNCOLOR,0.3);
	m_BrushColor[3] = GetColor(VipColor.KPCOLOR,0.3);
	m_BrushColor[4] = GetColor(VipColor.KPCOLOR,0.5);
	m_BrushColor[5] = VipColor.KPCOLOR;
	m_BrushColor[6] = VipColor.LEVELCOLOR;

	m_GraphBottom = 0;
	for(int i=0;i<7;i++)
		m_Brush[i].CreateSolidBrush(m_BrushColor[i]);

	::SetTimer(m_pView->GetSafeHwnd(), 0x1115, 10*1000, NULL);

	m_bOnlyDrawGraph = FALSE;
}

UTickStat::~UTickStat()
{
	::KillTimer(m_pView->GetSafeHwnd(), 0x1115);
	for(int i=0;i<7;i++)
		m_Brush[i].DeleteObject();
	TDEL(m_pHQDBFile);
}

void UTickStat::SetDrawMode(BOOL bPile)
{
	m_bPile = bPile;
}

void UTickStat::SetDataType(BOOL bVolume)
{
	m_bVolume = bVolume;
}

int  UTickStat::ProcessMsg(UINT message, WPARAM wParam, LPARAM lParam)
{
	if(m_nCurGP==-1) //如果Unit为空，则另行处理
	{
		if(message==WM_PAINT)
		{
			CREATE_TMPDC
			g_d.Bar(&m_MyDC,&m_rcIn,BACK_BRUSH);

			pDC->BitBlt (left,top,Width(),Height(),&m_MyDC,0,0,SRCCOPY); //从内存DC拷贝到屏幕DC
			DESTORY_TMPDC
			return 1;
		}
		return UBase::ProcessMsg(message,wParam,lParam);
	}
	switch(message)
	{
	case UM_READDATA:
		ReadData();
		ProcessMsg(WM_PAINT);
		return 1;
	case WM_TIMER:
		if(wParam==0x1115)
			ProcessMsg(UM_READDATA);
		return 1;
	case WM_PAINT:
		{
			if(wParam == 1)
				ProcessMsg(UM_READDATA,lParam);

			CREATE_TMPDC
			g_d.Bar(&m_MyDC,&m_rcIn,BACK_BRUSH);
			m_MyDC.SetBkMode(TRANSPARENT);

			CFont *oldfont = g_d.SelectFont(&m_MyDC,INFO_FONT);
			CPen *oldpen = g_d.SelectPen(&m_MyDC,AXIS_PEN);
			g_d.DrawLine(&m_MyDC,0,0,0,m_rcIn.bottom);
			if(g_GH.bLeftHQInfo && !m_nInCom2)
				g_d.DrawLine(&m_MyDC,m_rcIn.right-1,0,m_rcIn.right-1,m_rcIn.bottom);
			g_d.DrawLine(&m_MyDC,0,m_rcIn.bottom,m_rcIn.right,m_rcIn.bottom);
			DrawIt(&m_MyDC);
			g_d.RestorePen(&m_MyDC,oldpen);
			g_d.RestoreFont(&m_MyDC,oldfont);

			pDC->BitBlt (left,top,Width(),Height(),&m_MyDC,0,0,SRCCOPY); //从内存DC拷贝到屏幕DC
			DESTORY_TMPDC
		}
		return 1;
	case WM_LBUTTONDOWN:
		{
			break;
			CPoint pt(LOWORD(lParam),HIWORD(lParam));
			if(!PtInRect(pt)) return 1;
			pt.Offset(-left,-top);
			if(pt.x>0 && pt.x<HQ_TEXTH && pt.y>0 && pt.y<HQ_TEXTH)
				m_bPile = !m_bPile;
			else if(pt.x>HQ_TEXTH && pt.x<2*HQ_TEXTH && pt.y>0 && pt.y<HQ_TEXTH)
				m_bVolume = !m_bVolume;
			else if(pt.x>2*HQ_TEXTH && pt.x<3*HQ_TEXTH && pt.y>0 && pt.y<HQ_TEXTH)
				m_bAll = !m_bAll;
			else 
				break;
			ProcessMsg(WM_PAINT);
		}
		break;
	default:
		break;
	}
	return UBase::ProcessMsg(message,wParam,lParam);
}

void UTickStat::ReadData()
{
	m_TickStat = m_pStkInfo->its;
	TransData();
}

void UTickStat::TransData()
{
	
	if(m_bVolume)
	{
		if(m_bAll)
		{
			m_dValue[0] = m_TickStat.fVol[1][1]+m_TickStat.fVol[1][3];
			m_dValue[1] = m_TickStat.fVol[2][1]+m_TickStat.fVol[2][3];
			m_dValue[2] = m_TickStat.fVol[0][1]+m_TickStat.fVol[0][3]-m_TickStat.fVol[1][1]-m_TickStat.fVol[1][3]-m_TickStat.fVol[2][1]-m_TickStat.fVol[2][3];
			m_dValue[3] = m_TickStat.fVol[0][0]+m_TickStat.fVol[0][2]-m_TickStat.fVol[1][0]-m_TickStat.fVol[1][2]-m_TickStat.fVol[2][0]-m_TickStat.fVol[2][2];
			m_dValue[4] = m_TickStat.fVol[2][0]+m_TickStat.fVol[2][2];
			m_dValue[5] = m_TickStat.fVol[1][0]+m_TickStat.fVol[1][2];
			m_dTotal = m_TickStat.fVol[0][0]+m_TickStat.fVol[0][1]+m_TickStat.fVol[0][2]+m_TickStat.fVol[0][3];
		}
		else
		{
			m_dValue[0] = m_TickStat.fVol[1][1];
			m_dValue[1] = m_TickStat.fVol[2][1];
			m_dValue[2] = m_TickStat.fVol[0][1]-m_TickStat.fVol[1][1]-m_TickStat.fVol[2][1];
			m_dValue[3] = m_TickStat.fVol[0][0]-m_TickStat.fVol[1][0]-m_TickStat.fVol[2][0];
			m_dValue[4] = m_TickStat.fVol[2][0];
			m_dValue[5] = m_TickStat.fVol[1][0];
			m_dTotal = m_TickStat.fVol[0][0]+m_TickStat.fVol[0][1];
		}
	}
	else
	{
		if(m_bAll)
		{
			m_dValue[0] = m_TickStat.VolNum[1][1]+m_TickStat.VolNum[1][3];
			m_dValue[1] = m_TickStat.VolNum[2][1]+m_TickStat.VolNum[2][3];
			m_dValue[2] = m_TickStat.VolNum[0][1]+m_TickStat.VolNum[0][3]-m_TickStat.VolNum[1][1]-m_TickStat.VolNum[1][3]-m_TickStat.VolNum[2][1]-m_TickStat.VolNum[2][3];
			m_dValue[3] = m_TickStat.VolNum[0][0]+m_TickStat.VolNum[0][2]-m_TickStat.VolNum[1][0]-m_TickStat.VolNum[1][2]-m_TickStat.VolNum[2][0]-m_TickStat.VolNum[2][2];
			m_dValue[4] = m_TickStat.VolNum[2][0]+m_TickStat.VolNum[2][2];
			m_dValue[5] = m_TickStat.VolNum[1][0]+m_TickStat.VolNum[1][2];
			m_dTotal = m_TickStat.VolNum[0][0]+m_TickStat.VolNum[0][1]+m_TickStat.VolNum[0][2]+m_TickStat.VolNum[0][3];
		}
		else
		{
			m_dValue[0] = m_TickStat.VolNum[1][1];
			m_dValue[1] = m_TickStat.VolNum[2][1];
			m_dValue[2] = m_TickStat.VolNum[0][1]-m_TickStat.VolNum[1][1]-m_TickStat.VolNum[2][1];
			m_dValue[3] = m_TickStat.VolNum[0][0]-m_TickStat.VolNum[1][0]-m_TickStat.VolNum[2][0];
			m_dValue[4] = m_TickStat.VolNum[2][0];
			m_dValue[5] = m_TickStat.VolNum[1][0];
			m_dTotal = m_TickStat.VolNum[0][0]+m_TickStat.VolNum[0][1];
		}
	}
	//容错
	double sum = 0.0;
	int i=0;
	for(i=0;i<6;i++)
	{
		if(m_dValue[i] <= -1) break;
		sum += m_dValue[i];
	}
	if(sum-m_dTotal>1 || i<6)
	{
		for(i=0;i<6;i++) m_dValue[i] = 0.0;
		m_dTotal = 0.0;
	}
}

void UTickStat::DrawIt(CDC *pDC)
{
	DrawFrame(pDC);
	if(Height() < 2*HQ_TEXTH) 
		return;
	m_bOnlyDrawGraph = FALSE;
	if(Height() < HQ_TEXTH*10) 
		m_bOnlyDrawGraph = TRUE;
	m_GraphBottom = DrawGraph(pDC);
	if(!m_bOnlyDrawGraph)
		DrawList(pDC);
	
}

void UTickStat::DrawFrame(CDC *pDC)
{
//	if(Height() < HQ_TEXTH || Width() < 160) return;	//如果空间太小，不画

	g_d.Bar(pDC,1,0,Width(),HQ_TEXTH,GRAYBACK_BRUSH);
	CSize ss = g_d.DisplayText(pDC,2,1,VipColor.TXTCOLOR,"资金流分析");
	g_d.DisplayText(pDC,2+ss.cx+2,1,VipColor.KPCOLOR,"(万元)");
	return;
	g_d.DrawLine(pDC,0,HQ_TEXTH,Width(),HQ_TEXTH);
	g_d.DrawLine(pDC,HQ_TEXTH,0,HQ_TEXTH,HQ_TEXTH);								//饼/柱
	g_d.DrawLine(pDC,2*HQ_TEXTH,0,2*HQ_TEXTH,HQ_TEXTH);							//笔数/成交量
	g_d.DrawLine(pDC,3*HQ_TEXTH,0,3*HQ_TEXTH,HQ_TEXTH);							//所有单/主动单

	CFont *oldfont = g_d.SelectFont(pDC,GRAPH_FONT);
	if(m_bPile) g_d.DisplayText(pDC,4,2,VipColor.TXTCOLOR,0,_F("饼"));
	else g_d.DisplayText(pDC,4,2,VipColor.TXTCOLOR,0,_F("柱"));
	if(m_bVolume) g_d.DisplayText(pDC,HQ_TEXTH+4,2,VipColor.TXTCOLOR,0,_F("笔"));
	else g_d.DisplayText(pDC,HQ_TEXTH+4,2,VipColor.TXTCOLOR,0,_F("量"));
	if(m_bAll) g_d.DisplayText(pDC,2*HQ_TEXTH+4,2,VipColor.TXTCOLOR,0,_F("主"));
	else g_d.DisplayText(pDC,2*HQ_TEXTH+4,2,VipColor.TXTCOLOR,0,_F("全"));

	if(m_bPile)
	{
		if(m_bVolume)
		{
			if(m_bAll) g_d.DisplayText(pDC,3*HQ_TEXTH+4,2,VipColor.TXTCOLOR,0,_F("龙虎看盘(柱/成交量/所有单)"));
			else g_d.DisplayText(pDC,3*HQ_TEXTH+4,2,VipColor.TXTCOLOR,0,_F("龙虎看盘(柱/成交量/主动单)"));
		}
		else
		{
			if(m_bAll) g_d.DisplayText(pDC,3*HQ_TEXTH+4,2,VipColor.TXTCOLOR,0,_F("龙虎看盘(柱/笔数/所有单)"));
			else g_d.DisplayText(pDC,3*HQ_TEXTH+4,2,VipColor.TXTCOLOR,0,_F("龙虎看盘(柱/笔数/主动单)"));
		}
	}
	else
	{
		if(m_bVolume)
		{
			if(m_bAll) g_d.DisplayText(pDC,3*HQ_TEXTH+4,2,VipColor.TXTCOLOR,0,_F("龙虎看盘(饼/成交量/所有单)"));
			else g_d.DisplayText(pDC,3*HQ_TEXTH+4,2,VipColor.TXTCOLOR,0,_F("龙虎看盘(饼/成交量/主动单)"));
		}
		else
		{
			if(m_bAll) g_d.DisplayText(pDC,3*HQ_TEXTH+4,2,VipColor.TXTCOLOR,0,_F("龙虎看盘(饼/笔数/所有单)"));
			else g_d.DisplayText(pDC,3*HQ_TEXTH+4,2,VipColor.TXTCOLOR,0,_F("龙虎看盘(饼/笔数/主动单)"));
		}
	}
	

	g_d.RestoreFont(pDC,oldfont);
}

int UTickStat::DrawGraph(CDC *pDC)
{
	CRect rc;
	CPoint ptCentre;

	CheckColor();
	int nListHeight=9*HQ_TEXTH;
	if(m_bOnlyDrawGraph)
		nListHeight=0;

	if(m_bPile)
	{
		rc.left		= Width()/2+HQ_TEXTH/4;
		rc.right	= Width()-HQ_TEXTH/4;
		rc.top		= HQ_TEXTH+HQ_TEXTH/2;
		rc.bottom	= min(Height()-nListHeight-HQ_TEXTH/2, nListHeight);
		DrawPile(pDC,rc,m_dValue,6,m_Brush);
	}
	else
	{
		long lRadius = min((Width()-HQ_TEXTH/2)/2,Height()-nListHeight-HQ_TEXTH);
		if(lRadius<2) return 0;
		lRadius /= 2;
		rc.left		= Width()/2+HQ_TEXTH/4;
		rc.right	= Width()-HQ_TEXTH/4;
		rc.top		= HQ_TEXTH+HQ_TEXTH/2;
	//	rc.bottom	= min(Height()-nListHeight-HQ_TEXTH/2, nListHeight);
		rc.bottom	= rc.top+2*lRadius+1.5f*HQ_TEXTH;
		ptCentre.x	= (rc.left+rc.right)/2; 
		ptCentre.y	= (rc.top+rc.bottom)/2;
		DrawCycle(pDC,ptCentre,lRadius,0.25,m_dValue,6,m_dTotal,m_Brush);
	}

	g_d.DotLine(pDC, rc.left-HQ_TEXTH/4, HQ_TEXTH, rc.left-HQ_TEXTH/4, rc.bottom, VipColor.KPCOLOR);
	g_d.DrawLine(pDC, 0, rc.bottom, Width(), rc.bottom, VipColor.KPCOLOR, PS_SOLID, 2);
	g_d.DotLine(pDC, 0, rc.top+rc.Height()/2, rc.left, rc.top+rc.Height()/2, VipColor.KPCOLOR);

	float fIn = m_TickStat.fAmount[1][0]+m_TickStat.fAmount[2][0]+(m_TickStat.fAmount[0][0]-m_TickStat.fAmount[1][0]-m_TickStat.fAmount[2][0]);
	float fOut = m_TickStat.fAmount[1][1]+m_TickStat.fAmount[2][1]+(m_TickStat.fAmount[0][1]-m_TickStat.fAmount[1][1]-m_TickStat.fAmount[2][1]);

	CFont *oldfont = g_d.SelectFont(pDC,MEDIUM_FONT);
	CSize cs = g_d.DisplayText(pDC,1,HQ_TEXTH+1,VipColor.KPCOLOR,0,_F("流入"));
	if(fIn>COMPPREC) g_d.DisplayText(pDC,1,2*HQ_TEXTH+1,VipColor.KPCOLOR,0,"%6.1f万元",fIn/10000.0f);

	cs = g_d.DisplayText(pDC,1,rc.top+rc.Height()/2+1,VipColor.KNCOLOR,0,_F("流出"));
	if(fOut>COMPPREC) g_d.DisplayText(pDC,1,rc.top+rc.Height()/2+HQ_TEXTH+1,VipColor.KNCOLOR,0,"%6.1f万元",fOut/10000.0f);
	
	return rc.bottom;
}

void UTickStat::DrawList(CDC *pDC)
{
	int i=0;
	long lListStart = 0;
//	lListStart = Height()-7*HQ_TEXTH;
	lListStart = m_GraphBottom;
	//框线
	g_d.DrawLine(pDC,0,lListStart,Width(),lListStart);
	g_d.DrawLine(pDC,0,lListStart+3*HQ_TEXTH,Width(),lListStart+3*HQ_TEXTH);
	g_d.DrawLine(pDC,0,lListStart+6*HQ_TEXTH,Width(),lListStart+6*HQ_TEXTH);

	g_d.DrawLine(pDC,HQ_TEXTH,lListStart,HQ_TEXTH,lListStart+6*HQ_TEXTH);
	
	CFont *oldfont = g_d.SelectFont(pDC,GRAPH_FONT);
	CBrush	*pOldBrush = NULL;
	CRect rc;
	//字
	g_d.DisplayText(pDC,HQ_TEXTH+4,lListStart+2,VipColor.TXTCOLOR,0,_F("大"));
	g_d.DisplayText(pDC,HQ_TEXTH+4,lListStart+HQ_TEXTH+2,VipColor.TXTCOLOR,0,_F("中"));
	g_d.DisplayText(pDC,HQ_TEXTH+4,lListStart+2*HQ_TEXTH+2,VipColor.TXTCOLOR,0,_F("小"));
	g_d.DisplayText(pDC,HQ_TEXTH+4,lListStart+3*HQ_TEXTH+2,VipColor.TXTCOLOR,0,_F("大"));
	g_d.DisplayText(pDC,HQ_TEXTH+4,lListStart+4*HQ_TEXTH+2,VipColor.TXTCOLOR,0,_F("中"));
	g_d.DisplayText(pDC,HQ_TEXTH+4,lListStart+5*HQ_TEXTH+2,VipColor.TXTCOLOR,0,_F("小"));
	g_d.DisplayText(pDC,7,lListStart+6*HQ_TEXTH+2,VipColor.TXTCOLOR,0,_F("不明"));
	if(m_bAll)
	{
		g_d.DisplayText(pDC,4,lListStart+2,VipColor.KPCOLOR,0,_F("买"));
		g_d.DisplayText(pDC,4,lListStart+2*HQ_TEXTH+2,VipColor.KPCOLOR,0,_F("入"));
		g_d.DisplayText(pDC,4,lListStart+3*HQ_TEXTH+2,VipColor.KPCOLOR,0,_F("卖"));
		g_d.DisplayText(pDC,4,lListStart+5*HQ_TEXTH+2,VipColor.KPCOLOR,0,_F("出"));
	}
	else
	{
		g_d.DisplayText(pDC,4,lListStart+2,VipColor.KNCOLOR,0,_F("流"));
		g_d.DisplayText(pDC,4,lListStart+1*HQ_TEXTH+2,VipColor.KNCOLOR,0,_F("  "));
		g_d.DisplayText(pDC,4,lListStart+2*HQ_TEXTH+2,VipColor.KNCOLOR,0,_F("入"));
		g_d.DisplayText(pDC,4,lListStart+3*HQ_TEXTH+2,VipColor.KNCOLOR,0,_F("流"));
		g_d.DisplayText(pDC,4,lListStart+4*HQ_TEXTH+2,VipColor.KNCOLOR,0,_F("  "));
		g_d.DisplayText(pDC,4,lListStart+5*HQ_TEXTH+2,VipColor.KNCOLOR,0,_F("出"));
	}
	//图例
	for(i=0;i<3;i++)
	{
		rc = CRect(HQ_TEXTH*2+2,lListStart+i*HQ_TEXTH+4,HQ_TEXTH*3-2,lListStart+(i+1)*HQ_TEXTH-4);
		pDC->FillSolidRect(&rc,m_BrushColor[5-i]);
		
		rc = CRect(HQ_TEXTH*2+2,lListStart+(i+3)*HQ_TEXTH+4,HQ_TEXTH*3-2,lListStart+(i+4)*HQ_TEXTH-4);
		pDC->FillSolidRect(&rc,m_BrushColor[i]);
	}
	rc = CRect(HQ_TEXTH*2+2,lListStart+6*HQ_TEXTH+4,HQ_TEXTH*3-2,lListStart+7*HQ_TEXTH-4);
	pDC->FillSolidRect(&rc,m_BrushColor[6]);
	//说明
	double dTotalVal1 = 0;
	double dTotalVal2 = 0;
	if(m_bVolume)
	{
		dTotalVal1 = m_TickStat.fAmount[0][0]+m_TickStat.fAmount[0][1]+m_TickStat.fAmount[0][4];
		dTotalVal2 = dTotalVal1;
	}
	else
	{
		dTotalVal1 = m_TickStat.VolNum[0][0]+m_TickStat.VolNum[0][2]+m_TickStat.VolNum[0][4]
			+m_TickStat.VolNum[0][1]+m_TickStat.VolNum[0][3];
		dTotalVal2 = m_TickStat.VolNum[0][0]+m_TickStat.VolNum[0][1]+m_TickStat.VolNum[0][4];
	}
	if(m_bAll)
	{
		DrawListItem(pDC,lListStart,0,m_TickStat.fVol[1][0]+m_TickStat.fVol[1][2],m_TickStat.VolNum[1][0]+m_TickStat.VolNum[1][2],dTotalVal1);
		DrawListItem(pDC,lListStart,1,m_TickStat.fVol[2][0]+m_TickStat.fVol[2][2],m_TickStat.VolNum[2][0]+m_TickStat.VolNum[2][2],dTotalVal1);
		DrawListItem(pDC,lListStart,2,m_TickStat.fVol[0][0]+m_TickStat.fVol[0][2]-m_TickStat.fVol[1][0]-m_TickStat.fVol[1][2]-m_TickStat.fVol[2][0]-m_TickStat.fVol[2][2],
			m_TickStat.VolNum[0][0]+m_TickStat.VolNum[0][2]-m_TickStat.VolNum[1][0]-m_TickStat.VolNum[1][2]-m_TickStat.VolNum[2][0]-m_TickStat.VolNum[2][2],dTotalVal1);
		DrawListItem(pDC,lListStart,3,m_TickStat.fVol[1][1]+m_TickStat.fVol[1][3],m_TickStat.VolNum[1][1]+m_TickStat.VolNum[1][3],dTotalVal1);
		DrawListItem(pDC,lListStart,4,m_TickStat.fVol[2][1]+m_TickStat.fVol[2][3],m_TickStat.VolNum[2][1]+m_TickStat.VolNum[2][3],dTotalVal1);
		DrawListItem(pDC,lListStart,5,m_TickStat.fVol[0][1]+m_TickStat.fVol[0][3]-m_TickStat.fVol[1][1]-m_TickStat.fVol[1][3]-m_TickStat.fVol[2][1]-m_TickStat.fVol[2][3],
			m_TickStat.VolNum[0][1]+m_TickStat.VolNum[0][3]-m_TickStat.VolNum[1][1]-m_TickStat.VolNum[1][3]-m_TickStat.VolNum[2][1]-m_TickStat.VolNum[2][3],dTotalVal1);
		DrawListItem(pDC,lListStart,6,m_TickStat.fVol[0][4],m_TickStat.VolNum[0][4],dTotalVal1);
	}
	else
	{
		DrawListItemAmo(pDC,lListStart,0,m_TickStat.fAmount[1][0],m_TickStat.VolNum[1][0],dTotalVal2);
		DrawListItemAmo(pDC,lListStart,1,m_TickStat.fAmount[2][0],m_TickStat.VolNum[2][0],dTotalVal2);
		DrawListItemAmo(pDC,lListStart,2,m_TickStat.fAmount[0][0]-m_TickStat.fAmount[1][0]-m_TickStat.fAmount[2][0],
			m_TickStat.VolNum[0][0]-m_TickStat.VolNum[1][0]-m_TickStat.VolNum[2][0],dTotalVal2);
		DrawListItemAmo(pDC,lListStart,3,m_TickStat.fAmount[1][1],m_TickStat.VolNum[1][1],dTotalVal2);
		DrawListItemAmo(pDC,lListStart,4,m_TickStat.fAmount[2][1],m_TickStat.VolNum[2][1],dTotalVal2);
		DrawListItemAmo(pDC,lListStart,5,m_TickStat.fAmount[0][1]-m_TickStat.fAmount[1][1]-m_TickStat.fAmount[2][1],
			m_TickStat.VolNum[0][1]-m_TickStat.VolNum[1][1]-m_TickStat.VolNum[2][1],dTotalVal2);
		DrawListItemAmo(pDC,lListStart,6,m_TickStat.fAmount[0][4],m_TickStat.VolNum[0][4],dTotalVal2);
	}
	int nNextStart = lListStart+7*HQ_TEXTH;
	g_d.DrawLine(pDC,0,nNextStart,Width(),nNextStart);

	int nPileStart = nNextStart+4;
	CSize jdzs = g_d.DisplayText(pDC,1,nPileStart,VipColor.TXTCOLOR,0,_F("净大单"));
	g_d.DisplayText(pDC,1,nPileStart+HQ_TEXTH,VipColor.TXTCOLOR,0,_F("净中单"));
	g_d.DisplayText(pDC,1,nPileStart+2*HQ_TEXTH,VipColor.TXTCOLOR,0,_F("净小单"));

	int nPileWidth = Width()-(jdzs.cx+4);
	int nPileCenter = (jdzs.cx+4)+nPileWidth/2;
	float fJBAmo = m_TickStat.fAmount[1][0]-m_TickStat.fAmount[1][1];
	float fJMAmo = m_TickStat.fAmount[2][0]-m_TickStat.fAmount[2][1];
	float fJSAmo = (m_TickStat.fAmount[0][0]-m_TickStat.fAmount[1][0]-m_TickStat.fAmount[2][0])-(m_TickStat.fAmount[0][1]-m_TickStat.fAmount[1][1]-m_TickStat.fAmount[2][1]);
	
	float fMaxVol = max(max(fJBAmo,fJMAmo),fJSAmo);
	float fMinVol = min(min(fJBAmo,fJMAmo),fJSAmo);
	
	float fWidtha = m_TickStat.fAmount[0][0]+m_TickStat.fAmount[0][1];
	fWidtha/=6.18f;
	float fWidth = 0.0f;
	if(fMaxVol<0||fMinVol>0) fWidth = fabs(fMinVol);
	else fWidth = (fabs(fMaxVol)+fabs(fMinVol));
	fWidth = max(fWidtha, fWidth);

	int nWidth = 0.5f*nPileWidth*(fabs(fJBAmo)/fWidth);
	if(fJBAmo>0)
		g_d.Bar(pDC, nPileCenter, nPileStart, nPileCenter+nWidth, nPileStart+0.8f*HQ_TEXTH, VipColor.KPCOLOR);
	else 
		g_d.Bar(pDC, nPileCenter-nWidth, nPileStart, nPileCenter, nPileStart+0.8f*HQ_TEXTH, VipColor.KNCOLOR);

	nWidth = 0.382f*nPileWidth*(fabs(fJMAmo)/fWidth);
	if(fJMAmo>0)
		g_d.Bar(pDC, nPileCenter, nPileStart+HQ_TEXTH, nPileCenter+nWidth, nPileStart+HQ_TEXTH+0.8f*HQ_TEXTH, VipColor.KPCOLOR);
	else 
		g_d.Bar(pDC, nPileCenter-nWidth, nPileStart+HQ_TEXTH, nPileCenter, nPileStart+HQ_TEXTH+0.8f*HQ_TEXTH, VipColor.KNCOLOR);

	nWidth = 0.382f*nPileWidth*(fabs(fJSAmo)/fWidth);
	if(fJSAmo>0)
		g_d.Bar(pDC, nPileCenter, nPileStart+2*HQ_TEXTH, nPileCenter+nWidth, nPileStart+2*HQ_TEXTH+0.8f*HQ_TEXTH, VipColor.KPCOLOR);
	else 
		g_d.Bar(pDC, nPileCenter-nWidth, nPileStart+2*HQ_TEXTH, nPileCenter, nPileStart+2*HQ_TEXTH+0.8f*HQ_TEXTH, VipColor.KNCOLOR);

	nNextStart = nPileStart+2*HQ_TEXTH+0.8f*HQ_TEXTH+3;
	g_d.DrawLine(pDC,0,nNextStart,Width(),nNextStart);

	float fJJJ = fJBAmo+fJMAmo+fJSAmo;
	CSize cs;
	if(fJJJ>COMPPREC) 
	{
		cs = g_d.DisplayText(pDC,1,nNextStart+3,VipColor.TXTCOLOR,0,_F("净流入: "));
		if(fabs(fJJJ)>COMPPREC) g_d.DisplayText(pDC,1+cs.cx,nNextStart+3,VipColor.KPCOLOR,0,_F(" %6.1f万元"), fabs(fJJJ)/10000.0f);
	}
	else
	{
		cs = g_d.DisplayText(pDC,1,nNextStart+3,VipColor.TXTCOLOR,0,_F("净流出: "));
		if(fabs(fJJJ)>COMPPREC) g_d.DisplayText(pDC,1+cs.cx,nNextStart+3,VipColor.KNCOLOR,0,_F(" %6.1f万元"), fabs(fJJJ)/10000.0f);
	}
	g_d.DrawLine(pDC,0,nNextStart+cs.cy+4,Width(),nNextStart+cs.cy+4);
}

void UTickStat::DrawPile(CDC *pDC,CRect rc,double *dValue,int ValueNum,CBrush *pBrush)
{
	long lMid = ValueNum/2;
	double dSum1 = 0.0,dSum2 = 0.0;
	long Pos1 = 0,Pos2 = 0;
	long i=0;
	CRect TmpRect;

	for(i=0;i<lMid;i++)			dSum1 += dValue[i];
	for(i=lMid;i<2*lMid;i++)	dSum2 += dValue[i];
	dSum1 = max(dSum1,dSum2);
	if(dSum1 < 1.0-COMPPREC) return;

	double ValRate = rc.Height()/dSum1;
	dSum1 = 0.0;
	dSum2 = 0.0;

	for(i=0;i<lMid;i++)
	{
		Pos1	= rc.bottom - long(dSum1*ValRate);
		dSum1	+= dValue[lMid-i-1];
		Pos2	= rc.bottom - long(dSum1*ValRate);

		TmpRect = CRect(rc.left,Pos2,rc.left+rc.Width()/2,Pos1);
		pDC->FillSolidRect(&TmpRect,m_BrushColor[lMid-i-1]);
		g_d.DrawRect(pDC,&TmpRect);
		
		Pos1	= rc.bottom - long(dSum2*ValRate);
		dSum2	+= dValue[lMid+i];
		Pos2	= rc.bottom - long(dSum2*ValRate);

		TmpRect = CRect(rc.left+rc.Width()/2,Pos2,rc.right,Pos1);
		pDC->FillSolidRect(&TmpRect,m_BrushColor[lMid+i]);
		g_d.DrawRect(pDC,&TmpRect);
	}
}

void UTickStat::DrawCycle(CDC *pDC,CPoint ptCentre,long lRadius,double dStartRate,double *dValue,int ValueNum,double dTotal,CBrush *pBrush)
{
	CPoint	ptDraw[2];
	CRect	rc;
	double	sum = 0.0;
	CBrush	*pOldBrush = NULL;
	int		i = 0;
	rc.left		= ptCentre.x-lRadius;
	rc.right	= ptCentre.x+lRadius;
	rc.top		= ptCentre.y-lRadius;
	rc.bottom	= ptCentre.y+lRadius;
	if(lRadius<0) return;
	if(dStartRate>=1) dStartRate = 0.0;
	if(dTotal<1.0-COMPPREC) return;
	
	pDC->Arc(rc.left,rc.top,rc.right,rc.bottom,rc.right,ptCentre.y,rc.right,ptCentre.y);
	sum = dStartRate;

	for(i=0;i<ValueNum;i++)
	{
		pOldBrush = pDC->SelectObject(&(pBrush[i]));
		if(sum<=dStartRate || sum>=1+dStartRate)
		{
			ptDraw[0].x = ptCentre.x+long(lRadius*cos(2*PI*dStartRate));
			ptDraw[0].y = ptCentre.y-long(lRadius*sin(2*PI*dStartRate));
		}
		else
		{
			ptDraw[0].x = ptCentre.x+long(lRadius*cos(2*PI*sum));
			ptDraw[0].y = ptCentre.y-long(lRadius*sin(2*PI*sum));
		}
		sum += dValue[i]/dTotal;
		ptDraw[1].x = ptCentre.x+long(lRadius*cos(2*PI*sum));
		ptDraw[1].y = ptCentre.y-long(lRadius*sin(2*PI*sum));
		if(abs(ptDraw[0].x-ptDraw[1].x)<=1 && abs(ptDraw[0].y-ptDraw[1].y)<=1 && dValue[i]/dTotal<0.25)
		{
			g_d.DrawLine(pDC,ptCentre.x,ptCentre.y,ptDraw[1].x,ptDraw[1].y);
			g_d.DrawLine(pDC,ptDraw[1].x,ptDraw[1].y,ptCentre.x,ptCentre.y);
		}
		else
		{
			pDC->Pie(rc,ptDraw[0],ptDraw[1]);
		}
		g_d.RestoreBrush(pDC,pOldBrush);
	}
	if(sum<1+dStartRate)
	{
		pOldBrush = pDC->SelectObject(&(pBrush[ValueNum]));
		ptDraw[0].x = ptCentre.x+long(lRadius*cos(2*PI*sum));
		ptDraw[0].y = ptCentre.y-long(lRadius*sin(2*PI*sum));
		ptDraw[1].x = ptCentre.x+long(lRadius*cos(2*PI*dStartRate));
		ptDraw[1].y = ptCentre.y-long(lRadius*sin(2*PI*dStartRate));
		if(abs(ptDraw[0].x-ptDraw[1].x)<=1 && abs(ptDraw[0].y-ptDraw[1].y)<=1)
		{
			g_d.DrawLine(pDC,ptCentre.x,ptCentre.y,ptDraw[1].x,ptDraw[1].y);
			g_d.DrawLine(pDC,ptDraw[1].x,ptDraw[1].y,ptCentre.x,ptCentre.y);
		}
		else pDC->Pie(rc,ptDraw[0],ptDraw[1]);
		g_d.RestoreBrush(pDC,pOldBrush);
	}
}

void UTickStat::DrawListItem(CDC *pDC,long lListStart,int nLine,double dVol,DWORD dwVolNum,double dTotalVal)
{
	if(dTotalVal<1.0-COMPPREC) return;
	double dRate = 0.0;
	if(m_bVolume)
	{
		dRate = ((dTotalVal<1)?0:(dVol/dTotalVal));
		if(dVol<100000) g_d.DisplayText(pDC,HQ_TEXTH*3,lListStart+nLine*HQ_TEXTH+2,VipColor.TXTCOLOR,0,_F("%8.0f手"),dVol);
		else g_d.DisplayText(pDC,HQ_TEXTH*3,lListStart+nLine*HQ_TEXTH+2,VipColor.TXTCOLOR,0,_F("%8.0f万手"),dVol/10000);
	}
	else
	{
		dRate = ((dTotalVal<1)?0:(dwVolNum/dTotalVal));
		if(dwVolNum<100000) g_d.DisplayText(pDC,HQ_TEXTH*3,lListStart+nLine*HQ_TEXTH+2,VipColor.TXTCOLOR,0,_F("%d笔"),dwVolNum);
		else g_d.DisplayText(pDC,HQ_TEXTH*3,lListStart+nLine*HQ_TEXTH+2,VipColor.TXTCOLOR,0,_F("%8.1f万笔"),(1.0*dwVolNum)/10000);
	}
	if(Width()>HQ_TEXTH*8) g_d.DisplayText(pDC,HQ_TEXTH*6+2,lListStart+nLine*HQ_TEXTH+2,VipColor.TXTCOLOR,0,_F("%8.2f%%"),dRate*100);
}

void UTickStat::DrawListItemAmo(CDC *pDC,long lListStart,int nLine,float fAmo,DWORD dwVolNum,double dTotalVal)
{
	if(dTotalVal<1.0-COMPPREC) return;
	double dRate = 0.0;
	
	dRate = ((dTotalVal<1)?0:(fAmo/dTotalVal));
	g_d.DisplayText(pDC,HQ_TEXTH*3,lListStart+nLine*HQ_TEXTH+2,VipColor.TXTCOLOR,0,_F("%8.0f万元"),fAmo/10000);

	if(Width()>HQ_TEXTH*9) g_d.DisplayText(pDC,HQ_TEXTH*7+2,lListStart+nLine*HQ_TEXTH+2,VipColor.TXTCOLOR,0,_F("%8.2f%%"),dRate*100);
}

void UTickStat::CheckColor()
{
	int i=0;
	if(m_DownColor != VipColor.DOWNCOLOR)
	{
		m_BrushColor[0] = VipColor.DOWNCOLOR;
		m_BrushColor[1] = GetColor(VipColor.DOWNCOLOR,0.5);
		m_BrushColor[2] = GetColor(VipColor.DOWNCOLOR,0.2);
		m_DownColor		= VipColor.DOWNCOLOR;

		for(i=0;i<3;i++)
		{
			m_Brush[i].DeleteObject();
			m_Brush[i].CreateSolidBrush(m_BrushColor[i]);
		}
	}
	if(m_UpColor != VipColor.KPCOLOR)
	{
		m_BrushColor[3] = GetColor(VipColor.KPCOLOR,0.2);
		m_BrushColor[4] = GetColor(VipColor.KPCOLOR,0.5);
		m_BrushColor[5] = VipColor.KPCOLOR;
		m_UpColor		= VipColor.KPCOLOR;

		for(i=3;i<6;i++)
		{
			m_Brush[i].DeleteObject();
			m_Brush[i].CreateSolidBrush(m_BrushColor[i]);
		}
	}
	if(m_LevelColor != VipColor.LEVELCOLOR)
	{
		m_BrushColor[6] = VipColor.LEVELCOLOR;
		m_Brush[6].DeleteObject();
		m_Brush[6].CreateSolidBrush(m_BrushColor[6]);
		m_LevelColor	= VipColor.LEVELCOLOR;
	}
}

COLORREF UTickStat::GetColor(COLORREF rcBase,double dRate)
{
	int colorr=int(rcBase)%256;
	int colorg=int(rcBase)/256%256;
	int colorb=int(rcBase)/65536%256;
	if(dRate>1)	dRate=1;
	if(dRate<0)	dRate=0;
	colorr	= int(double(colorr)*dRate);
	colorg	= int(double(colorg)*dRate);
	colorb	= int(double(colorb)*dRate);
	return(RGB(colorr,colorg,colorb));
}

