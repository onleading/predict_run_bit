#include "stdafx.h"
#include "UKList.h"
#include "ComView.h"

UKList::UKList(LPCOMINFO pComInfo):UBase(pComInfo)
{
	m_nStart = m_nPos = m_nMaxItem = 0;
	m_nAnalyDataNum = 0;
	m_nDrawAnaly = 0;

	m_bDataReady = FALSE;
}

UKList::~UKList()
{

}

int  UKList::ProcessMsg(UINT message, WPARAM wParam, LPARAM lParam)
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
	case WM_PAINT:
		{
			if(wParam == 1)
				ProcessMsg(UM_READDATA,lParam);

			CREATE_TMPDC
			g_d.Bar(&m_MyDC,&m_rcIn,BACK_BRUSH);
			m_MyDC.SetBkMode(TRANSPARENT);

			CFont *oldfont = g_d.SelectFont(&m_MyDC,INFO_FONT);
			CPen  *oldpen = g_d.SelectPen(&m_MyDC,AXIS_PEN);
			g_d.DrawLine(&m_MyDC,0,0,0,m_rcIn.bottom);
			g_d.DrawLine(&m_MyDC,0,m_rcIn.bottom,m_rcIn.right,m_rcIn.bottom);
			DrawIt(&m_MyDC);
			g_d.RestorePen(&m_MyDC,oldpen);
			g_d.RestoreFont(&m_MyDC,oldfont);

			pDC->BitBlt (left,top,Width(),Height(),&m_MyDC,0,0,SRCCOPY); //从内存DC拷贝到屏幕DC
			DESTORY_TMPDC
		}
		return 1;
	case WM_KEYDOWN:
		{
			UINT nChar = (UINT)wParam;
			switch(nChar)
			{
			case VK_ESCAPE: //如果是日线报表,则返回至日线图
				m_pView->PostMessage(WM_COMMAND,ID_ZOOMTOCORNER);
				return 1;
			case VK_UP:
			case VK_PRIOR:
				if(m_nStart <= 0) return 1;
				m_nPos = max(max(m_nStart,m_nMaxItem)-1,0);
				ProcessMsg(WM_PAINT);
				return 1;
			case VK_DOWN:
			case VK_NEXT:
				if(m_nPos >= m_nDrawAnaly-1) return 1;
				m_nPos = min(m_nDrawAnaly-1,m_nPos+m_nMaxItem);
				ProcessMsg(WM_PAINT);
				return 1;
			default:				
				break;
			}
		}
		break;
	default:
		break;
	}
	return UBase::ProcessMsg(message,wParam,lParam);
}

//与ZST极其相似,请阅读Uzst中相关代码
void UKList::ReadData(BOOL bDataReady,BOOL bReqData)
{
	memset(m_pAnalyData,0,1024*sizeof(struct AnalyData));
	m_nAnalyDataNum = 0;
	m_nStart = m_nMaxItem = 0;

	if ( bDataReady ) 
	{
		if(OnLine)
		{
			m_nAnalyDataNum = g_pGetData->GetAnalyDataACK(m_pStkInfo,m_pAnalyData,PER_DAY,0);
		}
		else
		{
			m_nAnalyDataNum  = g_pStockIO->ReadVipDocData(m_pStkInfo,m_pAnalyData,RECENTNUM,PER_DAY);
		}
	}
	else if(OnLine)
	{
		m_pView->SendMessage(UM_GETMAINID); //直接让父窗口填写g_nMainID
		g_nAssisID = SetComAssisID(m_nWhichUnit,0);
		m_nAnalyDataNum = g_pGetData->GetAnalyData(m_pStkInfo,0,RECENTNUM,PER_DAY,1,m_pAnalyData);
	}

	m_nPos = m_nAnalyDataNum-1;
	m_nDrawAnaly = m_nAnalyDataNum;
}

char *KLineStr[] = {"时间","开盘","最高","最低","收盘","涨跌","涨幅(%%)","振幅(%%)",
"成交量","成交额","换手(%%)",NULL};
char *ZSKLineStr[] = {"时间","开盘","最高","最低","收盘","涨跌","涨幅(%%)","振幅(%%)",
"成交量","成交额",NULL};

void UKList::DrawIt(CDC *pDC)
{
	if(Height() < 20) return;	//如果空间太小，不画
	int TextH=HQ_TEXTH+1,YFrom=2,XFrom=4;
	int colnum,rownum,totalnum,drawnum,drawcol,tmpX,i,j;
	CSize size = g_d.DisplayText(pDC,16,YFrom,VipColor.BIGTITLECOLOR,_F("%6s %8s   日线报表"),m_pStkInfo->Code,m_pStkInfo->Name);
	g_d.DisplayText(pDC,size.cx+42,YFrom,GridColor.Grid_Level_Color,_F("Up/PageUp:上翻 Down/PageDown:下翻"));
	//画竖线
	int tmpColNum=m_bZs?11:12;
	colnum = max(1,(m_rcIn.Width()-XFrom)/KLIST_WIDTH);

	YFrom+=23;
	g_d.DrawLine(pDC,0,YFrom,colnum*KLIST_WIDTH+1,YFrom);

	for(i = 0;i < colnum;i++)
	{
		tmpX=2*KLIST_WIDTH/tmpColNum+i*KLIST_WIDTH;
		g_d.DrawLine(pDC,tmpX,YFrom,tmpX,m_rcIn.bottom);
		for(j=1;j < tmpColNum-2;j++)
			g_d.DrawLine(pDC,tmpX+j*KLIST_WIDTH/tmpColNum-2,YFrom,tmpX+j*KLIST_WIDTH/tmpColNum-2,m_rcIn.bottom);
		g_d.Bar(pDC,tmpX-2*KLIST_WIDTH/tmpColNum,YFrom+1,tmpX,m_rcIn.bottom,g_GUI.LightenScaleColor(50,VipColor.BACKCOLOR));
	}
	//画横线
	YFrom+=2;
	for(i = 0;i < colnum;i++)
	{
		int nXTmp = XFrom;
		for(j=0;m_bZs?(ZSKLineStr[j]!=NULL):(KLineStr[j]!=NULL);j++)
		{
			g_d.DisplayText(pDC,KLIST_WIDTH*i+nXTmp,YFrom,VipColor.TXTCOLOR,m_bZs?_F(ZSKLineStr[j]):_F(KLineStr[j]));
			if(j==0) nXTmp += 2*KLIST_WIDTH/tmpColNum;
			else nXTmp += KLIST_WIDTH/tmpColNum;
		}
		g_d.DrawLine(pDC,KLIST_WIDTH*(i+1),YFrom,KLIST_WIDTH*(i+1),m_rcIn.bottom);
	}
	YFrom+=20;
	g_d.DrawLine(pDC,0,YFrom,colnum*KLIST_WIDTH+1,YFrom);
	YFrom+=4;
	
	if(m_nDrawAnaly <= 0) return;
	//画具体的内容,totalnum为最多可容纳的条数
	rownum = (m_rcIn.Height()-YFrom)/(TextH+1);
	totalnum = rownum*colnum;
	m_nMaxItem = totalnum;
	drawnum = drawcol = 0;
	int oldYFrom = YFrom;
	m_nStart = max(0,m_nPos-totalnum+1);
	float tmpBasePrice;
	for(i = m_nStart; i <= m_nPos;i++)
	{	
		tmpBasePrice=(i>0)?m_pAnalyData[i-1].Close:m_pAnalyData[0].Open;
		//时间
		tmpX=XFrom;
		g_d.DisplayText(pDC,KLIST_WIDTH*drawcol+tmpX+2,YFrom,VipColor.TXTCOLOR,0,"%s",GetTdxDateWeekStr(m_pAnalyData[i].Time.Date));
		//开盘
		tmpX+=2*KLIST_WIDTH/tmpColNum;
		g_d.BjPrintf(pDC,KLIST_WIDTH*drawcol+tmpX,YFrom,m_pAnalyData[i].Open,tmpBasePrice,m_XsFlag,0);
		//最高
		tmpX+=KLIST_WIDTH/tmpColNum;
		g_d.BjPrintf(pDC,KLIST_WIDTH*drawcol+tmpX,YFrom,m_pAnalyData[i].High,tmpBasePrice,m_XsFlag,0);
		//最低
		tmpX+=KLIST_WIDTH/tmpColNum;
		g_d.BjPrintf(pDC,KLIST_WIDTH*drawcol+tmpX,YFrom,m_pAnalyData[i].Low,tmpBasePrice,m_XsFlag,0);
		//收盘
		tmpX+=KLIST_WIDTH/tmpColNum;
		g_d.BjPrintf(pDC,KLIST_WIDTH*drawcol+tmpX,YFrom,m_pAnalyData[i].Close,tmpBasePrice,m_XsFlag,0);
		tmpBasePrice=(i>0)?m_pAnalyData[i-1].Close:0.0;
		//涨跌
		tmpX+=KLIST_WIDTH/tmpColNum;
		if(tmpBasePrice > COMPPREC)
			g_d.BjPrintf(pDC,KLIST_WIDTH*drawcol+tmpX,YFrom,m_pAnalyData[i].Close-tmpBasePrice,0.0,m_XsFlag,0);
		//涨幅
		tmpX+=KLIST_WIDTH/tmpColNum;
		if(tmpBasePrice > COMPPREC)
			g_d.BjPrintf(pDC,KLIST_WIDTH*drawcol+tmpX,YFrom,100*(m_pAnalyData[i].Close-tmpBasePrice)/tmpBasePrice,0.0,m_XsFlag,0);
		//振幅
		tmpX+=KLIST_WIDTH/tmpColNum;
		if(tmpBasePrice > COMPPREC)
			g_d.BjPrintf(pDC,KLIST_WIDTH*drawcol+tmpX,YFrom,100*(m_pAnalyData[i].High-m_pAnalyData[i].Low)/tmpBasePrice,0.0,m_XsFlag,0);
		//成交量
		tmpX+=KLIST_WIDTH/tmpColNum;
		g_d.DisplayText(pDC,KLIST_WIDTH*drawcol+tmpX,YFrom,GridColor.Grid_Volume_Color,0,MakeJE3((float)(m_pAnalyData[i].Volume/m_pStkInfo->Unit)));
		//成交额
		tmpX+=KLIST_WIDTH/tmpColNum;
		g_d.DisplayText(pDC,KLIST_WIDTH*drawcol+tmpX,YFrom,VipColor.TXTCOLOR,0,MakeJE3(m_pAnalyData[i].Amount/m_pStkInfo->AmoUnit));
		//换手率
		tmpX+=KLIST_WIDTH/tmpColNum;
		if(!m_bZs && m_pStkInfo->ActiveCapital > 1)
		{
			float ftmp=0.0;
			if(m_bQz) //权证的换手率作特别处理
				ftmp=m_pAnalyData[i].Volume*m_pStkInfo->Unit/(100.0*m_pStkInfo->ActiveCapital);
			else
				ftmp=m_pAnalyData[i].Volume/(100.0*m_pStkInfo->ActiveCapital);
			g_d.DisplayText(pDC,KLIST_WIDTH*drawcol+tmpX,YFrom,VipColor.TXTCOLOR,0,"%6.2f",ftmp);
		}
		//		
		YFrom+=TextH+1;
		g_d.DrawLine(pDC,0,YFrom-1,KLIST_WIDTH*(drawcol+1),YFrom-1);
		drawnum++;
		if(drawnum >= rownum*(drawcol+1))
		{
			drawcol++;
			YFrom=oldYFrom;
		}
	}
}
