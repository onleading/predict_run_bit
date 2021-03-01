#include "stdafx.h"
#include "UHycj.h"
#include "ComView.h"

UHycj::UHycj(LPCOMINFO pComInfo):UBase(pComInfo)
{
	m_bEmpty = TRUE;
	m_bDataReady = FALSE;

	m_setcode = SH;
	int i;
	for(i=0;i<10;i++)
		m_HyRect[i].SetRectEmpty();
	for(i=0;i<5;i++)
		m_HyGPRect[i].SetRectEmpty();
	m_nCurHy = 0;
}

UHycj::~UHycj()
{

}

char *Level2_Hy_Code[]={"A","B","C0","C1","C2","C3","C4","C5","C6","C7",
                        "C8","D","E","F","G","H","I","J","K","L",
						"M","C9"};
char *Level2_Hy_Str[]={"农,林,牧,渔业","采掘业","食品,饮料","纺织,服装,皮毛","木材,家具","造纸,印刷","石油,化学,塑胶","电子","金属,非金属","机械,设备,仪表",
                       "医药,生物制品","电力煤气及水供应","建筑业","交通运输,仓储业","信息技术业","批发和零售贸易","金融,保险业","房地产业","社会服务业","传播与文化产业",
					   "综合类","其他类"};
char * GetHY_Str(char *HyCode)
{
	for(int i=0;i<22;i++)
	{
		if(stricmp(Level2_Hy_Code[i],HyCode)==0) break;
	}
	if(i<22)	
		return _F(Level2_Hy_Str[i]);
	return HyCode;
}

int  UHycj::ProcessMsg(UINT message, WPARAM wParam, LPARAM lParam)
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
	case WM_TIMER:
		if(wParam==0x1113)
			ProcessMsg(UM_READDATA);
		return 1;
	case WM_PAINT:
		{
			CREATE_TMPDC
			g_d.Bar(&m_MyDC,&m_rcIn,BACK_BRUSH);
			m_MyDC.SetBkMode(TRANSPARENT);

			CFont *oldfont = g_d.SelectFont(&m_MyDC,SMALL_FONT);
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
			int n,i=-1,j=-1;
			for(n=0;n<10;n++)
			{
				if(m_HyRect[n].PtInRect(pt))
				{i=n;break;}
			}
			for(n=0;n<5;n++)
			{
				if(m_HyGPRect[n].PtInRect(pt))
				{j=n;break;}
			}			
			if(i>=0 || j>=0)
			{
				SetCursor(m_hFxtMove);
				return 1;
			}
			else
				SetCursor(LoadCursor(NULL,IDC_ARROW));
		}
		return 1;
	case WM_LBUTTONDOWN:
		{
			CPoint pt(LOWORD(lParam),HIWORD(lParam));
			pt.x-=left;pt.y-=top;
			int n,i=-1,j=-1;
			for(n=0;n<10;n++)
			{
				if(m_HyRect[n].PtInRect(pt))
				{i=n;break;}
			}
			for(n=0;n<5;n++)
			{
				if(m_HyGPRect[n].PtInRect(pt))
				{j=n;break;}
			}
			if(i>=0)
			{
				m_nCurHy=i;
				ProcessMsg(WM_PAINT);
			}
			else if(j>=0)
			{
				MemStkInfo *tmpInfo=g_pStockIO->Get_MemStkInfo(g_Mtl.HYGPList[m_nCurHy*5+j].Code,m_setcode);
				if(tmpInfo)
					ToZst(tmpInfo->Code,tmpInfo->setcode,FALSE);
			}
		}
		return 1;
	case WM_LBUTTONDBLCLK:
		{
			CPoint pt(LOWORD(lParam),HIWORD(lParam));
			pt.x-=left;pt.y-=top;
			int n,i=-1,j=-1;
			for(n=0;n<10;n++)
			{
				if(m_HyRect[n].PtInRect(pt))
				{i=n;break;}
			}
			for(n=0;n<5;n++)
			{
				if(m_HyGPRect[n].PtInRect(pt))
				{j=n;break;}
			}
			if(i<0 && j<0)
			{
				int GPHeight=6*(HQ_TEXTH-2)+2;
				if(m_nZoomFlag==0)
				{
					if(pt.y<m_rcIn.Height()-GPHeight)	m_nZoomFlag=1;
					else								m_nZoomFlag=2;
				}
				else
					m_nZoomFlag=0;
			}
			ProcessMsg(WM_PAINT);
		}
		return 1;
	default:
		break;
	}
	return UBase::ProcessMsg(message,wParam,lParam);
}

void UHycj::ReadData(BOOL bDataReady,BOOL bReqData)
{
	if(!OnLine || !CanLevel2())
		return;
	if(!bDataReady)
	{
		if(bReqData)
		{
			m_pView->SendMessage(UM_GETMAINID); //直接让父窗口填写g_nMainID
			g_nAssisID = SetComAssisID(m_nWhichUnit,0);
			g_pGetData->GetHYRank(m_setcode);
		}
	}
	else
		g_pGetData->GetHYRankACK(g_Mtl.MarketTotalValueTraded,g_Mtl.HYList,g_Mtl.HYGPList);
}

void UHycj::DrawPart1(CDC *pDC,int TotalHeight,int TextH,int XWidth,int XFrom[],int YFrom)
{
	if(TotalHeight<2*TextH) return;
	g_d.DisplayText(pDC,XFrom[0],YFrom,VipColor.TXTCOLOR,_F("行业成交额排名"));
	g_d.DisplayText(pDC,XFrom[2],YFrom,VipColor.TXTCOLOR,XWidth,_F("成交额"));
	YFrom+=TextH;
	g_d.DotLine(pDC,0,YFrom-1,m_rcIn.right,YFrom-1,VipColor.AXISCOLOR,2);
	YFrom+=1;
	int i,row=(TotalHeight-1-TextH)/TextH;
	CSize size;
	char IndustryCode[10];
	for(i=0;i<min(10,row);i++)
	{
		strncpy(IndustryCode,g_Mtl.HYList[i].IndustryCode,8);
		IndustryCode[8]=0;
		if(strlen(IndustryCode)>0)
		{
			size=g_d.DisplayText(pDC,XFrom[0],YFrom,VipColor.TXTCOLOR,GetHY_Str(IndustryCode));
			m_HyRect[i].SetRect(XFrom[0],YFrom,XFrom[0]+size.cx,YFrom+TextH-2);
			g_d.DisplayText(pDC,XFrom[2],YFrom,VipColor.TXTCOLOR,XWidth,"%s",MakeJE(g_Mtl.HYList[i].IndustryTotalValueTraded));
		}
		YFrom+=TextH;	
	}
}

void UHycj::DrawPart2(CDC *pDC,int TotalHeight,int TextH,int XWidth,int XFrom[],int YFrom)
{
	if(TotalHeight<2*TextH) return;
	int i,row=TotalHeight/TextH;
	CSize size;
	MemStkInfo *tmpInfo;
	for(i=0;i<min(5,row);i++)
	{
		tmpInfo=g_pStockIO->Get_MemStkInfo(g_Mtl.HYGPList[m_nCurHy*5+i].Code,m_setcode);
		if(tmpInfo)
		{
			size=g_d.DisplaySameWidtchText(pDC,XFrom[0],YFrom,GetBlockGPColor(tmpInfo,GridColor.Grid_CodeName_Color),tmpInfo->Name);
			m_HyGPRect[i].SetRect(XFrom[0],YFrom,XFrom[0]+size.cx,YFrom+TextH-2);
			g_d.DisplayText(pDC,XFrom[2],YFrom,VipColor.TXTCOLOR,XWidth,"%s",MakeJE(g_Mtl.HYGPList[m_nCurHy*5+i].TotalValueTraded));
			YFrom+=TextH;
		}
	}
}

void UHycj::DrawIt(CDC *pDC)
{
	if(Height() < 20) return;	//如果空间太小，不画
	if(m_bZs || m_bDSFlag) return;
	int i;
	for(i=0;i<10;i++)
		m_HyRect[i].SetRectEmpty();
	for(i=0;i<5;i++)
		m_HyGPRect[i].SetRectEmpty();
	int nFixedWidth=m_rcIn.right;
	int TextH=HQ_TEXTH-2,YFrom=2,XFrom[3];
	XFrom[0]=HQ_XSPACE;
	XFrom[1]=(nFixedWidth-2*HQ_XSPACE)/3+HQ_XSPACE;
	XFrom[2]=(nFixedWidth-2*HQ_XSPACE)*2/3+HQ_XSPACE;
	int XWidth=(nFixedWidth-2*HQ_XSPACE)/3;
	int HYHeight=min(m_rcIn.bottom,11*TextH+2);
	if(m_nZoomFlag==1)
		DrawPart1(pDC,m_rcIn.bottom,TextH,XWidth,XFrom,YFrom);
	else if(m_nZoomFlag==2)
		DrawPart2(pDC,m_rcIn.bottom,TextH,XWidth,XFrom,YFrom);
	else
	{
		DrawPart1(pDC,HYHeight,TextH,XWidth,XFrom,YFrom);
		g_d.DrawLine(pDC,0,HYHeight,m_rcIn.right,HYHeight);
		YFrom=HYHeight+2;
		DrawPart2(pDC,m_rcIn.bottom-HYHeight,TextH,XWidth,XFrom,YFrom);
	}
}
