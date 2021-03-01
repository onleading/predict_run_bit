#include "stdafx.h"
#include "UBTurn.h"

#define	SEATBS_GAP	62

UBTurn::UBTurn(LPCOMINFO pComInfo):UBase(pComInfo)
{
	m_nSel = 0;
	m_bDrawName = FALSE;
	
	memset(m_Seat[0],0,sizeof(Seat_Content)*MAX_SEATNUM);
	memset(m_Seat[1],0,sizeof(Seat_Content)*MAX_SEATNUM);

	m_bDataReady = FALSE;
}

UBTurn::~UBTurn()
{
}

int  UBTurn::ProcessMsg(UINT message, WPARAM wParam, LPARAM lParam)
{
	if(m_nCurGP==-1) //���UnitΪ�գ������д���
	{
		if(message==WM_PAINT)
		{
			CREATE_TMPDC
			g_d.Bar(&m_MyDC,&m_rcIn,BACK_BRUSH);

			pDC->BitBlt (left,top,Width(),Height(),&m_MyDC,0,0,SRCCOPY); //���ڴ�DC��������ĻDC
			DESTORY_TMPDC
			return 1;
		}
		return UBase::ProcessMsg(message,wParam,lParam);
	}
	if(!m_bDSFlag||m_pStkInfo->bigmarket!=HKGP_TYPE)
	{
		if(message!=WM_PAINT)
			return UBase::ProcessMsg(message,wParam,lParam);
	}
	switch(message)
	{
	case UM_READDATA:
		m_bDataReady = FALSE;
		ReadData(FALSE,wParam==0);
		return 1;
	case UM_GETDATA_ACK://����Զ�̰�,����ͷ��طֿ�
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
			if(memcmp(&tmpHq,&m_Hq,sizeof(CURRDATA)) || memcmp(&tmpHqEx,&m_HqEx,sizeof(EXCURRDATA))) //������������б�,��������Seat��Ϣ
			{
				m_Hq = tmpHq;
				m_HqEx = tmpHqEx;
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

			CFont *oldfont = g_d.SelectFont(&m_MyDC,GRAPH_FONT);
			CPen *oldpen = g_d.SelectPen(&m_MyDC,AXIS_PEN);
			g_d.DrawLine(&m_MyDC,0,0,0,m_rcIn.bottom);
			if(g_GH.bLeftHQInfo && !m_nInCom2)
				g_d.DrawLine(&m_MyDC,m_rcIn.right-1,0,m_rcIn.right-1,m_rcIn.bottom);
			//���жϷ�ʱͼ���Ƿ����л���
			BOOL bCanSwitch=FALSE;
		#ifdef LEVEL2
			bCanSwitch=TRUE;
		#else
			if(CanLevel2())bCanSwitch=TRUE;
		#endif
			if(!m_nInCom2 && !bCanSwitch)
				g_d.DrawLine(&m_MyDC,m_rcIn.left,0,m_rcIn.right,0);
			g_d.DrawLine(&m_MyDC,0,m_rcIn.bottom,m_rcIn.right,m_rcIn.bottom);
			DrawIt(&m_MyDC);
			g_d.RestorePen(&m_MyDC,oldpen);
			g_d.RestoreFont(&m_MyDC,oldfont);

			pDC->BitBlt (left,top,Width(),Height(),&m_MyDC,0,0,SRCCOPY); //���ڴ�DC��������ĻDC
			DESTORY_TMPDC
		}
		return 1;
	case WM_LBUTTONDOWN:
		{
			CPoint pt(LOWORD(lParam),HIWORD(lParam));
			pt.x-=left;pt.y-=top;	
			if(m_rcCon.PtInRect(pt))
			{
				m_bDrawName = !m_bDrawName;
				ProcessMsg(WM_PAINT);
			}
		}
		return 1;
	default:
		break;
	}
	return UBase::ProcessMsg(message,wParam,lParam);
}

void UBTurn::ReadData(BOOL bDataReady,BOOL bReqData)
{
	memset(&m_Hq,0,sizeof(CURRDATA));
	memset(&m_HqEx,0,sizeof(EXCURRDATA));

	if(OnLine)
	{
		memset(m_Seat[0],0,sizeof(Seat_Content)*MAX_SEATNUM);
		memset(m_Seat[1],0,sizeof(Seat_Content)*MAX_SEATNUM);
		g_pStockIO->DSGetCurrData(m_pStkInfo,&m_Hq,&m_HqEx);
		if(!bDataReady)
		{
			if(bReqData)
			{
				m_pView->SendMessage(UM_GETMAINID); //ֱ���ø�������дg_nMainID
				g_nAssisID = SetComAssisID(m_nWhichUnit,1);
				g_pGetData->GetSeatInfo(m_pStkInfo,0,MAX_SEATNUM);
			}
		}
		else
			g_pGetData->GetSeatInfoACK(m_pStkInfo,m_Seat[0],m_Seat[1],MAX_SEATNUM);
		return;
	}
}

void UBTurn::DrawIt(CDC *pDC, BOOL bDrawName)
{
	bDrawName = m_bDrawName;

	int i,j;
	if(Height() < 20) return;	//����ռ�̫С������

	int TextH=18,YFrom=3,XFrom=3,Cell_Wid=58;
	g_d.DrawLine(pDC, 0, YFrom+TextH, m_rcIn.Width(), YFrom+TextH);
	g_d.DrawLine(pDC, Width()/2, 0, Width()/2, Height());
	//��ʾ������
	CRect rcTitle(XFrom, YFrom, m_rcIn.Width()-XFrom, YFrom+(TextH+3));
	int fcolor = VipColor.LEVELCOLOR;
	if (m_Hq.Buyp[0] > m_Hq.Close+0.00001)
		fcolor = VipColor.KPCOLOR;
	else if (m_Hq.Buyp[0] < m_Hq.Close-0.00001) 
		fcolor = VipColor.DOWNCOLOR;
	g_d.DisplayText(pDC, rcTitle, fcolor, DT_LEFT|DT_VCENTER, "��һ��: %.3f", m_Hq.Buyp[0]);
	fcolor = VipColor.LEVELCOLOR;
	if (m_Hq.Sellp[0] > m_Hq.Close+0.00001)
		fcolor = VipColor.KPCOLOR;
	else if (m_Hq.Sellp[0] < m_Hq.Close-0.00001) 
		fcolor = VipColor.DOWNCOLOR;
	g_d.DisplayText(pDC, rcTitle, fcolor, DT_RIGHT|DT_VCENTER, "��һ��: %.3f", m_Hq.Sellp[0]);
	//
	CString strShowSwitch=" �л������ ";
	if(bDrawName)
		strShowSwitch=" �л������ ";
	CSize codeoname = pDC->GetTextExtent(_T(strShowSwitch));
	m_rcCon.top = 0;
	m_rcCon.left = XFrom+(m_rcIn.Width()-2*XFrom-codeoname.cx)/2;
	m_rcCon.bottom = YFrom+TextH;
	m_rcCon.right = m_rcCon.left+codeoname.cx;
	g_d.Bar(pDC, &m_rcCon, AXIS_BRUSH);
	m_rcCon.top+=3;
	g_d.DisplayText(pDC, m_rcCon, VipColor.TXTCOLOR, DT_CENTER|DT_VCENTER, _F(strShowSwitch));

	YFrom += (TextH+5);
	
	int RowNum = (Height()-YFrom-2)/TextH;
	int ColNum = (Width()/2-5)/Cell_Wid;
	int DrawNum=0;
	XFrom = 5;
	for(i=0;i < ColNum;i++)
	{
		if(DrawNum>=MAX_SEATNUM) 
			break;
		for(j=0;j < RowNum;j++)
		{
			if(DrawNum>=MAX_SEATNUM) 
				break;
			CFont *pOldFont = g_d.SelectFont(pDC,INFO_FONT);
			if(m_Seat[0][DrawNum].SeatNo[0]=='+' || m_Seat[0][DrawNum].SeatNo[0]=='-' || m_Seat[0][DrawNum].SeatNo[0]==' ')
				g_d.DisplayText(pDC,XFrom,YFrom+j*TextH+1,VipColor.KPCOLOR,m_Seat[0][DrawNum].SeatNo);
			else
			{
				char SeatStr[61]={0};
				if(bDrawName)
				{
					long nSeatID = atol(m_Seat[0][DrawNum].SeatNo);
					if(GetSeatNameFromSID(nSeatID, SeatStr))
						pOldFont = g_d.SelectFont(pDC,SMALL2_FONT);
				}
				if(SeatStr[0]==0) 
					strcpy(SeatStr, m_Seat[0][DrawNum].SeatNo);
				g_d.DisplayText(pDC,XFrom,YFrom+j*TextH+1,VipColor.TXTCOLOR,SeatStr);
			}
			if(pOldFont) 
				g_d.RestoreFont(pDC,pOldFont); 			
			DrawNum++;
		}
		XFrom+=Cell_Wid;
	}

	DrawNum=0;
	XFrom=Width()/2+5;
	for(i=0;i < ColNum;i++)
	{
		if(DrawNum>=MAX_SEATNUM) 
			break;
		for(j=0;j < RowNum;j++)
		{
			if(DrawNum>=MAX_SEATNUM) 
				break;
			CFont *pOldFont = g_d.SelectFont(pDC,INFO_FONT);
			if(m_Seat[1][DrawNum].SeatNo[0]=='+' || m_Seat[1][DrawNum].SeatNo[0]=='-' || m_Seat[1][DrawNum].SeatNo[0]==' ')
				g_d.DisplayText(pDC,XFrom,YFrom+j*TextH+1,VipColor.KPCOLOR,m_Seat[1][DrawNum].SeatNo);
			else
			{
				char SeatStr[61]={0};
				if(bDrawName)
				{
					long nSeatID = atol(m_Seat[1][DrawNum].SeatNo);
					if(GetSeatNameFromSID(nSeatID, SeatStr))
						pOldFont = g_d.SelectFont(pDC,SMALL2_FONT);
				}
				if(SeatStr[0]==0) 
					strcpy(SeatStr, m_Seat[1][DrawNum].SeatNo);
				g_d.DisplayText(pDC,XFrom,YFrom+j*TextH+1,VipColor.TXTCOLOR,SeatStr);
			}
			if(pOldFont) 
				g_d.RestoreFont(pDC,pOldFont); 			
			DrawNum++;
		}
		XFrom+=Cell_Wid;
	}
}
