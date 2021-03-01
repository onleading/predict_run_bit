// TestYieldWnd.cpp : implementation file
//

#include "stdafx.h"
#include "tdxw.h"
#include "ParamTestDlg.h"
#include "AllTestMapDlg.h"
#include "MyDetailDlg.h"
#include "TestYieldWnd.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CString sHelpu("→右移 ←左移 Ctrl+→快速右移 Ctrl+←快速左移 ↓缩小 ↑放大");
CString sHelpd("Enter显示明细 Esc隐藏十字光标 Home移至起始日期 End移至终结日期");
const char *g_axesText[] = {"交易(次)", "累\r\n计\r\n盈\r\n亏\r\n(%%)", "成\r\n交\r\n额\r\n(万元)"};
const int g_mainMAParam[] = {5, 10, 20, -1};
const int g_subMAParam[] = {5, 10, -1};
/////////////////////////////////////////////////////////////////////////////
// CTestYieldWnd

CTestYieldWnd::CTestYieldWnd()
{
	m_maxPerPage = 60;
	m_fSplitRadio = 0.7f;
	m_nStart =0;
	m_nCursorPos = -1;
	m_bHiLight = FALSE;
	m_iCurPos = -1;
}

CTestYieldWnd::~CTestYieldWnd()
{

}


BEGIN_MESSAGE_MAP(CTestYieldWnd, CWnd)
//{{AFX_MSG_MAP(CTestYieldWnd)
	ON_WM_PAINT()
	ON_WM_MOUSEMOVE()
	ON_WM_CREATE()
	ON_WM_SETCURSOR()
	ON_WM_LBUTTONDOWN()
	ON_WM_CLOSE()
	ON_WM_ERASEBKGND()
	ON_WM_KEYDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
void CTestYieldWnd::SetData(map<long, testDetail, less_equal<long> > &dataMap, vector<testDetail> &dataDetails)
{
	m_allTestData = dataMap;
	m_sTestData = dataDetails;
	
	int nData = dataMap.size();
	KInfo curK;memset(&curK, 0, sizeof(KInfo));
	m_allPt = vector<KInfo>(m_allTestData.size(), curK);
}

void CTestYieldWnd::GetWndHW(int &height, int &withd)
{
	CRect rectWnd;
	GetWindowRect(&rectWnd);
	ScreenToClient(&rectWnd);
	height = rectWnd.Height();
	withd = rectWnd.Width();
}

void CTestYieldWnd::SetOrigin(CDC *pDC)
{
	int height, withd;
	GetWndHW(height, withd);
	CPoint ptBottomLeft(0, 0-height);
	pDC->LPtoDP(&ptBottomLeft);
	pDC->SetViewportOrg(ptBottomLeft);
}

void CTestYieldWnd::DrawProfit(CDC *pDC)
{
	const int AXIS_DIVIDER_LENGTH = 4;
	const int AXIS_FONT_HEIGHT = 16;
	int i, nProfit = min(m_allTestData.size()-m_nStart, m_maxPerPage);
	
	int height, withd;
	GetWndHW(height, withd);
	CPen *oldPen = g_d.SelectPen(pDC, AXIS_PEN);
	CFont *oldFont = g_d.SelectFont(pDC, GRAPH_FONT);
	// Base coordinates for axes
	
	TEXTMETRIC tm;
	pDC->GetTextMetrics(&tm);
	const CPoint ORIGIN(0.07*withd+2*tm.tmAveCharWidth, 0.97*height-2*tm.tmHeight);
	const CPoint Y_EXTENT(ORIGIN.x, 0.1*height);
	const CPoint X_EXTENT(0.96*withd, ORIGIN.y);
	
	// Draw axes
	CRect torc(Y_EXTENT, X_EXTENT);
	m_drawBorder.CopyRect(&torc);
	g_d.DrawRect(pDC, &m_drawBorder, VipColor.AXISCOLOR);
	CRect uprc(m_drawBorder.left, m_drawBorder.top, m_drawBorder.right, m_fSplitRadio*m_drawBorder.bottom);
	m_mainBorder.CopyRect(&uprc);
	g_d.DrawRect(pDC, &m_mainBorder, VipColor.AXISCOLOR);
	CRect dnrc(m_drawBorder.left, m_fSplitRadio*m_drawBorder.bottom, m_drawBorder.right, m_drawBorder.bottom);
	m_volBorder.CopyRect(&dnrc);
	g_d.DrawRect(pDC, &m_volBorder, VipColor.AXISCOLOR);

	CRect vText(ORIGIN.x-0.07*withd-1.5*tm.tmAveCharWidth, Y_EXTENT.y, ORIGIN.x, ORIGIN.y);
	g_d.DisplayText(pDC, vText, VipColor.ZBCOLOR[2], DT_LEFT|DT_VCENTER, g_axesText[1]);
	g_d.DisplayText(pDC, m_drawBorder.right-3.5*tm.tmAveCharWidth, m_drawBorder.bottom + AXIS_FONT_HEIGHT / 2, VipColor.ZBCOLOR[2], g_axesText[0]);
	CRect VolText(ORIGIN.x-0.07*withd-1.5*tm.tmAveCharWidth, m_volBorder.top+0.6*tm.tmHeight,
		ORIGIN.x,ORIGIN.y);
	g_d.DisplayText(pDC, VolText, VipColor.ZBCOLOR[2], DT_LEFT|DT_VCENTER, g_axesText[2]);
	
	int nScale;
	float fProfit, fTopProfit, fButtonProfit;
	
    float nXIncrement = (float)m_drawBorder.Width() / (float)(nProfit+1);
	CString sXAxes, sYAxes;
	map<long, testDetail, less_equal<long> >::iterator iter = m_allTestData.begin();
	for(i=0, fProfit = 0.0f;i<m_nStart;i++,iter++)
		fProfit += (*iter).second.fProfit;
	fTopProfit = fButtonProfit = fProfit;
	float nMaxProfit = fTopProfit;
	float nMinProfit = fButtonProfit;
	long nMaxAmount = 0;
	iter = m_allTestData.begin();
	advance(iter, m_nStart);
	nScale = nProfit/20+1;
	for(i = 0; i < nProfit; i++, iter++)
	{
		int xPoint = (int)((float)m_drawBorder.left + ( (float)(i + 1) * nXIncrement));
		m_allPt[m_nStart+i].xPos = xPoint;
		testDetail curDetail = (*iter).second;
		fTopProfit = fProfit + curDetail.fTopProfit;
		fButtonProfit = fProfit + curDetail.fButtonProfit;
		fProfit += curDetail.fProfit;
		
		nMaxProfit = max(nMaxProfit, fTopProfit);
		nMinProfit = min(nMinProfit, fButtonProfit);
		nMaxAmount = max(nMaxAmount, curDetail.lCount*curDetail.fPrice);

		if(!(i%nScale))
		{
			g_d.DrawLine(pDC, xPoint, m_mainBorder.bottom, xPoint, m_mainBorder.bottom - AXIS_DIVIDER_LENGTH);
			sXAxes.Format("%d",m_nStart + i+ 1);
			g_d.DisplayText(pDC, xPoint-3, m_drawBorder.bottom + AXIS_FONT_HEIGHT / 2, VipColor.ZBCOLOR[2], sXAxes);
		}
	}
	
	// Divide y-axis into suitable scale based on 
	// the difference between max and min prices on file
	nMaxProfit += 5.0f;
	nMinProfit -= 5.0f;
	nMaxAmount *=1.1;
	
	int iScale = int(nMaxProfit) - int(nMinProfit);		//以1%递增
	nScale = iScale/50+1;
	float nYIncrement = (float)(m_mainBorder.bottom - m_mainBorder.top - 2*tm.tmHeight) / (float)iScale;
	for(i = 0; i < iScale; i++)
	{
		int yPoint = (int)((float)m_mainBorder.bottom - ((float)i * nYIncrement));
		
		int iCurrentProfit = (int)nMinProfit + i;
		iter = m_allTestData.begin();
		fProfit = fTopProfit = fButtonProfit = 0.0f;	
		for(int p=0;p<m_nStart;p++,iter++)
			fProfit += (*iter).second.fProfit;
		iter = m_allTestData.begin();
		advance(iter, m_nStart);
		for(int j = 0; j < nProfit; j++, iter++)
		{
			if(fProfit >= float(iCurrentProfit) && 
				fProfit < float(iCurrentProfit) + 1.0)
			{
				float dFraction = fProfit - float(iCurrentProfit);
				m_allPt[m_nStart+j].yOpenPos = yPoint - int(dFraction * nYIncrement);
			}
			fTopProfit = fProfit + (*iter).second.fTopProfit;
			if(fTopProfit >= float(iCurrentProfit) &&
				fTopProfit < float(iCurrentProfit) + 1.0)
			{
				float dFraction = fTopProfit - float(iCurrentProfit);
				m_allPt[m_nStart+j].yHighPos = yPoint - int(dFraction * nYIncrement);
			}
			fButtonProfit = fProfit + (*iter).second.fButtonProfit;
			if(fButtonProfit >= float(iCurrentProfit) &&
				fButtonProfit < float(iCurrentProfit) + 1.0)
			{
				float dFraction = fButtonProfit - float(iCurrentProfit);
				m_allPt[m_nStart+j].yLowPos = yPoint - int(dFraction * nYIncrement);
			}		
			fProfit += (*iter).second.fProfit;
			if(fProfit >= float(iCurrentProfit) && 
				fProfit < float(iCurrentProfit) + 1.0)
			{
				float dFraction = fProfit - float(iCurrentProfit);
				m_allPt[m_nStart+j].yClosePos = yPoint - int(dFraction * nYIncrement);
			}
		}
		if(!(iCurrentProfit%(5*nScale)))
		{
			g_d.DotLine(pDC, m_mainBorder.left, yPoint, m_mainBorder.right, yPoint, VipColor.ZBCOLOR[2], m_mainBorder.Width()/62);			
			sYAxes.Format("%d", iCurrentProfit);			  
			int nTextSize = pDC->GetTextExtent(sYAxes).cx;
			nTextSize += 6;			  
			g_d.DisplayText(pDC, m_mainBorder.left - nTextSize, yPoint-4, VipColor.ZBCOLOR[2], sYAxes);
		}
	}

	int nVolGap = 5;
	float nVolIncrement = (float)(m_volBorder.bottom - m_volBorder.top - 1.5*tm.tmHeight) / (float)nVolGap;
	for(i=0;i<nVolGap;i++)
	{
		int yPoint = m_volBorder.bottom-i*nVolIncrement;
		sYAxes.Format("%d", i*nMaxAmount/nVolGap/10000);
		int nTextSize = pDC->GetTextExtent(sYAxes).cx;
		nTextSize += 6;
		g_d.DisplayText(pDC, m_volBorder.left - nTextSize, yPoint-4, VipColor.ZBCOLOR[2], sYAxes);
	}
	iter = m_allTestData.begin();
	advance(iter, m_nStart);
	float fVolScal = (float)(m_volBorder.bottom - m_volBorder.top - 1.5*tm.tmHeight)/(float)nMaxAmount;
	for(i=0;i<nProfit;i++,iter++)
	{
		long curAmount = (*iter).second.lCount*(*iter).second.fPrice;
		m_allPt[m_nStart+i].yVolPos = m_volBorder.bottom-fVolScal*curAmount;
	}
	
	g_d.SelectPen(pDC, ZB3_PEN);
	for(i = 0; i < nProfit; i++)
	{
		DrawKLine(pDC, i, 60/m_maxPerPage+1);
		DrawVol(pDC, i, 60/m_maxPerPage+1);
	}
	DrawMainMa(pDC);
	DrawSubMa(pDC);
	
	testDetail curDetail;
	CString str("9999年99月99日");
	CString sDay;
	int nTextSize = pDC->GetTextExtent(str).cx;
	CRect sDayStart(m_drawBorder.left-0.6*nTextSize, m_drawBorder.bottom+AXIS_FONT_HEIGHT+0.5*tm.tmHeight, 
		m_drawBorder.left+0.7*nTextSize, m_drawBorder.bottom+AXIS_FONT_HEIGHT+2*tm.tmHeight);
	CRect sDayEnd(m_drawBorder.right-0.7*nTextSize, m_drawBorder.bottom+AXIS_FONT_HEIGHT+0.5*tm.tmHeight,
		m_drawBorder.right+0.6*nTextSize, m_drawBorder.bottom+AXIS_FONT_HEIGHT+2*tm.tmHeight);
	iter = m_allTestData.begin();
	advance(iter, m_nStart);
	curDetail = (*iter).second;
	sDay.Format("%d年%d月%d日",curDetail.nTime.year, curDetail.nTime.month, curDetail.nTime.day);
	g_d.DisplayText(pDC, sDayStart, VipColor.LEVELCOLOR, DT_RIGHT|DT_BOTTOM, sDay);
	advance(iter, nProfit-1);
	curDetail = (*iter).second;
	sDay.Format("%d年%d月%d日",curDetail.nTime.year, curDetail.nTime.month, curDetail.nTime.day);	
	g_d.DisplayText(pDC, sDayEnd, VipColor.LEVELCOLOR, DT_LEFT|DT_BOTTOM, sDay);

	g_d.RestorePen(pDC, oldPen);
	g_d.RestoreFont(pDC, oldFont);
}

void CTestYieldWnd::DrawKLine(CDC *pDC, int nk, int nOff)
{
	CPen *pOldPen = g_d.SelectPen(pDC, KP_PEN);
	if(m_allPt[m_nStart+nk].yClosePos<m_allPt[m_nStart+nk].yOpenPos)
	{
		g_d.Bar(pDC, m_allPt[m_nStart+nk].xPos-nOff, m_allPt[m_nStart+nk].yClosePos, m_allPt[m_nStart+nk].xPos+nOff, m_allPt[m_nStart+nk].yOpenPos, KP_BRUSH);
		g_d.DrawLine(pDC, m_allPt[m_nStart+nk].xPos, m_allPt[m_nStart+nk].yHighPos, m_allPt[m_nStart+nk].xPos, m_allPt[m_nStart+nk].yLowPos);
	}
	else if(m_allPt[m_nStart+nk].yClosePos>m_allPt[m_nStart+nk].yOpenPos)
	{
		g_d.SelectPen(pDC, KN_PEN);
		g_d.Bar(pDC, m_allPt[m_nStart+nk].xPos-nOff, m_allPt[m_nStart+nk].yOpenPos, m_allPt[m_nStart+nk].xPos+nOff, m_allPt[m_nStart+nk].yClosePos, KN_BRUSH);
		g_d.DrawLine(pDC, m_allPt[m_nStart+nk].xPos, m_allPt[m_nStart+nk].yHighPos, m_allPt[m_nStart+nk].xPos, m_allPt[m_nStart+nk].yLowPos);
	}
	else
	{
		g_d.DrawLine(pDC, m_allPt[m_nStart+nk].xPos-nOff, m_allPt[m_nStart+nk].yClosePos, m_allPt[m_nStart+nk].xPos+nOff, m_allPt[m_nStart+nk].yClosePos);
		g_d.DrawLine(pDC, m_allPt[m_nStart+nk].xPos, m_allPt[m_nStart+nk].yHighPos, m_allPt[m_nStart+nk].xPos, m_allPt[m_nStart+nk].yLowPos);
	}
	g_d.RestorePen(pDC, pOldPen);
}

void CTestYieldWnd::DrawVol(CDC *pDC, int nk, int nOff)
{
	if ( m_allPt[m_nStart+nk].yClosePos <= m_allPt[m_nStart+nk].yOpenPos)
		g_d.Bar(pDC,m_allPt[m_nStart+nk].xPos-nOff,m_volBorder.bottom,m_allPt[m_nStart+nk].xPos+nOff,m_allPt[m_nStart+nk].yVolPos,KP_BRUSH);
	else 
		g_d.Bar(pDC, m_allPt[m_nStart+nk].xPos-nOff,m_volBorder.bottom,m_allPt[m_nStart+nk].xPos+nOff,m_allPt[m_nStart+nk].yVolPos,KN_BRUSH);
}

void CTestYieldWnd::DrawMainMa(CDC *pDC)
{
	int i, j, k;
	for(i=0;g_mainMAParam[i]!=-1;i++)
	{
		int nVal = min(m_allTestData.size()-m_nStart,m_maxPerPage);
		float *val = new float[nVal];
		memset(val, 0, nVal*sizeof(float));		
		int iParam = g_mainMAParam[i];
		for(j=0;j<nVal;j++)
		{
			for(k=0;k<=min(iParam-1, max(m_nStart, j));k++)  
				val[j] += (m_allPt[m_nStart+j-k].yClosePos>COMPPREC?m_allPt[m_nStart+j-k].yClosePos:m_allPt[m_nStart+j].yClosePos);
			val[j] /= k;
		}
		for(j=1;j<nVal;j++)
			g_d.DrawLine(pDC, m_allPt[m_nStart+j-1].xPos, val[j-1], m_allPt[m_nStart+j].xPos, val[j], VipColor.ZBCOLOR[i]);
		delete []val;val=NULL;
	}
}

void CTestYieldWnd::DrawSubMa(CDC *pDC)
{
	int i, j, k;
	for(i=0;g_subMAParam[i]!=-1;i++)
	{
		int nVal = min(m_allTestData.size()-m_nStart,m_maxPerPage);
		float *val = new float[nVal];
		memset(val, 0, nVal*sizeof(float));
		int iParam = g_subMAParam[i];
		for(j=0;j<nVal;j++)
		{
			for(k=0;k<=min(iParam-1, max(m_nStart, j));k++) 
				val[j] += (m_allPt[m_nStart+j-k].yVolPos>COMPPREC?m_allPt[m_nStart+j-k].yVolPos:m_allPt[m_nStart+j].yVolPos);
			val[j] /= k;
		}
		for(j=1;j<nVal;j++)
			g_d.DrawLine(pDC, m_allPt[m_nStart+j-1].xPos, val[j-1], m_allPt[m_nStart+j].xPos, val[j], VipColor.ZBCOLOR[i]);
		delete []val;val=NULL;
	}
}

void CTestYieldWnd::ShowHelpTip(CDC* pDC)
{
	CRect rc, rcHelpu, rcHelpd;
	GetWindowRect(&rc);
	ScreenToClient(&rc);
	TEXTMETRIC tm;
	pDC->GetTextMetrics(&tm);
	CFont *oldFont = g_d.SelectFont(pDC, GRAPH_FONT);
	rcHelpu.top = rc.top;rcHelpu.bottom = (rc.top+m_mainBorder.top)/2;
	rcHelpu.left = (m_mainBorder.left+m_mainBorder.right)/4;rcHelpu.right = m_mainBorder.right;
	rcHelpd.top = (rc.top+m_mainBorder.top)/2;rcHelpd.bottom = m_mainBorder.top;
	rcHelpd.left = (m_mainBorder.left+m_mainBorder.right)/4;rcHelpd.right = m_mainBorder.right;
	g_d.DisplayText(pDC, rcHelpu, VipColor.TXTCOLOR, DT_LEFT|DT_BOTTOM, _F(sHelpu));
	g_d.DisplayText(pDC, rcHelpd, VipColor.TXTCOLOR, DT_LEFT|DT_BOTTOM, _F(sHelpd));
	g_d.RestoreFont(pDC, oldFont);
}
/////////////////////////////////////////////////////////////////////////////
// CTestYieldWnd message handlers
void CTestYieldWnd::OnPaint() 
{
	CPaintDC dc(this); // device context for painting

	CRect drawRect;
	GetWindowRect(&drawRect);
	ScreenToClient(&drawRect);
	
	CBitmap		tempBitmap;				//作出临时位图
	CDC memDC;							//作出内存DC
	memDC.CreateCompatibleDC (&dc);
	tempBitmap.CreateCompatibleBitmap (&dc,drawRect.Width (),drawRect.Height());
	CBitmap *pOldBitmap = memDC.SelectObject (&tempBitmap);
	memDC.SetBkMode (TRANSPARENT);
	memDC.FillSolidRect(&drawRect, VipColor.BACKCOLOR);
	
	DrawProfit(&memDC);
	ShowHelpTip(&memDC);
	
	CBitmap Bitmap;
	Bitmap.CreateCompatibleBitmap(&dc,drawRect.Width(),drawRect.Height());
	//确定MemDC的大小
	m_memDC.SelectObject(&Bitmap);
	//将这个屏幕的都存入MemDC
	m_memDC.BitBlt(0,0,drawRect.Width(),drawRect.Height(),&memDC,0,0,SRCCOPY);
	//将临时图片删除
	::DeleteObject(Bitmap.m_hObject);
	if(m_nCursorPos>-1) ShowCurInfo(&memDC);

	dc.BitBlt (0,0,drawRect.Width(),drawRect.Height(),&memDC,0,0,SRCCOPY);//从内存DC拷贝到屏幕DC	
	memDC.SelectObject(pOldBitmap);
	tempBitmap.DeleteObject();
	memDC.DeleteDC();
}

void CTestYieldWnd::OnMouseMove(UINT nFlags, CPoint point) 
{
	if(m_nCursorPos>-1) 
		return;

	CDC *pDC=GetDC();
	int i;
	CRect rc, rcName, rcTime, rcText, rcVolText;
	GetWindowRect(&rc);
	ScreenToClient(&rc);
	TEXTMETRIC tm;
	pDC->GetTextMetrics(&tm);
	rcTime.top = rc.top;rcTime.bottom = (rc.top+m_mainBorder.top)/2;
	rcTime.left = m_mainBorder.left;rcTime.right = m_mainBorder.right;
	rcName.top = (rc.top+m_mainBorder.top)/2;rcName.bottom = m_mainBorder.top;
	rcName.left = m_mainBorder.left;rcName.right = m_mainBorder.right;
	rcText.top = m_mainBorder.top;rcText.bottom = m_mainBorder.top+1.5*tm.tmHeight;
	rcText.left = m_mainBorder.left;rcText.right = m_mainBorder.right;
	rcVolText.top = m_volBorder.top;rcVolText.bottom = m_volBorder.top+1.5*tm.tmHeight;
	rcVolText.left = m_volBorder.left;rcVolText.right = m_volBorder.right;
	
	CDC memDC;
	memDC.CreateCompatibleDC (pDC);
	float fTotYield;
	BOOL bRefresh = FALSE;
	map<long, testDetail, less_equal<long> >::iterator iter;
	iter = m_allTestData.begin();
	for(i=0, fTotYield = 0.0f;i<m_nStart;i++,iter++)
		fTotYield += (*iter).second.fProfit;
	testDetail curDetail;
	for(i=m_nStart;i<min(m_nStart+m_maxPerPage,m_allTestData.size());i++)
	{
		if(fabs(m_allPt[i].xPos-point.x)<2 && fabs(m_allPt[i].yClosePos-point.y)<2)
		{
			iter = m_allTestData.begin();
			m_iCurPos = i;
			for(int j=0;j<=i;j++,iter++)
			{
				fTotYield += (*iter).second.fProfit;
			}
			iter--;
			curDetail = (*iter).second;
			m_bHiLight = TRUE;
			bRefresh = TRUE;
			break;
		}
	}		
	if(m_bHiLight&&bRefresh)
	{
		float fCurProfit = curDetail.fProfit;
		float fTopProfit = curDetail.fTopProfit;
		float fButtonProfit = curDetail.fButtonProfit;
		long fVol = curDetail.lCount;
		char getDate[16];
		sprintf(getDate, "%d-%d-%d", curDetail.nTime.year, curDetail.nTime.month, curDetail.nTime.day);
		char    name[NAME_LEN+1]; 			//股票代码
		strcpy(name, (*g_pStockIO)[(*iter).second.iGp]->Name);
		CBitmap		tempBitmap;				//作出临时位图
		tempBitmap.CreateCompatibleBitmap (pDC,rc.Width (),rc.Height());
		CBitmap *pOldBitmap = memDC.SelectObject (&tempBitmap);
		memDC.SetBkMode (TRANSPARENT);
		memDC.BitBlt(0, 0, rc.Width(), rc.Height(), &m_memDC, 0, 0, SRCCOPY);
		
		CFont *oldFont = g_d.SelectFont(&memDC, GRAPH_FONT);
		COLORREF disColor = curDetail.fProfit>COMPPREC?VipColor.KPCOLOR:VipColor.DOWNCOLOR;
		g_d.DisplayText(&memDC, &rcName, disColor, DT_LEFT|DT_BOTTOM, "股票:%s", name);
		g_d.DisplayText(&memDC, &rcTime, disColor, DT_LEFT|DT_BOTTOM, "时间:%s", getDate);
		g_d.DisplayText(&memDC, &rcText, disColor, DT_LEFT|DT_BOTTOM, "本次盈亏:%.2f%% ,最大盈亏:%.2f%%, 最小盈亏:%.2f%%, 总盈亏:%.2f%%", 
			fCurProfit, fTopProfit, fButtonProfit, fTotYield);
		g_d.DisplayText(&memDC, &rcVolText, disColor, DT_LEFT|DT_BOTTOM, "成交额:%d元", fVol);
		pDC->BitBlt (0,0,rc.Width(),rc.Height(),&memDC,0,0,SRCCOPY);//从内存DC拷贝到屏幕DC
		g_d.RestoreFont(&memDC, oldFont);
		memDC.SelectObject(pOldBitmap);
		tempBitmap.DeleteObject();
		memDC.DeleteDC();
	}
	if(m_bHiLight&&!bRefresh) 
	{
		pDC->BitBlt(0,0,rc.Width(),rc.Height(),&m_memDC,0,0,SRCCOPY);
		m_bHiLight = FALSE;
	}
	
	CWnd::OnMouseMove(nFlags, point);
}

void CTestYieldWnd::ShowCurInfo(CDC *pDC)
{
	if(m_nCursorPos==-1) return;
	CRect rc, rcName, rcTime, rcText, rcVolText;
	GetWindowRect(&rc);
	ScreenToClient(&rc);
	TEXTMETRIC tm;
	pDC->GetTextMetrics(&tm);
	rcTime.top = rc.top;rcTime.bottom = (rc.top+m_mainBorder.top)/2;
	rcTime.left = m_mainBorder.left;rcTime.right = m_mainBorder.right;
	rcName.top = (rc.top+m_mainBorder.top)/2;rcName.bottom = m_mainBorder.top;
	rcName.left = m_mainBorder.left;rcName.right = m_mainBorder.right;
	rcText.top = m_mainBorder.top;rcText.bottom = m_mainBorder.top+1.5*tm.tmHeight;
	rcText.left = m_mainBorder.left;rcText.right = m_mainBorder.right;
	rcVolText.top = m_volBorder.top;rcVolText.bottom = m_volBorder.top+1.5*tm.tmHeight;
	rcVolText.left = m_volBorder.left;rcVolText.right = m_volBorder.right;

	map<long, testDetail, less_equal<long> >::iterator iter;
	iter = m_allTestData.begin();
	advance(iter, m_nStart+m_nCursorPos);
	testDetail curDetail = (*iter).second;

	float fTotYield = 0.0f;
	iter = m_allTestData.begin();
	for(int i=0;i<=m_nStart+m_nCursorPos;i++,iter++)
		fTotYield += (*iter).second.fProfit;
	
	float fCurProfit = curDetail.fProfit;
	float fTopProfit = curDetail.fTopProfit;
	float fButtonProfit = curDetail.fButtonProfit;
	long fAmount = curDetail.lCount*curDetail.fPrice;
	char getDate[16];
	sprintf(getDate, "%d-%d-%d", curDetail.nTime.year, curDetail.nTime.month, curDetail.nTime.day);
	char    name[NAME_LEN+1]; 			//股票代码
	strcpy(name, (*g_pStockIO)[curDetail.iGp]->Name);

	CDC memDC;
	memDC.CreateCompatibleDC (pDC);
	CBitmap		tempBitmap;				//作出临时位图
	tempBitmap.CreateCompatibleBitmap (pDC,rc.Width (),rc.Height());
	CBitmap *pOldBitmap = memDC.SelectObject (&tempBitmap);
	memDC.SetBkMode (TRANSPARENT);
	memDC.BitBlt(0, 0, rc.Width(), rc.Height(), &m_memDC, 0, 0, SRCCOPY);

	CPoint point;
	point.x = m_allPt[m_nStart+m_nCursorPos].xPos;
	point.y = m_allPt[m_nStart+m_nCursorPos].yClosePos;
	g_d.DrawLine(&memDC, m_drawBorder.left, point.y, m_drawBorder.right, point.y, VipColor.TXTCOLOR);
	g_d.DrawLine(&memDC, point.x, m_drawBorder.top, point.x, m_drawBorder.bottom, VipColor.TXTCOLOR);
	
	CFont *oldFont = g_d.SelectFont(&memDC, GRAPH_FONT);
	COLORREF disColor = curDetail.fProfit>COMPPREC?VipColor.KPCOLOR:VipColor.DOWNCOLOR;
	g_d.DisplayText(&memDC, &rcName, disColor, DT_LEFT|DT_BOTTOM, "股票:%s", name);
	g_d.DisplayText(&memDC, &rcTime, disColor, DT_LEFT|DT_BOTTOM, "时间:%s", getDate);
	g_d.DisplayText(&memDC, &rcText, disColor, DT_LEFT|DT_BOTTOM, "本次盈亏:%.2f%% ,最大盈亏:%.2f%%, 最小盈亏:%.2f%%, 总盈亏:%.2f%%", 
		fCurProfit, fTopProfit, fButtonProfit, fTotYield);
	g_d.DisplayText(&memDC, &rcVolText, disColor, DT_LEFT|DT_BOTTOM, "成交额:%d元", fAmount);
	pDC->BitBlt (0,0,rc.Width(),rc.Height(),&memDC,0,0,SRCCOPY);//从内存DC拷贝到屏幕DC
	g_d.RestoreFont(&memDC, oldFont);
	memDC.SelectObject(pOldBitmap);
	tempBitmap.DeleteObject();
	memDC.DeleteDC();
}

int CTestYieldWnd::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	CPaintDC dc(this); // device context for painting
	m_memDC.CreateCompatibleDC(&dc);	
	return 0;
}

BOOL CTestYieldWnd::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	if(m_bHiLight)
	{
		SetCursor(AfxGetApp()->LoadCursor(IDC_MOVE2));
		return TRUE;
	}
	
	return CWnd::OnSetCursor(pWnd, nHitTest, message);
}

void CTestYieldWnd::OnLButtonDown(UINT nFlags, CPoint point) 
{
	if(!m_bHiLight||m_iCurPos==-1) return;
	vector<testDetail> showDetail;
	map<long, testDetail, less_equal<long> >::iterator iter = m_allTestData.begin();
	advance(iter, m_nStart+m_iCurPos);
	testDetail curDetail = (*iter).second;
	BOOL bFind = FALSE;
	for(int i=m_sTestData.size()-1;i>=0;i--)
	{
		if(!bFind&&!memcmp(&m_sTestData[i], &curDetail, sizeof(testDetail)))
		{
			showDetail.push_back(m_sTestData[i]);
			bFind = TRUE;
			continue;
		}
		if(bFind&&m_sTestData[i].iGp==curDetail.iGp&&m_sTestData[i].iToDo==1)
			showDetail.push_back(m_sTestData[i]);
		if(bFind&&m_sTestData[i].iToDo!=1)
			bFind=FALSE;
	}
	reverse(showDetail.begin(), showDetail.end());
	
	char sTitle[128];
	int index = curDetail.iGp;
	sprintf(sTitle,"%s", (*g_pStockIO)[index]->Name);
	m_pShowDetail = new CMyDetailDlg(this);
	m_pShowDetail->m_sName = (CString)(sTitle) + (CString)("单次");
	m_pShowDetail->m_testDetails = showDetail;
	m_pShowDetail->Create(IDD_DETAIL);
	m_pShowDetail->ShowWindow(SW_SHOW);
	
	CWnd::OnLButtonDown(nFlags, point);
}

void CTestYieldWnd::OnClose() 
{
	DestroyWindow();
	
	CWnd::OnClose();
}

void CTestYieldWnd::PostNcDestroy() 
{
	delete this;
	
	CWnd::PostNcDestroy();
}

BOOL CTestYieldWnd::OnEraseBkgnd(CDC* pDC) 
{
	return FALSE;
}
/*
void CTestYieldWnd::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	CPaintDC dc(this);
	CPoint pt;
	GetCursorPos(&pt);
	ScreenToClient(&pt);
	switch(nChar)
	{
	case VK_RIGHT:
		if(m_nStart+MAX_PER_PAGE<m_allTestData.size())
		{
			if((::GetKeyState(VK_CONTROL)&0xFF00)) m_nStart+=min(4, m_allTestData.size()-MAX_PER_PAGE-m_nStart);
			else m_nStart++;
			Invalidate();
		}
		break;
	case VK_LEFT:
		if(m_nStart>0)
		{
			if((::GetKeyState(VK_CONTROL)&0xFF00)) m_nStart-=min(4, m_nStart);
			else m_nStart--;
			Invalidate();
		}
		break;
	case VK_HOME:
		m_nStart = 0;
		Invalidate();
		break;
	case  VK_END:
		m_nStart = m_allTestData.size()-MAX_PER_PAGE;
		Invalidate();
		break;
	default:
		break;
	}
	
	CWnd::OnKeyDown(nChar, nRepCnt, nFlags);
}
*/


void CTestYieldWnd::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	CDC *pDC=GetDC();
	int i, index, curMove = 0, curSet = min(m_allTestData.size(), m_maxPerPage), nTmp = m_nStart, moveSet = 10;;
	BOOL bFind;
	testDetail curDetail;
	vector<testDetail> showDetail;
	map<long, testDetail, less_equal<long> >::iterator iter = m_allTestData.begin();
	char sTitle[128];
	CPoint pt;
	GetCursorPos(&pt);
	ScreenToClient(&pt);
	switch(nChar)
	{
	case VK_RETURN:
		if(m_nCursorPos==-1) break;
		advance(iter, m_nStart+m_nCursorPos);
		curDetail = (*iter).second;
		bFind = FALSE;
		for(i=m_sTestData.size();i>=0;i--)
		{
			if(!bFind&&!memcmp(&m_sTestData[i], &curDetail, sizeof(testDetail)))
			{
				showDetail.push_back(m_sTestData[i]);
				bFind = TRUE;
				continue;
			}
			if(bFind&&m_sTestData[i].iGp==curDetail.iGp&&m_sTestData[i].iToDo==1)
				showDetail.push_back(m_sTestData[i]);
			if(bFind&&m_sTestData[i].iToDo!=1)
				bFind=FALSE;
		}
		reverse(showDetail.begin(), showDetail.end());
		
		index = curDetail.iGp;
		sprintf(sTitle,"%s", (*g_pStockIO)[index]->Name);
		m_pShowDetail = new CMyDetailDlg(this);
		m_pShowDetail->m_sName = (CString)(sTitle) + (CString)("单次");
		m_pShowDetail->m_testDetails = showDetail;
		m_pShowDetail->Create(IDD_DETAIL);
		m_pShowDetail->ShowWindow(SW_SHOW);
		break;

	case VK_RIGHT:
		if(m_nCursorPos==-1)
		{
			m_nCursorPos = curSet-1;
			for(i=0;i<curSet;i++)
			{
				if(m_allPt[m_nStart+i].xPos>=pt.x)
				{
					m_nCursorPos = i;
					break;
				}
			}
		}
		else if(m_nCursorPos==curSet-1&&m_nStart+m_maxPerPage<m_allTestData.size())
		{
			if((::GetKeyState(VK_CONTROL)&0xFF00)) m_nStart+=min(4, m_allTestData.size()-m_maxPerPage-m_nStart);
			else m_nStart++;
			Invalidate();
			break;
		}
		else if(m_nCursorPos<curSet-1&&(::GetKeyState(VK_CONTROL)&0xFF00)) m_nCursorPos=curSet-1;
		else if(m_nCursorPos<curSet-1) m_nCursorPos++;
		ShowCurInfo(pDC);
		break;
	case VK_LEFT:
		if(m_nCursorPos==-1)
		{
			m_nCursorPos = 0;
			for(i=curSet-1;i>=0;i--)
			{
				if(m_allPt[m_nStart+i].xPos<=pt.x)
				{
					m_nCursorPos = i;
					break;
				}
			}
		}
		else if(m_nCursorPos==0&&m_nStart>0)
		{
			if((::GetKeyState(VK_CONTROL)&0xFF00)) m_nStart-=min(4, m_nStart);
			else m_nStart--;
			Invalidate();
			break;
		}
		else if(m_nCursorPos>0&&(::GetKeyState(VK_CONTROL)&0xFF00)) m_nCursorPos=0;
		else if(m_nCursorPos>0) m_nCursorPos--;
		ShowCurInfo(pDC);
		break;
	case VK_UP:
		m_maxPerPage -= moveSet;
		m_maxPerPage = max(m_maxPerPage, 6);
		if(m_nCursorPos>-1) curMove = (int)((float)moveSet*((float)(m_nCursorPos+1)/(float)curSet));
		else curMove = moveSet/2; 
		m_nStart = min(max(0, m_allTestData.size()-6), m_nStart+curMove);
		if(m_nCursorPos>-1) m_nCursorPos -= (m_nStart - nTmp);
		Invalidate();
		break;
	case VK_DOWN:
		m_maxPerPage += moveSet;
		m_maxPerPage = min(m_maxPerPage, m_allTestData.size());
		if(m_nCursorPos>-1) curMove = (int)((float)moveSet*((float)(m_nCursorPos+1)/(float)curSet));
		else curMove = moveSet/2; 
		m_nStart = min(max(0, m_nStart-curMove), m_allTestData.size()-m_maxPerPage);
		if(m_nCursorPos>-1) m_nCursorPos -= (m_nStart - nTmp);
		Invalidate();
		break;
	case VK_HOME:
		m_nStart = m_nCursorPos = 0;
		Invalidate();
		break;
	case  VK_END:
		m_nStart = m_allTestData.size()>m_maxPerPage?m_allTestData.size()-m_maxPerPage:0;
		m_nCursorPos = curSet-1;
		Invalidate();
		break;
	case VK_ESCAPE:
		m_nCursorPos = -1;
		Invalidate();
		break;
	default:
		break;
	}
	
	CWnd::OnKeyDown(nChar, nRepCnt, nFlags);
}
