#include "stdafx.h"
#include "UWtfb.h"
#include "ComView.h"

UWtfb::UWtfb(LPCOMINFO pComInfo):UBase(pComInfo)
{
	m_bEmpty = TRUE;
	m_bDataReady = FALSE;

	m_TPPrice1 = m_TPPrice2 = 0.0f;
}

UWtfb::~UWtfb()
{

}

int  UWtfb::ProcessMsg(UINT message, WPARAM wParam, LPARAM lParam)
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
		m_bDataReady = FALSE;
		ReadData(FALSE,wParam==0);
		return 1;
	case UM_GETDATA_ACK://对于远程版,请求和返回分开
		switch(wParam)
		{
		case 0:	
			m_bDataReady = TRUE;
			ReadData(TRUE);
			break;
		}
		ProcessMsg(WM_PAINT);
		return 1;
	case ASYN_HQ_MESSAGE:
		{
			if(!m_pStkInfo)		return 1;
			short nTmpCurGp = g_pStockIO->GetIndex(m_pStkInfo->Code, m_pStkInfo->setcode);
			if(wParam == nTmpCurGp)
				ProcessMsg(UM_UPDATE);
		}
		return 1;
	case UM_UPDATE:
		if(!m_bDataReady) return 1;
		g_pStockIO->ReadCurrStockData(m_pStkInfo,&m_Hq,&m_Hq_L2);
		ProcessMsg(WM_PAINT);
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

void UWtfb::ReadData(BOOL bDataReady,BOOL bReqData)
{
	g_pStockIO->ReadCurrStockData(m_pStkInfo,&m_Hq,&m_Hq_L2);
	if(OnLine)
	{
		if(!bDataReady)
		{
			if(bReqData)
			{
				m_pView->SendMessage(UM_GETMAINID); //直接让父窗口填写g_nMainID
				g_nAssisID = SetComAssisID(m_nWhichUnit,0);
				g_pGetData->GetCurrStockData(m_pStkInfo);
			}
		}
		else
			g_pGetData->GetCurrStockDataACK(m_pStkInfo,&m_Hq,&m_Hq_L2);
	}
	m_TPPrice1=GetTPPrice(m_pStkInfo,m_Hq.Close,TRUE);
	m_TPPrice2=GetTPPrice(m_pStkInfo,m_Hq.Close,FALSE);
}

void UWtfb::DrawIt(CDC *pDC)
{
	if(Height() < 20 || Width() < 80) return;	//如果空间太小，不画
	m_bEmpty = m_Hq.Now < COMPPREC;
	if(m_bDSFlag)
		return;

	COLORREF yellow = RGB(255,255,0);
	COLORREF gray = RGB(128,128,128);
	
	if(m_Hq_L2.SellAvp<COMPPREC||m_Hq_L2.TotalSellv<1)
	{
		m_Hq_L2.SellAvp = 0.0f;
		float fAmountAll = m_Hq.Sellp[0]*(float)m_Hq.Sellv[0]+m_Hq.Sellp[1]*(float)m_Hq.Sellv[1]+m_Hq.Sellp[2]*(float)m_Hq.Sellv[2]+
			m_Hq.Sellp[3]*(float)m_Hq.Sellv[3]+m_Hq.Sellp[4]*(float)m_Hq.Sellv[4];
		float fVolAll = (float)m_Hq.Sellv[0]+(float)m_Hq.Sellv[1]+(float)m_Hq.Sellv[2]+(float)m_Hq.Sellv[3]+(float)m_Hq.Sellv[4];
		if(fVolAll>COMPPREC)
			m_Hq_L2.SellAvp = fAmountAll/fVolAll;
		m_Hq_L2.TotalSellv = fVolAll;
	}
	if(m_Hq_L2.BuyAvp<COMPPREC||m_Hq_L2.TotalBuyv<1)
	{
		m_Hq_L2.BuyAvp = 0.0f;
		float fAmountAll = m_Hq.Buyp[0]*(float)m_Hq.Buyv[0]+m_Hq.Buyp[1]*(float)m_Hq.Buyv[1]+m_Hq.Buyp[2]*(float)m_Hq.Buyv[2]+
			m_Hq.Buyp[3]*(float)m_Hq.Buyv[3]+m_Hq.Buyp[4]*(float)m_Hq.Buyv[4];
		float fVolAll = (float)m_Hq.Buyv[0]+(float)m_Hq.Buyv[1]+(float)m_Hq.Buyv[2]+(float)m_Hq.Buyv[3]+(float)m_Hq.Buyv[4];
		if(fVolAll>COMPPREC)
			m_Hq_L2.BuyAvp = fAmountAll/fVolAll;
		m_Hq_L2.TotalBuyv = fVolAll;
	}
	
	int nSellTop = m_rcIn.top;
	int nSellBotton = nSellTop+m_rcIn.Height()*(m_TPPrice1-m_Hq.Sellp[0])/(m_TPPrice1-m_TPPrice2);
	if(m_Hq.Sellp[0]<COMPPREC) nSellBotton = nSellTop;
	int nSellAverMid = nSellTop+m_rcIn.Height()*(m_TPPrice1-m_Hq_L2.SellAvp)/(m_TPPrice1-m_TPPrice2);

	int nBuyBotton = m_rcIn.bottom;
	int nBuyTop = nBuyBotton-m_rcIn.Height()*(m_Hq.Buyp[0]-m_TPPrice2)/(m_TPPrice1-m_TPPrice2);
	if(m_Hq.Buyp[0]<COMPPREC) nBuyTop = nBuyBotton;
	int nBuyAverMid = nBuyBotton-m_rcIn.Height()*(m_Hq_L2.BuyAvp-m_TPPrice2)/(m_TPPrice1-m_TPPrice2);

	float fSBl = 0.5f;
	if(nSellAverMid<(nSellTop+nSellBotton)/2)
		fSBl = 1.0f-((float)nSellAverMid-(float)nSellTop)/((float)nSellBotton-(float)nSellTop);
	else 
		fSBl = 1.0f-((float)nSellBotton-(float)nSellAverMid)/((float)nSellBotton-(float)nSellTop);
	float fSFlag = pow(fSBl,0.382);
	if(nSellBotton==nSellTop) fSFlag = 0.0f;

	float fBBl = 0.5f;
	if(nBuyAverMid<(nBuyTop+nBuyBotton)/2)
		fBBl = 1.0f-((float)nBuyAverMid-(float)nBuyTop)/((float)nBuyBotton-(float)nBuyTop);
	else 
		fBBl = 1.0f-((float)nBuyBotton-(float)nBuyAverMid)/((float)nBuyBotton-(float)nBuyTop);
	float fBFlag = pow(fBBl,0.382);
	if(nBuyBotton==nBuyTop) fBFlag = 0.0f;

	int nSellHeight = 0.0f, nBuyHeight = 0.0f;
	float fSellHight = m_Hq_L2.TotalSellv/((float)nSellBotton-(float)nSellTop);
	if(nSellBotton==nSellTop) 
		fSellHight = 0.0f;
	float fBuyHight = m_Hq_L2.TotalBuyv/((float)nBuyBotton-(float)nBuyTop);
	if(nBuyBotton==nBuyTop) 
		fBuyHight = 0.0f;
	int nGoodWidth = (float)m_rcIn.Width()/max(fSFlag,fBFlag);
	if(fSellHight>fBuyHight) 
	{
		nSellHeight = nGoodWidth;
		nBuyHeight = nGoodWidth*(fBuyHight/fSellHight);
	}
	else
	{		
		nBuyHeight = nGoodWidth;
		nSellHeight = nGoodWidth*(fSellHight/fBuyHight);
	}

	//Sell
	CPoint pts[4]={0};
	pts[0].x = m_rcIn.left;
	pts[0].y = nSellTop;
	pts[1].x = (float)m_rcIn.left+fSFlag*(float)nSellHeight;
	pts[1].y = (float)nSellTop+fSFlag*((float)nSellAverMid-(float)nSellTop);
	pts[2].x = (float)m_rcIn.left+fSFlag*(float)nSellHeight;
	pts[2].y = (float)nSellBotton-fSFlag*((float)nSellBotton-(float)nSellAverMid);
	pts[3].x = m_rcIn.left;
	pts[3].y = nSellBotton;

	pDC->BeginPath();
	pDC->MoveTo(pts[0].x, pts[0].y);
	pDC->LineTo(pts[3].x, pts[3].y);
	pDC->PolyBezier(pts,4);
	pDC->EndPath();
	g_d.SelectBrush(pDC,DOWN_BRUSH);
	pDC->StrokeAndFillPath();

	g_d.DrawO(pDC, 2, nSellAverMid, &yellow, 3);

	//Buy
	CPoint ptb[4]={0};
	ptb[0].x = m_rcIn.left;
	ptb[0].y = nBuyTop;
	ptb[1].x = (float)m_rcIn.left+fBFlag*(float)nBuyHeight;
	ptb[1].y = (float)nBuyTop+fBFlag*((float)nBuyAverMid-(float)nBuyTop);
	ptb[2].x = (float)m_rcIn.left+fBFlag*(float)nBuyHeight;
	ptb[2].y = (float)nBuyBotton-fBFlag*((float)nBuyBotton-(float)nBuyAverMid);
	ptb[3].x = m_rcIn.left;
	ptb[3].y = nBuyBotton;

	pDC->BeginPath();
	pDC->MoveTo(ptb[0].x, ptb[0].y);
	pDC->LineTo(ptb[3].x, ptb[3].y);
	pDC->PolyBezier(ptb,4);
	pDC->EndPath();
	g_d.SelectBrush(pDC,UP_BRUSH);
	pDC->StrokeAndFillPath();

	g_d.DrawO(pDC, 2, nBuyAverMid, &yellow, 3);

	int i=4;
	while(m_Hq_L2.Sellp[i]<COMPPREC&&i>0)
	{
		m_Hq_L2.Sellp[i] = m_Hq_L2.Sellp[i-1];
		--i;
	}
	m_Hq_L2.Sellp[4] = m_Hq_L2.Sellp[i];
	if(m_Hq_L2.Sellp[4]<COMPPREC)
	{
		i=4;
		while(m_Hq.Sellp[i]<COMPPREC&&i>0)
		{
			m_Hq.Sellp[i] = m_Hq.Sellp[i-1];
			--i;
		}
		m_Hq_L2.Sellp[4] = m_Hq.Sellp[i];
	}

	i=4;
	while(m_Hq_L2.Buyp[i]<COMPPREC&&i>0)
	{
		m_Hq_L2.Buyp[i] = m_Hq_L2.Buyp[i-1];
		--i;
	}
	m_Hq_L2.Buyp[4] = m_Hq_L2.Buyp[i];
	if(m_Hq_L2.Buyp[4]<COMPPREC)
	{
		i=4;
		while(m_Hq.Buyp[i]<COMPPREC&&i>0)
		{
			m_Hq.Buyp[i] = m_Hq.Buyp[i-1];
			--i;
		}
		m_Hq_L2.Buyp[4] = m_Hq.Buyp[i];
	}

	//灰色可见委托区域
	int nSell10Top = nSellTop+m_rcIn.Height()*(m_TPPrice1-m_Hq_L2.Sellp[4])/(m_TPPrice1-m_TPPrice2);
	if(m_Hq_L2.Sellp[4]<COMPPREC)
		nSell10Top = nBuyTop;
	int nBuy10Bottom = nBuyBotton-m_rcIn.Height()*(m_Hq_L2.Buyp[4]-m_TPPrice2)/(m_TPPrice1-m_TPPrice2);
	if(m_Hq_L2.Buyp[4]<COMPPREC)
		nBuy10Bottom = nSellBotton;

	CRect rcVisual(0, nSell10Top, m_rcIn.Width()/3, nBuy10Bottom);

	CDC m_MyDC;
	CBitmap m_MyBitmap,*m_pOldBitmap;
	m_MyDC.CreateCompatibleDC(pDC);
	m_MyBitmap.CreateCompatibleBitmap(pDC,rcVisual.Width(),rcVisual.Height());
	m_pOldBitmap = m_MyDC.SelectObject(&m_MyBitmap);
	CRect RcFill(0,0,rcVisual.Width(),rcVisual.Height());
	g_d.Bar(&m_MyDC,&RcFill,INVERSE_BRUSH);
	m_MyDC.SetBkMode(TRANSPARENT);
	pDC->BitBlt(0,nSell10Top,Width(),Height(),&m_MyDC,0,0,SRCINVERT);
	m_MyDC.SelectObject(m_pOldBitmap);
	m_MyBitmap.DeleteObject();
	m_MyDC.DeleteDC();
	g_d.DrawO(pDC, 1, nSell10Top, &gray, 1);
	g_d.DrawO(pDC, 1, nBuy10Bottom, &gray, 1);

	int nCLoseY = nSellTop+m_rcIn.Height()*(m_TPPrice1-m_Hq.Close)/(m_TPPrice1-m_TPPrice2);
	CPen *oldpen = g_d.SelectPen(pDC,TXT_PEN);
	CFont *oldfont = g_d.SelectFont(pDC,SMALL2_FONT);	
	g_d.DrawLine(pDC, 0, nCLoseY, m_rcIn.Width(), nCLoseY);
	if(m_pStkInfo && (m_Hq_L2.TotalBuyv+m_Hq_L2.TotalSellv)>COMPPREC)
	{
		CString tmpTxt;
		tmpTxt.Format("%s 买比:%.1f%%",m_pStkInfo->Name,m_Hq_L2.TotalBuyv*100.0/(m_Hq_L2.TotalBuyv+m_Hq_L2.TotalSellv));
		g_d.DisplayText(pDC, m_rcIn.Width()/3, nCLoseY-15, VipColor.TXTCOLOR, m_rcIn.Width()*2/3-2, tmpTxt);
	}
	g_d.RestoreFont(pDC,oldfont);
	g_d.RestorePen(pDC,oldpen);
}
