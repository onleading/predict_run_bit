#include "stdafx.h"
#include "UGlHq.h"
#include "GridView.h"

BOOL l_bWTOnlyShowLink = FALSE;

UGlHq::UGlHq(LPCOMINFO pComInfo):UBase(pComInfo)
{
	FontSize.cx		= FontSize.cy = 0;
	nTitleHeight	= 0;
	nCellHeight		= 0;
	nXHColWidth	= 0;

	m_GPRect.SetRectEmpty();

	m_nType			= GLHQ_BLOCK_GP;	
	m_nSwitchHeight	= SWITCH_HEIGHT;
    nTotalRow		= nTotalCol = 0;
	nStartRow		= nRowNum = nEndRow = nCurRow = 0;
	nStartCol		= nColNum = nEndCol = 0;
	nCurSortIndex	= 1;
	bAscendSort		= TRUE;

	nTotalCol		= GLHQ_COL_NUM;
	m_aGlHqIdx		= g_aGlHqIdx_Def;

	m_nBlockNum		= 0;
	m_nCurOrder		= DEF_GLHQ_ORDER;

	m_pVScrollBar	= NULL;
	m_pSwitchBar	= NULL;
	m_rcBlockHome.SetRectEmpty();

}

UGlHq::~UGlHq()
{
	if(m_pVScrollBar)	delete m_pVScrollBar;
	if(m_pSwitchBar)	delete m_pSwitchBar;
}

BOOL UGlHq::Create(CWnd *pParentWnd,int nType)
{
	m_nType			= nType;
	m_nSwitchHeight = 0;
	if(m_nType==GLHQ_BLOCK_GP)
		m_nSwitchHeight = SWITCH_HEIGHT;

	m_pVScrollBar=new CMyScrollBar;
	if(!m_pVScrollBar || !m_pVScrollBar->Create(_T("MyScrollBar"),"",WS_CHILD|WS_VISIBLE,CRect(0,0,0,0),pParentWnd,ID_GLHQ_VSCROLL))
		return FALSE;
	m_pVScrollBar->Init(X_VERTICAL,pParentWnd->m_hWnd,UM_VSCROLLBAR,m_nWhichUnit);
	m_pSwitchBar = new CNewZfjsTab(pParentWnd->m_hWnd);
	m_pSwitchBar->SetStyle(SwitchStyle_default);
	if(!m_pSwitchBar || !m_pSwitchBar->Create(_T("NewZfjsTab"),"",WS_CHILD|WS_VISIBLE,CRect(0,0,0,0),pParentWnd,ID_GLHQ_SWITCHBAR))
		return FALSE;
	return TRUE;
}


int  UGlHq::ProcessMsg(UINT message, WPARAM wParam, LPARAM lParam)
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
	switch(message)
	{
	case UM_READDATA:	//��ȡ����(������Or��������)
 		ReadData(wParam==0);
		ProcessMsg(WM_PAINT);
		return 1;
	case WM_TIMER:		//��ʱˢ��
		ProcessMsg(WM_PAINT);
		return 1;
	case UM_BK_GLHQ_OK:	//���ݵõ���
		if(nCurSortIndex>0)
			Sort();
		ProcessMsg(WM_PAINT);
		return 1;
	case WM_PAINT:		//��
		{
			if(wParam == 1)
				ProcessMsg(UM_READDATA,lParam);

			CREATE_TMPDC
			g_d.Bar(&m_MyDC,&m_rcIn,BACK_BRUSH);
			m_MyDC.SetBkMode(TRANSPARENT);

			CFont *oldfont = g_d.SelectFont(&m_MyDC,SMALL2_FONT);
			CPen *oldpen = g_d.SelectPen(&m_MyDC,AXIS_PEN);

			g_d.DrawLine(&m_MyDC,0,0,0,m_rcIn.bottom);
			if(g_GH.bLeftHQInfo && !m_nInCom2)
				g_d.DrawLine(&m_MyDC,m_rcIn.right-1,0,m_rcIn.right-1,m_rcIn.bottom);
			if(g_bTickTStatus)
				g_d.DrawLine(&m_MyDC,0,m_rcIn.top,m_rcIn.right,m_rcIn.top);
			g_d.DrawLine(&m_MyDC,0,m_rcIn.bottom,m_rcIn.right,m_rcIn.bottom);
			DrawIt(&m_MyDC);

			g_d.RestorePen(&m_MyDC,oldpen);
			g_d.RestoreFont(&m_MyDC,oldfont);

			pDC->BitBlt(left,top,Width()-GLHQ_VSCROLL_WID,Height()-m_nSwitchHeight,&m_MyDC,0,0,SRCCOPY); //���ڴ�DC��������ĻDC
			DESTORY_TMPDC
		}
		return 1;
	case WM_SIZE:
		UBase::ProcessMsg(message,wParam,lParam);
		SetControlPos();
		return 1;
	case UM_GLHQ_SWITCH://�л����
		{
			m_nCurOrder = wParam;
			short	nStkIndex[MAXGPNUM]={0};
			int		nStkNum = GetGPDomain(nStkIndex,MAXGPNUM);
			ToSomeRow(0);
			SetGPDomain(nStkIndex,nStkNum);
			SubscribleHQ();
			ProcessMsg(WM_PAINT);
		}
		return 1;
	case WM_LBUTTONDBLCLK:
		{
			CPoint pt(LOWORD(lParam),HIWORD(lParam));
			pt.x-=left;pt.y-=top;
			OnClick(pt);
			int iNo = GetCurrentGPIndex();
			MemStkInfo *pInfo = (*g_pStockIO)[iNo];
			if(pInfo)
			{
				if(m_nInCom2)
					ToZst(pInfo->Code,pInfo->setcode,FALSE);
				else
					ShowGpInCom(pInfo->Code,pInfo->setcode,FALSE,lpnUsedStkIndex,nTotalRow,iNo);
			}
		}
		return 1;
	case WM_LBUTTONDOWN:
		{
			CPoint pt(LOWORD(lParam),HIWORD(lParam));
			pt.x-=left;pt.y-=top;	
			if(m_rcBlockHome.PtInRect(pt))
			{
				int nBlockNo = m_BlockInf[m_nCurOrder].nBlockNo;
				CGridView *tempWnd = (CGridView *)SwitchToView("Grid");
				if(tempWnd)
					tempWnd->m_pHQGrid->ProcessMsg(WM_CHANGEBLOCK,nBlockNo);
				return 1;
			}
			OnClick(pt);
			OnLButtonDown(pt);
		}
		return 1;
	case UM_VSCROLLBAR:
		{		
			int nPos = (int)wParam;
			if(nPos != nStartRow)
			{
				int tempint = nCurRow-nStartRow;
				nStartRow=nPos;
				if(nStartRow+tempint<nTotalRow-1)
					nCurRow=nStartRow+tempint;
				else
					nCurRow=nTotalRow-1;
				ProcessMsg(WM_PAINT);
			}
			m_pVScrollBar->SetScrollPos(nPos);	
		}
		return 1;
	case UM_GET_STOCK:
		return GetCurrentGPIndex();
	default:
		break;
	}
	return UBase::ProcessMsg(message,wParam,lParam);
}

//�Ƿ�����С��״̬����С��״̬����ȡ����,����
BOOL UGlHq::IsHideStatus()
{
	return (m_rcIn.Width()<1 || m_rcIn.Height()<1);
}

void UGlHq::ReadData(BOOL bReqData)
{
	if(IsHideStatus()) 
		return;
	if(!bReqData && OnLine) //�ѻ�ʱ,ҲҪ��ʾ����
		return;
	//�õ����ɵĹ������,�õ���ǰ������
	AfterSetStock();
	//��������
	SubscribleHQ();
}

void UGlHq::DrawIt(CDC *pDC)
{
	if(IsHideStatus()) 
		return;
	//������ߴ�
	GetSomeSize(pDC);
	//����RowNum��ColNum
	AdjustRowColNum();
	//��������
	DrawGrid(pDC);
}

short UGlHq::GetCurrentGPIndex()
{
	if(nTotalRow<1)	return -1;
	if(lpnUsedStkIndex[nCurRow]<0) return -1;
	return(lpnUsedStkIndex[nCurRow]);
}

void UGlHq::AfterSetStock()
{
	if(m_nType==GLHQ_BLOCK_GP||m_nType==GLHQ_ONLYGP)
	{
		m_nBlockNum = GetInBlockInfo(m_nCurGP,m_BlockInf,TRUE);
		if(m_pSwitchBar)
		{
			m_pSwitchBar->ClearAllLabel();
			for(int i=0;i<m_nBlockNum;i++)
				m_pSwitchBar->InsertSwitchLabel(i,m_BlockInf[i].strBlockName,WM_COMMAND,ID_GLHQ_SWITCH1+i);
		}
		SetControlPos();
	}

	short	nStkIndex[MAXGPNUM]={0};
	int		nStkNum = GetGPDomain(nStkIndex,MAXGPNUM);
	SetGPDomain(nStkIndex,nStkNum);
}

void UGlHq::SubscribleHQ()
{
	//�˶��Ŀ��Ա�ȡ��
	if(nTotalRow>0&&(m_nType==GLHQ_BLOCK_GP||m_nType==GLHQ_ONLYGP))
		g_pBkData->Subscrible(m_pView->m_hWnd,UM_BK_GLHQ_OK,m_nWhichUnit,0,WIDEDATA_HQ,lpnUsedStkIndex,nTotalRow,1);
}

//�õ���ǰ������Ĺ�Ʊ��
int UGlHq::GetGPDomain(short *lpnStkIndex,int nMaxNum)
{
	if(m_nCurGP<0) return 0;
	//�������鲿�ֵĴ���
	if(m_nCurOrder<0 || m_nCurOrder>=m_nBlockNum)
		m_nCurOrder = DEF_GLHQ_ORDER;
	if(m_nCurOrder<0 || m_nCurOrder>=m_nBlockNum)
		m_nCurOrder = 0;
	if(m_pSwitchBar)
		m_pSwitchBar->SetCurLabel(m_nCurOrder);
	int nBlockNo = m_BlockInf[m_nCurOrder].nBlockNo;
	if(nBlockNo==-10)
		return GetGlGpIndex(m_nCurGP,lpnStkIndex,nMaxNum);
	else if(nBlockNo==-11)
	{
		//����Ʒ��
		if(IsLCIdx(m_pStkInfo))
			return GetLcIdxGP(m_pStkInfo->Code,lpnStkIndex,nMaxNum);
	}
	return GetGpIndex(nBlockNo,-1,lpnStkIndex,nMaxNum);
}

//����Ʊ�����õ���Unit��
void UGlHq::SetGPDomain(short *lpnStkIndex,int nStkNum)
{

	nTotalCol	= GLHQ_COL_NUM;
	m_aGlHqIdx	= g_aGlHqIdx_Def;

	//���Ϊ�գ���ʾ�ָ���ԭ������
	if(!lpnStkIndex)
	{
		if(nTotalRow>0)
			memcpy(lpnUsedStkIndex,Raw_lpnUsedStkIndex,nTotalRow*sizeof(short));
		return;
	}
	//��ʼ��ֵ
	nTotalRow = max(0,nStkNum);
	if(nTotalRow>0)
	{
		memcpy(lpnUsedStkIndex,lpnStkIndex,nTotalRow*sizeof(short));
		memcpy(Raw_lpnUsedStkIndex,lpnUsedStkIndex,nTotalRow*sizeof(short));
		//��RowIndex��ֵ
		for(int i=0;i<nTotalRow;i++)
			lpnUsedRowIndex[i] = Raw_lpnUsedRowIndex[i] = i;
	}	
	if(nCurSortIndex>0)
		Sort();
	if(m_pVScrollBar)
	{
		m_pVScrollBar->SetScrollRange (0,nTotalRow-1);
		if(nCurRow>=nTotalRow)
			ToSomeRow(0);
	}
}

//********************************************************//
//                �ڲ��ú���							  //
//********************************************************//

void UGlHq::SetControlPos()
{
	if(!m_pSwitchBar || !m_pVScrollBar) return;	
	m_pSwitchBar->SetWindowPos(NULL,left,bottom-m_nSwitchHeight,Width()-GLHQ_VSCROLL_WID,m_nSwitchHeight,SWP_SHOWWINDOW);
	m_pVScrollBar->SetWindowPos(NULL,right-GLHQ_VSCROLL_WID,top,GLHQ_VSCROLL_WID,Height(),SWP_SHOWWINDOW);

	m_pVScrollBar->SetScrollPos(m_pVScrollBar->GetScrollPos());

	if(g_d.IsBlackColorScheme(VipColor.BACKCOLOR))
		m_pSwitchBar->SetFontColorSchema(VipColor.BACKCOLOR,RGB(128,0,0),RGB(150,150,150),RGB(45,45,45),VipColor.BACKCOLOR,VipColor.BACKCOLOR,RGB(128,255,255),VipColor.BACKCOLOR,RGB(110,0,0),15,"Arial");
	else
		m_pSwitchBar->SetFontColorSchema(VipColor.BACKCOLOR,VipColor.BACKCOLOR,RGB(210,210,210),VipColor.BACKCOLOR,VipColor.AXISCOLOR,VipColor.TXTCOLOR,VipColor.KPCOLOR,VipColor.TXTCOLOR,VipColor.AXISCOLOR,15,"Arial");
	m_pVScrollBar->SetScrollColor(VipColor.AXISCOLOR,1,VipColor.BACKCOLOR,VipColor.AXISCOLOR);
}

void UGlHq::GetSomeSize(CDC *pDC)
{
	FontSize = pDC->GetTextExtent("8");
	nTitleHeight = FontSize.cy+3;
	nCellHeight = FontSize.cy+6;
	nXHColWidth = FontSize.cx*4;

	m_GPRect=m_rcIn;
	m_GPRect.right=m_GPRect.right-GLHQ_VSCROLL_WID;
	m_GPRect.bottom=m_GPRect.bottom-m_nSwitchHeight;
}

void UGlHq::AdjustRowColNum()
{
	//��������
	nRowNum = (m_GPRect.Height()-nTitleHeight)/nCellHeight;
	if(nStartRow + nRowNum >= nTotalRow)
		nRowNum = nTotalRow-nStartRow;
	nEndRow = nStartRow + nRowNum - 1;
	if(nEndRow < 0) nEndRow = -1;
	//��������
	if(nStartCol + nColNum >= nTotalCol)
		nColNum = nTotalCol-nStartCol;
	nEndCol = nStartCol + nColNum-1;
	if(nEndCol < 0) nEndCol = 0;
}

int UGlHq::GetItemWidth(int ColID)
{
	if(ColID<0 || ColID>=TOTAL_COL)
		return 0;
	if(ColID==ZQJC && m_nCurOrder==0 && m_nType==GLHQ_BLOCK_GP)
		return 18*FontSize.cx+4;
	return g_aAllItemWidth[ColID]*FontSize.cx+4;
}

int UGlHq::GetCellLeft(int nCol,int & nWidth) 
{   
	int i=0,xtmp=m_GPRect.left+nXHColWidth;
	while(i<nCol)
	{   
		xtmp+=GetItemWidth(m_aGlHqIdx[i]);
		i++;
	}
	nWidth=GetItemWidth(m_aGlHqIdx[nCol]);
	return xtmp;
}

LPSTR UGlHq::GetItemTitle(int ColID)
{
	return g_lpszAllItemName[ColID];
}

void UGlHq::ToSomeRow(int row)
{
	nCurRow = nStartRow = row;
	if(m_pVScrollBar)
		m_pVScrollBar->SetScrollPos(row);
}

//********************************************************//
//                ���廭����							  //
//********************************************************//

void UGlHq::DrawGrid(CDC *pDC)
{
	//����Ŀ��,�����г���ʼһЩ����:����nColNum
	DrawTitle(pDC);
	//��ʼ����������
	MemStkInfo *	pInfo=m_pStkInfo;
	CurrStockData	hqtmp={0};
	EXCURRDATA		hqtmp_ex={0};
	for(int j = 0;j < nRowNum;j++)
	{
		g_d.DisplayText(pDC,m_GPRect.left,m_GPRect.top+nTitleHeight+j*nCellHeight+(nCellHeight-FontSize.cy)/2,VipColor.TXTCOLOR,nXHColWidth-6,"%4d",nStartRow+j+1);	
		pInfo = (*g_pStockIO)[lpnUsedStkIndex[nStartRow+j]];
		if(!pInfo) continue;
		g_pStockIO->ReadCurrStockData (pInfo,&hqtmp,NULL,&hqtmp_ex);
		for(int i=0;i<nColNum;i++)
			DrawItem(pDC,j,i+nStartCol,pInfo?GetXSFlag(pInfo):NULL,pInfo,&hqtmp,&hqtmp_ex);
	}
	//��ָʾ��
	DrawHighLine(pDC);
}

void UGlHq::DrawTitle(CDC *pDC)
{
	m_rcBlockHome.SetRectEmpty();
	if(m_nCurOrder>=0 && m_nCurOrder<m_nBlockNum)
	{
		int nBlockNo = m_BlockInf[m_nCurOrder].nBlockNo;
		if(nBlockNo>=0)
		{
			m_rcBlockHome.SetRect(10,2,26,18);
			CRect rcTmpIn;

			rcTmpIn = m_rcBlockHome;
			rcTmpIn.DeflateRect(3,2,3,3);
			g_d.DrawRect(pDC, rcTmpIn, VipColor.AXISCOLOR, PS_SOLID, 1);
			g_d.DrawLine(pDC, rcTmpIn.left, rcTmpIn.top+4, rcTmpIn.right, rcTmpIn.top+4, VipColor.AXISCOLOR);
		}
	}
	CFont *oldfont = g_d.SelectFont(pDC,SMALL_FONT);
	CString tmpTitle;
	int		width=0,xtmp=m_GPRect.left+nXHColWidth;
	CSize	tmpSize;
	int		tempTitlePos = m_GPRect.top+3;
	for(int i=0;i<nTotalCol && xtmp < m_GPRect.right;i++)
	{
		int	ColID = m_aGlHqIdx[i];
		tmpTitle = GetItemTitle(ColID);
		width = GetItemWidth(ColID);
		if(width<1) continue;
		if(xtmp+width < m_GPRect.right+3) //���ڳ������Ĳ���,����
		{
			if((nCurSortIndex!=nStartCol+i) || nCurSortIndex==0 )
			{
				if(ColID==ZQJC)
					g_d.DisplayText(pDC,xtmp+4,tempTitlePos,GridColor.GridTitleTextColor,0,_F(tmpTitle));
				else
					g_d.DisplayText(pDC,xtmp,tempTitlePos,GridColor.GridTitleTextColor,width-4,_F(tmpTitle));
			}
			else
			{
				if(ColID==ZQJC)
				{
					tmpSize=g_d.DisplayText(pDC,xtmp+1,tempTitlePos,GridColor.GridTitleTextColor,0,_F(tmpTitle));
					g_d.DisplayText(pDC,xtmp+tmpSize.cx-3,tempTitlePos,GridColor.GridTitleArrowColor,0,(bAscendSort==TRUE)?_F("��"):_F("��"));
				}
				else
				{
					g_d.DisplayText(pDC,xtmp,tempTitlePos,GridColor.GridTitleTextColor,width-FontSize.cx-1,_F(tmpTitle));
					g_d.DisplayText(pDC,xtmp,tempTitlePos,GridColor.GridTitleArrowColor,width+2,(bAscendSort==TRUE)?_F("��"):_F("��"));
				}
			}
		}
		xtmp+=width;
	}
	nColNum = i;
	g_d.RestoreFont(pDC,oldfont);
}

void UGlHq::DrawGPName(CDC *pDC,int row,int col,char *lpszStr,COLORREF color,BOOL LeftFlag,BOOL SameWidth,MemStkInfo *pStock)
{
	int width = 0,tempPos;
 	int x = GetCellLeft(col,width);
	//ĳЩ�в�������˴���
	if(width<1) return;
	int nLeftPos  = x;
	CSize zs = CSize(0, 0);
	tempPos = m_GPRect.top+nTitleHeight + row*nCellHeight + (nCellHeight-FontSize.cy)/2;
	if(x+width < m_GPRect.right+3) //���ڳ������Ĳ���,����
	{
		nLeftPos = LeftFlag?x+5:x;
		if(SameWidth)
			zs = g_d.DisplaySameWidtchText(pDC,nLeftPos,tempPos,color,lpszStr);
		else
			zs = g_d.DisplayText(pDC,nLeftPos,tempPos,color,LeftFlag?0:width-5,lpszStr);
	}
	//д��ʶ
	DrawMarkFlag(pStock,pDC,nLeftPos+zs.cx, tempPos-2,left,top,m_pView);
}

//PosFlag��ʾ�ڷ�����д��λ�ã�0:����,1:ƫ��,2:ƫ��,PosFlag2��ʾ���ֶ��뷽ʽ,0:��,1:����,2:��
void UGlHq::DrawCellStringText(CDC *pDC,int row,int col,int PosFlag,int PosFlag2,COLORREF color,char *fmt,...)
{
    va_list arg_ptr;
    char tmp[256];	//���֧��255���ַ�
    va_start(arg_ptr,fmt);
    vsprintf(tmp,fmt,arg_ptr);
    va_end(arg_ptr);
	CString tmpstr=tmp;
	tmpstr.TrimLeft();
	tmpstr.TrimRight();

	int width = 0;
	int x = GetCellLeft(col,width);
	//ĳЩ�в�������˴���
	if(width<1) return;
	int nLeftPos  = x;
	if(x+width >= m_GPRect.right+3) //���ڳ������Ĳ���,����
		return;

	UINT nFormat1,nFormat2;
	switch(PosFlag)  
	{
	case 0:	nFormat1=DT_VCENTER;	break;
	case 1:	nFormat1=DT_TOP;		break;
	case 2:	nFormat1=DT_BOTTOM;		break;
	}
	switch(PosFlag2)  
	{
	case 0:	nFormat2=DT_LEFT;		break;
	case 1:	nFormat2=DT_CENTER;		break;
	case 2:	nFormat2=DT_RIGHT;		break;
	}

	int y=m_GPRect.top+nTitleHeight + row*nCellHeight;
	int oldmode=pDC->SetBkMode(TRANSPARENT);
	COLORREF oldcor= pDC->SetTextColor(color);
	pDC->DrawText(_F(tmpstr),CRect(nLeftPos+4,y,nLeftPos+width-4,y+nCellHeight),DT_SINGLELINE|nFormat1|nFormat2);
	pDC->SetTextColor(oldcor);
	pDC->SetBkMode(oldmode);
}

void UGlHq::DrawItem(CDC *pDC,int row,int col,int xsflag,MemStkInfo * pInfo,CurrStockData * hqp,EXCURRDATA *hqpex)
{
	int		nColItem = m_aGlHqIdx[col];
	float	ftmp,float_out;

	switch(nColItem)
	{
	case ZQJC:	//��Ʊ����
		{
			if(!pInfo) break;
			COLORREF cColor = GetBlockGPColor(pInfo,GridColor.Grid_CodeName_Color,-1);
			if(pInfo==m_pStkInfo)
				cColor = VipColor.LEVELCOLOR;
			DrawGPName(pDC,row,col,pInfo->Name,cColor,TRUE,TRUE,pInfo);
		}
		break;
	case ZAF:	//�Ƿ�
		if (hqp->Close>COMPPREC)
		{	
			ftmp=hqp->Now;
			//�����ǵ��ͷ��ȵļ��㷽��,���ڿ���ʽ���Ͼ�������
			if(ftmp<COMPPREC && hqp->Buyp[0]>COMPPREC && fabs(hqp->Buyp[0]-hqp->Sellp[0])<COMPPREC)
				ftmp=hqp->Buyp[0];
			if(ftmp>COMPPREC)
			{
				ftmp = ftmp - hqp->Close;
				float_out = ftmp/hqp->Close*(float)100.0;
				DrawCellStringText(pDC,row,col,0,2,GetItemColor(float_out,.0),"%s",FormatFloat2Str(float_out,XS2));
				break;
			}
		}
		DrawCellStringText(pDC,row,col,0,2,VipColor.TXTCOLOR,MEANLESS_STR);
		break;
	case ZJCJ:  //��ǰ�ּ�
		if(hqp->Now>COMPPREC)
			DrawCellStringText(pDC,row,col,0,2,GetItemColor(hqp->Now,hqp->Close),"%s",FormatFloat2Str(hqp->Now,xsflag));
		else
			DrawCellStringText(pDC,row,col,0,2,VipColor.TXTCOLOR,MEANLESS_STR);
		break;
	case QRSD:	//���ǵ�
		ftmp=hqp->Now;
		//�����ǵ��ͷ��ȵļ��㷽��,���ڿ���ʽ���Ͼ�������
		if(ftmp<COMPPREC && hqp->Buyp[0]>COMPPREC && fabs(hqp->Buyp[0]-hqp->Sellp[0])<COMPPREC)
			ftmp=hqp->Buyp[0];
		if(ftmp>COMPPREC)
		{
			ftmp = (ftmp - hqp->Close);
			DrawCellStringText(pDC,row,col,0,2,GetItemColor(ftmp,.0),"%s",FormatFloat2Str(ftmp,xsflag));
			break;
		}
		DrawCellStringText(pDC,row,col,0,2,VipColor.TXTCOLOR,MEANLESS_STR);
		break;
	case ZGJM:  //����
		if(!pInfo) break;
		if( !testzs(pInfo) && hqp->Buyp[0] > COMPPREC)
			DrawCellStringText(pDC,row,col,0,2,GetItemColor(hqp->Buyp[0],hqp->Close),"%s",FormatFloat2Str(hqp->Buyp[0],xsflag));
		else
			DrawCellStringText(pDC,row,col,0,2,VipColor.TXTCOLOR,MEANLESS_STR);
		break;
	case ZDJM:	//����
		if(!pInfo) break;
		if( !testzs(pInfo) && hqp->Sellp[0] > COMPPREC)
			DrawCellStringText(pDC,row,col,0,2,GetItemColor(hqp->Sellp[0],hqp->Close),"%s",FormatFloat2Str(hqp->Sellp[0],xsflag));
		else
			DrawCellStringText(pDC,row,col,0,2,VipColor.TXTCOLOR,MEANLESS_STR);
		break;
	case CJL:   //�ɽ���
		DrawCellStringText(pDC,row,col,0,2,GridColor.Grid_Volume_Color,MakeVol3(hqp->Volume));
		break;
	case XS:	//����
		if( !testzs(pInfo) )
		{
			COLORREF cColor;
			if(pInfo->setcode<2 && IsHligtNowVol(hqp->NowVol,pInfo))
				cColor=RGB(192,0,192);
			else
				cColor=(hqp->Flag.InOutFlag==0)?(GridColor.Grid_Up_Color):((hqp->Flag.InOutFlag==1)?GridColor.Grid_Down_Color:GridColor.Grid_Level_Color);
			DrawCellStringText(pDC,row,col,0,2,cColor,MakeVol3(hqp->NowVol));
			break;
		}
		DrawCellStringText(pDC,row,col,0,2,VipColor.TXTCOLOR,MEANLESS_STR);
		break;
	case ZANGSU://����
		if(OnLine && CanShowZangsu(pInfo))
		{
			ftmp = pInfo->zangsu;
			if(hqp->Now>COMPPREC)
			{
				DrawCellStringText(pDC,row,col,0,2,GetItemColor(ftmp,.0),"%s",FormatFloat2Str(ftmp,XS2));
				break;
			}
		}
		DrawCellStringText(pDC,row,col,0,2,VipColor.TXTCOLOR,MEANLESS_STR);
		break;
	case J_HSL:	//������
		if( !testzs(pInfo) && pInfo->ActiveCapital > 1 )
		{
			float_out=hqp->Volume*pInfo->Unit/(pInfo->ActiveCapital*100.0);
			DrawCellStringText(pDC,row,col,0,2,VipColor.TXTCOLOR,"%s",FormatFloat2Str(float_out,XS2));
			break;
		}
		DrawCellStringText(pDC,row,col,0,2,VipColor.TXTCOLOR,MEANLESS_STR);
		break;
	case J_LTSZ: //��ͨ��ֵ����(��)
		if( !testzs(pInfo) )
		{
			if(hqp->Now>COMPPREC)	ftmp = hqp->Now;
			else					ftmp = hqp->Close;
			float_out=pInfo->ActiveCapital*ftmp/10000.0;
			DrawCellStringText(pDC,row,col,0,2,VipColor.TXTCOLOR,_F("%14.2f��"),float_out);
			break;
		}
		DrawCellStringText(pDC,row,col,0,2,VipColor.TXTCOLOR,MEANLESS_STR);
		break;
	case JRKP:	//����
		if(hqp->Open>COMPPREC)
			DrawCellStringText(pDC,row,col,0,2,GetItemColor(hqp->Open,hqp->Close),"%s",FormatFloat2Str(hqp->Open,xsflag));
		else
			DrawCellStringText(pDC,row,col,0,2,VipColor.TXTCOLOR,MEANLESS_STR);
		break;
	case ZRSP:	//���̼�
		if(hqp->Close>COMPPREC)
			DrawCellStringText(pDC,row,col,0,2,VipColor.TXTCOLOR,"%s",FormatFloat2Str(hqp->Close,xsflag));
		else
			DrawCellStringText(pDC,row,col,0,2,VipColor.TXTCOLOR,MEANLESS_STR);
		break;
	case ZGCJ:	// ��߼�
		if(hqp->Max>COMPPREC)
			DrawCellStringText(pDC,row,col,0,2,GetItemColor(hqp->Max,hqp->Close),"%s",FormatFloat2Str(hqp->Max,xsflag));
		else
			DrawCellStringText(pDC,row,col,0,2,VipColor.TXTCOLOR,MEANLESS_STR);
		break;
	case ZDCJ:	// ��ͼ�
		if(hqp->Min>COMPPREC)
			DrawCellStringText(pDC,row,col,0,2,GetItemColor(hqp->Min,hqp->Close),"%s",FormatFloat2Str(hqp->Min,xsflag));
		else
			DrawCellStringText(pDC,row,col,0,2,VipColor.TXTCOLOR,MEANLESS_STR);
		break;
	case CJJE:	//�ܽ��
		if(pInfo->bigmarket!=QH_TYPE && hqp->Amount>COMPPREC)
		{
			DrawCellStringText(pDC,row,col,0,2,GridColor.Grid_Other_Color,MakeJE(hqp->Amount));
			break;
		}
		DrawCellStringText(pDC,row,col,0,2,VipColor.TXTCOLOR,MEANLESS_STR);
		break;
	case ZEF:	//���
		if (hqp->Close>COMPPREC && hqp->Now>COMPPREC)
		{
			if ( hqp->Max > COMPPREC && hqp->Min > COMPPREC )
				ftmp = hqp->Max - hqp->Min;
			else
				ftmp = 0.0;
			float_out=ftmp/hqp->Close*100.0;
			DrawCellStringText(pDC,row,col,0,2,VipColor.TXTCOLOR,"%s",FormatFloat2Str(float_out,XS2));
			break;
		}
		DrawCellStringText(pDC,row,col,0,2,VipColor.TXTCOLOR,MEANLESS_STR);
		break;
	case LIANGB://����
		if( !testzs(pInfo) )
		{
			int ii;
			short fz[8];
			InitStkTime(pInfo->setcode,fz);
			ii = GetTdxMinute(fz);
			if (ii > 0 && pInfo->VolBase > 0l)
			{
				float_out=(float)hqp->Volume/ii/pInfo->VolBase;
				DrawCellStringText(pDC,row,col,0,2,VipColor.TXTCOLOR,"%s",FormatFloat2Str(float_out,XS2));
				break;
			}
		}
		DrawCellStringText(pDC,row,col,0,2,VipColor.TXTCOLOR,MEANLESS_STR);
		break;
	case GEARING:	//�ܸ˱���
		if(pInfo->bigmarket==HKGP_TYPE)
			DrawCellStringText(pDC,row,col,0,2,VipColor.TXTCOLOR,"%.3f",pInfo->J_wfply);
		break;
	case EXECPRICE:	//��ʹ��
		if(pInfo->bigmarket==HKGP_TYPE)
			DrawCellStringText(pDC,row,col,0,2,VipColor.TXTCOLOR,"%.3f",pInfo->J_tzmgjz);
		break;
	case EXPIREDATE://��Ȩ������
		if(pInfo->bigmarket==HKGP_TYPE)
			DrawCellStringText(pDC,row,col,0,2,VipColor.TXTCOLOR,"%d",pInfo->J_start);
		break;
	default:  
		break;
	}
}

void UGlHq::DrawHighLine(CDC *pDC)
{
	if(nTotalRow < 1) return;
	int tempMode = pDC->SetROP2 (R2_XORPEN);
	int RightPos,Width=0,Top;
	if(nTotalCol==nEndCol+1)
		RightPos = GetCellLeft(nEndCol,Width);
	else
		RightPos = m_GPRect.right;
	RightPos = min(m_GPRect.Width(),RightPos+Width);
	Top = (nCurRow-nStartRow+1)*nCellHeight + nTitleHeight-1;
	if(Top < m_GPRect.Height())
	{
		if(g_bLongHighLine)
			g_d.DrawLine(pDC,m_GPRect.left+nXHColWidth+1,m_GPRect.top+Top,m_GPRect.left+RightPos-1,m_GPRect.top+Top,GridColor.GridHighLineColor^VipColor.BACKCOLOR,PS_SOLID,1);//�ұ߼�1,Width=3
		else
		{
			CRect rc;
			rc.SetRect(m_GPRect.left+1,m_GPRect.top+Top-nCellHeight+2,m_GPRect.left+RightPos-1,m_GPRect.top+Top-1);

			CRect   XRc(0,0,rc.Width(),rc.Height());
			CREATE_MEMDC(XRc)
			MemDC.FillSolidRect(&XRc,GridColor.GridBackColor^VipColor.FLOATCURSORCOLOR);
			pDC->BitBlt(m_GPRect.left+1,m_GPRect.top+Top-nCellHeight+2,
				XRc.Width(),XRc.Height(),&MemDC,0,0,SRCINVERT);
			DELETE_MEMDC
		}
	}
	pDC->SetROP2 (tempMode); 
}

float UGlHq::fGetSortValue(int nSort_Type,short xh,short tag)
{
	MemStkInfo *pInfo = (*g_pStockIO)[xh];
	if(!pInfo) 
		return MEANLESS_DATA;

	float					ftmp;
	struct CurrStockData	hqtmp={0};
	EXCURRDATA				hqtmp_ex={0};
	
	g_pStockIO->ReadCurrStockData (pInfo,&hqtmp,NULL,&hqtmp_ex);

	switch( nSort_Type)
	{
	case ZANGSU://��������
		if(OnLine && CanShowZangsu(pInfo))	
			ftmp = pInfo->zangsu;
		else		
			ftmp = MEANLESS_DATA;
		return ftmp;
	case ZQJC: //��Ʊ���� 
		return atof(pInfo->Code);
	case ZRSP:  //��������
		return hqtmp.Close;
	case JRKP:  //���տ���
		return hqtmp.Open;
	case ZGCJ:  //���
		return hqtmp.Max;
	case ZDCJ:  //���
		return hqtmp.Min;
	case ZJCJ:
		return hqtmp.Now;
	case ZGJM:  //����
		return hqtmp.Buyp[0];
	case ZDJM:  //����
		return hqtmp.Sellp[0];
	case CJL:   //�ɽ���
		return hqtmp.Volume;
	case QRSD:  //���ǵ�
		ftmp=hqtmp.Now;
		//�����ǵ��ͷ��ȵļ��㷽��,���ڿ���ʽ���Ͼ�������
		if(ftmp<COMPPREC && hqtmp.Buyp[0]>COMPPREC && fabs(hqtmp.Buyp[0]-hqtmp.Sellp[0])<COMPPREC)
			ftmp=hqtmp.Buyp[0];
		if (ftmp < COMPPREC)	ftmp = 0.00;
		else					ftmp = (ftmp - hqtmp.Close);
		return ftmp;
	case XS:    //����
		if( !testzs(pInfo) )
			return hqtmp.NowVol;
		else
			return MEANLESS_DATA;
	case CJJE:  //�ɽ����
		return hqtmp.Amount;
	case ZAF: //�Ƿ���=���ǵ�/��������*100
		if (hqtmp.Close>COMPPREC && hqtmp.Now>COMPPREC)
		{	
			ftmp=hqtmp.Now;
			//�����ǵ��ͷ��ȵļ��㷽��,���ڿ���ʽ���Ͼ�������
			if(ftmp<COMPPREC && hqtmp.Buyp[0]>COMPPREC && fabs(hqtmp.Buyp[0]-hqtmp.Sellp[0])<COMPPREC)
				ftmp=hqtmp.Buyp[0];
			if ( ftmp < COMPPREC )
				ftmp = 0.0;
			else
				ftmp = ftmp-hqtmp.Close;
			return ftmp/hqtmp.Close*100.0;
		}
		return MEANLESS_DATA;
	case ZEF: //�����=(���-���)/��������*100
		if (hqtmp.Close>0)
		{
			if ( hqtmp.Max > COMPPREC && hqtmp.Min > COMPPREC )
				ftmp = hqtmp.Max - hqtmp.Min;
			else
				ftmp = 0.0;
			return	ftmp/hqtmp.Close*100.0;
		}
		return MEANLESS_DATA;
	case LIANGB:  //����
		if( !testzs(pInfo) )
		{
			short fz[8];
			InitStkTime(pInfo->setcode,fz);
			short ii = GetTdxMinute(fz);
			if (ii > 0 && pInfo->VolBase > 0l)
				return (float)hqtmp.Volume/ii/pInfo->VolBase;
		}
		return MEANLESS_DATA;
	case J_HSL:	//������%
		if( !testzs(pInfo) && pInfo->ActiveCapital > 1 )
			return 1.0*hqtmp.Volume*pInfo->Unit/(pInfo->ActiveCapital*100);
		else 
			return MEANLESS_DATA;
	case J_LTSZ: //��ͨ��ֵ����(��)
		if( !testzs(pInfo) )
		{
			if(hqtmp.Now>COMPPREC)	ftmp = hqtmp.Now;
			else					ftmp = hqtmp.Close;
			return pInfo->ActiveCapital*ftmp;
		}
		return MEANLESS_DATA;
	case GEARING:	//�ܸ˱���
		return pInfo->J_wfply;
	case EXECPRICE:	//��ʹ��
		return pInfo->J_tzmgjz;
	case EXPIREDATE://��Ȩ������,��������
		return pInfo->J_start;	
	default:
		return MEANLESS_DATA;
	}
	return MEANLESS_DATA;
}

void UGlHq::Sort()
{
	if(nTotalRow<1) return;
	
	int			i;
	BOOL		bNeedToTail = FALSE;
	int			nSort_Type = m_aGlHqIdx[nCurSortIndex];
	sort_struc  lpSort[MAXGPNUM];

	if(nSort_Type!=ZQJC) //һ����Ŀ����
	{
		for (i = 0;i < nTotalRow;i++) 
		{
			lpSort[i].fValue = fGetSortValue(nSort_Type,Raw_lpnUsedStkIndex[i],Raw_lpnUsedRowIndex[i]); //ȡ����ֵ
			lpSort[i].nIndex = Raw_lpnUsedStkIndex[i];
			lpSort[i].tag = i;
		}
		qsort((void *) lpSort, nTotalRow, sizeof(struct sort_struc), sort_func );
		if (bAscendSort == FALSE)
		{
			for ( i=0;i < nTotalRow;i++)
			{
				lpnUsedStkIndex[i] = lpSort[i].nIndex;
				lpnUsedRowIndex[i] = lpSort[i].tag;
			}
		}
		else	
		{
			for ( i=0;i < nTotalRow;i++)
			{
				lpnUsedStkIndex[i] = lpSort[nTotalRow-1-i].nIndex;
				lpnUsedRowIndex[i] = lpSort[nTotalRow-1-i].tag;
			}
		}
		bNeedToTail = TRUE;
	}
	if(bNeedToTail)
	{
		//���ǽ�MEANLESS_DATA������󣬲���������
		for(i = 0;i < nTotalRow && fGetSortValue(nSort_Type,lpnUsedStkIndex[i],lpnUsedRowIndex[i]) != MEANLESS_DATA;i++);
		if(i < nTotalRow)
		{
			int startpos,endpos,tempIndex;
			startpos = i;
			for(i = startpos+1;i < nTotalRow && fGetSortValue(nSort_Type,lpnUsedStkIndex[i],lpnUsedRowIndex[i]) == MEANLESS_DATA;i++);
			endpos = i - 1;
			for(i = endpos + 1;i < nTotalRow;i++)
			{
				tempIndex = lpnUsedStkIndex[i];
				lpnUsedStkIndex[i] = lpnUsedStkIndex[startpos+i-endpos-1];
				lpnUsedStkIndex[startpos+i-endpos-1] = tempIndex;
			}
		}
	}
}

//********************************************************//
//                ��Ϣ���ƺ���							  //
//********************************************************//

void UGlHq::OnClick(CPoint point)
{
	if(m_GPRect.PtInRect(point))
	{
		int temp = (point.y-m_GPRect.top-nTitleHeight) / nCellHeight ;
		if(temp >= 0 && temp < nRowNum)
		{
			nCurRow = temp + nStartRow;
			ProcessMsg(WM_PAINT);
			return;
		}
	}
}

void UGlHq::OnLButtonDown(CPoint point)
{
	if(m_GPRect.PtInRect(point))
	{
		if(point.y >= m_GPRect.top+nTitleHeight)
			return;
		int		Width,Left;
		CRect	ColRc;
		//ѡĳ��
		for(int i = 0;i < nEndCol-nStartCol+1;i++)
		{
			Left = GetCellLeft(i+nStartCol,Width);
			ColRc.SetRect(Left,m_GPRect.top,Left+Width,m_GPRect.top+nTitleHeight);
			if(ColRc.PtInRect (point))
			{
				if(nCurSortIndex!=nStartCol+i)
				{
					if(i != 0)
					{
						bAscendSort = TRUE;
						nCurSortIndex = nStartCol+i;
					}
					else
					{
						bAscendSort = FALSE;
						nCurSortIndex = 0;
					}
					if(nCurSortIndex==0)
					{
						ToSomeRow(0);
						SetGPDomain(NULL,0);
					}
					else
						Sort();
					ToSomeRow(0);
					ProcessMsg(WM_PAINT);
				}
				else if(i != 0)
				{	
					bAscendSort =! bAscendSort;
					Sort();
					ToSomeRow(0);
					ProcessMsg(WM_PAINT);
				}
				break;
			}
		}
	}
}
