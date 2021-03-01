// QJReport.cpp: implementation of the QJReport class.
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "tdxw.h"
#include "QJReport.h"
#include "ProgressWnd.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

QJReport::QJReport():Grid(2,TRUE)
{
	memset(fData,0,sizeof(struct QJData_Struct)*MAXGPNUM*11);

	pZsAnalyData = NULL;
	ZsAnalyDataNum = 0;
	//m_lQJEndDateֵ��m_lQJBgnDateֵ��ʼ������g_lQJEndDate��g_lQJBgnDate
	if(g_lQJEndDate==0)
		g_lQJEndDate = g_lOpenRq;
	m_lQJEndDate	= g_lQJEndDate;
	if(g_lQJBgnDate==0)
	{
		CTime theTime(m_lQJEndDate/10000,(m_lQJEndDate%10000)/100,(m_lQJEndDate%10000)%100,0,0,0); 
		CTimeSpan timeSpan(15,0,0,0); 	//15��
		CTime theTime2 = theTime-timeSpan;
		g_lQJBgnDate = theTime2.GetYear()*10000+theTime2.GetMonth()*100+theTime2.GetDay();
	}
	m_lQJBgnDate = g_lQJBgnDate;

	CurQJType = g_nCurQJType;//ȱʡΪ�ǵ�����

	SetPartMode(FALSE);
}

QJReport::~QJReport()
{
	TDEL(pZsAnalyData);

	g_lQJEndDate = m_lQJEndDate;
	g_lQJBgnDate = m_lQJBgnDate;
}

void QJReport::SetDomain(int nBlockFlag,int WhichType)
{
	Grid::SetDomain(nBlockFlag,WhichType);

	//���ô��ڱ���
	m_strTitle="";
	if(nBlockFlag == -1)				
		m_strTitle = GPType[m_WhichGPType];
	else
		m_strTitle = GetBlockCaption(nBlockFlag);
	m_strTitle = _F(m_strTitle);

	(m_pView->GetParent())->SetWindowText (_F("�������-")+m_strTitle);

	InitReport(); //���ñ����ʼ������ʼ�ػ�����
}

void QJReport::InsertSwitch(int nSCType)
{
	pSwitchBar->ClearAllLabel();

	m_bFixedGrid = FALSE;
	int iNo=0;
	CString tmpStr;
	pSwitchBar->m_bhasFLBtn=TRUE;
	pSwitchBar->InsertSwitchLabel(iNo++,_F("����"),WM_COMMAND,IDR_LONELY_MENU,3,TRUE);
	for(int i=0;i<g_CustomGridCat.size();i++)
	{
		if(g_CustomGridCat[i].FlagType==DSOPT_ITEM) 
			continue;
		if(g_CustomGridCat[i].wParam==ID_RIGHT_TYPE20 && (g_pLcIdxMng==NULL || g_pLcIdxMng->GetLcNum()==0))
			continue;		
		tmpStr=g_CustomGridCat[i].lpTitle;
		pSwitchBar->InsertSwitchLabel(iNo++,_F(tmpStr),g_CustomGridCat[i].msg,g_CustomGridCat[i].wParam,g_CustomGridCat[i].lParam,g_CustomGridCat[i].bMenu);
	}
}

void QJReport::InitReport()
{
	nTotalCol = 13-m_nFixedLeadCol;
    nCurRow = nStartRow = nStartCol = 0;
	nCurSortIndex = 2;	//������һ����������
	bAscendSort = TRUE;

	DrawFlag = DRAW_ALL;	
	m_pView->Invalidate(FALSE);
	RefreshScrollBar();	//������֯������
	//�ڵõ�����֮ǰ��ҲҪˢ�£��������ڶԻ����ԭ�򣬻�����
	GetData();  //�õ�����
	//����������������ΪnCurSortIndexΪ2������ҪSort();
	Sort();		//ȱʡΪ�������
	DrawFlag = DRAW_ALL;
	m_pView->Invalidate(FALSE);
	RefreshScrollBar();	//������֯������
}

void QJReport::GetData()
{
	RefreshAllHq();

	int i,j,tempInt,nReadDateNum;
	struct AnalyData*	pAnalyData = NULL;
	long				nBgnData,nEndData;
	long				nCurrDate;	//��ǰ����ֵ
	double              fSHSumAmount = 0.0,fSZSumAmount = 0.0;
	CurrStockData		Hq;
	int					m_nTodayNum;

	//�õ�ָ����һЩ��Ϣ,�Ա���һ�����գ���һ����������
	ZsAnalyDataNum = g_pStockIO->GetAnalyDataNum(g_pStockIO->Get_MemStkInfo(SH_DPCODE,SH),PER_DAY);
	TDEL(pZsAnalyData);
	pZsAnalyData = new AnalyData[ZsAnalyDataNum];
	ZsAnalyDataNum = g_pStockIO->ReadAnalyDataAndCurrHQ(g_pStockIO->Get_MemStkInfo(SH_DPCODE,SH),pZsAnalyData,ZsAnalyDataNum,PER_DAY,&Hq,m_nTodayNum);

	memset(fData,0,MAXGPNUM*11*sizeof(struct QJData_Struct)); 

	nReadDateNum = g_pStockIO->GetAnalyDataNum(g_pStockIO->Get_MemStkInfo(SH_DPCODE,SH),PER_DAY);
	pAnalyData = new AnalyData[nReadDateNum];
	nReadDateNum = g_pStockIO->ReadAnalyDataAndCurrHQ(g_pStockIO->Get_MemStkInfo(SH_DPCODE,SH),pAnalyData,nReadDateNum,PER_DAY,&Hq,m_nTodayNum);
	//����nReadDateNum��ʵ�ʿ��Զ�����(ָ��)����,�������ʼ���ݸ������������ݸ���
	for(nBgnData = 0;nBgnData < nReadDateNum && pAnalyData[nBgnData].Time.Date < m_lQJBgnDate;nBgnData++);
	for(nEndData = 0;nEndData < nReadDateNum && pAnalyData[nEndData].Time.Date < m_lQJEndDate;nEndData++);
	if(nBgnData >= nReadDateNum || nEndData >= nReadDateNum) fSHSumAmount = MEANLESS_DATA;
	else
	{
		for(nCurrDate = nBgnData; nCurrDate <= nEndData; nCurrDate++)
			fSHSumAmount += pAnalyData[nCurrDate].Amount;
	}
	TDEL(pAnalyData);
	nReadDateNum = g_pStockIO->GetAnalyDataNum(g_pStockIO->Get_MemStkInfo(SZ_DPCODE,SZ),PER_DAY);
	pAnalyData = new AnalyData[nReadDateNum];
	nReadDateNum = g_pStockIO->ReadAnalyDataAndCurrHQ(g_pStockIO->Get_MemStkInfo(SZ_DPCODE,SZ),pAnalyData,nReadDateNum,PER_DAY,&Hq,m_nTodayNum);
	for(nBgnData = 0;nBgnData < nReadDateNum && pAnalyData[nBgnData].Time.Date < m_lQJBgnDate;nBgnData++);
	for(nEndData = 0;nEndData < nReadDateNum && pAnalyData[nEndData].Time.Date < m_lQJEndDate;nEndData++);
	if(nBgnData >= nReadDateNum || nEndData >= nReadDateNum) fSZSumAmount = MEANLESS_DATA;
	else
	{
		for(nCurrDate = nBgnData; nCurrDate <= nEndData; nCurrDate++)
			fSZSumAmount+=pAnalyData[nCurrDate].Amount;
	}
	TDEL(pAnalyData);

	//�趨ģ̬�������Ի���
	CProgressWnd wndProgress(m_pView,_F("���ڼ��㣬���Ե�..."),TRUE);
	wndProgress.GoModal();
	wndProgress.SetRange(0,nTotalRow);
	wndProgress.SetStep(3);
	g_bDonotClose = TRUE;
	for(i = 0; i < nTotalRow; i++)
	{
		if(i%3 == 0)
		{
			//����������
			wndProgress.SetText((*g_pStockIO)[lpnUsedStkIndex[i]]->Name);
			wndProgress.StepIt();
		}
		if(i%10 == 0)
		{
			wndProgress.PeekAndPump();
			if(wndProgress.Cancelled())
			{
				TDX_MessageBox(m_pView->m_hWnd,"���㱻ȡ��,�������ݲ�������",MB_OK|MB_ICONINFORMATION);
				g_bDonotClose = FALSE;
				break;
			}
		}
		tempInt = lpnUsedStkIndex[i];	//�õ�i�����
		if(tempInt>=DS_FROM) continue;
		MemStkInfo *tmpInfo=(*g_pStockIO)[tempInt];
		if(!tmpInfo) continue;
				
		nReadDateNum = g_pStockIO->GetAnalyDataNum(tmpInfo,PER_DAY);
		TDEL(pAnalyData);
		pAnalyData = new AnalyData[nReadDateNum];
		int nTmpDateNum = g_pStockIO->ReadAnalyDataFromTime(tmpInfo,pAnalyData,nReadDateNum,m_lQJBgnDate,m_lQJEndDate,PER_DAY);
		nReadDateNum = g_pStockIO->ReadAnalyDataFromTime(tmpInfo,pAnalyData,nTmpDateNum+1+5,0,m_lQJEndDate,PER_DAY);				//���һ�죬��5
		if(nReadDateNum > 0  && g_bHisViewTQ)
			g_pStockIO->TQData(tmpInfo,pAnalyData,0,nReadDateNum-1,nReadDateNum,1,PER_DAY);	//1Ϊǰ��Ȩ
		for(nBgnData = 0;nBgnData < nReadDateNum && pAnalyData[nBgnData].Time.Date < m_lQJBgnDate;nBgnData++);	//20040907iwld 01�����ݴ���
		if(nBgnData>0) --nBgnData;			//�ҵ�ǰһ���
		for(nEndData = 0;nEndData < nReadDateNum && pAnalyData[nEndData].Time.Date < m_lQJEndDate;nEndData++);

		if(nBgnData >= nReadDateNum) nBgnData = nReadDateNum - 1;
		if(nEndData >= nReadDateNum) nEndData = nReadDateNum - 1;
		if(nReadDateNum <= 0 || nTmpDateNum<1)
		{
			fData[tempInt][0].f1 = fData[tempInt][0].f2 = MEANLESS_DATA;
			fData[tempInt][1].f1 = fData[tempInt][1].f2 = MEANLESS_DATA;
			fData[tempInt][2].f1 = fData[tempInt][2].f2 = MEANLESS_DATA;
			fData[tempInt][3].f1 = fData[tempInt][3].f2 = MEANLESS_DATA;
			fData[tempInt][4].f1 = fData[tempInt][4].f2 = MEANLESS_DATA;
			fData[tempInt][5].f1 = fData[tempInt][5].f2 = MEANLESS_DATA;
			fData[tempInt][6].f1 = fData[tempInt][6].f2 = MEANLESS_DATA;
			fData[tempInt][7].f1 = fData[tempInt][7].f2 = MEANLESS_DATA;
			fData[tempInt][8].f1 = fData[tempInt][8].f2 = MEANLESS_DATA;
			fData[tempInt][9].f1 = fData[tempInt][9].f2 = MEANLESS_DATA;
			fData[tempInt][10].f1 = fData[tempInt][10].f2 = MEANLESS_DATA;
			fData[tempInt][11].f1 = fData[tempInt][11].f2 = MEANLESS_DATA;
			fData[tempInt][12].f1 = fData[tempInt][12].f2 = MEANLESS_DATA;
		}
		else
		{
			//ǰ����
			if(nReadDateNum>nTmpDateNum)
				fData[tempInt][0].f1 = pAnalyData[nBgnData].Close;
			else if(pAnalyData[nBgnData].Close<COMPPREC)
				fData[tempInt][0].f1 = pAnalyData[min(nEndData,nBgnData+1)].Open;
			else 
				fData[tempInt][0].f1 = pAnalyData[nBgnData].Open;
			//�������
			fData[tempInt][1].f1 = QJMax(pAnalyData,nBgnData+1,nEndData);
			//�������
			fData[tempInt][2].f1 = QJMin(pAnalyData,nBgnData+1,nEndData);
			//����
			if(pAnalyData[nEndData].Close<COMPPREC) //�������ͣ�ƣ�ǰһ��
				fData[tempInt][3].f1 = pAnalyData[max(0,nEndData-1)].Close;
			else
				fData[tempInt][3].f1 = pAnalyData[nEndData].Close;
			//�ǵ�/����
			if(fData[tempInt][3].f1==MEANLESS_DATA || fData[tempInt][0].f1==MEANLESS_DATA)
				fData[tempInt][4].f1 = MEANLESS_DATA;
			else
				fData[tempInt][4].f1 = fData[tempInt][3].f1 - fData[tempInt][0].f1;
			if(fData[tempInt][0].f1 < COMPPREC || fabs(fData[tempInt][3].f1 - fData[tempInt][0].f1)<COMPPREC || fData[tempInt][0].f1==MEANLESS_DATA || fData[tempInt][3].f1==MEANLESS_DATA )
				fData[tempInt][4].f2 = MEANLESS_DATA;
			else
				fData[tempInt][4].f2 = 100.0*(fData[tempInt][3].f1 - fData[tempInt][0].f1) / fData[tempInt][0].f1;
			//��/����
			if(fData[tempInt][1].f1==MEANLESS_DATA || fData[tempInt][2].f1==MEANLESS_DATA ||fData[tempInt][0].f1==MEANLESS_DATA)	
				fData[tempInt][5].f1 = MEANLESS_DATA;
			else
				fData[tempInt][5].f1 = fData[tempInt][1].f1 - fData[tempInt][2].f1;
			if(fData[tempInt][0].f1 < COMPPREC || fabs(fData[tempInt][1].f1 - fData[tempInt][2].f1) < COMPPREC || fData[tempInt][1].f1 == MEANLESS_DATA || fData[tempInt][2].f1 == MEANLESS_DATA || fData[tempInt][0].f1 == MEANLESS_DATA)
				fData[tempInt][5].f2 = MEANLESS_DATA;
			else
				fData[tempInt][5].f2 = 100.0*(fData[tempInt][1].f1 - fData[tempInt][2].f1)/fData[tempInt][0].f1;
			fData[tempInt][5].f2Type = FLOAT2_VALUE;
			//�ɽ�����
			fData[tempInt][6].f1 = 0.00;
			for(j = nBgnData+1; j <= nEndData; j++)
				if(pAnalyData[j].Volume != MEANLESS_DATA)
					fData[tempInt][6].f1 += pAnalyData[j].Volume;
			fData[tempInt][6].f1Type = VOLUME_VALUE;
			//�ɽ����
			fData[tempInt][7].f1 = 0.00;
			for(j = nBgnData+1; j <= nEndData; j++)
				if(pAnalyData[j].Amount != MEANLESS_DATA)
					fData[tempInt][7].f1 += pAnalyData[j].Amount;
			fData[tempInt][7].f1Type = AMOUNT_VALUE;
			//�г�����
			if(tmpInfo->setcode == SZ)
			{
				if( fSZSumAmount < COMPPREC || fabs(fSHSumAmount)<COMPPREC || fSZSumAmount == MEANLESS_DATA || fData[tempInt][7].f1 == MEANLESS_DATA) 
					fData[tempInt][8].f1 = MEANLESS_DATA;
				else	fData[tempInt][8].f1 = 100.0*(fData[tempInt][7].f1 / fSZSumAmount);
			}
			else
			{
				if( fSHSumAmount < COMPPREC || fabs(fSHSumAmount)<COMPPREC || fSHSumAmount == MEANLESS_DATA || fData[tempInt][7].f1 == MEANLESS_DATA )
					fData[tempInt][8].f1 = MEANLESS_DATA;
				else	fData[tempInt][8].f1 = 100.0*(fData[tempInt][7].f1 / fSHSumAmount);
			}
			//������%
			if(tmpInfo->ActiveCapital*10000 > 1)
				fData[tempInt][9].f1 = CalcHSL(tmpInfo,fData[tempInt][6].f1,tmpInfo->ActiveCapital,PER_DAY);
			else
				fData[tempInt][9].f1 = 0.0;
			//���������
			float tmpvolume1 = pAnalyData[nBgnData+1].Volume;
			float tmpvolume2 = pAnalyData[nEndData].Volume;
			//���������
			for(int k=1;k<5;k++)
			{
				if(nBgnData+1-k>=0) tmpvolume1 += pAnalyData[nBgnData+1-k].Volume;
				if(nEndData-k>=0) tmpvolume2 += pAnalyData[nEndData-k].Volume;
			}
			//�������
			if(tmpvolume1 < COMPPREC || tmpvolume2 < COMPPREC || fabs(tmpvolume2-tmpvolume1)<COMPPREC || tmpvolume2==MEANLESS_DATA || tmpvolume1==MEANLESS_DATA)
				fData[tempInt][10].f1 = MEANLESS_DATA;
			else
				fData[tempInt][10].f1 = 100.0*(tmpvolume2-tmpvolume1) / tmpvolume1;
		}
	}
	g_bDonotClose = FALSE;
	TDEL(pAnalyData);
}

float QJReport::fGetSortValue(short xh)
{
	if(xh>=DS_FROM || nCurSortIndex == 1 || nCurSortIndex == 0 ) return 0.0; //������nCurSortIndex = 1�����
	int tmptype = GetColIndex(nCurSortIndex)-2;
	if(tmptype != 4 && tmptype != 5) 
		return fData[xh][tmptype].f1;
	else		//�����Ӧ���ٷֱ�����
		return fData[xh][tmptype].f2;
}

//������
void QJReport::Sort()
{
	int i;
	BOOL bOtherSort = FALSE;
	if(nCurSortIndex == 1)		//��֤ȯ��������
	{
		for (i = 0;i < nTotalRow;i++) 
		{
			strcpy(lpSort2[i].StrValue, (*g_pStockIO)[lpnUsedStkIndex[i]]->Name); //ȡ����ֵ
			lpSort2[i].nIndex = lpnUsedStkIndex[i];
			lpSort2[i].tag = 0;
		}
		qsort((void *) lpSort2, nTotalRow, sizeof(struct sort_struc2), sort_func2 );
		if (bAscendSort == FALSE)
			for ( i=0;i < nTotalRow;i++) lpnUsedStkIndex[i] = lpSort2[i].nIndex;
		else	
			for ( i=0;i < nTotalRow;i++) lpnUsedStkIndex[i] = lpSort2[nTotalRow-1-i].nIndex;
	}
	else
	{
		if(nCurSortIndex == 0)	//��֤ȯ��������(ʵ���������)
		{
			for (i = 0;i < nTotalRow;i++)
			{
				lpSort[i].fValue = lpnUsedStkIndex[i];
				lpSort[i].nIndex = lpnUsedStkIndex[i];
				lpSort[i].tag = 0;
			}
		}
		else					//��������
		{
			for (i = 0;i < nTotalRow;i++)
			{
				 lpSort[i].fValue = fGetSortValue(lpnUsedStkIndex[i]);
				 lpSort[i].nIndex = lpnUsedStkIndex[i];
				 lpSort[i].tag = 0;
			}
			bOtherSort = TRUE;
		}
		qsort((void *) lpSort, nTotalRow, sizeof(struct sort_struc), sort_func );
		if (bAscendSort == FALSE)
			for (i = 0;i < nTotalRow;i++) lpnUsedStkIndex[i] = lpSort[i].nIndex;
		else	
			for (i = 0;i < nTotalRow;i++) lpnUsedStkIndex[i] = lpSort[nTotalRow-1-i].nIndex;

		//������������͵��������ǽ�MEANLESS_DATA������󣬲���������
		if(bOtherSort)
		{
			for(i = 0;i < nTotalRow && fGetSortValue(lpnUsedStkIndex[i]) != MEANLESS_DATA;i++);
			if(i < nTotalRow)
			{
				int startpos,endpos,tempIndex;
				startpos = i;
				for(i = startpos+1;i < nTotalRow && fGetSortValue(lpnUsedStkIndex[i]) == MEANLESS_DATA;i++);
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
	return;
}

void QJReport::OnSize(UINT nType, int cx, int cy)
{
	SetRect(CRect(0,0,cx,cy));
}


//���㷽�����λ��,WidthΪ���ط�����,nCol��ʾ��˳���о������
int QJReport::GetCellLeft(int nCol,int & nWidth) 
{   
	int i=0,xtmp=nCellHeadWidth;
	if(m_nFixedLeadCol<1) i=nStartCol;	//û�й̶�����ͷ��
	while(i < nCol)
	{   
		xtmp+=(int)(lpnQJItemWidth[GetColIndex(i)]*FontSize.cx/2);
	    if (i == m_nFixedLeadCol-1) 
			i = nStartCol+m_nFixedLeadCol;
		else		
			i++;
	}
	nWidth = (int)(lpnQJItemWidth[GetColIndex(nCol)]*FontSize.cx/2);
	return xtmp;
}


//������
void QJReport::DrawTitle(CDC *pDC)
{
	int i,j,xtmp,width;
	//д������
	CString tempStr1;
	int pos=8;
	tempStr1.Format("�����������--%s",QJTypeStr[CurQJType]);
	CSize size = g_d.DisplayText(pDC,pos,2,GridColor.Grid_Up_Color,0,_F(tempStr1));
	pos += size.cx+5;
	if(m_pWriteFile)	m_WriteStr+=_F(tempStr1);
	size = g_d.DisplayText(pDC,pos,2,VipColor.TXTCOLOR,0,_F(" �г�:"));
	pos += size.cx+5;
	if(m_pWriteFile)	m_WriteStr+=_F(" �г�:");
	size = g_d.DisplayText(pDC,pos,2,GridColor.Grid_Volume_Color,0,"%s",m_strTitle);
	pos += size.cx+5;
	if(m_pWriteFile)	m_WriteStr+=m_strTitle;
	size = g_d.DisplayText(pDC,pos,2,VipColor.TXTCOLOR,0,_F(" ����:"));
	pos += size.cx+5;
	if(m_pWriteFile)	m_WriteStr+=_F(" ����:");

	CString tmpDateStr1=GetTdxDateWeekStr(m_lQJBgnDate);
	CString tmpDateStr2=GetTdxDateWeekStr(m_lQJEndDate);
	tempStr1.Format("%s - %s",tmpDateStr1,tmpDateStr2);
	size = g_d.DisplayText(pDC,pos,2,GridColor.Grid_Volume_Color,0,_F(tempStr1));
	pos += size.cx+10;
	if(m_pWriteFile)	m_WriteStr+=_F(tempStr1);
	if(m_pWriteFile)	
	{
		m_pWriteFile->WriteString(m_WriteStr+(CString)"\n");
		m_WriteStr = "";
	}
	size = g_d.DisplayText(pDC,pos,2,VipColor.TXTCOLOR,0,_F("���Ҽ���������"));
	pos += size.cx+5;		

	//��ʼ��
	int tempTitlePos = nSigTitleHeight+6;
	xtmp = nCellHeadWidth;
	for(i = j = 0;j < nTotalCol+m_nFixedLeadCol-1 && xtmp < m_DRect.right;i++)
	{
  		if (i>=m_nFixedLeadCol) 
			j = i+nStartCol;
		else	  
			j = i;
		width = (int)(lpnQJItemWidth[GetColIndex(j)]*FontSize.cx/2);
		if((nCurSortIndex != ((i>=m_nFixedLeadCol)?(nStartCol+i):i)) || (nCurSortIndex == 0) )
		{
			if(i<m_nFixedLeadCol)
				g_d.DisplayText(pDC,xtmp+10,tempTitlePos,GridColor.GridTitleTextColor,0,_F(lpszQJItemName[GetColIndex(j)]));
			else
				g_d.DisplayText(pDC,xtmp,tempTitlePos,GridColor.GridTitleTextColor,width-3,_F(lpszQJItemName[GetColIndex(j)]));
		}
		else
		{	
			g_d.DisplayText(pDC,xtmp,tempTitlePos,GridColor.GridTitleTextColor,width-FontSize.cx/2-3,_F(lpszQJItemName[GetColIndex(j)]));
			g_d.DisplayText(pDC,xtmp,tempTitlePos,GridColor.GridTitleArrowColor,width,(bAscendSort==TRUE)?_F("��"):_F("��"));
		}
		xtmp+=width;
		if(m_pWriteFile) 
			m_WriteStr+=_F(lpszQJItemName[GetColIndex(j)])+(CString)"\t";
	}
	if(m_pWriteFile)	
	{
		m_pWriteFile->WriteString(m_WriteStr+"\n");
		m_WriteStr = "";
	}
	if(xtmp > m_DRect.right)
		nColNum = j-nStartCol-m_nFixedLeadCol;	//˳���������
	else 
		nColNum = j-nStartCol-m_nFixedLeadCol+1;
	if(nColNum < 0) nColNum = 0;
}

void QJReport::GetSomeSize(CDC *pDC)
{
	FontSize = pDC->GetTextExtent("��");
	FontSize.cx+=2;
	nSigTitleHeight = FontSize.cy+6;
	nSigCellHeight = FontSize.cy+4;
	nTitleHeight = 2*nSigTitleHeight+4; //�õ��������߶�
	nCellHeight =  1.5*nSigCellHeight;	//�õ�����ĸ߶�
	nCellHeadWidth = 2.2*FontSize.cx;
	if(g_GH.NoXHCol) 
		nCellHeadWidth=0;
}

void QJReport::DrawReport(CDC *pDC)
{
	GetDrawRect(m_DRect);
	if(m_DRect.Height() < 40) return;
	int i,j;

	CREATE_GRIDMEMDC
	GetSomeSize(&memDC);

	//����Ŀ��,�����г���ʼһЩ����:����nColNum
	g_d.Bar(&memDC,0,0,m_DRect.Width(),nTitleHeight-nSigTitleHeight-4,g_GUI.MixedColor(VipColor.AXISCOLOR,25,VipColor.BACKCOLOR));
	DrawTitle(&memDC);
	//����RowNum��ColNum
	AdjustRowColNum();
	//��ʼ����
	int tempWidth,tempLeft;
	tempLeft = GetCellLeft(m_nFixedLeadCol+nEndCol,tempWidth);

	g_d.DrawLine(&memDC,0,nTitleHeight-nSigTitleHeight-5,m_DRect.Width(),
				nTitleHeight-nSigTitleHeight-5,VipColor.AXISCOLOR);
	for(i = 0;i <= nRowNum;i++)	//������
	{
		g_d.DrawLine(&memDC,0,nTitleHeight+i*nCellHeight-1,
				(tempColFlag==TRUE)?(tempLeft+tempWidth):(m_DRect.Width()),
				nTitleHeight+i*nCellHeight-1);
	}
	int xtmp = nCellHeadWidth ;
	for(j = 0;j < m_nFixedLeadCol+nColNum;j++) //������
	{			
		g_d.DrawLine(&memDC,xtmp-1,nSigTitleHeight,xtmp-1,
			(tempRowFlag==TRUE)?(nRowNum*nCellHeight+nTitleHeight):m_DRect.Height());
		if(j < m_nFixedLeadCol)		
			xtmp+=(int)(lpnQJItemWidth[GetColIndex(j)]*FontSize.cx/2);
		else			
			xtmp+=(int)(lpnQJItemWidth[GetColIndex(j+nStartCol)]*FontSize.cx/2);
	}
	g_d.DrawLine(&memDC,xtmp-1,nSigTitleHeight,xtmp-1,(tempRowFlag==TRUE)?(nRowNum*nCellHeight+nTitleHeight):m_DRect.Height());//�����ϸ�ѭ��

	//�ڷ�����д����
	MemStkInfo *tempStkInfo;
	CurrStockData CurrStockDatap;
	if(m_bWriteAllHQ) //����ȫ��ʱ
	{
		for(j = 0;j < nTotalRow;j++)
		{
			tempStkInfo = (*g_pStockIO)[lpnUsedStkIndex[j]];
			g_pStockIO->ReadCurrStockData (tempStkInfo,&CurrStockDatap);
			for(i=0;i<tempColNum+m_nFixedLeadCol;i++)
			{
				if(i<m_nFixedLeadCol)
					DrawItem(&memDC,j,i,2,tempStkInfo,&CurrStockDatap);
				else
					DrawItem(&memDC,j,i+nStartCol,GetXSFlag(tempStkInfo),tempStkInfo,&CurrStockDatap);
			}
			if(m_pWriteFile)
			{
				m_pWriteFile->WriteString(m_WriteStr+"\n");
				m_WriteStr = "";
			}
		}
	}
	else
	{
		for(j = 0;j < tempRowNum;j++)
		{
			tempStkInfo = (*g_pStockIO)[lpnUsedStkIndex[nStartRow+j]];
			g_pStockIO->ReadCurrStockData (tempStkInfo,&CurrStockDatap);
			for(i=0;i<tempColNum+m_nFixedLeadCol;i++)
			{
				if(i<m_nFixedLeadCol)
					DrawItem(&memDC,j,i,2,tempStkInfo,&CurrStockDatap);
				else
					DrawItem(&memDC,j,i+nStartCol,GetXSFlag(tempStkInfo),tempStkInfo,&CurrStockDatap);
			}
			if(m_pWriteFile)
			{
				m_pWriteFile->WriteString(m_WriteStr+"\n");
				m_WriteStr = "";
			}
		}
	}
	//д����������
	for(i=0;nCellHeadWidth>3 && i<tempRowNum;i++)
		g_d.DisplayText(&memDC,0,nTitleHeight+i*nCellHeight+(nCellHeight-FontSize.cy)/2,VipColor.TXTCOLOR,nCellHeadWidth-3,"%4d",nStartRow+i+1);
	//���ڴ�DC��������ĻDC
	pDC->BitBlt (m_DRect.left,m_DRect.top,m_DRect.Width(),m_DRect.Height(),&memDC,0,0,SRCCOPY);
	
	DrawHighLine(pDC);//��ָʾ��

	DELETE_GRIDMEMDC

	DrawFlag = DRAW_NOREADDATA;
}

void QJReport::DrawItem(CDC *pDC,int row,int col,int xsflag,MemStkInfo * scodetmp,CurrStockData * hqp)
{
	int tempInt = lpnUsedStkIndex[nStartRow+row];
	int tempCol = GetColIndex(col);	//����������������Ŀ
	if(tempInt>=DS_FROM && tempCol!=0 && tempCol!=1) 
		return;
	switch(tempCol)
	{
	case 0://��Ʊ����
		DrawCellText(pDC,row,col,0,scodetmp->Code,GridColor.Grid_CodeName_Color,TRUE);
		if(m_pWriteFile)	
		{
			if(m_bWriteExcel)
				m_WriteStr+=(CString)"=\""+scodetmp->Code+(CString)"\"";
			else
				m_WriteStr+=scodetmp->Code;
		}
		break;
	case 1://��Ʊ����
		DrawCellText(pDC,row,col,0,scodetmp->Name,GetBlockGPColor(scodetmp,GridColor.Grid_CodeName_Color,m_WhichGPType,m_nBlockFlag),TRUE,TRUE,scodetmp);
		if(m_pWriteFile)	m_WriteStr+=scodetmp->Name;
		break;
	case 2:
	case 3:
	case 4:
	case 5:
	case 8:
	case 9:
	case 10:
	case 11:
	case 12:
		if(fData[tempInt][tempCol-2].f1 != MEANLESS_DATA)
		{
			if(fData[tempInt][tempCol-2].f1Type == FLOAT_VALUE)
			{
				DrawCellText(pDC,row,col,0,fData[tempInt][tempCol-2].f1,xsflag,GridColor.Grid_Level_Color);
				if(m_pWriteFile)	m_WriteStr+=FloatToStr(fData[tempInt][tempCol-2].f1);
			}
			else if(fData[tempInt][tempCol-2].f1Type == VOLUME_VALUE)
			{
				DrawCellText( pDC, row, col,0, GridColor.Grid_Volume_Color,MakeVol(fData[tempInt][tempCol-2].f1));
				if(m_pWriteFile)	m_WriteStr+=MakeVol(fData[tempInt][tempCol-2].f1);
			}
			else
			{
				DrawCellText( pDC, row, col,0, GridColor.Grid_Other_Color,MakeJE(fData[tempInt][tempCol-2].f1));
				if(m_pWriteFile)	m_WriteStr+=MakeJE(fData[tempInt][tempCol-2].f1);
			}
		}
		else
			DrawCellText(pDC,row,col,0,MEANLESS_STR,GridColor.Grid_Level_Color);
		break;
	default:
		if(fData[tempInt][tempCol-2].f1 != MEANLESS_DATA)
		{
			//����ʱ,��һ�в�����
			if(fData[tempInt][tempCol-2].f1Type == FLOAT_VALUE)
				DrawCellText(pDC,row,col,1,fData[tempInt][tempCol-2].f1,xsflag,GridColor.Grid_Level_Color);
			else if(fData[tempInt][tempCol-2].f1Type == VOLUME_VALUE)
				DrawCellText( pDC, row, col,1, GridColor.Grid_Volume_Color,MakeVol(fData[tempInt][tempCol-2].f1));
			else
				DrawCellText( pDC, row, col,1, VipColor.TXTCOLOR,MakeJE(fData[tempInt][tempCol-2].f1));
		}
		if(fData[tempInt][tempCol-2].f2 != MEANLESS_DATA)
		{
			if(fData[tempInt][tempCol-2].f2Type == FLOAT_VALUE)
			{
				DrawCellText(pDC,row,col,2,fData[tempInt][tempCol-2].f2,0.0,2,1);
				if(m_pWriteFile)	m_WriteStr+=FloatToStr(fData[tempInt][tempCol-2].f2);
			}
			else if(fData[tempInt][tempCol-2].f2Type == FLOAT2_VALUE)
			{
				DrawCellText(pDC,row,col,2,fData[tempInt][tempCol-2].f2,2,VipColor.TXTCOLOR,1);
				if(m_pWriteFile)	m_WriteStr+=FloatToStr(fData[tempInt][tempCol-2].f2);
			}
			else if(fData[tempInt][tempCol-2].f2Type == VOLUME_VALUE)
			{
				DrawCellText( pDC, row, col,2, GridColor.Grid_Volume_Color,MakeVol(fData[tempInt][tempCol-2].f2));
				if(m_pWriteFile)	m_WriteStr+=MakeVol(fData[tempInt][tempCol-2].f2);
			}
			else
			{
				DrawCellText( pDC, row, col,2, VipColor.TXTCOLOR,MakeJE(fData[tempInt][tempCol-2].f2));
				if(m_pWriteFile)	m_WriteStr+=MakeJE(fData[tempInt][tempCol-2].f2);
			}
		}
		break;
	}
	if(m_pWriteFile)
		m_WriteStr+="\t";
}

//��������һ��ʱ
void QJReport::OnClick(CDC *pDC,CPoint point,BOOL IsRight)
{
	int temp;
	if(point.y > m_Rect.top + nTitleHeight)
	{
		temp = (point.y-m_Rect.top-nTitleHeight) / nCellHeight ;
		temp = max(0,temp);
		temp = min(temp,nRowNum-1);
		DrawHighLine(pDC);
		nCurRow = temp+nStartRow;
		DrawHighLine(pDC);
	}
	if(IsRight) //������Ҽ�
	{
		CNewMenu menu;
		menu.LoadMenu (IDR_RIGHTMENU_HIS);
//		menu.LoadToolBar(IDR_MAINFRAME); //ʹ�ù�������ͼ��
		CNewMenu *pPopup=(CNewMenu *)menu.GetSubMenu(2); //��ʷ�����Ҽ��˵�
		pPopup->SetDefaultItem(ID_RIGHT_OPEN);
		(pDC->GetWindow())->ClientToScreen (&point);
		pPopup->TrackPopupMenu (TPM_LEFTALIGN|TPM_RIGHTBUTTON,point.x,point.y,AfxGetMainWnd());
	}
}

void QJReport::OnLButtonDown(UINT nFlags, CPoint point) 
{
	int Width,Left;
	CClientDC dc(m_pView);

	OnClick (&dc,point,FALSE);

	if(point.y > m_Rect.top+nTitleHeight) return;
	//ѡĳ��
	for(int i = 0;i < m_nFixedLeadCol + (nEndCol - nStartCol + 1);i++)   // �������һ��
	{
		CRgn m_rgn;
		Left = GetCellLeft((i<m_nFixedLeadCol)?i:(i+nStartCol),Width);
		m_rgn.CreateRectRgn (m_Rect.left+Left,m_Rect.top+nTitleHeight-nSigCellHeight,m_Rect.left+Left+Width,m_Rect.top+nTitleHeight);
		if(m_rgn.PtInRegion (point))
		{
			if( (nCurSortIndex != i && i < m_nFixedLeadCol) || (nCurSortIndex != nStartCol+i && i > m_nFixedLeadCol-1) )
			{
				if(i != 0)
				{
					bAscendSort = TRUE;
					if(i > m_nFixedLeadCol-1)	
						nCurSortIndex = nStartCol + i;
					else	
						nCurSortIndex = i;
				}
				else
				{
					bAscendSort = FALSE;
					nCurSortIndex = 0;
				}
				Sort();
				ToSomeRow(0);
				DrawFlag = DRAW_ALL;
				DrawReport(&dc);
			}
			else if(i != 0)
			{	
				bAscendSort = !bAscendSort;
				Sort();
				ToSomeRow(0);
				DrawFlag = DRAW_ALL;
				DrawReport(&dc);
			}
		}
	}
}

//���������ֵ�ĺ���
float QJReport::QJMax(struct AnalyData* pAnalyData,short nBgnData,short nEndData)
{
	float fTemp = 0.0;
	for(int i = nBgnData; i <= nEndData; i++)
		if(fTemp < pAnalyData[i].High && pAnalyData[i].High != MEANLESS_DATA)
			fTemp = pAnalyData[i].High;
	if(fabs(fTemp) < COMPPREC) fTemp = MEANLESS_DATA;
	return fTemp;
}
//���������ֵ���������ĺ���
long QJReport::QJMaxDate(struct AnalyData* pAnalyData,short nBgnNum,short nEndNum)
{
	float	fTemp = 0.0;
	long	lMaxDate = pAnalyData[nBgnNum].Time.Date;
	for(int i = nBgnNum; i <= nEndNum; i++)
		if(fTemp<pAnalyData[i].High && pAnalyData[i].High != MEANLESS_DATA)
		{
			fTemp    = pAnalyData[i].High;
			lMaxDate = pAnalyData[i].Time.Date;
		}
	if(fabs(fTemp) < COMPPREC) lMaxDate = MEANLESS_DATA;
	return lMaxDate;
}
//���������ֵ�ĺ���
float QJReport::QJMin(struct AnalyData* pAnalyData,short nBgnNum,short nEndNum)
{
	float	fTemp = MEANLESS_DATA;	//fTempΪһ��λ�����ܳ����Ĵ�ֵ
	for(int i = nBgnNum; i <= nEndNum; i++)
		if(fTemp > pAnalyData[i].Low && pAnalyData[i].Low != MEANLESS_DATA)
			fTemp = pAnalyData[i].Low;
	return fTemp;
}
//���������ֵ���������ĺ���
long QJReport::QJMinDate(struct AnalyData* pAnalyData,short nBgnNum,short nEndNum)
{
	float	fTemp = MEANLESS_DATA;//fTempΪһ��ֵ
	long	lMinDate = pAnalyData[nBgnNum].Time.Date;
	for(int i = nBgnNum; i <= nEndNum; i++)
		if(fTemp>pAnalyData[i].Low && pAnalyData[i].Low != MEANLESS_DATA)
		{
			fTemp    = pAnalyData[i].Low;
			lMinDate = pAnalyData[i].Time.Date;
		}
	return lMinDate;
}

int QJReport::GetColIndex(int col)
{
	int *naTmp;
	if(CurQJType == 0)
		naTmp = naPriceExtent;	//�ǵ�����
	else if(CurQJType == 1)
		naTmp = naPopPoint;		//����������
	else if(CurQJType == 2)
		naTmp = naVolumeExtent;	//�������
	else 
		naTmp = naDiffExtent;	//�𵴷���
	return *(naTmp+col);
}

//�趨��������
void QJReport::SetQJType(int type)
{
	if(type < 0 || type > 3) return;
	CurQJType = type;

	nCurSortIndex = 2;		//������һ������,����
	bAscendSort = TRUE;
	Sort();
	ToSomeRow(0);
	ToSomeCol(0);
	DrawFlag = DRAW_ALL;
	m_pView->Invalidate(FALSE);
}

//������������
void  QJReport::SetDateSection(CTime date1,CTime date2)	
{
	long lDate = TimeToLong(date1);	//���µ�����ʼ����
	if(pZsAnalyData && ZsAnalyDataNum>0)
	{
		if(pZsAnalyData[0].Time.Date > lDate)
			m_lQJBgnDate = pZsAnalyData[0].Time.Date;
		else
		{
			int pos = 0;
			while(pZsAnalyData[pos].Time.Date < lDate && pos < ZsAnalyDataNum)
				pos++;
			if(pos >= ZsAnalyDataNum) pos = ZsAnalyDataNum - 1;
			m_lQJBgnDate   = pZsAnalyData[pos].Time.Date;
		}
		m_lQJEndDate = TimeToLong(date2);
		if(m_lQJEndDate < m_lQJBgnDate) m_lQJEndDate = m_lQJBgnDate;
		InitReport();
	}
}

//ʹ�������˯��
void QJReport::Sleep()
{
	pSwitchBar->SetWindowPos(0,0,0,0,0,SWP_SHOWWINDOW);
}

//��������Ѿ�����ı���
void QJReport::WakeUp()
{
	(m_pView->GetParent())->SetWindowText (_F("�������-"+m_strTitle));
	DrawFlag = DRAW_ALL;
	m_pView->Invalidate(FALSE);
	RefreshScrollBar();
}

void QJReport::OnCtrlLeft()
{
	if(nStartCol == 0) return;

	int xtmp,width;
	xtmp = nCellHeadWidth+(int)(lpnQJItemWidth[0]*FontSize.cx/2)+(int)(lpnQJItemWidth[1]*FontSize.cx/2);
	for(int i = nStartCol;i >= 0&&xtmp < m_DRect.Width();i--)
	{
		width = (int)(lpnQJItemWidth[GetColIndex(i+2)]*FontSize.cx/2);
		xtmp += width;
	}
	if(xtmp > m_DRect.Width()) nStartCol = i+1;
	else if(xtmp == m_DRect.Width()) nStartCol = i;
	else nStartCol = 0;

	DrawFlag = DRAW_NOREADDATA;
	m_pView->Invalidate(FALSE);
}

int  QJReport::ProcessMsg(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{
	case WM_PAINT:
		m_pView->Invalidate(FALSE);
		return 1;	
	}
	return 0;
}
