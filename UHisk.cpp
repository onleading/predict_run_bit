#include "stdafx.h"
#include "UHisk.h"

UHisk::UHisk(LPCOMINFO pComInfo):UBase(pComInfo)
{
	ActiveCapital = TotalCapital = 0;
}

UHisk::~UHisk()
{
}

int  UHisk::ProcessMsg(UINT message, WPARAM wParam, LPARAM lParam)
{
	if(m_nCurGP==-1) //如果Unit为空，则另行处理
	{
		if(message==WM_PAINT)
		{
			CREATE_TMPDC
			g_d.Bar(&m_MyDC,&m_rcIn,BACK_BRUSH);

			pDC->BitBlt (left,top,Width(),Height(),&m_MyDC,0,0,SRCCOPY); //从内存DC拷贝到屏幕DC
			DESTORY_TMPDC
		}
		return UBase::ProcessMsg(message,wParam,lParam);
	}
	switch(message)
	{
	case UM_READDATA:
		ReadData();
		return 1;
	case WM_PAINT:
		{
			if(wParam == 1)
				ReadData();
			CREATE_TMPDC
			g_d.Bar(&m_MyDC,&m_rcIn,BACK_BRUSH);
			m_MyDC.SetBkMode(TRANSPARENT);

			CFont *oldfont = g_d.SelectFont(&m_MyDC,INFO_FONT);
			CPen *oldpen = g_d.SelectPen(&m_MyDC,AXIS_PEN);
			g_d.DrawLine(&m_MyDC,0,0,0,m_rcIn.bottom);
			g_d.DrawLine(&m_MyDC,0,m_rcIn.bottom,m_rcIn.right,m_rcIn.bottom);
			DrawIt(&m_MyDC);			
			g_d.RestorePen(&m_MyDC,oldpen);
			g_d.RestoreFont(&m_MyDC,oldfont);

			pDC->BitBlt (left,top,Width(),Height(),&m_MyDC,0,0,SRCCOPY); //从内存DC拷贝到屏幕DC
			DESTORY_TMPDC
		}
		return 1;
	default:
		break;
	}
	return UBase::ProcessMsg(message,wParam,lParam);
}

void UHisk::ReadData()
{
	long tmpDate = (m_lDate==0)?g_lOpenRq:m_lDate;
	g_pStockIO->ReadLTGB_ZGB_OneDate(m_pStkInfo,tmpDate,&ActiveCapital,&TotalCapital,m_pStkInfo->ActiveCapital,m_pStkInfo->J_zgb);
}

char *HiskValueStr[14] = {"开盘价","最高价","最低价","收盘价","成交量","成交额","涨跌","涨幅","振幅","换手率","总股本","流通股","涨家数","跌家数"};
void UHisk::DrawIt(CDC *pDC)
{
	if(Height() < 20) return;	//如果空间太小，不画
	int nFixedWidth = m_rcIn.right;
	int TextH=HQ_TEXTH+1,YFrom=5,XFrom1=10,XFrom2=nFixedWidth/2,XWidth = nFixedWidth/2-10;

	g_d.DisplayText(pDC,XFrom1,YFrom,VipColor.TXTCOLOR,_F(HiskValueStr[0]));
	g_d.DisplayText(pDC,XFrom2,YFrom,VipColor.TXTCOLOR,XWidth,(m_XsFlag==1)?"%9.1f":((m_XsFlag==3)?"%9.3f":"%9.2f"),g_SyncAnalyData.Open);
	YFrom+=TextH;
	g_d.DisplayText(pDC,XFrom1,YFrom,VipColor.TXTCOLOR,_F(HiskValueStr[1]));
	g_d.DisplayText(pDC,XFrom2,YFrom,VipColor.TXTCOLOR,XWidth,(m_XsFlag==1)?"%9.1f":((m_XsFlag==3)?"%9.3f":"%9.2f"),g_SyncAnalyData.High);
	YFrom+=TextH;
	g_d.DisplayText(pDC,XFrom1,YFrom,VipColor.TXTCOLOR,_F(HiskValueStr[2]));
	g_d.DisplayText(pDC,XFrom2,YFrom,VipColor.TXTCOLOR,XWidth,(m_XsFlag==1)?"%9.1f":((m_XsFlag==3)?"%9.3f":"%9.2f"),g_SyncAnalyData.Low);
	YFrom+=TextH;
	g_d.DisplayText(pDC,XFrom1,YFrom,VipColor.TXTCOLOR,_F(HiskValueStr[3]));
	g_d.DisplayText(pDC,XFrom2,YFrom,VipColor.TXTCOLOR,XWidth,(m_XsFlag==1)?"%9.1f":((m_XsFlag==3)?"%9.3f":"%9.2f"),g_SyncAnalyData.Close);
	YFrom+=TextH;
	g_d.DisplayText(pDC,XFrom1,YFrom,VipColor.TXTCOLOR,_F(HiskValueStr[4]));
	if(!m_bZs)
	{
		if(need_justcjl(m_pStkInfo->setcode,m_pStkInfo->Code))
			g_d.DisplayText(pDC,XFrom2,YFrom,VipColor.TXTCOLOR,XWidth,_F("%u"),(long)(g_SyncAnalyData.Volume/m_pStkInfo->Unit));
		else
			g_d.DisplayText(pDC,XFrom2,YFrom,VipColor.TXTCOLOR,XWidth,"%u",g_SyncAnalyData.Volume);
	}
	YFrom+=TextH;
	g_d.DisplayText(pDC,XFrom1,YFrom,VipColor.TXTCOLOR,_F(HiskValueStr[5]));
	g_d.DisplayText(pDC,XFrom2,YFrom,VipColor.TXTCOLOR,XWidth,MakeJE(g_SyncAnalyData.Amount));
	YFrom+=TextH+3;
	g_d.DrawLine(pDC,0,YFrom-3,m_rcIn.right,YFrom-3);

	g_d.DisplayText(pDC,XFrom1,YFrom,VipColor.TXTCOLOR,_F(HiskValueStr[6]));
	if(g_SyncClose > COMPPREC)
		g_d.DisplayText(pDC,XFrom2,YFrom,VipColor.TXTCOLOR,XWidth,(m_XsFlag==1)?"%9.1f":((m_XsFlag==3)?"%9.3f":"%9.2f"),(g_SyncAnalyData.Close-g_SyncClose));
	YFrom+=TextH;
	g_d.DisplayText(pDC,XFrom1,YFrom,VipColor.TXTCOLOR,_F(HiskValueStr[7]));
	if(g_SyncClose > COMPPREC)
		g_d.DisplayText(pDC,XFrom2,YFrom,VipColor.TXTCOLOR,XWidth,"%6.2f%%",100*(g_SyncAnalyData.Close-g_SyncClose)/g_SyncClose);
	YFrom+=TextH;
	g_d.DisplayText(pDC,XFrom1,YFrom,VipColor.TXTCOLOR,_F(HiskValueStr[8]));
	if(g_SyncClose > COMPPREC)
		g_d.DisplayText(pDC,XFrom2,YFrom,VipColor.TXTCOLOR,XWidth,"%6.2f%%",100*(g_SyncAnalyData.High-g_SyncAnalyData.Low)/g_SyncClose);
	
	YFrom+=TextH+4;
	g_d.DrawLine(pDC,0,YFrom-4,m_rcIn.right,YFrom-4);

	if(m_bDSFlag && m_pStkInfo->bigmarket==QH_TYPE) 
	{
		g_d.DisplayText(pDC,XFrom1,YFrom,VipColor.TXTCOLOR,_F("结算价"));
		if(g_SyncAnalyData.YClose>COMPPREC)
			g_d.DisplayText(pDC,XFrom2,YFrom,VipColor.TXTCOLOR,XWidth,(m_XsFlag==1)?"%9.1f":((m_XsFlag==3)?"%9.3f":"%9.2f"),g_SyncAnalyData.YClose);
		return;
	}

	g_d.DisplayText(pDC,XFrom1,YFrom,VipColor.TXTCOLOR,_F(HiskValueStr[9]));
	if(ActiveCapital > 1)
	{
		if(m_bQz)
			g_d.DisplayText(pDC,XFrom2,YFrom,VipColor.TXTCOLOR,XWidth,"%6.2f%%",g_SyncAnalyData.Volume*m_pStkInfo->Unit/(100.0*ActiveCapital)); //除流通股本
		else
			g_d.DisplayText(pDC,XFrom2,YFrom,VipColor.TXTCOLOR,XWidth,"%6.2f%%",g_SyncAnalyData.Volume/(100.0*ActiveCapital)); //除流通股本
	}
	YFrom+=TextH;
	g_d.DisplayText(pDC,XFrom1,YFrom,VipColor.TXTCOLOR,_F(HiskValueStr[10]));
	if(ActiveCapital > 1)
		g_d.DisplayText(pDC,XFrom2,YFrom,VipColor.TXTCOLOR,XWidth,MakeGB(TotalCapital));
	YFrom+=TextH;
	g_d.DisplayText(pDC,XFrom1,YFrom,VipColor.TXTCOLOR,_F(HiskValueStr[11]));
	if(TotalCapital*10 > 1)
		g_d.DisplayText(pDC,XFrom2,YFrom,VipColor.TXTCOLOR,XWidth,MakeGB(ActiveCapital));

	if(m_bZs)
	{
		YFrom+=TextH+4;
		g_d.DrawLine(pDC,0,YFrom-4,m_rcIn.right,YFrom-4);
		g_d.DisplayText(pDC,XFrom1,YFrom,VipColor.TXTCOLOR,_F(HiskValueStr[12]));
		g_d.DisplayText(pDC,XFrom2,YFrom,VipColor.TXTCOLOR,XWidth,"%u",g_SyncAnalyData.zd.up);
		YFrom+=TextH;
		g_d.DisplayText(pDC,XFrom1,YFrom,VipColor.TXTCOLOR,_F(HiskValueStr[13]));
		g_d.DisplayText(pDC,XFrom2,YFrom,VipColor.TXTCOLOR,XWidth,"%u",g_SyncAnalyData.zd.down);
	}
}
