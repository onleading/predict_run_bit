#include "stdafx.h"
#include "USTurn.h"

USTurn::USTurn(LPCOMINFO pComInfo):UBase(pComInfo)
{
	m_nSeatNum = 0;
	m_nSel = 0;

	m_bDataReady = FALSE;
}

USTurn::~USTurn()
{
}

int  USTurn::ProcessMsg(UINT message, WPARAM wParam, LPARAM lParam)
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
	if(!m_bDSFlag||m_pStkInfo->bigmarket!=HKGP_TYPE)
	{
		m_nSeatNum = 0;		
		if(message!=WM_PAINT)
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
		case 1:	
			m_bDataReady = TRUE;
			ReadData(TRUE);
			break;
		}
		ProcessMsg(WM_PAINT);
		return 1;
	case ASYN_HQ_MESSAGE:
		if(!m_bDataReady) return 1;
		if(wParam == m_nCurGP)
		{
			CURRDATA	tmpHq;
			EXCURRDATA	tmpHqEx;
			g_pStockIO->DSGetCurrData(m_pStkInfo,&tmpHq,&tmpHqEx);
			if(memcpy(&tmpHq,&m_Hq,sizeof(CURRDATA)) || memcpy(&tmpHqEx,&m_HqEx,sizeof(EXCURRDATA))) //如果发现行情有变,重新请求Seat信息
			{
				m_bDataReady = FALSE;
				ReadData(FALSE,TRUE);
			}
		}
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
	case WM_MOUSEMOVE:
		{			
			CPoint pt(LOWORD(lParam),HIWORD(lParam));
			pt.x-=left;pt.y-=top;
			CRect rc;
			int i,j,TextH=18,YFrom=3,XFrom=3;
			int RowNum=(Height()-6-22)/18;
			for(i=0;i < 4;i++)
			{
				for(j=0;j < RowNum;j++)
				{
					if(i*RowNum+j>=m_nSeatNum) return 1;
					rc.left=XFrom+i*(g_nFixedWidth-6)/4;
					rc.top=YFrom+j*TextH+1;
					rc.right=XFrom+(i+1)*(g_nFixedWidth-6)/4;
					rc.bottom=YFrom+(j+1)*TextH+1;
					if(rc.PtInRect(pt))
					{
						if((i*RowNum+j)!=m_nSel && (i*RowNum+j)<m_nSeatNum)
						{
							m_nSel = i*RowNum+j;
							ProcessMsg(WM_PAINT);
						}
						return 1;
					}
				}
			}
		}
		return 1;
	default:
		break;
	}
	return UBase::ProcessMsg(message,wParam,lParam);
}

void USTurn::ReadData(BOOL bDataReady,BOOL bReqData)
{
	m_nSeatNum = 0;
	memset(&m_Hq,0,sizeof(CURRDATA));
	memset(&m_HqEx,0,sizeof(EXCURRDATA));

	if(OnLine)
	{
		memset(m_Seat,0,sizeof(Seat_Content)*MAX_SEATNUM);
		g_pStockIO->DSGetCurrData(m_pStkInfo,&m_Hq,&m_HqEx);
		if(!bDataReady)
		{
			if(bReqData)
			{
				BYTE BSFlag;
#ifndef EN_VERSION
				BSFlag = 1;
#else
				BSFlag = 11;
#endif
				
				//先请求分时数据
				m_pView->SendMessage(UM_GETMAINID); //直接让父窗口填写g_nMainID
				g_nAssisID = SetComAssisID(m_nWhichUnit,1);
				g_pGetData->GetSeatInfo(m_pStkInfo,BSFlag,MAX_SEATNUM);
			}
		}
		else
		{			
			m_nSeatNum = g_pGetData->GetSeatInfoACK(m_pStkInfo,m_Seat,MAX_SEATNUM);
		}
		return;
	}
}

void USTurn::DrawIt(CDC *pDC)
{
	int i,j;
	if(Height() < 20) return;	//如果空间太小，不画
	int TextH=18,YFrom=3,XFrom=3;
	int RowNum=(Height()-6-22)/18;
	int DrawNum=0;
	for(i=0;i < 4;i++)
	{
		if(DrawNum>=m_nSeatNum) break;
		for(j=0;j < RowNum;j++)
		{
			if(DrawNum>=m_nSeatNum) break;
			if(m_Seat[DrawNum].SeatNo[0]=='+' || m_Seat[DrawNum].SeatNo[0]=='-' || m_Seat[DrawNum].SeatNo[0]==' ')
				g_d.DisplayText(pDC,XFrom+4,YFrom+j*TextH+1,VipColor.VOLCOLOR,m_Seat[DrawNum].SeatNo);
			else
				g_d.DisplayText(pDC,XFrom+4,YFrom+j*TextH+1,VipColor.TXTCOLOR,m_Seat[DrawNum].SeatNo);
			if(DrawNum==m_nSel)
				g_d.DrawRect(pDC,XFrom,YFrom+j*TextH,XFrom+(g_nFixedWidth-4)/4-2,YFrom+(j+1)*TextH-1,VipColor.DOWNCOLOR);
			DrawNum++;
		}
		XFrom+=(g_nFixedWidth-6)/4;
	}
	if(m_nSel>=0 && m_nSel<m_nSeatNum) //显示席位号
	{
		if(m_Seat[m_nSel].SeatNo[0]!='+' && m_Seat[m_nSel].SeatNo[0]!='-' && m_Seat[m_nSel].SeatNo[0]!=' ')
			g_d.DisplayText(pDC,7,Height()-22-2,VipColor.DOWNCOLOR,m_Seat[m_nSel].SeatStr[0]==0?m_Seat[m_nSel].SeatNo:_F(m_Seat[m_nSel].SeatStr));
	}
}
