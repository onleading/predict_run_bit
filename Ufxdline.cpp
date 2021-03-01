/*
* Copyright (c) 2002,ͨ���ſ�����
* All rights reserved.
* 
* �ļ����ƣ�Ufxdline.cpp
* �ļ���ʶ��
* ժ    Ҫ�����߹���.
*
*/
#include "stdafx.h"
#include "Ufxt.h"
#include "SelfLineShowDlg.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////��ʼ����
 
/*	���߹��ߣ�Ӧ���������㶨��! Ȼ�󷴹�������������Զ���!

	���߹���ƽ��ʱ��û�б��н���Ϣת��תȥ��Upʱתһ�μ��ɡ�
	�ܹ����ߡ���ת��ƽ��ʱ���ô��롣
	����ʱ��Begin  -->  MoveDrawPanRatio
				MoveDrawPanRatioʱӦ�������������ڱ仯��
				��ƣ�����ʱ��Ȼ�ǹ̶�һ�㣬��ѡ������һ���㡣
				��ʱ�ڼ���Region��Ҫ���档
	����ƽ�Ƶ�ʱ�򣬾��������㶼�ڱ仯������
				��ƽ�ƵĹ�����û�б���ת�����ݣ��˷�ʱ�䡣
	����תʱ�����������е�����һ�����ڱ仯��
	��ֱ����ʾʱ���ڼ�����ȷ���ġ����㶼��һ���Թ̶��ġ�

  Ҫ�����븴�ã�
	1��MoveDrawPanRatio ��Եڼ���DrawLine���  nNo��
	2�����������������������Ϣ m_Point[0] , m_Point[1] ��
	3����ǰ�� point ��
	4������ת�����ⲿ��ɡ�
	5��ѡ�����ⲿ��ɡ�
	6���������̷ŵ��ⲿ����ʵ�֡�
	7���Ƿ���ʾ������ʾ
	
	void MoveDrawPanRatio( CDC * pDC , int nRegion , int nNo , CPoint point , BOOL bShow);

	����ת����
	1����ǰ�㡣
	2��ת���ڼ�����
	  void TransSelfLine(CPoint point , int nWhitch);

	ѡ��:
	  int SelectSelfLine(int nRegion,CPoint point);

*/
////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL UFxt::BeginDrawSelfLine(CDC * pDC,CPoint point)
{
	long i = 0;
	CPoint	XDPt = point;
	XDPt.x -= left;
	XDPt.y -= top;
	////////////////////////
	if(m_bOverStk)
	{
		TDX_MessageBox(NULL,"�ڵ���Ʒ��ʱ���ܻ���!",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;
	}
	if(g_DrawLineTool.GetLineNum() >= SELFLINEMAX)
	{
		TDX_MessageBox(m_pView->m_hWnd,"���߿ռ�����,�����ٻ��µ��Ի���!\n��ɾ�����������ٻ�����.",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;
	}
	for(i=0;i<m_nRegion;i++)
	{
		m_PtInArea = m_aRegion[i].DrawRc;
		m_AbsPtInArea = m_PtInArea;
		m_AbsPtInArea.OffsetRect(left,top);
		if(m_PtInArea.PtInRect(XDPt))	break;
	}
	if(i == m_nRegion)	return FALSE;
	if( (m_nWay == ID_XXHG || m_nWay == ID_XXHGC || m_nWay == ID_XXHGD) && m_aRegion[i].aIndexInfo[0]->nSet != HIS_DAT)	
	{
		TDX_MessageBox(m_pView->m_hWnd,"һ��ָ�����ϲ��������Իع�Ȼ���!",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;
	}
	//�������ĸ�������
	m_nSelfLineIn = i;	
	/////////////////////
	CRect r(&m_PtInArea);
	r.OffsetRect(left,top);
	m_pView->ClientToScreen(&r);
	ClipCursor(&r);
	::SetClassLong(m_pView->GetSafeHwnd(),GCL_HCURSOR,NULL);
	SetCursor(m_hDraw);
	m_nChgMouse ++;

	MemStkInfo * pscode = NULL;
	if(m_nSelfLineIn == 0 || m_aRegion[m_nSelfLineIn].aIndexInfo[0]->nSet!= HIS_DAT)
		pscode = m_pStkInfo;
	else  pscode = (*g_pStockIO)[m_aRegion[m_nSelfLineIn].aIndexInfo[0]->nCurGP];
	if(pscode==NULL) return FALSE;
	
	memset(&m_NewDrawLine,0,sizeof(DRAWLINE));
	m_NewDrawLine.nWay    = m_nWay;
	m_NewDrawLine.SetCode = pscode->setcode;
	strcpy(m_NewDrawLine.Code,pscode->Code);
	strcpy(m_NewDrawLine.acCode,m_aRegion[m_nSelfLineIn].aIndexInfo[0]->acCode);
	m_NewDrawLine.nPeriod = m_nPeriod;
	////////////////////
	m_Point[0] = point;
	m_Point[1] = point;
	m_Point[2] = point;
	m_Point[3] = point;
	m_Point[4] = point;
	TransSelfLine(m_Point[0],NEWLINE_ID,0);
	return TRUE;
}
//����ʱ�ƶ����
void UFxt::DrawingMouseMove(CDC * pDC,CPoint point,int nNo )
{
	if(m_bOverStk || m_Point[1] == point)
		return;

	LPDRAWLINE pDrawLine = NULL;
	long lDrawLineNum = g_DrawLineTool.GetAllLine(pDrawLine);
	if(nNo!=NEWLINE_ID && (nNo<0 || nNo>=lDrawLineNum)) return;

	DRAWLINE  &OneLine = ((nNo==NEWLINE_ID)?m_NewDrawLine:(pDrawLine[nNo]));

	MoveDrawPanRatio(pDC,nNo);
	if(m_nPtNum == 0)		m_Point[0] = point;
	else if(m_nPtNum == 1)	m_Point[1] = point;
	else if(m_nPtNum == 2)	m_Point[2] = point;
	MoveDrawPanRatio(pDC,nNo);
	
	return;
}
//��������ɵĻ���λ��
void UFxt::SelfLineChg(CDC * pDC,CPoint point)
{
	if(m_bOverStk  || point==m_SelectInfo.nBeginPt )	return;
	if(m_SelectInfo.nSet==SELFLINE_OBJECT && m_SelectInfo.bExist && m_SelectInfo.IsMovable)
	{
		LPDRAWLINE pDrawLine = NULL;
		long lDrawLineNum = g_DrawLineTool.GetAllLine(pDrawLine);
		if(m_SelectInfo.nSelfLineNo<0 || m_SelectInfo.nSelfLineNo>=lDrawLineNum) return;

		HCURSOR hTmpCurSor = m_hDrawTurn;
		if(m_SelectInfo.nInOnePoint==1 || m_SelectInfo.nInOnePoint==2 || m_SelectInfo.nInOnePoint == 3)
			hTmpCurSor = m_hDrawTurn;
		else hTmpCurSor = m_hDrawMove;

		DRAWLINE  &OneLine = pDrawLine[m_SelectInfo.nSelfLineNo]; 	
		if(!m_SelectInfo.bMoved)
		{
			::SetClassLong(m_pView->GetSafeHwnd(),GCL_HCURSOR,NULL);
			SetCursor(hTmpCurSor);
			m_nChgMouse ++;
			m_Point[0] = UTransToPoint(m_SelectInfo.nSelfLineNo,0);
			m_Point[1] = UTransToPoint(m_SelectInfo.nSelfLineNo,1);
			m_Point[2] = UTransToPoint(m_SelectInfo.nSelfLineNo,2);
			COLORREF crBak = VipColor.DRAWLINETOOLCOLOR;
			VipColor.DRAWLINETOOLCOLOR = VipColor.ZBCOLOR[4];
			MoveDrawPanRatio(pDC,m_SelectInfo.nSelfLineNo);
			VipColor.DRAWLINETOOLCOLOR = crBak;
		}
		else MoveDrawPanRatio(pDC,m_SelectInfo.nSelfLineNo);
		//
		if(m_SelectInfo.nInOnePoint == 1)
			m_Point[0] += (point - m_SelectInfo.nBeginPt);
		else if(m_SelectInfo.nInOnePoint == 2)
			m_Point[1] += (point - m_SelectInfo.nBeginPt);
		else if(m_SelectInfo.nInOnePoint == 3)
			m_Point[2] += (point - m_SelectInfo.nBeginPt);
		else
		{
			m_Point[0] += (point - m_SelectInfo.nBeginPt);
			m_Point[1] += (point - m_SelectInfo.nBeginPt);
			m_Point[2] += (point - m_SelectInfo.nBeginPt);
		}
		MoveDrawPanRatio(pDC,m_SelectInfo.nSelfLineNo);
		m_SelectInfo.nBeginPt = point;
		m_SelectInfo.bMoved   = TRUE;
	}
}
/////////////////////////��ʼ�๦�ܻ��ߺ���
//Before call the function ,please fill m_Point[0] and m_Point[1] .
void UFxt::MoveDrawPanRatio(CDC *pDC,int nNo,BOOL bShow,int nDrawMode)
{
	int	nSelect = SELNONE_ID;
	CFont *oldfont = NULL;
	if(m_bOverStk) return;
	if(bShow)
	{
		oldfont = g_d.SelectFont(pDC,GRAPH_FONT);
		pDC->SetBkMode(TRANSPARENT);
		if(m_SelectInfo.bExist && m_SelectInfo.nSet==SELFLINE_OBJECT)
		{
			nSelect = m_SelectInfo.nSelfLineNo;
			m_bEvenSelected = TRUE;
		}
	}
	LPDRAWLINE pDrawLine = NULL;
	long lDrawLineNum = g_DrawLineTool.GetAllLine(pDrawLine);
	if(nNo!=NEWLINE_ID && (nNo<0 || nNo>=lDrawLineNum)) return;

	DRAWLINE &OneLine = ((nNo<0)?m_NewDrawLine:(pDrawLine[nNo]));
	long lFixPoint = 0;
	//
	switch(OneLine.nWay)
	{
	case ID_READY:		break;
	case ID_GOLDPRICE: 
		{
			m_Point[2] = m_Point[1];
			m_Point[3] = m_Point[1];
			float maxVal = GetValFromY(m_nSelfLineIn,m_Point[1].y-top);
			m_Point[3].y = GetYPosOfRegion(m_nSelfLineIn,0)+top;
			//
			lFixPoint = g_DrawLineTool.GetMaxSetPtNum(OneLine.nWay);
			if(m_bDrawLine)
			{
				lFixPoint = m_nPtNum+1;
				if(lFixPoint>=2 && !m_bDraw2nd)
				{
					m_bDraw2nd = TRUE;
					break;
				}
			}
			//
			g_DrawLineTool.DrawOne(pDC,&OneLine,m_Point,lFixPoint,m_aRegion[m_nSelfLineIn].DrawRc,left,top,!bShow,(nSelect==nNo),&maxVal);
		}
		break;	
	case ID_GOLDAIM:
		{
			float fVal[3];
			fVal[0] = GetValFromY(m_nSelfLineIn,m_Point[0].y-top);
			fVal[1] = GetValFromY(m_nSelfLineIn,m_Point[1].y-top);
			fVal[2] = GetValFromY(m_nSelfLineIn,m_Point[2].y-top);
			//
			lFixPoint = g_DrawLineTool.GetMaxSetPtNum(OneLine.nWay);
			if(m_bDrawLine)
			{
				lFixPoint = m_nPtNum+1;
				if(lFixPoint>=2 && !m_bDraw2nd)
				{
					m_bDraw2nd = TRUE;
					break;
				}
				if(lFixPoint>=3 && !m_bDraw3rd)
				{
					m_bDraw3rd = TRUE;
					lFixPoint = 2;
				}
			}
			//
			g_DrawLineTool.DrawOne(pDC,&OneLine,m_Point,lFixPoint,m_aRegion[m_nSelfLineIn].DrawRc,left,top,!bShow,(nSelect==nNo),fVal);
		}
		break;
	case ID_XXHGD:
	case ID_XXHGC:
	case ID_XXHG:
		if(CalcLinear(m_Point[0].x-left,m_Point[1].x-left))
		{
			//
			lFixPoint = g_DrawLineTool.GetMaxSetPtNum(OneLine.nWay);
			if(m_bDrawLine) lFixPoint = m_nPtNum+1;
			//
			g_DrawLineTool.DrawOne(pDC,&OneLine,m_Point,lFixPoint,m_aRegion[m_nSelfLineIn].DrawRc,left,top,!bShow,(nSelect==nNo));
		}
		break;
	case ID_GOLD:
	case ID_DFBX:
	case ID_BDX:
	case ID_LDZX:
	case ID_LDXD:
	case ID_ARROW:
	case ID_RAY:
	case ID_ZSX:
	case ID_SSGS:
	case ID_ZX:
	case ID_YHX:
		//
		lFixPoint = g_DrawLineTool.GetMaxSetPtNum(OneLine.nWay);
		if(m_bDrawLine)
		{
			lFixPoint = m_nPtNum+1;
			if(lFixPoint>=2 && !m_bDraw2nd)
			{
				m_bDraw2nd = TRUE;
				break;
			}
		}
		//
		g_DrawLineTool.DrawOne(pDC,&OneLine,m_Point,lFixPoint,m_aRegion[m_nSelfLineIn].DrawRc,left,top,!bShow,(nSelect==nNo));
		break;
	case ID_PXZX:
	case ID_PXX:
		//
		lFixPoint = g_DrawLineTool.GetMaxSetPtNum(OneLine.nWay);
		if(m_bDrawLine)
		{
			lFixPoint = m_nPtNum+1;
			if(lFixPoint>=2 && !m_bDraw2nd)
			{
				m_bDraw2nd = TRUE;
				break;
			}
			if(lFixPoint>=3 && !m_bDraw3rd)
			{
				m_bDraw3rd = TRUE;
				lFixPoint = 2;
			}
		}
		//
		g_DrawLineTool.DrawOne(pDC,&OneLine,m_Point,lFixPoint,m_aRegion[m_nSelfLineIn].DrawRc,left,top,!bShow,(nSelect==nNo));
		break;
	case ID_UPFLAG:
	case ID_DOWNFLAG:
	case ID_TXTTOOLS:
		if(bShow && m_AbsPtInArea.PtInRect(m_Point[0]))
		{
			//
			lFixPoint = g_DrawLineTool.GetMaxSetPtNum(OneLine.nWay);
			if(m_bDrawLine)	lFixPoint = m_nPtNum+1;
			//
			g_DrawLineTool.DrawOne(pDC,&OneLine,m_Point,lFixPoint,m_aRegion[m_nSelfLineIn].DrawRc,left,top,!bShow,(nSelect==nNo));
		}
		break;
	case ID_ZQX:
		{
			m_Point[2] = m_Point[0];
			m_Point[3] = m_Point[1];
			m_Point[2].x = GetAbsItemNoFromX(m_Point[2].x-left);
			m_Point[3].x = GetAbsItemNoFromX(m_Point[3].x-left);
			if(m_Point[3].x<m_Point[2].x)
			{
				m_Point[3].x += m_Point[2].x;
				m_Point[2].x = m_Point[3].x - m_Point[2].x;
				m_Point[3].x = m_Point[3].x - m_Point[2].x;
			}
			else if(m_Point[3].x == m_Point[2].x)
				break;
			//
			lFixPoint = g_DrawLineTool.GetMaxSetPtNum(OneLine.nWay);
			if(m_bDrawLine) lFixPoint = m_nPtNum+1;
			//
			g_DrawLineTool.DrawOne(pDC,&OneLine,m_Point,lFixPoint,m_aRegion[m_nSelfLineIn].DrawRc,left,top,!bShow,(nSelect==nNo),m_pAllPosOfX,m_nDataNum);
		}
		break;
	case ID_FBLQ:
	case ID_GANNTIME: 
		{
			m_Point[2] = m_Point[1];
			m_Point[2].x = GetAbsItemNoFromX(m_Point[2].x-left);
			//
			lFixPoint = g_DrawLineTool.GetMaxSetPtNum(OneLine.nWay);
			if(m_bDrawLine)
			{
				lFixPoint = m_nPtNum+1;
				if(lFixPoint>=2 && !m_bDraw2nd)
				{
					m_bDraw2nd = TRUE;
					break;
				}
				if(lFixPoint>=3 && !m_bDraw3rd)
				{
					m_bDraw3rd = TRUE;
					lFixPoint = 2;
				}
			}
			//
			g_DrawLineTool.DrawOne(pDC,&OneLine,m_Point,lFixPoint,m_aRegion[m_nSelfLineIn].DrawRc,left,top,!bShow,(nSelect==nNo),m_pAllPosOfX,m_nDataNum);
		}
		break;
	case ID_DEL:
		break;
	}
	if ( bShow )
		g_d.RestoreFont(pDC,oldfont);
}

//����Ϣת����m_Point[0] B ��
CPoint UFxt::UTransToPoint(int nNo,int nWhitch)
{
	int		  dx = 0;
	CPoint	  point(0,0);
	
	LPDRAWLINE pDrawLine = NULL;
	long lDrawLineNum = g_DrawLineTool.GetAllLine(pDrawLine);
	if(nNo!=NEWLINE_ID && (nNo<0 || nNo>=lDrawLineNum)) return point;

	DRAWLINE  &OneLine = ((nNo<0)?m_NewDrawLine:(pDrawLine[nNo]));
	//ͨ�����£��粻�����У����ⷽʽ����
	long	  &lTime = (nWhitch == 0 ? OneLine.TimeS : OneLine.TimeE);
	float	  &fVal  = (nWhitch == 0 ? OneLine.fPriceS : OneLine.fPriceE);
	switch ( OneLine.nWay )
	{
	case ID_READY:		break;
	case ID_PXX:
	case ID_PXZX:
	case ID_GOLDAIM:
		point.x = GetItemNoFromTime(lTime);
		if(point.x<0)
		{
			m_bLineValid = FALSE;
			point.x = m_pAllPosOfX[0];
			return point;
		}
		else point.x = m_pAllPosOfX[point.x];
		point.y = GetYPosOfRegion(m_nSelfLineIn,fVal);
		if ( nWhitch == 2 )
		{
			point.x = GetItemNoFromTime(OneLine.TimeR[0]);
			if(point.x<0)
			{
				m_bLineValid = FALSE;
				point.x = m_pAllPosOfX[0];
				return point;
			}
			else point.x = m_pAllPosOfX[point.x];
			point.y = GetYPosOfRegion(m_nSelfLineIn,OneLine.fPriceR[0]);
		}
		break;
	case ID_LDXD:
	case ID_LDZX:
	case ID_ARROW:
	case ID_RAY:
	case ID_GOLD:
	case ID_DFBX:
	case ID_BDX:
	case ID_XXHGD:
	case ID_XXHGC:
	case ID_XXHG:
	case ID_ZSX:
	case ID_SSGS:
	case ID_ZX:
	case ID_YHX:
	case ID_ZQX:
	case ID_FBLQ:
	case ID_GANNTIME:
	case ID_GOLDPRICE:
		if(nWhitch==0 || nWhitch==1)
		{
			point.x = GetItemNoFromTime(lTime);
			if(point.x<0)
			{
				m_bLineValid = FALSE;
				point.x = m_pAllPosOfX[0];
				return point;
			}
			else point.x = m_pAllPosOfX[point.x];
			point.y = GetYPosOfRegion(m_nSelfLineIn,fVal);
		}
		break;
	case ID_TXTTOOLS:
	case ID_UPFLAG:
	case ID_DOWNFLAG:
		if(nWhitch==0)
		{
			point.x = GetItemNoFromTime(lTime);
			if(point.x<0)
			{
				 m_bLineValid = FALSE;
				point.x = m_pAllPosOfX[0];
				return point;
			}
			else point.x = m_pAllPosOfX[point.x];
			point.y = GetYPosOfRegion(m_nSelfLineIn,fVal);
		}
		break;
	case ID_DEL:
		break;
	}
	point.Offset(left,top);
	return point;
}

//��¼��ת������Ϣ��nWhitch�ǵڼ�����
void UFxt::TransSelfLine(CPoint point , int nNo,int nWhitch)
{
	LPDRAWLINE pDrawLine = NULL;
	long lDrawLineNum = g_DrawLineTool.GetAllLine(pDrawLine);
	if(nNo!=NEWLINE_ID && (nNo<0 || nNo>=lDrawLineNum)) return;
	//
	DRAWLINE  &OneLine = ((nNo==NEWLINE_ID)?m_NewDrawLine:(pDrawLine[nNo]));
	//ͨ�����£��粻�����У����ⷽʽ����
	m_pAnalyData = m_pObject->pHisDay;
	long	  &lTime = (nWhitch == 0 ? OneLine.TimeS : OneLine.TimeE);
	float	  &fVal  = (nWhitch == 0 ? OneLine.fPriceS : OneLine.fPriceE);
	point.Offset(-left,-top);
	switch ( OneLine.nWay )
	{
	case ID_READY:		break;
	case ID_PXX:
	case ID_PXZX:
	case ID_GOLDAIM:
		if(nWhitch >= 2)
		{
			memcpy(&OneLine.TimeR[0],&m_pAnalyData[GetAbsItemNoFromX(point.x)].Time.Date,sizeof(long));
			OneLine.fPriceR[0] = (float)GetValFromY(m_nSelfLineIn,point.y);
		}
		else
		{
			memcpy(&lTime,&m_pAnalyData[GetAbsItemNoFromX(point.x)].Time.Date,sizeof(long));
			fVal = (float)GetValFromY(m_nSelfLineIn,point.y);
		}
		break;
	case ID_GOLDPRICE:
	case ID_LDXD:
	case ID_ARROW:
	case ID_RAY:
	case ID_GOLD:
	case ID_DFBX:
	case ID_BDX:
	case ID_ZQX:
	case ID_ZSX: 
	case ID_SSGS:  
	case ID_ZX:
	case ID_LDZX:
	case ID_YHX:
	case ID_XXHGD:
	case ID_XXHGC:
	case ID_XXHG:
	case ID_FBLQ:  
	case ID_GANNTIME:
		if(nWhitch >= 2) return;
		memcpy(&lTime,&m_pAnalyData[GetAbsItemNoFromX(point.x)].Time.Date,sizeof(long)),
		fVal = (float)GetValFromY(m_nSelfLineIn,point.y);
		break;
	case ID_TXTTOOLS:
	case ID_UPFLAG:
	case ID_DOWNFLAG:
		if(nWhitch == 0)
		{
			memcpy(&lTime,&m_pAnalyData[GetAbsItemNoFromX(point.x)].Time.Date,sizeof(long)),
			fVal = (float)GetValFromY(m_nSelfLineIn,point.y);
		}
		break;
	case ID_DEL:
		break;
	}
}

//�ڻ���ʱ�����ܶ�������������²����������� (m_nSelfLineIn�ڱ�)
void UFxt::ShowSelfLine(CDC * pDC)
{	
	int i,j;
	//���뻥��
	if(m_bDrawLine || !g_DrawLineTool.GetShowFlag() || (m_SelectInfo.nSet == SELFLINE_OBJECT && m_SelectInfo.IsMovable))
		return;
	m_PtInArea = m_aRegion[0].DrawRc;
	m_AbsPtInArea = m_PtInArea;
	m_AbsPtInArea.OffsetRect(left,top);
	m_pAnalyData = m_aRegion[0].aIndexInfo[0]->pHisDay;
	LPDRAWLINE pDrawLine;
	long lDrawLine = g_DrawLineTool.GetAllLine(pDrawLine);
	for(i=0;i<lDrawLine;i++)
	{
//		m_nSelfLineIn = SELNONE_ID;
		m_bLineValid = TRUE;
		if(pDrawLine[i].lLineNo==i && pDrawLine[i].nPeriod==m_nPeriod)	//δ��ɾ��
		{	//�ȱȽ�K���ٱȽϹ�Ʊ����Ϊ�����ж�ɡ�
			if ( strcmp(pDrawLine[i].acCode,"STANDK" )==0 )
			{
				MemStkInfo * pscode = NULL;
				for(j=0;j<m_nRegion;j++)
				{
					MemStkInfo * pscode = (*g_pStockIO)[m_aRegion[j].aIndexInfo[0]->nCurGP];
					if(m_aRegion[j].aIndexInfo[0]->nSet==HIS_DAT 
						&& strcmp(pDrawLine[i].Code,pscode->Code)==0 
						&& pDrawLine[i].SetCode==pscode->setcode
						)
					{
						m_PtInArea = m_aRegion[j].DrawRc;
						m_AbsPtInArea = m_PtInArea;
						m_AbsPtInArea.OffsetRect(left,top);
						m_pAnalyData = m_aRegion[j].aIndexInfo[0]->pHisDay;
						m_nSelfLineIn = j;
						m_Point[0] = UTransToPoint(i,0);
						m_Point[1] = UTransToPoint(i,1);
						m_Point[2] = UTransToPoint(i,2);
						//UTransToPoint����ʧ�ܵ�һ�ֱ��֡�
						//�����ϸ񣬾��޷���ʾд����Ϣ��
						if(m_bLineValid)
							MoveDrawPanRatio(pDC,i,TRUE,R2_COPYPEN);
					}
				}
			}
			else if(strcmp(pDrawLine[i].Code,m_pStkInfo->Code)==0  && pDrawLine[i].SetCode==m_pStkInfo->setcode)
			{
				for(int j=0;j<m_nRegion;j++)
				{
					if(m_aRegion[j].aIndexInfo[0]->nSet != HIS_DAT)
					{
						if(strcmp(pDrawLine[i].acCode,m_aRegion[j].aIndexInfo[0]->acCode) == 0)
						{
							m_PtInArea = m_aRegion[j].DrawRc;
							m_AbsPtInArea = m_PtInArea;
							m_AbsPtInArea.OffsetRect(left,top);
							m_nSelfLineIn = j;
							m_Point[0] = UTransToPoint(i,0);
							m_Point[1] = UTransToPoint(i,1);
							m_Point[2] = UTransToPoint(i,2);
							//UTransToPoint����ʧ�ܵ�һ�ֱ��֡�
							if(m_bLineValid)
								MoveDrawPanRatio(pDC,i,TRUE,R2_COPYPEN);
						}
					}
				}
			}
		}
	}
	m_Point[0] = m_Point[1] = m_Point[2] = CPoint(left,top);
}

BOOL UFxt::CalcLinear(long xPosS,long xPosE)
{
	//f=Slope*(i-Ex)+Ey
	//Slope=Sxy/Sxx
	//Sxy=��((i-Ex)*(V-Ey))=��(i*V)-n*Ex*Ey
	//Sxx=��((i-Ex)*(i-Ex))=��(i*i)-n*Ex*Ex
	//Ex=��i/n
	//Ey=��V/n
	if(xPosS>xPosE)						//����˳��
	{
		xPosS += xPosE;
		xPosE = xPosS - xPosE;
		xPosS -= xPosE;
	}
	if(m_nSelfLineIn<0) return FALSE;
	long SItemNo = GetAbsItemNoFromX(xPosS);
	long EItemNo = GetAbsItemNoFromX(xPosE);
	if(m_aRegion[m_nSelfLineIn].aIndexInfo[0]->nSet != HIS_DAT)	return FALSE;
	if(SItemNo<0 || EItemNo<0) return FALSE;
	if(SItemNo==EItemNo) return FALSE;
	double Ex=0,Ey=0,Sxy=0,Sxx=0,Slope=0;
	long nItemNum = EItemNo-SItemNo+1;
	int i=0;
	for(i=SItemNo;i<=EItemNo;i++)
	{
		Ex	+= i;
		Ey	+= m_pAnalyData[i].Close;
		Sxy	+= i*m_pAnalyData[i].Close;
		Sxx += i*i;
	}
	Ex /= nItemNum;
	Ey /= nItemNum;
	Sxy -= Ex*Ey*nItemNum;
	Sxx -= Ex*Ex*nItemNum;
	Slope = Sxy/Sxx;
	m_Point[2].x = m_Point[4].x = m_Point[6].x = xPosS+left;
	m_Point[3].x = m_Point[5].x = m_Point[7].x = xPosE+left;
	m_Point[2].y = GetYPosOfRegion(m_nSelfLineIn,Slope*(SItemNo-Ex)+Ey)+top;
	m_Point[3].y = GetYPosOfRegion(m_nSelfLineIn,Slope*(EItemNo-Ex)+Ey)+top;
	double dMax=0,dMin=0;
	double dLinear=0;
	for(i=SItemNo;i<=EItemNo;i++)
	{
		dLinear = Slope*(i-Ex)+Ey;
		dMax = max(dMax,m_pAnalyData[i].High-dLinear);
		dMin = max(dMin,dLinear-m_pAnalyData[i].Low);
	}
	m_Point[4].y = GetYPosOfRegion(m_nSelfLineIn,Slope*(SItemNo-Ex)+Ey+dMax)+top;
	m_Point[5].y = GetYPosOfRegion(m_nSelfLineIn,Slope*(EItemNo-Ex)+Ey+dMax)+top;
	m_Point[6].y = GetYPosOfRegion(m_nSelfLineIn,Slope*(SItemNo-Ex)+Ey-dMin)+top;
	m_Point[7].y = GetYPosOfRegion(m_nSelfLineIn,Slope*(EItemNo-Ex)+Ey-dMin)+top;

	return TRUE;
}

int UFxt::SelectSelfLine(int nRegion,CPoint point)
{
	long i,j;

	if(m_bDrawLine || !g_DrawLineTool.GetShowFlag())
		return SELNONE_ID;
	m_Point[0] = m_Point[1] = m_Point[2] = CPoint(left,top);

	LPDRAWLINE pDrawLine= NULL;
	long lDrawLineNum	= g_DrawLineTool.GetAllLine(pDrawLine);
	if(pDrawLine==NULL || lDrawLineNum<=0) return SELNONE_ID;
	//
	char *kCode = "STANDK";
	MemStkInfo * pscode = NULL;
	char *acCode = NULL;
	long lMaxPoint = 0;
	long lSelPos = LINESEL_SELNONE;
	//
	if(nRegion == 0 || m_aRegion[nRegion].aIndexInfo[0]->nSet!= HIS_DAT)
	{
		pscode = m_pStkInfo;
		acCode = m_aRegion[nRegion].aIndexInfo[0]->acCode;
	}
	else if(m_aRegion[nRegion].aIndexInfo[0]->nSet == HIS_DAT)
	{
		pscode = (*g_pStockIO)[m_aRegion[nRegion].aIndexInfo[0]->nCurGP];
		acCode = kCode;
	}
	if(pscode==NULL || acCode==NULL) return SELNONE_ID;
	CRect r(&(m_aRegion[nRegion].DrawRc));
	r.OffsetRect(left,top);
	if(!r.PtInRect(point)) return SELNONE_ID;
	//

	for(i=0;i<lDrawLineNum;i++)
	{
		if(pDrawLine[i].lLineNo==i
			&& strcmp(pDrawLine[i].Code,pscode->Code)==0 && pDrawLine[i].SetCode==pscode->setcode
			&& pDrawLine[i].nPeriod == m_nPeriod && strcmp(pDrawLine[i].acCode,acCode)==0)
		{
			lMaxPoint = g_DrawLineTool.GetMaxSetPtNum(pDrawLine[i].nWay);
			if(lMaxPoint<1 || lMaxPoint>3) continue;
			for(j=lMaxPoint;j>0;j--)
			{
				switch(j)
				{
				case 3:
					m_Point[j-1].x = m_pAllPosOfX[GetItemNoFromTime(pDrawLine[i].TimeR[0])]+left;
					m_Point[j-1].y = GetYPosOfRegion(nRegion,pDrawLine[i].fPriceR[0])+top;
					break;
				case 2:
					m_Point[j-1].x = m_pAllPosOfX[GetItemNoFromTime(pDrawLine[i].TimeE)]+left;
					m_Point[j-1].y = GetYPosOfRegion(nRegion,pDrawLine[i].fPriceE)+top;
					break;
				case 1:
					m_Point[j-1].x = m_pAllPosOfX[GetItemNoFromTime(pDrawLine[i].TimeS)]+left;
					m_Point[j-1].y = GetYPosOfRegion(nRegion,pDrawLine[i].fPriceS)+top;
					break;
				}
			}
			switch(pDrawLine[i].nWay)
			{
			case ID_XXHGD:
			case ID_XXHGC:
			case ID_XXHG:
				if(CalcLinear(m_Point[0].x-left,m_Point[1].x-left))
					lSelPos = g_DrawLineTool.CheckSel(&(pDrawLine[i]),m_Point,8,point,m_aRegion[nRegion].DrawRc,left,top);
				else lSelPos = LINESEL_SELNONE;
				break;
			case ID_ZQX:
				m_Point[2] = m_Point[0];
				m_Point[3] = m_Point[1];
				m_Point[2].x = GetAbsItemNoFromX(m_Point[2].x-left);
				m_Point[3].x = GetAbsItemNoFromX(m_Point[3].x-left);
				if(m_Point[3].x<m_Point[2].x)
				{
					m_Point[3].x += m_Point[2].x;
					m_Point[2].x = m_Point[3].x - m_Point[2].x;
					m_Point[3].x = m_Point[3].x - m_Point[2].x;
				}
				else if(m_Point[3].x == m_Point[2].x)
				{
					lSelPos = LINESEL_SELNONE;
					break;
				}
				lSelPos = g_DrawLineTool.CheckSel(&(pDrawLine[i]),m_Point,4,point,m_aRegion[nRegion].DrawRc,left,top,m_pAllPosOfX,m_nDataNum);
				break;
			case ID_FBLQ:
				m_Point[2] = m_Point[1];
				m_Point[2].x = GetAbsItemNoFromX(m_Point[2].x-left);
				lSelPos = g_DrawLineTool.CheckSel(&(pDrawLine[i]),m_Point,3,point,m_aRegion[nRegion].DrawRc,left,top,m_pAllPosOfX,m_nDataNum);
				break;
			case ID_GANNTIME:
				m_Point[2] = m_Point[1];
				m_Point[2].x = GetAbsItemNoFromX(m_Point[2].x-left);
				lSelPos = g_DrawLineTool.CheckSel(&(pDrawLine[i]),m_Point,3,point,m_aRegion[nRegion].DrawRc,left,top,m_pAllPosOfX,m_nDataNum);
				break;
			case ID_GOLDPRICE:
				m_Point[2] = m_Point[1];
				m_Point[3] = m_Point[1];
				m_Point[3].y = GetYPosOfRegion(m_nSelfLineIn,0)+top;
				lSelPos = g_DrawLineTool.CheckSel(&(pDrawLine[i]),m_Point,4,point,m_aRegion[nRegion].DrawRc,left,top);
				break;
			default:
				lSelPos = g_DrawLineTool.CheckSel(&(pDrawLine[i]),m_Point,lMaxPoint,point,m_aRegion[nRegion].DrawRc,left,top);
				break;
			}
			switch(lSelPos)
			{
			case 0:
			case 1:
			case 2:
			case 3:
			case 4:
				if(lSelPos<0 || lSelPos>=lMaxPoint) break;
				m_SelectInfo.nInOnePoint = lSelPos+1;
			case LINESEL_SELNOTPOINT:
				m_SelectInfo.bExist		= 1;
				m_SelectInfo.IsMovable	= 1;
				m_SelectInfo.nInRegion	= nRegion;
				m_SelectInfo.nSet		= SELFLINE_OBJECT;
				m_SelectInfo.nSelfLineNo= i;
				m_SelectInfo.nBeginPt.x	= point.x;
				m_SelectInfo.nBeginPt.y	= point.y;
				m_SelectInfo.bMoved		= 0;
				return i;
			default:
				break;
			}
		}
	}
	return SELNONE_ID;
}

void UFxt::SetSelfLine(UINT nID)
{
	switch(nID)
	{
	case ID_DEL:
		{
			LPDRAWLINE pDrawLine;
			long lDrawLine = g_DrawLineTool.GetAllLine(pDrawLine);
			if(g_DrawLineTool.GetShowFlag() && m_SelectInfo.nSet == SELFLINE_OBJECT)
			{
				if(m_SelectInfo.nSelfLineNo >= 0 && m_SelectInfo.nSelfLineNo < lDrawLine)
				{
					g_DrawLineTool.DeleteLine(m_SelectInfo.nSelfLineNo);
					g_DrawLineTool.CleanDel();
					memset(&m_SelectInfo,0,sizeof(FXTSELINFO));
					ProcessMsg(WM_PAINT);
				}
			}
			else 
			{
				BOOL bIsExist = 0;	
				for(int i=0;i<lDrawLine;i++)
				{
					if(strcmp(pDrawLine[i].Code,m_pStkInfo->Code)==0 && pDrawLine[i].SetCode==m_pStkInfo->setcode)
					{
						bIsExist = 1;
						break;
					}
				}
				if(bIsExist &&  TDX_MessageBox(m_pView->m_hWnd,"��Ҫɾ����֤ȯ���������л����� ?",MB_YESNO|MB_ICONQUESTION) == IDYES)
				{
					for(int i=0;i<lDrawLine;i++)
					{
						if(strcmp(pDrawLine[i].Code,m_pStkInfo->Code)==0 && pDrawLine[i].SetCode==m_pStkInfo->setcode)
							g_DrawLineTool.DeleteLine(i);
					}
					g_DrawLineTool.CleanDel();
				}
				ProcessMsg(WM_PAINT);
			}
		}
		break;
	case ID_READY:
		ResetSelfLine();
		break;
	case ID_SHOWALLLINE:
		{
			CSelfLineShowDlg dlg;
			if(dlg.DoModal()==IDOK)
				ProcessMsg(WM_PAINT);
		}
		break;
	case ID_HIDESELFLINE:
		g_DrawLineTool.SetShowFlag(!g_DrawLineTool.GetShowFlag());
		ProcessMsg(WM_PAINT);
		break;
	case ID_PROPERTES:
		if(g_DrawLineTool.GetShowFlag() && m_SelectInfo.nSet == SELFLINE_OBJECT
			&& m_SelectInfo.nSelfLineNo >= 0 && m_SelectInfo.nSelfLineNo < g_DrawLineTool.GetLineNum())
		{
			ProcessMsg(WM_PAINT);
		}
		break;
	default:
		ResetSelfLine();
		m_nWay = nID;
		m_bDrawLine = TRUE;
		break;
	}
}

UINT UFxt::GetSelfLine()
{
	return m_nWay;
}
